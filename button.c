#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <usb.h>
#include <unistd.h>

#include "button.h"

struct usb_dev_handle *initPanel(unsigned int devnum) {
	struct usb_bus *bus;
	struct usb_bus *busses;
	struct usb_bus *GPbus;
	struct usb_device *dev;
	struct usb_device *GPdevice = NULL;
	struct usb_dev_handle *GPhandle = NULL;
	//Initialize libusb and have it scan for devices
	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	for(bus = busses; bus; bus = bus->next) {  //iterate over the linked list of USB busses.  usb_bus->next = NULL on the last device
		for(dev = bus->devices; dev; dev = dev->next) {  //iterate over the linked list of devices in this bus.  usb_bu
			//fprintf(stderr, "%s %s %X %X\n", bus->dirname, dev->filename, dev->descriptor.idVendor, dev->descriptor.idProduct);
			if((dev->descriptor.idVendor == VENDORID) && (dev->descriptor.idProduct == PRODUCTID)) {
				if(devnum == 0) {
					GPbus = bus;
					GPdevice = dev;
				} else {
					devnum--;
				}
			}
		}
	}
	if(GPdevice == NULL) {
		fprintf(stderr, "ERROR: USB Glitter Panel was not found!\n");
		return(NULL);
	}
	GPhandle = usb_open(GPdevice);
	if(GPhandle == NULL) {
		fprintf(stderr, "ERROR: USB Glitter Panel could not be opened!\n");
		return(NULL);
	}
	//fprintf(stderr, "INFO: USB Glitter Panel found on %s, %s.\n", GPbus->dirname, GPdevice->filename);
	return GPhandle;
}

int main(int argc, char **argv) {
	struct usb_dev_handle *dev = initPanel(0);
	if(dev == NULL) {
		fprintf(stderr, "ERROR: Couldn't initialize USB Glitter Panel!\n");
		return(EXIT_FAILURE);
	}

	char data[8];
	//data[0] = atoi(argv[1]);
	//usb_interrupt_write(dev, 0x02, (char *)data, 1, 1000);
	char last = 0;
	while(1){
		usb_interrupt_read(dev, 0x01, data, 1, 1000);
		//printf("%x\n", data[0]&0xff);
		if((data[0] & 0xF) == 5){
			if(last != 5){
				printf("button pressed\n");
				last = 5;
			}
		}else if((data[0] & 0xF) == 2){
			if(last != 2){
				printf("foo\n");
				last = 2;
			}
		}else if((data[0] & 0xF) == 4){
			if(last != 4){
				printf("bar\n");
				last = 4;
			}
		}else{
			last = 0;
		}
		usleep(10000);
	}

	usb_close(dev);
	//fprintf(stderr, "Done.\n");
	return(EXIT_SUCCESS);
}

