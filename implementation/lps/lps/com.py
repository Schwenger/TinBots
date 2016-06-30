# -*- coding: utf-8 -*-

import threading
import time

import bluetooth

from lps.event import Event


class TinBot(threading.Thread):
    def __init__(self, address, controller):
        self.address = address
        self.controller = controller

        super().__init__()

        self.socket = bluetooth.BluetoothSocket()
        try:
            self.socket.connect((address, 1))
        except bluetooth.BluetoothError:
            return
        self.controller.devices[self.address] = self
        self.start()

    def send_lps_position(self, x, y, phi):
        x = int(x * 100).to_bytes(2, 'big')
        y = int(y * 100).to_bytes(2, 'big')
        phi = int(phi * 1000).to_bytes(2, 'big');

    def send(self, command, payload):
        pass


    def run(self):
        try:
            while True:
                time.sleep(0.5)
                self.socket.send(b'\x00\x00\x10\x01\xAA')
                time.sleep(0.5)
                self.socket.send(b'\x00\x00\x10\x01\x55')
        finally:
            del self.controller.devices[self.address]
            self.controller.device_deleted.fire(self)


class Controller(threading.Thread):
    def __init__(self, detector):
        super().__init__()
        self.detector = detector

        self.device_new = Event()
        self.device_deleted = Event()

        self.devices = {}

    def run(self):
        while True:
            devices = bluetooth.discover_devices()
            for address in devices:
                name = bluetooth.lookup_name(address)
                if name and name.startswith('e-puck_'):
                    if address not in self.devices:
                        self.device_new.fire(TinBot(address, self))
            time.sleep(10)
