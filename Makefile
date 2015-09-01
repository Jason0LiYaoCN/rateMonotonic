CC=gcc
CFLAGS=-lm 

all:
	clear
	$(CC) rm.c -o rm $(CFLAGS)
	./rm < input.txt > output.txt
	rm rm
	cat output.txt

clean:
	rm rm
	rm *.o
