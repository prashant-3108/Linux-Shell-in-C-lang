all: FSreport

FSreport:
	gcc -g -Wall -o FSreport FSreport.c -lm

clean:
	$(RM) FSreport
