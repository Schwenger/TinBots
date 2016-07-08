# -*- coding: utf-8 -*-

# Copyright (C) 2016, Maximilian Köhl <mail@koehlma.de>

import asyncio
import base64
import concurrent.futures
import functools
import io
import json
import os
import re
import traceback

import aiohttp

from aiohttp import web
from aiohttp import streams

streams.DEFAULT_LIMIT = 1024 * 1024 * 200

import numpy

__path__ = os.path.dirname(__file__)


class ApiError(Exception):
    def __init__(self, code=400, message=None):
        self.code = code
        self.message = message


def api_request():
    def decorator(handler):
        handler = asyncio.coroutine(handler)

        @asyncio.coroutine
        def wrapper(server, request):
            try:
                try:
                    data = yield from request.json()
                except ValueError:
                    raise ApiError(400, 'invalid json')

                result = yield from handler(server, request, data)
                response = {'status': 'success', 'result': result}
                return web.Response(content_type='application/json',
                                    text=json.dumps(response))
            except ApiError as api_error:
                error = api_error
            except Exception as exception:
                print(traceback.format_exc())
                error = ApiError(500, str(exception))
            response = {'status': 'error', 'code': error.code,
                        'message': error.message}
            return web.Response(status=error.code, text=json.dumps(response),
                                content_type='application/json')

        functools.update_wrapper(wrapper, handler)
        return wrapper

    return decorator


default_data_path = os.path.join(__path__, '..', 'data')
default_index_file = os.path.join(default_data_path, 'index.html')


class Server:
    def __init__(self, detector, loop=None, host=None, port=None):
        self.detector = detector
        self.detector.new_data += self.new_data

        self.host = host or '0.0.0.0'
        self.port = port or 8080

        self.loop = loop

        self.application = web.Application()
        self.router = self.application.router

        self.router.add_route('GET', '/websocket', self.websocket)
        self.clients = []
        self.events = {'lps_data': []}

        self.router.add_route('GET', '/', self.index)
        self.router.add_static('/', default_data_path)

        self._ws_rpc = {
            'register_event': self._ws_register_event,
            'unregister_event': self._ws_unregister_event,
        }

    def _ws_register_event(self, data, websocket):
        event = data['event']
        try:
            self.events[event].append(websocket)
        except KeyError:
            print('unable to register for event ' + event)

    def _ws_unregister_event(self, data, websocket):
        event = data['event']
        try:
            if websocket in self.events[event]:
                self.events[event].remove(websocket)
        except KeyError:
            print('unable to unregister for event ' + event)

    def new_data(self, output, positions):
        buffer = io.BytesIO()
        output.save(buffer, format="JPEG")
        image = base64.b64encode(buffer.getvalue())
        data = {
            'event': 'lps_data',
            'image': image.decode('ascii'),
            'positions': positions
        }
        self.loop.call_soon_threadsafe(self.send_data, json.dumps(data))

    def send_data(self, data):
        for client in self.events['lps_data']:
            client.send_str(data)

    @asyncio.coroutine
    def websocket(self, request):
        websocket = web.WebSocketResponse()
        yield from websocket.prepare(request)

        self.clients.append(websocket)

        while True:
            try:
                data = yield from websocket.receive_str()
                if data == 'close':
                    websocket.close()
                else:
                    try:
                        data = json.loads(data)
                        self._ws_rpc[data['method']](data, websocket)
                    except KeyError:
                        print('invalid ws rpc - method')
                        # websocket.send_str({'error': 'invalid ws rpc call'})
                    except ValueError:
                        print('invalid ws rpc - value error')
                        # websocket.send_str({'error': 'invalid ws rpc call'})
            except aiohttp.errors.WSClientDisconnectedError:
                self.clients.remove(websocket)
                for event_listeners in self.events.values():
                    if websocket in event_listeners:
                        event_listeners.remove(websocket)
                return websocket
            except Exception as error:
                for event_listeners in self.events.values():
                    if websocket in event_listeners:
                        event_listeners.remove(websocket)
                print('error ' + str(error))
                break

    @asyncio.coroutine
    def index(self, _):
        with open(default_index_file, 'rb') as index:
            return web.Response(body=index.read(), content_type='text/html')

    @asyncio.coroutine
    def setup(self, host=None, port=None, loop=None):
        handler = self.application.make_handler()
        self.host = host or self.host
        self.port = port or self.port
        self.loop = loop or self.loop or asyncio.get_event_loop()
        return self.loop.create_server(handler, self.host, self.port)
