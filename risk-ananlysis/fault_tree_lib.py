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

from functools import reduce
from operator import mul
from math import nan, expm1


trees = set()
nodes = {}
FUN_FACTS = False


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
    cached_fail_prob = None

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
        self.display_rate = True
        assert 'failure_rate' not in self.parameters

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
            if self.display_rate:
                # Q: Why '%g'?
                # A: Properly display very small rates
                # >>> '%.9f' % (1e-12/9)
                # '0.000000000'
                # >>> '%.9g' % (1e-12/9)
                # '1.11111111e-13'
                label = label + '\n%.5g' % self.get_fail_prob()
            label = r'\n'.join(line.strip() for line in label.splitlines())
            label = '"{}"'.format(label)
        params = dict(self.parameters)  # Copy by value
        params['label'] = label
        params = ('{}={}'.format(name, value) for name, value in params.items())
        return '{}[{}];'.format(self.make_name(), ','.join(params))

    def get_fail_prob(self):
        self.cached_fail_prob = self.cached_fail_prob or self.compute_fail_prob()
        return self.cached_fail_prob

    def graphviz_edges(self):
        return '\n'.join('{} -> {};'.format(child.make_name(), self.make_name())
                         for child in self.children)


class Reference(Node):
    def __init__(self, orig):
        super().__init__(**orig.parameters)
        self.orig = orig
        self.parameters['shape'] = 'box'
        self.parameters['label'] = self.parameters['label'] + '\n(reference)'
        # Sadly, it's not possible to create a bold+dashed box,
        # so we don't handle that case.
        self.parameters['style'] = 'dashed'

    def compute_fail_prob(self):
        return self.orig.get_fail_prob()


class Failure(Node):
    def __init__(self, label, **parameters):
        super().__init__(label=label, **parameters)
        self.parameters['shape'] = 'box'

    def as_leaf(self):
        # == In case you want to see all the 'diamonds' resolved by this:
        # return self

        # == Create a "reference" box:
        return Reference(self)

    def compute_fail_prob(self):
        if len(self.children) != 1:
            print('Warning: Failure has no children: "%s"' %
                  self.parameters['label'])
            return nan
        return self.children[0].get_fail_prob()

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
    def __init__(self, label, failure_rate=nan, **parameters):
        super().__init__(label=label, **parameters)
        self.failure_rate = failure_rate
        self.parameters['shape'] = 'ellipse'
        if failure_rate is nan:
            print('Warning: failure rate not defined for primary fault "%s"' %
                  self.parameters['label'])

    def compute_fail_prob(self):
        if self.failure_rate is nan:
            return nan
        # F(t) = 1 - e^{-\lambda t}
        # F(1 h) = 1 - e^{-\lambda}
        return abs(expm1(-self.failure_rate))


class Secondary(Failure):
    def __init__(self, label, **parameters):
        super().__init__(label=label, **parameters)
        self.display_rate = False
        self.parameters['shape'] = 'diamond'

    def compute_fail_prob(self):
        return 0


class Gate(Node):
    def __init__(self, label, *children):
        super().__init__(None, *children, label=label)
        self.display_rate = False
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

    def compute_fail_prob(self):
        fail_probs = [c.get_fail_prob() for c in self.children]
        return reduce(mul, fail_probs, 1)


class OR(Gate):
    def __init__(self, *children):
        assert len(children) >= 2
        super().__init__('OR', *children)

    def compute_fail_prob(self):
        succ_probs = [1 - c.get_fail_prob() for c in self.children]
        return 1 - reduce(mul, succ_probs, 1)


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
        print('generating {} failed.  Wrote dotcode to {} ...'
              .format(filename, tofile))
        with open(tofile, 'w') as fp:
            fp.write(rawstring)
    assert exitcode == 0
    if FUN_FACTS:
        for n in nodes:
            print('Fun fact: overall failure rate of {} for "{}"'
                  .format(n.cached_rate(),
                          n.parameters['label'].replace('\n', ' ')))
