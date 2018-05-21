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

  for(int y = 0; y < 16; ++y)
    for(int x = 0; x < 16; ++x)
      ledPanelSetPixel(hDev, x, y);

  sleep(1);

  ledPanelClear(hDev);
  closeLedPanel(hDev);

  printf("LED panel has been closed\n");

  return error;
}

