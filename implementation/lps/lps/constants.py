# -*- coding: utf-8 -*-

# Copyright (C) 2016, Maximilian Köhl <mail@koehlma.de>

import enum


VICTIM_HUE = 0.39
TIN_BOT_RED_HUE = 0.95
TIN_BOT_BLUE_HUE = 0.54

TIN_BOT_RED_ID = 1
TIN_BOT_BLUE_ID = 2

COLOR_MAP = {
    TIN_BOT_RED_ID: ('red', TIN_BOT_RED_HUE),
    TIN_BOT_BLUE_ID: ('blue', TIN_BOT_BLUE_HUE)
}

HUES = (VICTIM_HUE, TIN_BOT_RED_HUE, TIN_BOT_BLUE_HUE)

BROADCAST_ADDRESS = 0xFF


class Modes(enum.IntEnum):
    ALONE = 0
    FULL = 1
    RHR = 2
    VICDIR = 3
