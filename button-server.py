#!/usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler
import json
from subprocess import Popen,PIPE
import random
import os
import sys
import time
from threading import Thread, Timer

CBEAM = 'http://c-beam.cbrp3.c-base.org/rpc/'
RETRY_INTERVAL = 10

jsonrpc_retrytime = 0
def jsonrpc_notify():
	global jsonrpc_retrytime
	if time.time() > jsonrpc_retrytime:
		try:
			requests.post(CBEAM, data=json.dumps({'method': 'partymode', 'params': [], 'id': 0}))
		except Exception as e:
			print('JSONRPC request failed, waiting', RETRY_INTERVAL, 'seconds:', e)
			jsonrpc_retrytime = time.time() + RETRY_INTERVAL

proc = Popen(['./button'], stdin=PIPE, stdout=PIPE)
def button_open():
	proc.stdin.write(b'o\n')
	proc.stdin.flush()
def button_close():
	proc.stdin.write(b'c\n')
	proc.stdin.flush()

def state_loop():
	global proc, button_state
	while True:
		print('Reading')
		button_state = proc.stdout.readline()
		print('Read:', button_state)
		if button_state:
			print('Received:', button_state)
			sys.stdout.flush()
			if button_state == 'button pressed':
				print('Button pressed.')
				jsonrpc_notify()
state_thread = Thread(target=state_loop)
state_thread.daemon = True
state_thread.start()

class ButtonHandler(BaseHTTPRequestHandler):
	def do_POST(self):
		global audiofiles
		self.send_response(200)
		self.end_headers()
		postlen = int(self.headers['Content-Length'])
		postdata = str(self.rfile.read(postlen), 'utf-8')
		data = json.loads(postdata)
		method = data.get('method')
		if method == 'open':
			button_open()
			closer = Timer(10, button_close)
			closer.start()

if __name__ == '__main__':
	HOST, PORT = '', 1337
	server = HTTPServer((HOST, PORT), ButtonHandler)
	server.serve_forever()
