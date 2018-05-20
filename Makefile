all: clean button ledpanel takaratomy.so

clean:
	rm -f *.o *.so button ledpanel

takaratomy.o: takaratomy.c
	gcc -c takaratomy.c -std=gnu11 -o takaratomy.o

takaratomy.so: takaratomy.c
	gcc -fPIC -shared takaratomy.c -std=gnu11 -lusb -o takaratomy.so

button: takaratomy.o button.c
	gcc -o button -std=gnu11 button.c takaratomy.o -lusb

ledpanel: takaratomy.o ledpanel.c
	gcc -o ledpanel -std=gnu11 ledpanel.c takaratomy.o -lusb

