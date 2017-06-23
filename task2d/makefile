all: myshell
 
myshell: JobControl.o LineParser.o myshell.o 
	gcc -Wall  -g  -o myshell myshell.o LineParser.o JobControl.o
JobControl.o: JobControl.c JobControl.h
	gcc -Wall  -g -c JobControl.c
LineParser.o: LineParser.c LineParser.h
	gcc -Wall  -g -c LineParser.c
myshell.o : myshell.c
	gcc -Wall  -g -c myshell.c
clean:
	rm -f *.o  myshell
