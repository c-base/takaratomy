#!/usr/bin/python2
# coding=utf-8

# when executing locally, do not forget the export:
# export MSGFLO_BROKER=mqtt://c-beam/

from time import gmtime, strftime, sleep
import os.path
import msgflo
import gevent
import ctypes

class ShutdownButton(msgflo.Participant):
    def __init__(self, role):
        d = {
            'role': 'shutdown-button',
            'component': 'c-base/shutdown-button',
            'label': 'Shuts down the space station and initiates an evacuation',
            'icon': 'power-off',
            'inports': [],
            'outports': [
                { 'id': 'pressed', 'type': 'boolean' },
            ],
        }
        msgflo.Participant.__init__(self, d, role)

    def process(self, inport, msg):
        self.ack(msg)
        gevent.Greenlet.spawn(self.loop)

    def loop (self):
        while True:
            gevent.sleep(1)

    def onButtonPress(self):
        self.send('press', True)

if __name__ == "__main__":
    print("Shutdown Button")

    BASEDIR = os.path.dirname(os.path.realpath(__file__))
    lib = ctypes.cdll.LoadLibrary(os.path.join(BASEDIR, "takaratomy.so"))

    print("Now opening lid...")
    sleep(1)
    print("Now closing lid")

    # msgflo.main(ShutdownButton)

