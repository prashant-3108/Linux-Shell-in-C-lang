#include <stdio.h>     /* Input/Output General*/
#include <stdlib.h>    /* Utilities */
#include <unistd.h>    /* Symbolic Constants and system calls*/
#include <sys/types.h> /* Primitive System Data Types */
#include <sys/wait.h>  /* Wait for Process Termination */
#include <errno.h>     /* Errors */
#include <string.h>    /* String Func */
#include <signal.h>    /* Signals Handling */
#include <fcntl.h>     /* File Control*/

#define MAX_LINE 1024 /* The maximum size a command can have is assumed to be 1024 */

const char s[2] = " "; // for tokenising the command line into arguments .
int count = 0;
// for the total commands in the running shell

pid_t prcs[1000000]; // process array so that to store the child and parent process ids and kill them at the end.
pid_t background[1000000];

char *args[1000000];

int cnt_id = 0;
int cnt_bg = 1;

char shell_line[1024]; // Will be the array of char storing what is to be shown at shell like :: >

/* __environment variables */

char *HISTFILE;
char *PATH;
char *HOME;

/* Helper Functions */

// To display the history --

void displayHistory()
{
    int i = 0;
    char *history = (char *)malloc(1024); // array of chatracter to store the history from HISTFILE
    char *pth = (char *)malloc(1024);
    char *file = (char *)malloc(1024);
    strcpy(file, ".user_history");
    strcpy(pth, HOME);
    int n = strlen(pth);
    pth[n++] = '/';
    for (int i = 0; i < strlen(file); i++)
    {
        pth[n++] = file[i];
    }
    pth[n] = '\0';
    // printf("%s\n", pth);

    // pth stores entire path of .user_history
    FILE *fp;
    if ((fp = fopen(pth, "r+")) == NULL) // opening the HHISTFILE
    {
        printf("Error! opening file");
    }

    while (EOF != fscanf(fp, "%1024[^\n]\n", history)) // scanning the file and reading history line by line
    {
        printf(" %d ", i + 1);

        printf(" %s\n", history);
        i++;
    }
    free(history);
    fclose(fp);
    free(pth);
    free(file);
}

// to print last n histories..

void display_n_History(int num)
{
    char *history = (char *)malloc(1024);
    // char *history = (char *)malloc(1024); // array of chatracter to store the history from HISTFILE
    char *pth = (char *)malloc(1024);
    char *file = (char *)malloc(1024);
    strcpy(file, ".user_history");
    strcpy(pth, HOME);
    int n = strlen(pth);
    pth[n++] = '/';
    for (int i = 0; i < strlen(file); i++)
    {
        pth[n++] = file[i];
    }
    pth[n] = '\0';

    // pth stores entire path of .user_history
    FILE *fp;
    if ((fp = fopen(pth, "r+")) == NULL)
    {
        printf("Error! opening file");
    }

    // reads text until newline is encountered
    char c;
    int tempcount = 0;
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n') // Increment count if this character is newline
            tempcount = tempcount + 1;
    fclose(fp);

    if ((fp = fopen(pth, "r+")) == NULL)
    {
        printf("Error! opening file");
    }

    int i = 0, k = 1;
    int counter = tempcount - num;
    if (counter < 0)
    {
        counter = 0;
    }
    while (EOF != fscanf(fp, "%1024[^\n]\n", history))
    {

        // fscanf(fp, "%[^\n]", history);
        if (i < counter)
        {
            i++;
            k++;
            continue;
        }
        printf(" %d ", k);

        printf(" %s\n", history);
        k++;
        i++;
    }
    fclose(fp);
    free(history);
    free(pth);
    free(file);
}

// removing character from array of char.
void removeCharacter(char *str, char c) // takes input as array of char ad the character to be removed.
{
    int j = 0, i;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] != c)
        {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    return;
}

// Handles " ";
// to format the command line entered into arguments to be sent to execvp function.
void format(char *command, char *args[], size_t *length, ssize_t *nread, int *T)
{
    char *token;
    char *temp[1024];
    strtok(command, "\n");
    strtok(command, ";"); // if command has ; at end considering it also
    // printf("%s\n",command);
    int cntr = 0;
    token = strtok(command, s);
    temp[cntr++] = token;
    while (token != NULL) // tokening string using strtok to get arguments in list
    {
        token = strtok(NULL, s);
        if (token != NULL)
        {
            temp[cntr++] = token;
        }
    }
    // cntr--;
    // This Handles if command has argument which has space like :: mkdir "operating system"
    int exact_cnt = 0, j = 0;
    for (int i = 0; i < cntr; i++)
    {
        if (temp[i][0] != '"')
        {
            args[j++] = temp[i];
            exact_cnt++;
        }
        if (temp[i][0] == '"' && temp[i][strlen(temp[i]) - 1] == '"')
        {
            removeCharacter(temp[i], '"');
            args[j++] = temp[i];
            exact_cnt++;
        }
        if (temp[i][0] == '"' && temp[i + 1][strlen(temp[i + 1]) - 1] == '"')
        {
            removeCharacter(temp[i], '"');
            removeCharacter(temp[i + 1], '"');
            int x = strlen(temp[i]);
            temp[i][x] = ' ';
            temp[i][x + 1] = '\0';

            strcat(temp[i], temp[i + 1]);
            args[j++] = temp[i];
            exact_cnt++;
            // j++;
            i++;
        }
    }
    args[j] = NULL;
    *T = exact_cnt;
    return;
}

void handler(int sig)
{

    // printf("Background Process Terminated Successfully.\n");
}

int main(int argc, char *argv[])
{

    char *command;    // command line would be entered
    char *args[1024]; // Maximum 1024 Tokens can be there in one command

    size_t length = 0;
    ssize_t nread = 0;

    // Dynamicaly allocated memory to environment Variables and command line
    HISTFILE = (char *)malloc(1024);
    HOME = (char *)malloc(1024);
    PATH = (char *)malloc(1024);
    strcpy(PATH, "/bin");
    strcpy(HISTFILE, ".user_history");
    getcwd(HOME, 1024);

    char *pth = (char *)malloc(1024);
    char *file = (char *)malloc(1024);
    strcpy(file, ".user_history");

    strcpy(pth, HOME);
    int n = strlen(pth);
    pth[n++] = '/';
    for (int i = 0; i < strlen(file); i++)
    {
        pth[n++] = file[i];
    }
    pth[n] = '\0';
    // pth stores entire path of .user_history

    command = (char *)malloc(sizeof(char) * 1024);

    strcpy(shell_line, "");

    printf("%s > ", shell_line);
    nread = getline(&command, &length, stdin); // entering the first command.

    FILE *fp; // Printing it into history file
    fp = fopen(pth, "a+");
    if (fp == NULL)
    { // error handled if file not opening
        printf("Error! In .user_history");
        exit(1);
    }
    fprintf(fp, "%s", command);
    fclose(fp);

    // strcpy(history[count], command);
    count++;
    // loop for continuing asking for command until exit or quit has been wrote in the command..

    while ((strncmp(command, "exit", 4) != 0) && (strncmp(command, "quit", 4) != 0))
    {
        int tokens = 0;
        format(command, args, &length, &nread, &tokens); // Formating the command to a array of strings

        if ((args[0][0]) == '\n') // If simple enter is pressed directly goto take the next input (inp)
        {
            goto inp; // inp: is defined at last of this loop where input is taken to command line
        }

        // History

        // if simple history is passes as argument directly display all history/
        if ((strcmp(args[0], "history") == 0) && (tokens == 1))
        {
            displayHistory();
            goto inp;
        }

        // if history is passed with argument -c then clear the history till now..

        if ((tokens == 2) && (strcmp(args[0], "history") == 0) && (strcmp(args[1], "-c") == 0))
        {
            FILE *fp;
            fp = fopen(pth, "w"); // is done by opening file in write mode and dwriting nothing in it.
            if (fp == NULL)
            {
                printf("Error! In .user_history");
                exit(1);
            }
            fclose(fp);
            goto inp;
        }

        // if history has number then printing last n history and throwing error if not a numeric value

        if ((tokens == 2) && (strcmp(args[0], "history") == 0))
        {
            int num = atoi(args[1]);

            if (num == 0 && args[1][0] != '\0')
            {
                printf("myshell: history: %s: numeric argument required\n", args[1]);
                goto inp;
            }

            display_n_History(num);
            goto inp;
        }

        // if tokens of history are greater than two than can't execute that command
        if ((tokens > 2) && (strcmp(args[0], "history") == 0))
        {
            printf("history : Too many arguments.\n");
            goto inp;
        }

        // History Ended

        // cd

        // if simple cd gives current directory
        if (strcmp(args[0], "cd") == 0 && tokens == 1)
        {
            getcwd(shell_line, 1024);
            goto inp;
        }

        // changing directories using chdir() function
        if (strcmp(args[0], "cd") == 0 && tokens == 2)
        {
            if (strcmp(args[1], "/usr/bin/") == 0)
            {
                int ch = chdir(args[1]);
                if (ch == -1)
                {
                    printf("- myshell : %s : No such file or directory.\n", args[1]);
                    goto inp;
                }
                else
                {
                    getcwd(shell_line, 1024);
                }
                goto inp;
            }
            if (args[1][0] == '~') // getting to home directory
            {
                int ch = chdir(HOME);
                printf("%d\n", ch);
                if (ch == -1)
                {
                    printf("- myshell : Can't Locate HOME directory.\n");
                    goto inp;
                }
                else
                {
                    getcwd(shell_line, 1024);
                }
                goto inp;
            }

            int ch = chdir(args[1]);
            if (ch == -1)
            {
                printf("- myshell : %s : No such file or directory.\n", args[1]);
            }
            else
            {
                getcwd(shell_line, 1024);
            }
            goto inp;
        }
        // Too many arguments handled
        if (strcmp(args[0], "cd") == 0 && tokens > 2)
        {
            printf("cd : Too many arguments.\n");
            goto inp;
        }

        // cd ended

        // Export Handled;

        if ((strcmp(args[0], "export") == 0) && tokens == 2)
        {
            int check_home = 0;
            int check_histfile = 0;
            int check_path = 0;

            if (args[1][0] == 'P' && args[1][1] == 'A' && args[1][2] == 'T' && args[1][3] == 'H')
            {
                check_path = 1;
            }
            else if (args[1][0] == 'H' && args[1][1] == 'O' && args[1][2] == 'M' && args[1][3] == 'E')
            {
                check_home = 1;
            }
            else if (args[1][0] == 'H' && args[1][1] == 'I' && args[1][2] == 'S' && args[1][3] == 'T' && args[1][4] == 'F' && args[1][5] == 'I' && args[1][6] == 'L' && args[1][7] == 'E')
            {
                check_histfile = 1;
            }
            else
            {
                printf("Please enter Valid Argument.\n");
                goto inp;
            }

            if(check_path)
            {
                int  k = 5,j = 0;
                char temp[strlen(args[1]) - k + 1];

                for(int i = 5;i<strlen(args[1]);i++)
                {
                    temp[j++] = args[1][i];
                }
                temp[j] = '\0';

                strcpy(PATH,temp);
                goto inp;
            }
            else if(check_home)
            {
                int k = 5,j = 0;
                char temp[strlen(args[1]) - k + 1];

                for(int i = 5;i<strlen(args[1]);i++)
                {
                    temp[j++] = args[1][i];
                }
                temp[j] = '\0';

                strcpy(HOME,temp);
                goto inp;
            }
            else if(check_histfile)
            {
                int k = 8,j = 0;
                char temp[strlen(args[1]) - k + 1];

                for(int i = 8;i<strlen(args[1]);i++)
                {
                    temp[j++] = args[1][i];
                }
                temp[j] = '\0';

                strcpy(HISTFILE,temp);
                goto inp;
            }
        }

        if ((strcmp(args[0], "export") == 0) && tokens > 2)
        {
            printf("export : Too many arguments.\n");
            goto inp;
        }

        // export done

        // checking what symbols are present in command

        //      | or & or < or >

        // whatever is present it's boolean is marked ok..

        int _and = 0; // and  &
        int _inp = 0, idx_i = -1; // input <
        int _out = 0, idx_o = -1; // ouptut >
        int _pip = 0, idx_p = -1; // pipe |
        // int x_c = 0;

        for (int i = 0; i < tokens; i++)
        {
            if (args[i][0] == '<')
            {
                _inp = 1;
                idx_i = i;
                // x_c++;
            }
            else if (args[i][0] == '>')
            {
                _out = 1;
                idx_o = i;
                // x_c++;
            }
            else if (args[i][0] == '&')
            {
                _and = 1;
                // x_c++;
            }
            else if (args[i][0] == '|')
            {
                _pip = 1;
                idx_p = i;
                // x_c++;
            }
        }

        if (_pip) // if piping is present
        {

            pid_t pid1; // needing two child processes for both ends of the pipe
            pid_t pid2;

            char *demo1[idx_p + 1]; // making demo arrays for left and right arguments of the pipe.
            int i = 0;
            for (i = 0; i < idx_p; i++)
            {
                demo1[i] = args[i];
            }
            demo1[i] = NULL;

            char *demo2[tokens - idx_p];
            int j = 0;
            for (i = idx_p + 1; i < tokens; i++)
            {
                demo2[j] = args[i];
                j++;
            }
            demo2[j] = NULL;

            int fd[2]; // File Descriptor!!

            int p = pipe(fd); // piping the file descriptor..
            if (p == -1)
            {
                printf("Failed Piping!\n"); // if fails returning to the input
                goto inp;
            }
            int st1 = 0, st2 = 0; // status

            // left | right

            pid1 = fork(); // handling left part of pipe
            prcs[cnt_id++] = pid1;
            if (pid1 < 0)
            {
                perror("fork");
                exit(-1);
            }
            if (pid1 == 0)
            {
                prcs[cnt_id++] = pid1;

                dup2(fd[1], STDOUT_FILENO); // duplicating output descriptor..
                close(fd[0]);

                // checking if input or output commands are there in left

                int sub_inp = 0, sudidx_i = 0;
                int sub_out = 0, subidx_o = 0;

                for (int i = 0; i < idx_p; i++)
                {
                    if (args[i][0] == '<')
                    {
                        sub_inp = 1;
                        sudidx_i = i;
                    }
                    else if (args[i][0] == '>')
                    {
                        sub_out = 1;
                        subidx_o = i;
                    }
                }

                // if present using input output redirection doing that
                int st = 0;
                if (sub_inp && sub_out)
                {
                    FILE *fp = freopen(demo1[sudidx_i + 1], "r", stdin);

                    if (fp == NULL)
                    {
                        printf("Error opening the file !!\n");
                        exit(2);
                    }

                    FILE *fp2 = freopen(demo1[subidx_o + 1], "w+", stdout);
                    if (fp2 == NULL)
                    {
                        printf("Error Writing the file !!\n");
                        exit(1);
                    }

                    char *demo[sudidx_i + 1];
                    int i = 0;
                    for (i = 0; i < sudidx_i; i++)
                    {
                        demo[i] = demo1[i];
                    }

                    demo[i] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp2);
                    fclose(fp);
                    close(fd[1]);
                    exit(st);
                }
                if (sub_out && !sub_inp)
                {
                    FILE *fp = freopen(demo1[subidx_o + 1], "w+", stdout);
                    if (fp == NULL)
                    {
                        printf("Error Writing the file !!\n");
                        exit(1);
                    }

                    char *demo[subidx_o + 1];
                    int i = 0;
                    for (i = 0; i < subidx_o; i++)
                    {
                        demo[i] = demo1[i];
                    }
                    demo[i] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp);
                    // fflush(stdout);
                    close(fd[1]);
                    exit(st);
                }
                if (sub_inp && !sub_out)
                {
                    FILE *fp = freopen(demo1[sudidx_i + 1], "r", stdin);

                    if (fp == NULL)
                    {
                        printf("Error opening the file !!\n");
                        exit(2);
                    }

                    char *demo[sudidx_i + 1];
                    int i = 0;
                    for (i = 0; i < sudidx_i; i++)
                    {
                        demo[i] = demo1[i];
                    }

                    demo[i] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp);
                    // fflush(stdin);
                    close(fd[1]);
                    exit(st);
                }
                // if inp or out not there in left then simply executing that command
                st2 = execvp(demo1[0], demo1);

                close(fd[1]); // closing the output descriptor

                exit(st1);
            }
            // right process of pipe
            pid2 = fork();
            prcs[cnt_id++] = pid2;
            if (pid2 < 0)
            {
                perror("fork");
                exit(-1);
            }

            if (pid2 == 0)
            {
                prcs[cnt_id++] = pid2;

                dup2(fd[0], STDIN_FILENO); // duplicating for stdin
                close(fd[1]);              // closing the output descriptor

                int sub_inp = 0, sudidx_i = 0;
                int sub_out = 0, subidx_o = 0;

                for (int i = idx_p + 1; i < tokens; i++)
                {
                    if (args[i][0] == '<')
                    {
                        sub_inp = 1;
                        sudidx_i = i;
                    }
                    else if (args[i][0] == '>')
                    {
                        sub_out = 1;
                        subidx_o = i;
                    }
                }
                int st = 0;

                // if both input and output
                if (sub_inp && sub_out)
                {
                    FILE *fp = freopen(args[sudidx_i + 1], "r", stdin);

                    if (fp == NULL)
                    {
                        printf("Error opening the file !!\n");
                        exit(2);
                    }

                    FILE *fp2 = freopen(args[subidx_o + 1], "w+", stdout);
                    if (fp2 == NULL)
                    {
                        printf("Error Writing the file !!\n");
                        exit(1);
                    }

                    char *demo[sudidx_i + 1];
                    int i = 0, j = 0;
                    for (i = idx_p + 1; i < sudidx_i; i++)
                    {
                        demo[j++] = args[i];
                    }

                    demo[j] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp2);
                    fclose(fp);
                    close(fd[0]);
                    exit(st);
                }
                if (sub_out && !sub_inp)
                {
                    FILE *fp = freopen(args[subidx_o + 1], "w+", stdout);
                    if (fp == NULL)
                    {
                        printf("Error Writing the file !!\n");
                        exit(1);
                    }

                    char *demo[subidx_o + 1];
                    int i = 0, j = 0;
                    for (i = idx_p + 1; i < subidx_o; i++)
                    {
                        demo[j++] = args[i];
                    }
                    demo[j] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp);
                    // fflush(stdout);
                    close(fd[0]);
                    exit(st);
                }
                if (sub_inp && !sub_out)
                {
                    FILE *fp = freopen(args[sudidx_i + 1], "r", stdin);

                    if (fp == NULL)
                    {
                        printf("Error opening the file !!\n");
                        exit(2);
                    }

                    char *demo[sudidx_i + 1];
                    int i = 0, j = 0;
                    for (i = idx_p + 1; i < sudidx_i; i++)
                    {
                        demo[j++] = args[i];
                    }

                    demo[j] = NULL;

                    st = execvp(demo[0], demo);
                    if (st == -1)
                    {
                        printf("Cannot be Executed!! Enter a Vaid Command.\n");
                    }

                    fclose(fp);
                    // fflush(stdin);
                    close(fd[0]);
                    exit(st);
                }

                st2 = execvp(demo2[0], demo2);
                close(fd[0]);
                exit(st2);
            }

            close(fd[0]); // at last we should close the ends of file descriptor for safety
            close(fd[1]);

            int w1st, w2st; // waiting for both the child processes to end
            waitpid(pid1, &w1st, 0);
            waitpid(pid2, &w2st, 0);
            goto inp;
        }

        // if and is present..
        else if (_and)
        {
            pid_t sub_pid; /* child's process id */
            int st = 0;

            struct sigaction sa;      // to handle signal when child has completed.
            sigemptyset(&sa.sa_mask); // emptying all signal set

            sa.sa_flags = 0;

            // sa.sa_handler = handler;
            // sigaction(SIGCHLD, &sa, 0);
            // signal(SIGCHLD,SIG_IGN);

            sub_pid = fork();
            background[cnt_bg] = sub_pid;

            if (sub_pid == -1) // if fork unsuccessful then program crashes
            {
                perror("fork");
                exit(1);
            }
            else if (sub_pid == 0) // if child process
            {
                st = execvp(args[0], args); // execute the command and return its status
                exit(st);
            }

            // in parent it is not waiting for the child to end just puts the pid id and
            printf("[%d] %d\n", cnt_bg, sub_pid);
            cnt_bg++;
            goto inp;
        }
        else // if not & and piping
        {
            pid_t sub_pid; /* child's process id */
            int st = 0;

            sub_pid = fork(); // forking and getting child process

            if (sub_pid >= 0)
            {
                if (sub_pid == 0) // in child process
                {
                    prcs[cnt_id++] = sub_pid;

                    // handling commands if directly entered the environment variables
                    if ((strcmp(args[0], "$HOME") == 0) && tokens == 1)
                    {
                        printf("- myshell : %s : Is a Directory.\n", HOME);
                        exit(0);
                    }
                    if ((strcmp(args[0], "$HISTFILE") == 0) && tokens == 1)
                    {
                        printf("- myshell : %s : Is a File.\n", HISTFILE);
                        exit(0);
                    }
                    if ((strcmp(args[0], "$PATH") == 0) && tokens == 1)
                    {
                        printf("- myshell : %s : No such file or directory.\n", PATH);
                        exit(0);
                    }

                    // Handling echo with environment variables.

                    if ((strcmp(args[0], "echo") == 0) && tokens == 2 && ((strcmp(args[1], "$HISTFILE") == 0) || (strcmp(args[1], "$HOME") == 0) || (strcmp(args[1], "$PATH") == 0)))
                    {
                        if ((strcmp(args[1], "$HISTFILE") == 0))
                        {
                            printf("%s\n", HISTFILE);
                        }
                        if ((strcmp(args[1], "$HOME") == 0))
                        {
                            printf("%s\n", HOME);
                        }
                        if ((strcmp(args[1], "$PATH") == 0))
                        {
                            printf("%s\n", PATH);
                        }
                        exit(0);
                    }

                    // If < and > are both present
                    if (_inp && _out)
                    {
                        FILE *fp = freopen(args[idx_i + 1], "r", stdin); // opening input file and redirecting it to stdin

                        if (fp == NULL)
                        {
                            printf("Error opening the file !!\n");
                            exit(2);
                        }

                        FILE *fp2 = freopen(args[idx_o + 1], "w+", stdout); // opening outputfile and redirecting it into stdout
                        if (fp2 == NULL)
                        {
                            printf("Error Writing the file !!\n");
                            exit(1);
                        }

                        // Creating a demo array of arguments and command to be executed
                        char *demo[idx_i + 1];
                        int i = 0;
                        for (i = 0; i < idx_i; i++)
                        {
                            demo[i] = args[i];
                        }

                        demo[i] = NULL; // last element should be a null pointer to go into execvp call

                        st = execvp(demo[0], demo); // executing the command from where the input will be redirected from stdin and output from stdout to file
                        if (st == -1)
                        {
                            printf("Cannot be Executed!! Enter a Vaid Command.\n");
                        }

                        fclose(fp2);
                        fclose(fp); // closing files and exit child process
                        exit(st);
                    }

                    if (_out) // if single output is there same procedure
                    {
                        FILE *fp = freopen(args[idx_o + 1], "w+", stdout);
                        if (fp == NULL)
                        {
                            printf("Error Writing the file !!\n");
                            exit(1);
                        }

                        char *demo[idx_o + 1];
                        int i = 0;
                        for (i = 0; i < idx_o; i++)
                        {
                            demo[i] = args[i];
                        }
                        demo[i] = NULL;

                        st = execvp(demo[0], demo);
                        if (st == -1)
                        {
                            printf("Cannot be Executed!! Enter a Vaid Command.\n");
                        }

                        fclose(fp);
                        exit(st);
                    }
                    if (_inp) // if single input is there same procedure
                    {
                        FILE *fp = freopen(args[idx_i + 1], "r", stdin);

                        if (fp == NULL)
                        {
                            printf("Error opening the file !!\n");
                            exit(2);
                        }

                        char *demo[idx_i + 1];
                        int i = 0;
                        for (i = 0; i < idx_i; i++)
                        {
                            demo[i] = args[i];
                        }

                        demo[i] = NULL;

                        st = execvp(demo[0], demo);
                        if (st == -1)
                        {
                            printf("Cannot be Executed!! Enter a Vaid Command.\n");
                        }

                        fclose(fp);
                        // fflush(stdin);
                        exit(st);
                        continue;
                    }
                    // if none of the specified command are run then this will get run and result output to simple programs or give error
                    st = execvp(args[0], args);
                    exit(st);
                }
                else
                {
                    prcs[cnt_id++] = sub_pid;
                    int w_status;

                    waitpid(sub_pid, &w_status, 0);
                    if (WIFEXITED(w_status))
                    {
                        // error codes handled with different typs of errors
                        int statuscode = WEXITSTATUS(w_status);
                        if (statuscode != 0)
                        {
                            if (statuscode == 2)
                            {
                                goto inp;
                            }
                            else if (args[0][0] == '.' && args[0][1] == '/')
                            {
                                printf("-myShell: %s: No such file or directory.\n", args[0]);
                            }
                            else if (statuscode == 255)
                            {
                                printf("-bash: %s: command not found.\n", args[0]);
                            }
                            else
                            {
                                printf("Failed to execute the command.\n");
                            }
                        }
                    }
                }
            }
            else // else error in forking
            {
                perror("fork");
                exit(-1);
            }
        }
    // goto inp; has this lines of code where we take input and put it into the history file.
    inp:
        fflush(stdin);
        fflush(stdout);
        printf("%s > ", shell_line);
        nread = getline(&command, &length, stdin);

        FILE *fp;
        fp = fopen(pth, "a+");
        if (fp == NULL)
        {
            printf("Error! In .user_history");
            exit(1);
        }
        fprintf(fp, "%s", command);
        fclose(fp);
        count++;
    }
    // if the user enter exit shell terminates
    printf("    MyShell is Terminating...\n");

    // all child process are terminated

    int i = 0;
    for (i = 0; i < cnt_id; i++)
    {
        kill(prcs[i], SIGTERM);
    }

    // dynamic memory is being freed and parent shell process exit successfully..
    free(command);
    free(HISTFILE);
    free(HOME);
    free(PATH);
    free(pth);
    free(file);

    exit(0);
}