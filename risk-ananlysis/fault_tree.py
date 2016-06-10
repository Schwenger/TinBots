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

from fault_tree_lib import Tree, Failure as F, Toplevel as T, Primary as P, Secondary as S

import hw


def software_bug():
    return P('software failure (bug)')


# Communication
class CBP(Tree):
    medium = P('primary environment failure\n(not enough light, no clear line of sight, ...)')
    sender = S('missing color blob')
    receiver = hw.Camera.failure

    failure = F('color blob protocol failure')
    failure << (medium | sender | receiver)


class LPS(Tree):
    medium = hw.Bluetooth.medium
    sender = hw.Raspberry.failure
    receiver = hw.Bluetooth.receiver

    with F('LPS link down') as link_down:
        service_outage = P('primary LPS service outage (NR1)')

        # Raspberry board failure includes Bluetooth sender failure
        link_down << (service_outage | medium | sender | receiver)

    with F('LPS sends no data') as no_data:
        no_data << (CBP.failure | software_bug())

    failure = F('LPS failure')
    failure << (link_down | no_data)


class SOS(Tree):
    medium = P('primary medium failure\n(interference, ...)')

    sender = hw.Victim.failure
    receiver = hw.ExtBoard.failure

    failure = F('SOS failure')
    failure << (medium | sender | receiver)  # includes WAU



class Proximity:
    failure = F('primary proximity\nsensor fault')

    sparse_walls = F('walls are\nextremely sparse')

    software = F('software failure\n(overzealous escort-ignoring)')

    false_negative = F('obstacle\nnot detected')
    false_negative << (failure | sparse_walls | software)

    avoid_sys = F('avoidance watchdog fails')

    # FIXME: use driver independent collision avoidance
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
    recognition << (rec_sw | SOS.failure)

    magnet_trigger_acc = F('magnets unintentionally trigger')

    unintentional = F('picking up the victim\nwas accidental')
    unintentional << (Proximity.false_negative & recognition & magnet_trigger_acc)





bad_firmware = S('bad firmware or\nwrong program uploaded')


bump = T('run into walls')
bump << (Proximity.collision | bad_firmware)


class EscortNoLED(Tree):
    escort_led_failure = P('primary indicator LED failure')
    forgot_escort_led = F('not implemented\n(forgotten)')

    with F('Tin Bot is not\naware of escorting') as not_escorting:
        memory_fault = F('forgot what happened\n(primary memory fault)')
        not_escorting << (memory_fault | Escort.unintentional)

    failure = T('escorting,\nbut no LED')
    failure << (escort_led_failure | not_escorting | forgot_escort_led)


class StandingStill(Tree):
    with F('no initial position from LPS') as no_initial_lps:
        no_initial_lps << LPS.failure

    with F('software initialization failure') as software_init:
        bug = F('software fault (bug)')
        setup = S('bad setup\n(secondary failure)')

        software_init << (bug | setup)

    with F('wheels unable to turn') as wheel_fault:
        blocked = S('wheels blocked\n(secondary failure)')

        wheel_fault << (hw.Motor.failure | blocked)

    software = P('primary software failure') # bug

    failure = T('standing still')
    failure << (no_initial_lps | software_init | software | wheel_fault | hw.EPuck.failure)


class Uncooperative(Tree):
    with F('sender failure') as sender:
        sender << (software_bug() | hw.Bluetooth.sender)

    with F('receiver failure') as receiver:
        receiver << (software_bug() | hw.Bluetooth.receiver)

    failure = T('uncooperative behavior\n(visit cells twice, ...)')
    failure << (sender | receiver | hw.Bluetooth.medium)


class VictimLost(Tree):
    magnet_weak = F('primary magnet failure\n(e.g., too weak)')
    belt_weak = F('primary belt failure\n(magnet slips from victim)')
        # Your magnet is weak, your belt is weak, your bloodline is
        # weak, and you will *not* survive winter!
    victim_dropped = F('victim dropped')
    victim_dropped << (belt_weak | magnet_weak)

    pulled_away = F('pulled away by\nother Tin Bot')
    pulled_away << (Escort.unintentional & Uncooperative.failure)

    failure = T('victim lost while escorting')
    failure << (pulled_away | victim_dropped)


ignore_victim = T('not using information\nabout victim')


class Spin(Tree):
    spin_design = F('design error\n(i.e., unhandled edge case)')

    spin_sw = F('confused software')
    spin_sw << (Uncooperative.failure | P('primary sensor fault'))

    failure = T('turning around forever')
    failure << (hw.Motor.failure | spin_design | spin_sw)


class Jerk(Tree):
    failure = T('spurious/unreasonable\nmovements (LPS)')
    failure << LPS.failure


class GOWrong(Tree):
    with F('design error') as soft:
        spec = F('misunderstanding\nabout MR14')
        check = F('no double checking')

        soft << (spec & check)

    failure = T(r'moving to the \"gathered position\"\ninstead \"towards the victim\"')
    failure << (ignore_victim | Jerk.failure | soft | Uncooperative.failure)


class NoPowerLED(Tree):
    primary = P('primary power LED failure')
    forgot_power_led = F('not implemented\n(forgotten)')
    not_turned_on = S('secondary failure\nuser did not turn\non the E-Puck')

    failure = T('no power LED')
    failure << (primary | not_turned_on | hw.Battery.failure | forgot_power_led)


class VictimSilent(Tree):
    not_turned_on = S('secondary failure,\nuser did not turn\non the E-Puck')
    ir_led_defect = P('primary IR LED failure')
    software = F('victim software failure')

    failure = T('victim\'s LED does not\nsend valid signal')
    failure << (not_turned_on | ir_led_defect | software | hw.Victim.failure)


class SeeNoLed(Tree):
    forgot_ir_recv_led = F('not implemented\n(forgotten)')
    ir_recv_led_defect = F('primary failure\nof the display-LED')

    failure = T('clear line of sight,\nbut no LED')
    failure << (VictimSilent.failure | ir_recv_led_defect | SOS.failure | forgot_ir_recv_led)


victim404 = T('victim cannot be found')


no_escort = T('not moving the victim out;\nat least not on shortest path')

"""
# TODO: whole system outage <= all Tin Bots became defunct or victim failure
with T('system failure\n(victim remains in maze)') as system_failure:
    tin_bot_failure = F('Tin Bot failure\n')
    tin_bot_failure << (victim_lost | standing_still | ignore_victim | spin | jerk | go_wrong | no_escort)

    # FIXME: how to model redundancy in fault trees — do wee need 2 copies of the Tin Bot tree?
    system_failure << ((tin_bot_failure & tin_bot_failure) | victim_silent)
"""


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for tree in get_trees():
        if isinstance(tree.failure, T):
            generate(tree.failure, filename=tree.__name__.lower() + '.eps')
