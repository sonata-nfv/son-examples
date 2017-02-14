#!/usr/bin/env python

from prometheus_client import start_http_server, Summary, Histogram, Gauge, Counter, REGISTRY, CollectorRegistry, \
    pushadd_to_gateway, push_to_gateway, delete_from_gateway
from subprocess import Popen, PIPE, STDOUT
import threading
from time import sleep
import pty
import os
import re
import sys
from math import isnan

import logging
logging.basicConfig(level=logging.INFO)

MAX_READ = 1024

# vnf configuration parameters
vnf_name = os.environ.get('VNF_NAME')
#pushgateway = 'localhost:9091'
pushgateway = '172.17.0.1:9091'


class iperf():
    def __init__(self, option_string=''):

        self.read_loop = True
        options = option_string.split(' ')
        cmd = ['iperf'] + options
        cmd_str = 'iperf '+ option_string
        master, slave = pty.openpty()
        self.process = Popen(cmd, stdout=slave, stderr=slave, close_fds=False)
        self.stdout = os.fdopen( master, 'r', 10000 )

        # buffer which holds the iperf process output to read from
        self.readbuf = ''
        self.test_str = ''

        self.test_end = False

        # Prometheus export data
        # helper variables to calculate the metrics
        self.registry = CollectorRegistry()

        #buckets = (0.1, 0.2, 0.5, 1, 2, 5, 7, 10, 20, 50, 70, 90, float("inf"))
        self.prom_loss = Gauge('sonemu_packet_loss_percent', 'iperf packet loss (percent)',
                                          ['vnf_name'], registry=self.registry)

        self.prom_packets_loss = Gauge('sonemu_packets_loss_count', 'iperf packets lost (count)',
                                         ['vnf_name'], registry=self.registry)

        self.prom_packets_total = Gauge('sonemu_packets_total_count', 'iperf packets total (count)',
                                           ['vnf_name'], registry=self.registry)

        #buckets = (1, 9, 10, 11, 90, 100, 110, 900, 1000, 1100, float("inf"))
        self.prom_bandwith = Gauge('sonemu_bandwith_Mbitspersec', 'iperf bandwith (Mbits/sec)',
                                            ['vnf_name'], registry=self.registry)

        #buckets = (0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 5, 10, float("inf"))
        self.prom_jitter = Gauge('sonemu_jitter_ms', 'iperf jitter (ms)',
                                       ['vnf_name'], registry=self.registry)


        self.prom_bandwith.labels(vnf_name=vnf_name).set(float('nan'))
        self.prom_loss.labels(vnf_name=vnf_name).set(float('nan'))
        self.prom_packets_total.labels(vnf_name=vnf_name).set(float('nan'))
        self.prom_packets_loss.labels(vnf_name=vnf_name).set(float('nan'))
        self.prom_jitter.labels(vnf_name=vnf_name).set(float('nan'))

        while True:
            data = self.readline()
            if data :
                logging.info('stdout: {0}'.format(data))
                self.parse_beginning_of_test(data)
                self.parse_end_of_test(data)
                if not self.test_end:

                    bw = self.parse_bandwith(data)

                    if not isnan(bw):
                        self.prom_bandwith.labels(vnf_name=vnf_name).set(bw)
                    else:
                        self.prom_bandwith.labels(vnf_name=vnf_name).set(bw)
                        # end of iperf test, no real measurement
                        continue

                    loss = self.parse_loss(data)
                    self.prom_loss.labels(vnf_name=vnf_name).set(loss)

                    lost, total = self.parse_packets(data)
                    if lost and total:
                        self.prom_packets_total.labels(vnf_name=vnf_name).set(total)
                        self.prom_packets_loss.labels(vnf_name=vnf_name).set(lost)

                    jitter = self.parse_jitter(data)
                    self.prom_jitter.labels(vnf_name=vnf_name).set(jitter)
            else:
                self.prom_loss.labels(vnf_name=vnf_name).set(float('nan'))
                self.prom_jitter.labels(vnf_name=vnf_name).set(float('nan'))

            pushadd_to_gateway(pushgateway, job='sonemu-profile_sink', registry=self.registry)


    def read( self, maxbytes=MAX_READ ):
        """Buffered read from node, potentially blocking.
           maxbytes: maximum number of bytes to return"""
        count = len( self.readbuf )
        if count < maxbytes:
            data = os.read( self.stdout.fileno(), maxbytes - count )
            self.readbuf += data.decode("utf-8") # need to decode bytes to string
        if maxbytes >= len( self.readbuf ):
            result = self.readbuf
            self.readbuf = ''
        else:
            result = self.readbuf[ :maxbytes ]
            self.readbuf = self.readbuf[ maxbytes: ]
        return result

    def readline(self):
        """Buffered readline from node, potentially blocking.
           returns: line (minus newline) or None"""

        pos = self.readbuf.find('\n')
        if pos >=0:
            line = self.readbuf[0: pos]
            # logging.info('stdout: {0}'.format(line))
            # self.parse_loss(line)
            self.readbuf = self.readbuf[(pos + 1):]
            return line
        else:
            test_str = self.read(MAX_READ) # get MAX_READ bytes of the buffer
            self.readbuf = self.readbuf + test_str
            return None



    def parse_loss(self,iperf_line):
        loss = re.search('(\()((\d+\.)?\d+)(\%\))', iperf_line)
        if loss:
            logging.info('loss: {0} percent'.format(loss.group(2)))
            return float(loss.group(2))
        else:
            logging.info('no loss found')
            return float('nan')

    def parse_bandwith(self, iperf_line):
        bw = re.search('(\d+\.?\d+)(\sMbits\/sec)', iperf_line)
        if bw:
            logging.info('bw: {0} Mbits/sec'.format(bw.group(1)))
            return float(bw.group(1))
        else:
            return float('nan')

    def parse_packets(self, iperf_line):
        match = re.search('(\d+)\/\s*(\d+)\s*\(', iperf_line)
        if match:
            lost = match.group(1)
            total = match.group(2)
            logging.info('packets lost: {0} total: {1}'.format(lost, total))

            return int(lost), int(total)
        else:
            return None, None

    def parse_jitter(self, iperf_line):
        match = re.search('(\d+\.\d+)\sms', iperf_line)
        if match:
            logging.info('jitter: {0} ms'.format(match.group(1)))
            return float(match.group(1))
        else:
            logging.info('no jitter found')
            return float('nan')

    def parse_end_of_test(self, iperf_line):
        match = re.search('(-\s-\s)+', iperf_line)
        if match:
            logging.info('end: {0} '.format(match.group(1)))
            self.test_end = True
            return match

    def parse_beginning_of_test(self, iperf_line):
        match = re.search('(--)+', iperf_line)
        if match:
            logging.info('begin: {0} '.format(match.group(1)))
            self.test_end = False
            return match

    def read_stdout(self):
        while self.read_loop:
            print('read')

            self.process.stdout.flush()
            output = self.process.stdout.readline()
            if output == '' and self.process.poll() is not None:
                break
            if output:
                logging.info('stdout: {0}'.format(output))





if __name__ == "__main__":

    #min is 12bytes
    #iperf_server = iperf('-s -u -l18 -i -fm')
    iperf_cmd = sys.argv[1]
    iperf_server = iperf(iperf_cmd)
    #iperf_server = iperf('-s -u -i -fm')
    #iperf_client = iperf('-c localhost -u -i1')
