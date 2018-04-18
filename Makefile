all: clean main.c takaratomy.o button

clean:
	rm -f *.o

takaratomy.o: takaratomy.c
	gcc -c takaratomy.c -std=gnu11 -o takaratomy.o
	gcc -fPIC -shared takaratomy.c -std=gnu11 -lusb -o takaratomy.so

button: takaratomy.o
	gcc -o button -std=gnu11 main.c takaratomy.o -lusb

