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


Generate EPS file containing all nodes:
    python3 fault_tree.py

Generate EPS file containing specific nodes:
    python3 fault_tree.py node_name1 node_name2 …
"""

from fault_tree_lib import Failure, Toplevel, Primary, Secondary, graphviz

F = Failure
P = Primary
S = Secondary
T = Toplevel

bump = T('run into walls')

escorting_recognition = F('escort recognition fails')


class LPS:
    link_down = F('LPS link down')
    no_data = F('LPS sends no data')

    failure = F('LPS failure')
    failure << (link_down | no_data)


class Power:
    battery_defect = P(r'primary battery failure\nbattery defect')
    battery_not_charged = S(r'secondary battery failure\nbattery not charged or\nbattery not connected')
    wiring = F(r'failure in\nwiring or fuse')

    failure = F('power failure')
    failure << (battery_defect | battery_not_charged | wiring)


with T(r'escorting,\nbut no led') as escort_no_led:
    escort_no_led << P(r'primary indicator LED failure')
    escort_no_led << F(r'not implemented\n(forgotten)')

    with F(r'Tin Bot is not\naware of escorting') as not_escorting:
        memory_fault = F(r'forgot what happened\n(primary memory fault)')

        with Failure(r'picking up the victim\nwas accidental') as unintentional_escort:
            run_victim_over = F(r'run into \"victim-obstacle\"')
            run_victim_over << bump

            magnet_trigger_acc = Failure(r'magnets accidentally trigger')

            unintentional_escort << (run_victim_over & escorting_recognition & magnet_trigger_acc)

        not_escorting << (memory_fault | unintentional_escort)

    escort_no_led << not_escorting


see_no_led = T(r'clear line of sight,\nbut no LED')


victim_lost = T(r'victim lost while escorting')


with T(r'standing still') as standing_still:
    with F(r'no initial position from LPS') as no_initial_lps:
        no_initial_lps << LPS.failure

    with Failure(r'software initialization failure') as software_init:
        bug = F(r'software fault (bug)')
        setup = S(r'bad setup\n(secondary failure)')

        software_init << (bug | setup)

    with Failure(r'wheels unable to turn') as wheel_fault:
        motor = P(r'primary motor fault')
        blocked = S(r'wheels blocked\n(secondary failure)')

        wheel_fault << (motor | blocked)

    standing_still << (no_initial_lps | software_init | wheel_fault | Power.failure)


with T(r'uncooperative behavior (T2T)') as uncooperative:
    with F('communication failure') as communication:
        bluetooth = P('bluetooth sender/receiver failure')
        medium = P('medium failure (noise, inference, ...)')

        communication << (bluetooth | medium)

    software = F('software failure')

    uncooperative << (communication | software)


ignore_victim = T(r'not using information\nabout victim')


spin = T(r'turning around forever')


jerk = T(r'spurious/unreasonable\nmovements (LPS)')


with T(r'moving to the \"gathered position\"\ninstead \"towards the victim\"') as go_wrong:
    with F('design error') as soft:
        spec = F(r'misunderstanding\nabout MR14')
        check = F(r'no double checking')

        soft << (spec & check)

    go_wrong << (ignore_victim | jerk | soft | uncooperative)


with T(r'no power LED') as no_power_led:
    primary = P(r'primary power LED failure')
    not_turned_on = S(r'secondary failure\nuser did not turn\non the E-Puck')

    no_power_led << (primary | not_turned_on | Power.failure)


with T(r'victim\'s LED does not\nsend valid signal') as victim_silent:
    not_turned_on = S(r'secondary failure,\nuser did not turn\non the E-Puck')
    ir_led_defect = P(r'primary IR LED failure')
    software = F(r'victim software failure')

    victim_silent << (not_turned_on | ir_led_defect | software | Power.failure)


victim404 = T(r'victim cannot be found')


no_escort = T(r'not moving the victim out;\nat least not on shortest path')


import subprocess
import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        nodes = (eval(node_name) for node_name in sys.argv[1:])
    else:
        nodes = (escort_no_led, see_no_led, victim_lost, standing_still, uncooperative, ignore_victim, spin,
                 jerk, bump, go_wrong, no_power_led, victim_silent, victim404, no_escort)

    dot = subprocess.Popen(['dot', '-Tps', '-ofault-tree.eps'], stdin=subprocess.PIPE)
    dot.communicate(graphviz(*nodes).encode('utf-8'))
    dot.wait()
