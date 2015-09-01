all:
	clear
	gcc rm.c -o rm -lm
	./rm < input.txt
	rm rm