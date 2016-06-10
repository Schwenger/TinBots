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


class Bluetooth:
    sender = P('bluetooth sender failure')
    receiver = P('bluetooth receiver failure')

    medium = P('medium failure (noise, interference, ...)')

    failure = F('bluetooth communication failure')
    failure << (sender | receiver | medium)


class IR_perception:
    ir_recv_defect = F('primary IR receiver fault')

    # Other reasons go here

    failure = F(r"Can't perceive victim")
    failure << (ir_recv_defect | F('FIXME: WAU/SOS, interference'))


class LPS:
    with F('LPS link down') as link_down:
        service_outage = P('primary LPS service outage (NR1)')

        link_down << (service_outage | Bluetooth.failure)

    no_data = F('LPS sends no data')
    no_data << F('FIXME: LPS, CBP')

    failure = F('LPS failure')
    failure << (link_down | no_data)


class Power:
    battery_defect = P('primary battery failure\nbattery defect')
    battery_not_charged = S('secondary battery failure\nbattery not charged or\nbattery not connected')

    # FIXME: do we have a fuse? (at least there is no fuse planned for the victim)
    wiring = F('failure in\nwiring or fuse')

    failure = F('power failure')
    failure << (battery_defect | battery_not_charged | wiring)


class Proximity:
    failure = F('primary proximity\nsensor fault')

    sparse_walls = F('walls are\nextremely sparse')

    software = F('software failure\n(overzealous escort-ignoring)')

    false_negative = F('obstacle\nnot detected')
    false_negative << (failure | sparse_walls | software)

    avoid_sys = F('avoidance watchdog fails')

    rhr_collide = F('right-hand-rule\ncan collide')
    path_collide = F('path finder/executor\ncan collide')
    any_collide = F('some driver does\nnot prevent collision')
    any_collide << (rhr_collide | path_collide)

    coll_sw = F('software failure')
    coll_sw << (avoid_sys & any_collide)

    collision = F('collision with obstacle')
    collision << (coll_sw | false_negative)


class Escort:
    rec_sw = F('recognition method fails')

    recognition = F('escort recognition fails')
    recognition << (rec_sw | IR_perception.failure)

    magnet_trigger_acc = F('magnets unintentionally trigger')

    unintentional = F('picking up the victim\nwas accidental')
    unintentional << (Proximity.false_negative & recognition & magnet_trigger_acc)


class Motor:
    failure = F('primary motor fault')


bad_firmware = S('bad firmware or\nwrong program uploaded')


bump = T('run into walls')
bump << (Proximity.collision | bad_firmware)


with T('escorting,\nbut no LED') as escort_no_led:
    escort_led_failure = P('primary indicator LED failure')
    forgot_escort_led = F('not implemented\n(forgotten)')

    with F('Tin Bot is not\naware of escorting') as not_escorting:
        memory_fault = F('forgot what happened\n(primary memory fault)')
        not_escorting << (memory_fault | Escort.unintentional)

    escort_no_led << (escort_led_failure | not_escorting | forgot_escort_led)


with T('standing still') as standing_still:
    with F('no initial position from LPS') as no_initial_lps:
        no_initial_lps << LPS.failure

    with Failure('software initialization failure') as software_init:
        bug = F('software fault (bug)')
        setup = S('bad setup\n(secondary failure)')

        software_init << (bug | setup)

    with Failure('wheels unable to turn') as wheel_fault:
        blocked = S('wheels blocked\n(secondary failure)')

        wheel_fault << (Motor.failure | blocked)

    software = P('primary software failure') # bug

    standing_still << (no_initial_lps | software_init | software | wheel_fault | Power.failure)


with T('uncooperative behavior (T2T)') as uncooperative:
    with F('communication failure') as communication:
        bluetooth = P('bluetooth sender/receiver failure')
        medium = P('medium failure (noise, interference, ...)')

        communication << (bluetooth | medium)

    # FIXME: algorithmic?
    software = F('software failure\n(algorithmic)')

    uncooperative << (communication | software | bad_firmware)


with T('victim lost while escorting') as victim_lost:
    magnet_weak = F('primary magnet failure\n(e.g., too weak)')
    belt_weak = F('primary belt failure\n(magnet slips from victim)')
        # Your magnet is weak, your belt is weak, your bloodline is
        # weak, and you will *not* survive winter!
    victim_dropped = F('victim dropped')
    victim_dropped << (belt_weak | magnet_weak)

    pulled_away = F('pulled away by\nother Tin Bot')
    pulled_away << (Escort.unintentional & uncooperative)
    victim_lost << (pulled_away | victim_dropped)


ignore_victim = T('not using information\nabout victim')


with T('turning around forever') as spin:
    spin_design = F('design error\n(i.e., unhandled edge case)')

    spin_sw = F('confused software')
    spin_sw << (uncooperative | P('primary sensor fault'))

    spin << (Motor.failure | spin_design | spin_sw)


with T('spurious/unreasonable\nmovements (LPS)') as jerk:
    jerk << LPS.failure


with T(r'moving to the \"gathered position\"\ninstead \"towards the victim\"') as go_wrong:
    with F('design erro') as soft:
        spec = F('misunderstanding\nabout MR14')
        check = F('no double checking')

        soft << (spec & check)

    go_wrong << (ignore_victim | jerk | soft | uncooperative)


with T('no power LED') as no_power_led:
    primary = P('primary power LED failure')
    forgot_power_led = F('not implemented\n(forgotten)')
    not_turned_on = S('secondary failure\nuser did not turn\non the E-Puck')

    no_power_led << (primary | not_turned_on | Power.failure | forgot_power_led)


with T('victim\'s LED does not\nsend valid signal') as victim_silent:
    not_turned_on = S('secondary failure,\nuser did not turn\non the E-Puck')
    ir_led_defect = P('primary IR LED failure')
    software = F('victim software failure')

    victim_silent << (not_turned_on | ir_led_defect | software | Power.failure)


with T('clear line of sight,\nbut no LED') as see_no_led:
    forgot_ir_recv_led = F('not implemented\n(forgotten)')
    ir_recv_led_defect = F('primary failure\nof the display-LED')

    see_no_led << (victim_silent | ir_recv_led_defect | IR_perception.failure | forgot_ir_recv_led)


victim404 = T('victim cannot be found')


no_escort = T('not moving the victim out;\nat least not on shortest path')


# TODO: whole system outage <= all Tin Bots became defunct or victim failure
with T('system failure\n(victim remains in maze)') as system_failure:
    tin_bot_failure = F('Tin Bot failure\n')
    tin_bot_failure << (victim_lost | standing_still | ignore_victim | spin | jerk | go_wrong | no_escort)

    # FIXME: how to model redundancy in fault trees — do wee need 2 copies of the Tin Bot tree?
    system_failure << ((tin_bot_failure & tin_bot_failure) | victim_silent)


import subprocess
import sys

if __name__ == '__main__':
    if len(sys.argv) > 1:
        nodes = (eval(node_name) for node_name in sys.argv[1:])
    else:
        nodes = (escort_no_led, see_no_led, victim_lost, standing_still, uncooperative, ignore_victim, spin,
                 jerk, bump, go_wrong, no_power_led, victim_silent, victim404, no_escort, system_failure)

    dot = subprocess.Popen(['dot', '-Tps', '-ofault-tree.eps'], stdin=subprocess.PIPE)
    dot.communicate(graphviz(*nodes).encode('utf-8'))
    dot.wait()
