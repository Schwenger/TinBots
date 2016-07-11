# -*- coding: utf-8 -*-

import math
import threading
import time
import queue

import bluetooth

from lps.constants import COLOR_MAP, Modes
from lps.commands import Commands
from lps.event import Event


class TinBot:
    def __init__(self, name, address, controller):
        self.name = name.replace('e-puck_', 'tin_bot_')
        self.number = None
        self.color = None
        self.hue = None
        self.address = address
        self.controller = controller

        self._position = None

        self.package_event = Event()

        self.socket = bluetooth.BluetoothSocket()
        try:
            self.socket.connect((address, 1))
        except bluetooth.BluetoothError:
            return

        self.controller.devices[self.address] = self

        self.queue = queue.Queue()

        self.thread_receive = threading.Thread(target=self._receive_loop)
        self.thread_receive.start()

        self.thread_sending = threading.Thread(target=self._sending_loop)
        self.thread_sending.start()

        # send hello
        self.send(Commands.HELLO)

    def send(self, command, payload=b'', source=None, target=None):
        if isinstance(command, Commands):
            command = command.number
        self.queue.put((command, payload, source or 0, target or 0))

    @property
    def victim_phi(self):
        phi = math.atan2(self.controller.victim.position[1] - self.position[1],
                         self.controller.victim_position[0] - self.position[0])
        return phi % (2 * math.pi)

    # commands
    def start(self):
        self.send(Commands.START)

    def reset(self):
        self.send(Commands.RESET)

    def set_mode(self, mode):
        assert min(Modes) <= mode <= max(Modes)
        payload = Commands.SET_MODE.encode(mode)
        self.send(Commands.SET_MODE, payload)

    def request_info(self):
        self.send(Commands.DEBUG_INFO)

    @property
    def position(self):
        return self._position

    @position.setter
    def position(self, position):
        self._position = position
        payload = Commands.UPDATE_LPS.encode(*position)
        self.send(Commands.UPDATE_LPS, payload)

    def set_speed(self, left_speed, right_speed):
        payload = Commands.DEBUG_MOTORS.encode(left_speed, right_speed)
        self.send(Commands.DEBUG_MOTORS, payload)

    def set_leds(self, bitmask):
        payload = Commands.DEBUG_LED.encode(bitmask)
        self.send(Commands.DEBUG_LED, payload)

    # FIXME
    # def cmd_victim_phi(self, phi, source=None):
    #     payload = Commands.T2T_VICTIM_PHI.encode(phi)
    #     self.send(Commands.T2T_VICTIM_PHI, payload, source)

    # sending and receive loop
    def _sending_loop(self):
        time.sleep(2)
        while True:
            command, payload, source, target = self.queue.get()
            packet = bytes([source, target, command, len(payload)]) + payload
            self.socket.send(packet)

    def _receive_loop(self):
        time.sleep(2)
        buffer = b''
        try:
            while True:
                while len(buffer) < 4:
                    buffer += self.socket.recv(1024)
                source, target, command, length = buffer[:4]
                buffer = buffer[4:]
                while len(buffer) < length:
                    buffer += self.socket.recv(1024)
                payload = buffer[:length]
                buffer = buffer[length:]
                if command == Commands.HELLO:
                    self.number = source
                    self.color, self.hue = COLOR_MAP[self.number]
                    self.controller.device_new.fire(self)
                self.package_event.fire(self, source, target, command, payload)
        except bluetooth.btcommon.BluetoothError:
            pass
        finally:
            del self.controller.devices[self.address]
            if self.color:
                self.controller.device_deleted.fire(self)
