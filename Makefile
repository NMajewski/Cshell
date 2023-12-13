CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow \
         -pedantic-errors -fstack-protector-all -Wextra

.PHONY: all clean

all: sh3ll

sh3ll: sh3ll.o parser.tab.o lexer.o executor.o
		gcc -lreadline sh3ll.o lexer.o parser.tab.o executor.o -o sh3ll

parser.tab.o: parser.tab.c command.h
		gcc $(CFLAGS) -c parser.tab.c

lexer.o: lexer.c parser.tab.h
		gcc $(CFLAGS) -c lexer.c

executor.o: executor.c command.h executor.h
		gcc $(CFLAGS) -c executor.c

sh3ll.o: sh3ll.c executor.h lexer.h
		gcc $(CFLAGS) -c sh3ll.c

clean:
		rm -f sh3ll *.o a.out