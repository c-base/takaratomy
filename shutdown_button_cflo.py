#!/usr/bin/env python3

# when executing locally, do not forget the export:
# export MSGFLO_BROKER=mqtt://c-beam/

import ctypes
import os.path
from Enum import Enum
from time import sleep

import msgflo
import gevent

class Takaratomy:
    def __init__(self):
        BASEDIR = os.path.dirname(os.path.realpath(__file__))
        self.lib = ctypes.cdll.LoadLibrary(os.path.join(BASEDIR, 'takaratomy.so'))

        self.lib.openButton.restype          = ctypes.c_void_p
        self.lib.openButton.argtypes         = [ctypes.c_uint32]
        self.lib.closeButton.restype         = None
        self.lib.closeButton.argtypes        = [ctypes.c_void_p]
        self.lib.getButtonState.restype      = ctypes.c_uint32
        self.lib.getButtonState.argtypes     = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int32)]
        self.lib.openButtonLid.restype       = ctypes.c_int32
        self.lib.openButtonLid.argtypes      = [ctypes.c_void_p]
        self.lib.closeButtonLid.restype      = ctypes.c_int32
        self.lib.closeButtonLid.argtypes     = [ctypes.c_void_p]

    def open(self, devnum):
        self.handle = self.lib.openButton(devnum)
        return self.handle

    def close(self):
        self.lib.closeButton(self.handle)

    def get_state(self):
        state = ctypes.c_int32(0)
        res = self.lib.getButtonState(self.handle, ctypes.byref(state))
        v = state.value

        return ButtonState(state.value)

    def open_lid(self):
        self.lib.openButtonLid(self.handle)

    def close_lid(self):
        self.lib.closeButtonLid(self.handle)


class ShutdownButton(msgflo.Participant):
    def __init__(self, role):
        b = Takaratomy()
        b.open(0)

        d = {
            'role': 'shutdown-button',
            'component': 'c-base/shutdown-button',
            'label': 'Shuts down the space station and initiates an evacuation',
            'icon': 'power-off',
            'inports': [],
            'outports': [
                {'id': 'pressed', 'type': 'boolean'},
            ],
        }
        msgflo.Participant.__init__(self, d, role)

    def process(self, inport, msg):
        self.ack(msg)
        gevent.Greenlet.spawn(self.loop)

    def onButtonPress(self):
        self.send('press', True)

    def loop(self):
        while True:
            gevent.sleep(0.1)
            state = b.get_state()

            if(state == 1) # OPEN_BUTTON_PRESSED
              self.onButtonPress()


if __name__ == '__main__':
    print('Shutdown Button')

    msgflo.main(ShutdownButton)

