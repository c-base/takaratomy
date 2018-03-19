#ifndef _TAKARATOMY_H
#define _TAKARATOMY_H

#include <usb.h>

#define VENDORID (0x0F30)  // Takara Tomy (reported as Jess Technologies Co., Ltd)
#define PRODUCTID (0x0010) // USB Glitter Button Panel
// #define PRODUCTID (0x0040) // USB Led Matrix

enum Commands {
  CMD_OPEN  = 0x50, // alternatively: 0xE0
  CMD_CLOSE = 0x60  // alternatively: 0xD0
};

struct usb_dev_handle* initPanel(unsigned int devnum);

#endif // _TAKARATOMY_H

