#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <fcntl.h>
#include <stdint.h>

#include "takaratomy.h"

int main(int argc, char** ppArgv) {
  int error = 0;
  struct usb_dev_handle* hDev = openLedPanel(0);

  if(!hDev) {
    fprintf(stderr, "ERROR: Couldn't open USB LED matrix!\n");

    return 2;
  }

  printf("LED panel has been opened\n");

  // TODO: draw stuff here

  ledPanelSetPixel(hDev, 0, 0);
  ledPanelSetPixel(hDev, 1, 0);


  closeLedPanel(hDev);

  printf("LED panel has been closed\n");

  return error;
}

