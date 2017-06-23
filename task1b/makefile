all: myshell
 
myshell: LineParser.o myshell.o 
	gcc -Wall  -g  -o myshell myshell.o LineParser.o
LineParser.o: LineParser.c LineParser.h
	gcc -Wall  -g -c LineParser.c
myshell.o : myshell.c
	gcc -Wall  -g -c myshell.c
clean:
	rm -f *.o  myshell
