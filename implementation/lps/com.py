# -*- coding: utf-8 -*-

# Copyright (C) 2016, Maximilian Köhl <mail@koehlma.de>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3 as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.

import threading
import time

import bluetooth

tinbots = {}


class TinBot(threading.Thread):
    def __init__(self, address):
        self.address = address
        super().__init__()
        self.socket = bluetooth.BluetoothSocket()
        try:
            self.socket.connect((address, 1))
        except bluetooth.BluetoothError:
            return
        tinbots[self.address] = self
        self.start()

    def run(self):
        try:
            while True:
                time.sleep(0.5)
                self.socket.send(b'\x00\x00\x10\x01\xAA')
                time.sleep(0.5)
                self.socket.send(b'\x00\x00\x10\x01\x55')
        finally:
            del tinbots[self.address]

while True:
    devices = bluetooth.discover_devices()
    print('E-Pucks:')
    for address in devices:
        name = bluetooth.lookup_name(address)
        if name and name.startswith('e-puck_'):
            print(name)
            if address not in tinbots:
                TinBot(address)
    time.sleep(2)



