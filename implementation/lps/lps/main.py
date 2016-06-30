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

import asyncio

from lps.com import Controller
from lps.detector import Detector
from lps.server import Server

if __name__ == '__main__':
    loop = asyncio.get_event_loop()

    detector = Detector()
    detector.start()

    controller = Controller(detector)
    controller.start()

    server = Server(detector)

    loop.run_until_complete(server.setup())
    loop.run_forever()
