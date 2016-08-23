#!/usr/bin/env python

import os
import sys
import time
import socket
import logging
import sys
from time import sleep

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

SOCKFILE = "/tmp/snort_alert"
BUFSIZE = 65863


# Must to set your controller IP here
CONTROLLER_IP = '10.0.10.1'

# Controller port is 51234 by default.
# If you want to change the port number
# you need to set the same port number in the controller application.
CONTROLLER_PORT = 51234

# TODO: TLS/SSL wrapper for socket


class SnortListener():

    def __init__(self, controller_ip=CONTROLLER_IP, controller_port=CONTROLLER_PORT):
        self.unsock = None
        self.nwsock = None
        self.controller_ip = controller_ip
        self.controller_port = int(controller_port)

    def start_send(self):
        connected = False
        while not connected:
            '''Open a client on Network Socket'''
            self.nwsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.nwsock.connect((self.controller_ip, self.controller_port))
                connected = True
            except Exception, e:
                logger.info("Network socket connection error: %s" % e)
                logger.info("Waiting for {0}:{1}".format(self.controller_ip, self.controller_port))
                sleep(1)
                #sys.exit(1)

    def start_recv(self):
        '''Open a server on Unix Domain Socket'''
        if os.path.exists(SOCKFILE):
            os.unlink(SOCKFILE)

        self.unsock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.unsock.bind(SOCKFILE)
        logger.info("Unix Domain Socket listening...")
        self.recv_loop()

    def recv_loop(self):
        '''Receive Snort alert on Unix Domain Socket and
        send to Network Socket Server forever'''
        logger.info("Start the network socket client....")
        self.start_send()
        while True:
            data = self.unsock.recv(BUFSIZE)
            time.sleep(0.5)
            if data:
                logger.debug("Send {0} bytes of data.".format
                             (sys.getsizeof(data)))
                # data == 65900 byte
                self.tcp_send(data)
            else:
                pass

    def tcp_send(self, data):
        self.nwsock.sendall(data)
        logger.info("Send the alert messages to Ryu.")


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python pigrelay.py <CONTROLLER IP (required)> <CONTROLLER_PORT (default:51234)>")
        exit(0)
    controller_ip = sys.argv[1]
    if len(sys.argv) > 2:
        controller_port = sys.argv[2]
    else:
        controller_port = CONTROLLER_PORT

    server = SnortListener(controller_ip, controller_port)
    server.start_recv()

