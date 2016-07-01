# -*- coding: utf-8 -*-

# Shamelessly stolen from Jaspy:
# https://github.com/koehlma/jaspy/blob/master/jaspy/interactive.py

import asyncio
import os

from pygments.token import Token

from prompt_toolkit.styles import style_from_dict
from prompt_toolkit.shortcuts import print_tokens

from ptpython.repl import embed

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
        self.controller.device_new += self.on_device_new
        self.controller.device_deleted += self.on_device_deleted
        self.namespace = namespace
        self.cli = None

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

            self.cli.run_in_terminal(printer)

    def print_message(self, message, kind=SUCCESS):
        self.print_tokens([(kind, '<<< ' + message + os.linesep)])

    def on_package(self, device, source, target, command, payload):
        if command in {0x01}:
            return
        msg = '[{}] {} -> {} | {} | {!r}'.format(device.color, source, target,
                                                 command, payload)
        self.print_message(msg, INFO)

    def on_device_new(self, device):
        msg = '[NEW] Tin Bot: tin_bot_{} ({})'.format(device.color, device.address)
        device.on_package += self.on_package
        self.namespace['tin_bot_{}'.format(device.color)] = device
        self.print_message(msg, SUCCESS)

    def on_device_deleted(self, device):
        msg = '[DEL] Tin Bot: tin_bot_{} ({})'.format(device.color, device.address)
        del self.namespace['tin_bot_{}'.format(device.color)]
        self.print_message(msg, ERROR)
