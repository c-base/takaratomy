all: button.c
	gcc -o button -std=gnu11 button.c -lusb
