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

from lps.commands import Commands
from lps.constants import Modes
from lps.debugger import Debugger, INFO


class VictimDirectionTest:
    def __init__(self, tinbot, debugger=None):
        self.debugger = debugger or Debugger.current

        self.tinbot = tinbot

        self.thread = None
        self.iterations = None
        self.result = None
        self.done = threading.Event()

    def start(self, iterations=50):
        self.iterations = iterations
        self.result = []

        self.thread = threading.Thread(target=self.run)
        self.thread.start()

    def run(self):
        self.tinbot.package_event += self.on_package
        self.debugger.print_message('Starting Victim Direction Test', INFO)
        self.tinbot.set_mode(Modes.VICDIR)
        for iteration in range(self.iterations):
            time.sleep(0.5)
            self.done.clear()
            self.tinbot.start()
            self.done.wait()
            self.tinbot.reset()
        self.tinbot.package_event -= self.on_package

    def on_package(self, device, source, target, command, payload):
        if command != Commands.VICTIM_PHI:
            return
        x, y, phi = Commands.VICTIM_PHI.decode(payload)
        self.result.append((phi, self.tinbot.victim_phi))
        self.done.set()
