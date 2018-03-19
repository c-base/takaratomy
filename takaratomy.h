#ifndef _TAKARATOMY_H
#define _TAKARATOMY_H

#define VENDORID (0x0F30) //Takara Tomy (reported as Jess Technologies Co., Ltd)
#define PRODUCTID (0x0010) //USB Glitter Panel

struct usb_dev_handle* initPanel(unsigned int devnum);

#endif // _TAKARATOMY_H

