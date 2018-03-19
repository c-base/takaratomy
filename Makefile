all: button.c takaratomy.c
	gcc -o button -std=gnu11 button.c takaratomy.c -lusb
