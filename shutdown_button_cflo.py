#!/usr/bin/env python3

import os.path
from time import sleep

import msgflo
import gevent
from Takaratomy import Takaratomy


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

            if state == 1: # OPEN_BUTTON_PRESSED
              self.onButtonPress()


if __name__ == '__main__':
    print('Shutdown Button')

    msgflo.main(ShutdownButton)

