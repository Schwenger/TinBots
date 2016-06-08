#!/usr/bin/env python3
# -*- coding:utf-8 -*-

"""
Usage
=====

Failure Blöcke lassen sich mit Failure, Toplevel, Primary und Secondary erzeugen.

Syntax:
    Failure(description, [probability])

Mit "|" erzeugt man eine OR-Verzweigung, mit "&" eine AND-Verzweigung. Beide Operatoren
Klammern links und erlauben beliebig viele Blöcke aneinander zu hängen.

Syntax:
    a | b | c | … (erzeugt eine OR-Verzweigung)
    a & b & c & … (erzeugt eine AND-Verzweigung)

Kanten zwischen den Blöcken werden mit "<<" und ">>" erzeugt.

Mit `with` können Einrückungen realisiert werden.

Ansonsten geht alles, was mit Python auch geht. ;)
"""

from fault_tree_lib import Failure, Toplevel, Primary, Secondary, graphviz


escort_no_led = Toplevel(r'escorting,\nbut no led')


see_no_led = Toplevel(r'clear line of sight,\nbut no LED')


victim_lost = Toplevel(r'victim lost while escorting')


with Toplevel(r'standing still') as standing_still:
    no_initial_lps = Failure(r'no initial position from LPS')
    software_init = Failure(r'software initialization failure')

    standing_still << (no_initial_lps | software_init)


print(graphviz(escort_no_led, see_no_led, victim_lost, standing_still))
