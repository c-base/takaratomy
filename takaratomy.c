#include <stdio.h>
#include <string.h>
#include "takaratomy.h"

static struct usb_device* openUsbDevice(int vendorId, int productId, unsigned int devNum) {
  usb_init();
  usb_find_busses();
  usb_find_devices();

  for(struct usb_bus* pBus = usb_get_busses(); pBus; pBus = pBus->next) {
    for(struct usb_device* pDev = pBus->devices; pDev; pDev = pDev->next) {
      if((pDev->descriptor.idVendor == vendorId) && (pDev->descriptor.idProduct == productId)) {
        if(devNum == 0)
          return pDev;
        else
          devNum--;
      }
    }
  }

  return NULL;
}

struct usb_dev_handle* openButton(unsigned int devNum) {
  struct usb_device* pDev = openUsbDevice(VENDORID, BUTTON_PRODUCTID, devNum);

  if(!pDev) {
    fprintf(stderr, "ERROR: USB button was not found!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  struct usb_dev_handle* hDev = usb_open(pDev);

  if(!hDev) {
    fprintf(stderr, "ERROR: USB button could not be opened!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  usb_set_debug(1);

  if(usb_claim_interface(hDev, 0) < 0) {
    fprintf(stderr, "ERROR: Cannot claim interface!\n");

    return NULL;
  }

  return hDev;
}

static unsigned const char _pBlank[54] = {
  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x03, 0x5c, 0x08, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct GPpacket {
  char pHeader[22];
  unsigned short int pData[16];
};

static struct GPpacket _frameBuffer;

struct usb_dev_handle* openLedPanel(unsigned int devNum) {
  struct usb_device* pDev = openUsbDevice(VENDORID, LED_MATRIX_PRODUCTID, devNum);

  if(!pDev) {
    fprintf(stderr, "ERROR: USB LED panel was not found!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  struct usb_dev_handle* hDev = usb_open(pDev);

  if(!hDev) {
    fprintf(stderr, "ERROR: USB LED panel could not be opened!\n");
    printf("Did you perhaps forget to sudo or add a udev rule?\n");

    return NULL;
  }

  usb_set_debug(1);

  if(usb_claim_interface(hDev, 0) < 0) {
    fprintf(stderr, "ERROR: Cannot claim interface!\n");

    return NULL;
  }

  memcpy(_frameBuffer.pHeader, _pBlank, 54);

  return hDev;
}

void closeButton(struct usb_dev_handle* hDev) {
  usb_close(hDev);
}

void closeLedPanel(struct usb_dev_handle* hDev) {
  usb_close(hDev);
}

static int sendUsbCommand(struct usb_dev_handle* hDev, unsigned char cmd) {
  int ec;

  if((ec = usb_interrupt_write(hDev, 0x02, &cmd, 1, 10)) < 0) {
    printf("Error writing to USB device (%d): %s\n", ec, usb_strerror());
    return 2;
  }

  return 0;
}

static int requestButtonState(struct usb_dev_handle* hDev) {
  return sendUsbCommand(hDev, CMD_REQUEST_STATE);
}

int getButtonState(struct usb_dev_handle* hDev, enum ButtonState* pState) {
  int error;
  unsigned char data;
  static unsigned char last = 0;
  static enum ButtonState lastState = IDLE;

  error = usb_interrupt_read(hDev, 0x81, &data, 1, 10);
  int bytesRead = error >= 0 ? error : 0;

  if(bytesRead) {
    if(last != data) {
      if((data & 0x0F) == 5)
        *pState = BUTTON_PRESSED;
      if(data & 0x02)
        *pState = OPEN_BUTTON_PRESSED;
      if(data == 0x68)
        *pState = OPENING;
      if(data == 0x74)
        *pState = CLOSING;
      if(data == 0x44 && last == 0x60)
        *pState = OPEN;
      if(data == 0x58)
        *pState = CLOSED;

      last = data;
    }
    else
      *pState = IDLE;

    if(error = requestButtonState(hDev))
      return error;
  }

  return 0;
}

int openButtonLid(struct usb_dev_handle* hDev) {
  return sendUsbCommand(hDev, CMD_OPEN);
}

int closeButtonLid(struct usb_dev_handle* hDev) {
  return sendUsbCommand(hDev, CMD_CLOSE);
}

// Provisional panel handling:

#define BIT0 (1)
#define BIT1 (2)
#define BIT2 (4)
#define BIT3 (8)
#define BIT4 (16)
#define BIT5 (32)
#define BIT6 (64)
#define BIT7 (128)

static void writePanel(struct usb_dev_handle* hDev, struct GPpacket* pPacket) {
  usb_interrupt_write(hDev, 0x02, (char*)pPacket, 54, 1000);
  usb_interrupt_read(hDev, 0x81, NULL, 0, 1000);
}

void ledPanelClear(struct usb_dev_handle *hDev) {
  usb_interrupt_write(hDev, 0x02, (char*)_pBlank, 54, 1000);
  usb_interrupt_read(hDev, 0x81, NULL, 0, 1000);
}

void swap16bytes(unsigned short int *array) {
  array[0] = (array[0] >> 8) | (array[0] << 8);
  array[1] = (array[1] >> 8) | (array[1] << 8);
  array[2] = (array[2] >> 8) | (array[2] << 8);
  array[3] = (array[3] >> 8) | (array[3] << 8);
  array[4] = (array[4] >> 8) | (array[4] << 8);
  array[5] = (array[5] >> 8) | (array[5] << 8);
  array[6] = (array[6] >> 8) | (array[6] << 8);
  array[7] = (array[7] >> 8) | (array[7] << 8);
  array[8] = (array[8] >> 8) | (array[8] << 8);
  array[9] = (array[9] >> 8) | (array[9] << 8);
  array[10] = (array[10] >> 8) | (array[10] << 8);
  array[11] = (array[11] >> 8) | (array[11] << 8);
  array[12] = (array[12] >> 8) | (array[12] << 8);
  array[13] = (array[13] >> 8) | (array[13] << 8);
  array[14] = (array[14] >> 8) | (array[14] << 8);
  array[15] = (array[15] >> 8) | (array[15] << 8);
}

void pset(unsigned short int* array, unsigned int x, unsigned int y) {
  // single on bit moved to the position modulo 16, byte swapped.
  array[y & 15] |= 1 << ((x & 15) ^ BIT3);
}

inline void pclear(unsigned short int* array, unsigned int x, unsigned int y) {
  //single on bit moved to the position modulo 16, byte swapped and inverted
  array[y & 15] &= ~(1 << ((x & 15) ^ BIT3));
}

inline void ptoggle(unsigned short int* array, unsigned int x, unsigned int y) {
  //single on bit moved to the position modulo 16, byte swapped.
  array[y & 15] ^= 1 << ((x & 15) ^ BIT3);
}

inline int pget(unsigned short int* array, unsigned int x, unsigned int y) {
  return (array[y & 15] & (1 << ((x & 15) ^ BIT3)));
}

static void clearPacket(struct GPpacket* pPacket) {
  pPacket->pData[0]  = 0;
  pPacket->pData[1]  = 0;
  pPacket->pData[2]  = 0;
  pPacket->pData[3]  = 0;
  pPacket->pData[4]  = 0;
  pPacket->pData[5]  = 0;
  pPacket->pData[6]  = 0;
  pPacket->pData[7]  = 0;
  pPacket->pData[8]  = 0;
  pPacket->pData[9]  = 0;
  pPacket->pData[10] = 0;
  pPacket->pData[11] = 0;
  pPacket->pData[12] = 0;
  pPacket->pData[13] = 0;
  pPacket->pData[14] = 0;
  pPacket->pData[15] = 0;
}

void ledPanelSetPixel(struct usb_dev_handle* hDev, int x, int y) {
  pset(_frameBuffer.pData , x, y);
  writePanel(hDev, &_frameBuffer);
}

// --

// quick hack for LED matrix:
// 0: maybe command: 0x80 = Light up all LEDS
// 1: unknown
// 2: LED brightness
// 3: unknown
// 4: unknown
// 5: unknown
// 6: unknown
// 7: unknown

// unsigned char pBuf[8] = { 0x80, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00 };

// if((ec = usb_interrupt_write(hDev, 0x02, pBuf, 8, 10)) < 0) {
//   printf("Error writing to USB device (%d): %s\n", ec, usb_strerror());
//   return 2;
// }
// --

