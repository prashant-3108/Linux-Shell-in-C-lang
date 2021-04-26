# Linux Shell Simulation 

The following Program myshell.c is a simple version of linux bash shell made by using the c language. It has been capable of running the listed set of functions below.

	Helper Functions -

1. void displayHistory() - It displays the history from .user_history File but using fopen()
2. void display_n_History(int num)  - reads last n history and print them
3. void removeCharacter(char *str, char c) - Removes a certain character from array of characters

4. void format(char *command, char *args[], size_t *length, ssize_t *nread, int *T)
- This function formats the command line input given by the user into a array of strings we can say which is passed to the Unix System calls for execution of the command. Also returns the number of tokens which are present. Mainly uses strtok function in C.
It can handle the input such as :: mkdir "Operating System" in case the user wants to enter a string as an input with a space in it.

<br>

## 1.) SET - 1 Functions -

#### a.) The internal shell command "exit" which terminates the shell.  
This has been handled in while loop in which if user inputs exit or quit and kill all the remaining background or foreground process at the instant.

    	1. Concepts: shell commands, exiting the shell
    	2. System calls: exit(), kill()

#### b.)  A command with no arguments.
This has been handles in my shell and it gives different types ooutput if the system calls is not been executed perfectly using different error status codes like 0 , 255 , etc.
fork() is used to make a child process      	[ return -1 if not executed successfully]
execvp() is used to execute the command line    [ return -1 if not executed successfully]
waitpid() is used to make parent wait for the child process to end and then start again.

    	1. Concepts: Forking a child process, waiting for it to complete,
    	2. synchronous execution
    	3. System calls: fork(), execvp(), exit(), wait(), waitpid()
    	4. Example: ls

Handles error if thrown by any of the 3.

#### c.) **A command with arguments.**
This has been handled in my shell and it gives different types output if the system calls is not been executed perfectly using different error status codes like 0 , 255 , etc.
fork() is used to make a child process
execvp() is used to execute the command line
waitpid() is used to make parent wait for the child process to enand then start again.

    	1. Example: ls -l
    	2. Concepts: Command-line parameters

Handles error if thrown by any of the 3.

#### d.) A command, with or without arguments, executed in the background using &.
My shell executes the command and return immediately, not block it until the command finishes. it also prints the pid of the process when command gets executed with its number [which bg process it is] . Doesn't Support Background Processes with input output redirection and piping. Supports simple Unix Commands with argument and without arguments with &.

<br>

## 2.) SET - 2 Functions -

#### a.) A command, with or without arguments, whose output is redirected to a file.
it uses fork() to make a child process.
it opens the output file in write mode.
The C library function FILE *freopen(const char *filename, const char *mode, FILE *stream) associates a new filename with the given open stream and at the same time closes the old file in the stream. Here we set the stream a STDOUT to redirect output of the command execution to file.
execvp() is used to execute the command line whose output is written into file

    	1. Example: ls -l > foo
    	2. Concepts: File operations, output redirection
    	3. System calls: freopen() , dup()

Handles error if file has not opened and also handles previous errors like fork() and exec().. Parent waits till child ends the process.

#### b.) A command, with or without arguments, whose input is redirected from file.
it uses fork() to make a child process.
it opens the input file in read mode.
The C library function FILE *freopen(const char *filename, conschar *mode, FILE *stream) associates a new filename with the giveopen stream and at the same time closes the old file in the stream. Here we set the stream a STDIN to redirect input to the command execution.
execvp() is used to execute the command line in which input is taken from STDIN stream

		1. Example: sort < testfile
    	2. Concepts: File operations, input redirection
    	3. System calls: freopen() , dup()

Handles error if file has not opened or not present in directory and also handles previous errors like fork() and exec()..
Parent waits till child ends the process.

###### The shell also supports multiple input and output redirections.
	Eg. > ./some_program 2 < inputFile > outputFile

#### c.) A command, with or without arguments, whose output is piped to the input of another command.
This shell supports only one pipe for communication between the processes but supports input and output direction with hand in hand piping. This takes the output of the first command and makes it thinput to the second command.
It uses file descriptor and pipe() function for communication. fd[0] will be the fd(file descriptor) for the read end of pipe. fd[1] will be the fd for the write end of pipe.
Returns : 0 on Success. -1 on error.
Closing appropiate pipe ends at different times done.
Operations runs successfully.
Parent waits till left and right children of the pipe ends the process.


    	1. Example: ls -l | more
    	2. Concepts: Pipes, synchronous operation
    	3. System calls: pipe()

One level of Pipe is implemented here with supported input and output direction.

<br>

## 3.) SET - 3 Functions -

#### a.) Limited shell environment variables: PATH, HISTFILE, HOME.

Here, 1.PATH: contains the list of directories to be searched when commands are executed. 2. HISTFILE: contains the name of the file that contains a list of all inputs to the shell. The default will be the file .user_history in the directory in which the shell is initialized, i.e. the current working directory when the shell program is executed. 3. HOME - the default home directory will be the directory in which the shell is initialized, i.e. the current working directory when the shell program is executed.

#### b.) The history builtin command :

history prints out all of the input to the shell reading from the file by reading from the .user_history whose file path is given into fopen() which i sin the HOME directory..
(space) number (2 spaces) command line is the format.
supports history n
supports history Too many arguments
supports history -c - by clearing .user_history by opening it write mode and closing afterwords

#### c.) The cd builtin Command :

The supported functions are > cd gives present working directory
uses chdir() to change directory. > cd path > cd .. changes to parent directory

    > cd ~  changes to HOME directory which is our personal.


    > cd subdir1/subdir2 
	limitation --> doesn't support subdir1 or subdir2 with spaces in between them


    > cd /usr/bin/

###### Supports > cd too many arguments

#### d.) The export builtin Command :

The shell give the export attribute to the variables corresponding
to the specified names, which shall cause them to be in the
environment of subsequently executed commands. Environment variables
are set to =word when export command is runned.

###### Supports > export Too Many Arguments
