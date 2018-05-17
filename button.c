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

  unsigned int state = 0;
  int error = 0;

  while(1) {
    enum ButtonState bState;

    if(error = getButtonState(hDev, &bState))
      return error;

    switch(bState) {
      case BUTTON_PRESSED: printf("button pressed\n"); break;
      case OPEN_BUTTON_PRESSED: printf("open button pressed\n"); break;
      case OPENING: printf("opening\n"); break;
      case CLOSING: printf("closing\n"); break;
      case OPEN: printf("open\n"); break;
      case CLOSED: printf("closed\n"); break;
    }

    unsigned char option;

    int len = read(STDIN_FILENO, &option, 1);

    if(len > 0) {
      if(state == 0) {
        state = 1;

        if(option == 'c')
          error = closeButtonLid(hDev);
        else if(option == 'o')
          error = openButtonLid(hDev);
        else if(option == 'q') {
          printf("quitting...\n");

          return 0;
        }

        if(error)
          return error;
      }
      else {
        if(option == '\n')
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

  closeButton(hDev);

  return error;
}

