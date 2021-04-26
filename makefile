all: myshell.c
	gcc -g -Wall -o myshell myshell.c

clean:
	$(RM) myshell
