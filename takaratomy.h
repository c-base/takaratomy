#ifndef _TAKARATOMY_H
#define _TAKARATOMY_H

#include <usb.h>

#define VENDORID             (0x0F30) // Takara Tomy (reported as Jess Technologies Co., Ltd)
#define BUTTON_PRODUCTID     (0x0010) // USB Glitter Button Panel
#define LED_MATRIX_PRODUCTID (0x0040) // USB Led Matrix

enum Commands {
  CMD_REQUEST_STATE  = 0x40,
  CMD_OPEN           = 0x50, // alternatively: 0xE0
  CMD_CLOSE          = 0x60  // alternatively: 0xD0
};

enum ButtonState {
  IDLE,
  BUTTON_PRESSED,
  OPEN_BUTTON_PRESSED,
  OPENING,
  CLOSING,
  OPEN,
  CLOSED
};

struct usb_dev_handle* openButton(unsigned int devNum);
void closeButton(struct usb_dev_handle* hDev);
int getButtonState(struct usb_dev_handle* hDev, enum ButtonState* pState);
int openButtonLid(struct usb_dev_handle* hDev);
int closeButtonLid(struct usb_dev_handle* hDev);

#endif // _TAKARATOMY_H

