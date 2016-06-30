# -*- coding: utf-8 -*-


class Event:
    def __init__(self, name=None):
        self.name = name
        self.handlers = []

    def __iadd__(self, handler):
        self.handlers.append(handler)
        return self

    def __isub__(self, handler):
        self.handlers.remove(handler)
        return self

    def fire(self, *args, **kwargs):
        for handler in self.handlers:
            handler(*args, **kwargs)
