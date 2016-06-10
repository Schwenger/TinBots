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

from common import software_bug

import hw
import proto

# TODO: add references to specification document


class Proximity:
    failure = F('primary proximity\nsensor fault')

    sparse_walls = F('walls are\nextremely sparse')

    software = F('software failure\n(overzealous escort-ignoring)')

    false_negative = F('obstacle\nnot detected')
    false_negative << (failure | sparse_walls | software)

    avoid_sys = F('avoidance watchdog fails')

    # FIXME: use driver independent collision avoidance => "standing still" if all drivers continue failing
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
    recognition << (rec_sw | proto.SOS.failure)

    magnet_trigger_acc = F('magnets unintentionally trigger')

    unintentional = F('picking up the victim\nwas accidental')
    unintentional << (Proximity.false_negative & recognition & magnet_trigger_acc)


# FIXME: this is a OR-case of nearly everything (not only of bump)
bad_firmware = S('bad firmware or\nwrong program uploaded')

bump = T('run into walls')
bump << (Proximity.collision | bad_firmware)

ignore_victim = T('not using information\nabout victim')


# Fault-Trees

class EscortNoLED(Tree):
    led = P('primary indicator LED failure (MR9)')

    with F('not aware of escorting') as not_aware:
        memory_fault = P('memory fault\n(forgot what happened)')
        not_aware << (memory_fault | Escort.unintentional)

    failure = T('escorting,\nbut no LED')
    failure << (led | not_aware | software_bug())


class StandingStill(Tree):
    with F('no initial position from LPS') as no_initial_lps:
        user = S('user forgot to\nenable LPS')
        no_initial_lps << (proto.LPS.failure | user)

    with F('wheels do not turn') as wheel_fault:
        blocked = S('wheels blocked')
        wheel_fault << (hw.Motor.failure | blocked)

    bad_setup = S('bad setup')
    software = software_bug()

    failure = T('standing still')
    failure << (no_initial_lps | bad_setup | software | wheel_fault | hw.EPuck.failure)


class Uncooperative(Tree):
    with F('sender failure') as sender:
        sender << (software_bug() | hw.Bluetooth.sender)

    with F('receiver failure') as receiver:
        receiver << (software_bug() | hw.Bluetooth.receiver)

    failure = T('uncooperative behavior\n(visit cells twice, ...)')
    failure << (sender | receiver | hw.Bluetooth.medium)


class VictimLost(Tree):
    with F('victim dropped / unable to grab') as dropped:
        magnet_weak = P('primary magnet failure\n(e.g., too weak)')

        belt_weak = P('primary belt failure\n(magnet slips from victim)')
        # Your magnet is weak, your belt is weak, your bloodline is
        # weak, and you will *not* survive winter!

        dropped << (belt_weak | magnet_weak)

    pulled_away = F('pulled away by\nanother Tin Bot')
    pulled_away << (Escort.unintentional | Uncooperative.failure)

    failure = T('victim lost while escorting')
    failure << (pulled_away | dropped)


class SpuriousMovements(Tree):
    failure = T('spurious movements\n(e.g., spin around, drive circles, ...)')
    failure << (proto.LPS.failure | software_bug() | Proximity.failure | Uncooperative.failure)


# FIXME: merge this with SpuriousMovements?
class GOWrong(Tree):
    with F('design error') as soft:
        spec = F('misunderstanding\nabout MR14')
        # TODO: what does that mean? no code review by others?
        check = F('no double checking')

        soft << (spec & check)

    failure = T(r'moving to the \"gathered position\"\ninstead \"towards the victim\"')
    failure << (ignore_victim | SpuriousMovements.failure | soft)


class NoPowerLED(Tree):
    primary = P('primary power LED failure')

    failure = T('power LED is off')
    failure << (primary | hw.EPuck.failure)


class VictimSilent(Tree):
    software = software_bug()

    failure = T('victim\'s LED does not\nsend valid signal')
    failure << (software | proto.SOS.sender)


class SeeNoLed(Tree):
    primary = F('primary failure\nof the display-LED')
    software = software_bug()

    failure = T('clear line of sight,\nbut no LED indication')
    failure << (VictimSilent.failure | proto.SOS.receiver | primary | software)


class Victim404(Tree):
    not_placed_in = S('user did not place\nthe victim in the maze')
    unsolvable = S('unsolvable maze')

    failure = T('victim cannot be found')
    failure << (VictimSilent.failure | proto.SOS.receiver)


no_escort = T('not moving the victim out;\nat least not on shortest path')


class SystemFailure(Tree):
    tin_bot_failure = F('Tin Bot failure\n')
    tin_bot_failure << (VictimLost.failure | StandingStill.failure | ignore_victim | SpuriousMovements.failure |
                        GOWrong.failure | no_escort)

    # FIXME: how to model redundancy in fault trees — do wee need 2 copies of the Tin Bot tree?
    failure = T('system failure\n(victim remains in maze)')
    failure << ((tin_bot_failure & tin_bot_failure) | VictimSilent.failure)


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for tree in get_trees():
        if isinstance(tree.failure, T):
            generate(tree.failure, filename=tree.__name__.lower() + '.eps')
