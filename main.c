#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <fcntl.h>
#include <stdint.h>

#include "takaratomy.h"

static int runQuickMode(struct usb_dev_handle* hDev, const char* pCmd) {
  if (!strcmp(pCmd, "close"))
    return closeButtonLid(hDev);
  else if(!strcmp(pCmd, "open"))
    return openButtonLid(hDev);
  else {
    printf("Please specify a command (open/close)\n");

    return 1;
  }
}

static int runInteractiveMode(struct usb_dev_handle* hDev) {
  printf("Running in interactive mode.\n");
  printf("'o': open lid\n");
  printf("'c': close lid\n");
  printf("'q': quit\n");

  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);

  unsigned char data = 0;
  unsigned char last = 0;
  unsigned int state = 0;
  int error = 0;

  while(1) {
    if(!usb_interrupt_read(hDev, 0x81, &data, 1, 10)) {
      if(last != data) {
        if((data & 0x0F) == 5)
          printf("button pressed\n");
        if(data & 0x02)
          printf("open button pressed\n");
        if(data == 0x68)
          printf("opening\n");
        if(data == 0x74)
          printf("closing\n");
        if(data == 0x44 && last == 0x60)
          printf("open\n");
        if(data == 0x58)
          printf("closed\n");

        last = data;
      }

      if(error = requestButtonState(hDev))
        return error;
    }

    int len = read(STDIN_FILENO, &data, 1);

    if(len > 0) {
      if(state == 0) {
        state = 1;

        if(data == 'c')
          error = closeButtonLid(hDev);
        else if(data == 'o')
          error = openButtonLid(hDev);
        else if(data == 'q') {
          printf("quitting...\n");

          return 0;
        }

        if(error)
          return error;
      }
      else {
        if(data == '\n')
          state = 0;
      }
    }

    usleep(100000);
  }
}

int main(int argc, char** ppArgv) {
  int error = 0;
  struct usb_dev_handle* hDev = openButton(0);

  if(!hDev) {
    fprintf(stderr, "ERROR: Couldn't open USB button!\n");

    return 2;
  }

  if(argc == 2)
    error = runQuickMode(hDev, ppArgv[1]);
  else if(argc == 1)
    error = runInteractiveMode(hDev);
  else {
    printf("Invalid argument. Specify open/close to open/close or nothing to listen.\n");
    error = 1;
  }

  usb_close(hDev);

  return error;
}

