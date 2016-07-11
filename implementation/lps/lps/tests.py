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

from lps.commands import Commands


class VictimDirectionTest:
    def __init__(self, tinbot):
        self.tinbot = tinbot
        self.tinbot.on_package += self.on_package

    def start(self):
        self.tinbot.cmd_set_mode(3)
        import time
        time.sleep(0.5)
        self.tinbot.cmd_start()

    def on_package(self, device, source, target, command, payload):
        if command != Commands.T2T_VICTIM_PHI:
            return
        x, y, phi = Commands.T2T_VICTIM_PHI.decode(payload)
        print(x, y, phi)

