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

import time

import bluetooth

devices = bluetooth.discover_devices()

socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect(('10:00:E8:AD:75:9D', 1))

while True:
    time.sleep(0.5)
    socket.send(b'\x00\x00\x10\x01\xAA')
    time.sleep(0.5)
    socket.send(b'\x00\x00\x10\x01\x55')
