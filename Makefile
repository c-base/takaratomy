all: main.c takaratomy.c
	gcc -o button -std=gnu11 main.c takaratomy.c -lusb
