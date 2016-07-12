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

import enum
import struct


class Command:
    def __init__(self, number, spec=None, recv_spec=None):
        self.number = number
        self.send_spec = struct.Struct('<' + (spec or ''))
        self.recv_spec = struct.Struct('<' + (recv_spec or spec or ''))

    def __eq__(self, other):
        return self.number == other

    def __ne__(self, other):
        return self.number != other

    def __hash__(self):
        return hash(self.number)

    def encode(self, *arguments):
        return self.send_spec.pack(*arguments)

    def decode(self, payload):
        return self.recv_spec.unpack(payload)


class Commands(Command, enum.Enum):
    HELLO = 0x01, '', 'fhH'
    START = 0x02
    RESET = 0x03
    PRINT = 0x04

    SET_MODE = 0x10, 'B'
    UPDATE_LPS = 0x11, '3f'

    T2T_HEARTBEAT = 0x30
    T2T_VICTIM_PHI = 0x31, 'fff'
    T2T_VICTIM_XY = 0x32, 'HHH'
    T2T_UPDATE_MAP = 0x33, 'BB64B'
    T2T_DOCKED = 0x34
    T2T_COMPLETED = 0x35

    DEBUG_INFO = 0x60, '11fH7B'
    DEBUG_MOTORS = 0x61, 'ff'
    DEBUG_LED = 0x62, 'H'


PROLOG = '''
#ifndef EPUCK_COMMANDS_H
#define EPUCK_COMMANDS_H

enum Commands {
'''

EPILOG = '''
};

#endif
'''


if __name__ == '__main__':
    print(PROLOG)
    fields = []
    for command in Commands:
        fields.append('    CMD_{} = {}'.format(command.name, command.number))
    print(',\n'.join(fields))
    print(EPILOG)
