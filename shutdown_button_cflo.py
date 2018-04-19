#!/usr/bin/env python3

# when executing locally, do not forget the export:
# export MSGFLO_BROKER=mqtt://c-beam/

import ctypes
import os.path
from time import sleep

import msgflo
import gevent


class ShutdownButton(msgflo.Participant):
    def __init__(self, role):
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

    def loop(self):
        while True:
            gevent.sleep(1)

    def onButtonPress(self):
        self.send('press', True)

class Takaratomy:
    def __init__(self):
        BASEDIR = os.path.dirname(os.path.realpath(__file__))
        self.lib = ctypes.cdll.LoadLibrary(os.path.join(BASEDIR, 'takaratomy.so'))

        self.lib.getButtonState.restype      = ctypes.c_uint32
        self.lib.getButtonState.argtypes     = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int32)]
        self.lib.openButton.restype          = ctypes.c_void_p
        self.lib.openButton.argtypes         = [ctypes.c_uint32]
        self.lib.openButtonLid.restype       = ctypes.c_int32
        self.lib.openButtonLid.argtypes      = [ctypes.c_void_p]
        self.lib.closeButtonLid.restype      = ctypes.c_int32
        self.lib.closeButtonLid.argtypes     = [ctypes.c_void_p]

    def openButton(self, devnum):
        self.handle = self.lib.openButton(devnum)
        return self.handle

    #TODO: implement closeButton function

    def getButtonState(self):
        state = ctypes.c_int32(0)
        res = self.lib.getButtonState(self.handle, ctypes.byref(state))
        return state.value

    def openButtonLid(self):
        self.lib.openButtonLid(self.handle)

    def closeButtonLid(self):
        self.lib.closeButtonLid(self.handle)

if __name__ == '__main__':
    print('Shutdown Button')

    b = Takaratomy()
    b.openButton(0)

    while(True):
      state = b.getButtonState()
      print('Button state: %d' % state)
      sleep(0.1)

    print('Now opening lid...')

    b.openButtonLid()

    sleep(5)
    print('Now closing lid')

    res = b.closeButtonLid()
    sleep(5)

    # msgflo.main(ShutdownButton)

