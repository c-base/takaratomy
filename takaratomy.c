#include <stdio.h>
#include "takaratomy.h"

struct usb_dev_handle* initPanel(unsigned int devnum) {
  struct usb_bus* pBus             = NULL;
  struct usb_bus* pBusses          = NULL;
  struct usb_bus* pGpbus           = NULL;
  struct usb_device* pDev          = NULL;
  struct usb_device* pGpdevice     = NULL;
  struct usb_dev_handle* pGphandle = NULL;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  pBusses = usb_get_busses();

  for(pBus = pBusses; pBus; pBus = pBus->next) {
    for(pDev = pBus->devices; pDev; pDev = pDev->next) {
      if((pDev->descriptor.idVendor == VENDORID) && (pDev->descriptor.idProduct == PRODUCTID)) {
        if(devnum == 0) {
          pGpbus = pBus;
          pGpdevice = pDev;
        }
        else
          devnum--;
      }
    }
  }

  if(!pGpdevice) {
    fprintf(stderr, "ERROR: USB button was not found!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  pGphandle = usb_open(pGpdevice);

  if(!pGphandle) {
    fprintf(stderr, "ERROR: USB button could not be opened!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  usb_set_debug(1);

  if(usb_claim_interface(pGphandle, 0) < 0){
    fprintf(stderr, "ERROR: Cannot claim interface!\n");

    return NULL;
  }

  return pGphandle;
}
