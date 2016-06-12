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
import subprocess


trees = set()


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
    shape = None
    label = 'None'
    parameters = {}

    def __init__(self, parent=None, *children, label=None, shape=None, **parameters):
        self.label = label or self.label
        self.shape = shape or self.shape
        self.parent = parent
        self.children = list(children)
        self.parameters = dict(parameters)

    def __call__(self):
        return copy.deepcopy(self)

    def make_name(self):
        # Can't statically compute the name, because it wouldn't be
        # re-computed for a deep-copied node.
        return 'node{}'.format(id(self))

    def graphviz_node(self):
        label = r'\n'.join(line.strip() for line in self.label.splitlines())
        self.parameters['label'] = '"{}"'.format(label)
        if self.shape is not None:
            self.parameters['shape'] = self.shape
        params = ('{}={}'.format(name, value) for name, value in self.parameters.items())
        return '{}[{}];'.format(self.make_name(), ','.join(params))

    def graphviz_edges(self):
        return '\n'.join('{} -> {};'.format(child.make_name(), self.make_name())
                         for child in self.children)

    def as_leaf(self):
        # Or, in case you want to see all the 'diamonds' resolved by this:
        #return self
        return Node(label=self.label + '\n(reference)', shape=self.shape,
                    style='dashed', **self.parameters)


class Failure(Node):
    shape = 'box'

    def __init__(self, description=None, probability=None, **parameters):
        super().__init__(label=description, **parameters)
        self.description = description
        self.probability = probability

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
        return OR(None, self, other)

    def __and__(self, other):
        if isinstance(other, AND):
            other.children.append(self)
            return other
        return AND(None, self, other)

    __ror__ = __or__
    __rand__ = __and__


class Toplevel(Failure):
    parameters = {
        'style': 'bold'
    }


class Primary(Failure):
    shape = 'ellipse'


class Secondary(Failure):
    shape = 'diamond'


class Gate(Node):
    shape = 'box'

    def __or__(self, other):
        if isinstance(other, Failure) and isinstance(self, OR):
            self.children.append(other)
            return self
        return OR(None, self, other)

    def __and__(self, other):
        if isinstance(other, Failure) and isinstance(self, AND):
            self.children.append(other)
            return self
        return AND(None, self, other)


class AND(Gate):
    label = 'AND'


class OR(Gate):
    label = 'OR'


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


def generate(*nodes, filename='fault-tree.eps', create_gv=True):
    dot = subprocess.Popen(['dot', '-Tps', '-o' + filename], stdin=subprocess.PIPE)
    rawstring = graphviz(*nodes)
    if create_gv:
        with open(filename + '.dot', 'w') as text_file:
            text_file.write(rawstring)
    dot.communicate(rawstring.encode('utf-8'))
    exitcode = dot.wait()
    assert exitcode == 0
