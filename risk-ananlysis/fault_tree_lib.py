#!/usr/bin/env python3
# -*- coding:utf-8 -*-
#
# Copyright (C) 2016, Maximilian Köhl <mail@koehlma.de>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License version 3 as published by the Free
# Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this
# program. If not, see <http://www.gnu.org/licenses/>.

import collections
import copy
import inspect
import subprocess

from math import nan


trees = set()
nodes = {}


class TreeMeta(type):
    def __new__(mcs, name, bases, attributes):
        cls = super().__new__(mcs, name, bases, attributes)
        if cls.failure is not None:
            trees.add(cls)
        return cls

    def __call__(cls):
        return copy.deepcopy(cls)


class Tree(metaclass=TreeMeta):
    failure = None


def get_trees():
    return trees


class Node:
    parameters = {}

    def __init__(self, parent=None, *children, **parameters):
        frame = inspect.currentframe()
        while frame:
            code = frame.f_code
            if code.co_filename != __file__:
                nodes[self] = (code.co_filename, frame.f_lineno)
                break
            frame = frame.f_back
        self.parent = parent
        self.children = list(children)
        self.parameters = dict(parameters)
        assert 'children' not in self.parameters

    def __call__(self):
        return copy.deepcopy(self)

    def make_name(self):
        # Can't statically compute the name, because it wouldn't be
        # re-computed for a deep-copied node.
        return 'node{}'.format(id(self))

    def graphviz_node(self):
        label = '"<Unnamed>"'
        if self.parameters['label']:
            label = self.parameters['label']
            if not isinstance(self, Gate):
                # Q: Why '%g'?
                # A: Properly display very small rates
                # >>> '%.9f' % (1e-12/9)
                # '0.000000000'
                # >>> '%.9g' % (1e-12/9)
                # '1.11111111e-13'
                label = label + '\n%.5g' % self.compute_rate()
            label = r'\n'.join(line.strip() for line in label.splitlines())
            label = '"{}"'.format(label)
        params = dict(self.parameters)  # Copy by value
        params['label'] = label
        params = ('{}={}'.format(name, value) for name, value in params.items())
        return '{}[{}];'.format(self.make_name(), ','.join(params))

    def graphviz_edges(self):
        return '\n'.join('{} -> {};'.format(child.make_name(), self.make_name())
                         for child in self.children)


class Failure(Node):
    def __init__(self, label, failure_rate=nan, **parameters):
        super().__init__(label=label, **parameters)
        self.failure_rate = failure_rate
        self.parameters['shape'] = 'box'

    def as_leaf(self):
        # == In case you want to see all the 'diamonds' resolved by this:
        #return self

        # == Just create a "reference" box:
        params = dict(self.parameters)   # Copy by value
        params['label'] = params['label'] + '\n(reference)'
        # Sadly, it's not possible to create a bold+dashed box,
        # so we don't handle that case.
        params['style'] = 'dashed'
        return Failure(**params)

    def compute_rate(self):
        assert len(self.children) <= 1
        rate = self.failure_rate
        if rate is nan and self.children:
            assert len(self.children) == 1
            rate = self.children[0].compute_rate()
        return rate

    def __lshift__(self, other):
        other.parent = self
        self.children.append(other)

    def __rshift__(self, other):
        return other << self

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        return False

    def __or__(self, other):
        if isinstance(other, OR):
            other.children.append(self)
            return other
        return OR(self, other)

    def __and__(self, other):
        if isinstance(other, AND):
            other.children.append(self)
            return other
        return AND(self, other)

    __ror__ = __or__
    __rand__ = __and__


class Toplevel(Failure):
    def __init__(self, label, **parameters):
        super().__init__(label=label, **parameters)
        self.parameters['style'] = 'bold'


class Primary(Failure):
    def __init__(self, label, **parameters):
        super().__init__(label=label, **parameters)
        self.parameters['shape'] = 'ellipse'


class Secondary(Failure):
    def __init__(self, label, **parameters):
        super().__init__(label=label, **parameters)
        self.parameters['shape'] = 'diamond'


class Gate(Node):
    def __init__(self, label, *children):
        super().__init__(None, *children, label=label)
        self.parameters['shape'] = 'box'

    def __or__(self, other):
        if isinstance(other, Failure) and isinstance(self, OR):
            self.children.append(other)
            return self
        return OR(self, other)

    def __and__(self, other):
        if isinstance(other, Failure) and isinstance(self, AND):
            self.children.append(other)
            return self
        return AND(self, other)


class AND(Gate):
    def __init__(self, *children):
        assert len(children) >= 2
        super().__init__('AND', *children)

    def compute_rate(self):
        from functools import reduce
        from operator import mul
        return 1 - reduce(mul, [1 - c.compute_rate() for c in self.children], 1)


class OR(Gate):
    def __init__(self, *children):
        assert len(children) >= 2
        super().__init__('OR', *children)

    def compute_rate(self):
        rate = sum([c.compute_rate() for c in self.children])
        if rate > 1:
            # Have fun trying to fomrat this source code
            print('WARNING: failure rate of {} is greater than one: {}'.
                  format(self.parameters['label'], rate))
        return rate


def traverse(node):
    queue = collections.deque([node])
    while queue:
        current = queue.popleft()
        yield current
        queue.extend(current.children)


def graphviz(*nodes):
    code = ['digraph faulttree {', 'rankdir=BT;']
    all_nodes = set()
    for root in nodes:
        for node in traverse(root):
            all_nodes.add(node)
    for node in all_nodes:
        code.append(node.graphviz_node())
    for node in all_nodes:
        code.append(node.graphviz_edges())
    code.append('}')
    return '\n'.join(code)


def generate(*nodes, filename='fault-tree.eps', create_gv=False):
    dot = subprocess.Popen(['dot', '-Tps', '-o' + filename], stdin=subprocess.PIPE)
    rawstring = graphviz(*nodes)
    if create_gv:
        with open(filename + '.dot', 'w') as text_file:
            text_file.write(rawstring)
    dot.communicate(rawstring.encode('utf-8'))
    exitcode = dot.wait()
    if exitcode != 0:
        tofile = filename + '.dot'
        print('generating {} failed.  Wrote dotcode to {} ...'.format(filename, tofile))
        with open(tofile, 'w') as fp:
            fp.write(rawstring)
    assert exitcode == 0
