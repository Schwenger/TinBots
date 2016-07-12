# -*- coding: utf-8 -*-

# Shamelessly stolen from Jaspy:
# https://github.com/koehlma/jaspy/blob/master/jaspy/interactive.py

import asyncio
import os

from pygments.token import Token

from prompt_toolkit.styles import style_from_dict
from prompt_toolkit.shortcuts import print_tokens

from ptpython.repl import embed

from lps.commands import Commands

INFO = Token.Info
SUCCESS = Token.Success
WARNING = Token.Warning
ERROR = Token.Error

style = style_from_dict({
    Token.Info: '#5555FF',
    Token.Success: '#00FF00',
    Token.Warning: '#FF5500',
    Token.Error: '#FF0000',
    Token.Trace: '#888888',
    Token.Variable: '#FF8888'
}, include_defaults=True)


class Debugger:
    def __init__(self, controller, namespace=None):
        self.controller = controller
        self.controller.device_new_event += self.on_device_new
        self.controller.device_deleted_event += self.on_device_deleted
        self.controller.devices_visible_event += self.on_device_visible

        self.namespace = namespace
        self.cli = None

        self.handlers = {
            Commands.PRINT: self.on_print,
            Commands.DEBUG_INFO: self.on_debug_info
        }

        self.loop = asyncio.get_event_loop()

    def start(self):
        coroutine = embed(self.namespace, self.namespace, title='Tin Bot Console',
                          patch_stdout=True, history_filename='.tin_bot_history',
                          return_asyncio_coroutine=True)

        # HACK: nasty hack to gain access to the command line interface wrapper
        self.cli = coroutine.gi_frame.f_locals['cli']

        future = asyncio.ensure_future(coroutine)
        return future

    def print_tokens(self, tokens):
        if self.cli:
            def printer():
                print_tokens(tokens, style=style)
            self.loop.call_soon_threadsafe(self.cli.run_in_terminal, printer)

    def print_message(self, message, kind=SUCCESS):
        self.print_tokens([(kind, '<<< ' + message + os.linesep)])

    # command handles
    def on_print(self, device, source, target, payload):
        msg = '[{}] {}'.format(device.color, payload.decode('ascii'))
        self.print_message(msg, INFO)

    def on_debug_info(self, device, source, target, payload):
        info = Commands.DEBUG_INFO.decode(payload)
        msg = '[{}] PROXIMITY: {} {} {} {} {} {} {} {}'.format(device.color, *info[:8])
        self.print_message(msg, INFO)
        msg = '[{}] LPS      : {} {} {}'.format(device.color, *info[8:11])
        self.print_message(msg, INFO)
        msg = '[{}] FREQ     : {} '.format(device.color, info[11])
        self.print_message(msg, INFO)
        msg = '[{}] IR       : {} {} {} {} {} {}'.format(device.color, *info[12:18])
        self.print_message(msg, INFO)
        msg = '[{}] GRABBED  : {}'.format(device.color, info[18])
        self.print_message(msg, INFO)

    # event handlers
    def on_package(self, device, source, target, command, payload):
        if command in {Commands.HELLO}:
            return
        if command in self.handlers:
            self.handlers[command](device, source, target, payload)
            return
        color = device.color
        msg = '[{}] {} -> {} | {} | {!r}'.format(color, source, target, command, payload)
        self.print_message(msg, INFO)

    def on_device_new(self, device):
        msg = '[NEW] Tin Bot: tin_bot_{} ({})'.format(device.color, device.address)
        device.package_event += self.on_package
        self.namespace['tin_bot_{}'.format(device.color)] = device
        self.print_message(msg, SUCCESS)

    def on_device_visible(self, address):
        msg = '[VISIBLE] E-Puck: {}'.format(address)
        self.print_message(msg, INFO)

    def on_device_deleted(self, device):
        msg = '[DEL] Tin Bot: tin_bot_{} ({})'.format(device.color, device.address)
        del self.namespace['tin_bot_{}'.format(device.color)]
        self.print_message(msg, ERROR)
