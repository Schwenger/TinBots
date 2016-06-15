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
    failure = P('primary proximity\nsensor fault', failure_rate=1e-1)

    sparse_walls = S('walls are\noutside specification')

    software = P('software failure\n(overzealous escort-ignoring)', failure_rate=1e-3)

    false_negative = F('obstacle\nnot detected')
    false_negative << (failure | sparse_walls | software)

    avoid_sys = F('avoidance watchdog fails', failure_rate=0)
    avoid_sys << (software_bug | failure)

    collision = F('collision with obstacle')
    collision << (avoid_sys | false_negative)


class Escort:
    rec_sw = P('recognition method fails', 1e-3)

    recognition = F('escort recognition fails')
    recognition << (rec_sw | proto.SOS.failure)

    magnet_trigger_acc = P('magnets unintentionally trigger')

    hang = F('picked up victim\nthrough the wall\n(paper does not shield magnetic fields)')
    hang << (magnet_trigger_acc & Proximity.sparse_walls())

    open_space = F('picked up victim\ndirectly')
    open_space << (Proximity.false_negative & recognition & magnet_trigger_acc())

    unintentional = F('picking up the victim\nwas accidental')
    # hang is best instantiated for "standing still"
    unintentional << (hang.as_leaf() | open_space)


# FIXME: this is in the OR-case of nearly everything (not only of RunIntoWall)
bad_firmware = S('bad firmware or\nwrong program uploaded')


# Fault-Trees

class RunIntoWall(Tree):
    failure = T('run into walls')
    failure << (Proximity.collision | bad_firmware)


class EscortNoLED(Tree):
    led = P('primary indicator LED failure', failure_rate=1e-5)

    with F('not aware of escorting') as not_aware:
        not_aware << (hw.EPuck.memory_fault() | Escort.unintentional)

    failure = T('escorting,\nbut no LED')
    failure << (led | not_aware | software_bug())


class StandingStill(Tree):
    with F('no initial position from LPS') as no_initial_lps:
        user = S('user did not\nenable LPS')
        no_initial_lps << (proto.LPS.failure | user)

    with F('wheels do not turn') as wheel_fault:
        blocked = S('wheels blocked')
        wheel_fault << (hw.Motor.failure | blocked)

    bad_setup = S('bad setup')
    software = software_bug()

    failure = T('standing still')
    failure << (no_initial_lps | bad_setup | software | wheel_fault
                | hw.EPuck.failure | RunIntoWall.failure | Escort.hang)


class Uncooperative(Tree):
    with F('sender failure') as sender:
        sender << (software_bug() | hw.Bluetooth.sender())

    with F('receiver failure') as receiver:
        receiver << (software_bug() | hw.Bluetooth.receiver())

    design = P('protocol design failure (T2T)')

    failure = T('uncooperative behavior\n(explores cells twice, ...)')
    failure << (sender | receiver | hw.Bluetooth.medium() | design)


class IgnoreVictim(Tree):
    conflict = F('conflicting data\nabout victim')
    conflict << (hw.EPuck.memory_fault() | S('user moved\nvictim') |
                 Uncooperative.failure.as_leaf())

    late = P('too high min-distance\nfor sensing again')

    no_triang = F('triangulation fails')
    no_triang << (late | software_bug())

    overwrite = P('information gets overwritten\nbefore E-Puck picks it up\n(software bug)')

    failure = T('not using information\nabout victim')
    failure << (hw.ExtBoard.failure.as_leaf() | conflict | no_triang |
                overwrite)


class VictimLost(Tree):
    with F('victim dropped / unable to grab') as dropped:
        magnet_weak = P('primary magnet failure\n(e.g., too weak)', 1e-1)

        belt_weak = P('primary belt failure\n(magnet slips out of the belt)', 1e-1)
        # Your magnet is weak, your belt is weak, your bloodline is
        # weak, and you will *not* survive winter!

        dropped << (belt_weak | magnet_weak)

    pulled_away = F('pulled away by\nanother Tin Bot')
    pulled_away << (Escort.unintentional | Uncooperative.failure.as_leaf())

    failure = T('victim lost while escorting')
    failure << (pulled_away | dropped)


class VictimSilent(Tree):
    software = software_bug()

    failure = T('victim\'s LED does not\nsend valid signal')
    failure << (software | hw.Victim.failure)


class SpuriousMovements(Tree):
    turn = F('does not\nstop turning\nwhile sensing\nangle to victim')
    turn << (VictimSilent.failure.as_leaf() | hw.ExtBoard.failure)

    badcolor = S("Tin Bot's physical color\nis not hardcoded color")

    failure = T('spurious movements\n(e.g., spin around, drive circles, ...)')
    failure << (proto.LPS.failure.as_leaf() | software_bug() | badcolor |
                Proximity.failure() | Uncooperative.failure.as_leaf() | turn)


# Q: merge this with SpuriousMovements?
# A: No, because this actually does something different.
#    This tree is absolutely necessary because of MR14:
#        """
#        if there is information do not just run to the position where the information was
#        gathered but instead try to actually localize and find the victim efficiently
#        """
class GoWrong(Tree):
    spec = P('misunderstanding\nabout MR14', failure_rate=1e-2)
    check = P('not discovered during\npeer review', failure_rate=1e-3)

    failure = T(r'moving to the \"gathered position\"\ninstead \"towards the victim\"')
    failure << (spec & check)


class NoPowerLED(Tree):
    primary = P('primary power LED failure', failure_rate=1e-5)

    failure = T('power LED is off')
    failure << (primary | hw.EPuck.failure)


class SeeNoLed(Tree):
    primary = P('primary failure\nof the display-LED', failure_rate=1e-5)
    software = software_bug()

    not_turned_on = hw.EPuck.not_turned_on()

    failure = T('clear line of sight,\nbut no LED indication')
    failure << (VictimSilent.failure | proto.SOS.receiver | primary | software | not_turned_on)


class Victim404(Tree):
    not_placed_in = S('user did not place\nthe victim in the maze')
    unsolvable = S('unsolvable maze')

    rhr_hangs = F('right-hand-rule\ndrives in circles')
    rhr_hangs << (P('right-hand-rule\ncan drive in circles', failure_rate=inf) &
                  P('path-pruning fails', failure_rate=0) &
                  S('maze is not\n1-outerplanar'))

    drift = F('lost orientation')
    drift << (proto.LPS.failure &
              P('approximation algorithm\nis way off', failure_rate=1e-3) &
              P('drift from real orientation\ndoes not stabilize', failure_rate=1e-5))

    failure = T('victim cannot be found')
    failure << (VictimSilent.failure | proto.SOS.receiver() | rhr_hangs | drift
                | not_placed_in | unsolvable)


class NoEscort(Tree):
    indirect = F('taking a detour during escort')
    indirect << (Uncooperative.failure.as_leaf() | hw.Motor.failure() | hw.EPuck.memory_fault())

    failure = T('not moving the victim out;\nat least not on shortest known path')
    failure << (StandingStill.failure | Escort.unintentional.as_leaf() | indirect)


class SystemFailure(Tree):
    tin_bot_failure = F('Tin Bot failure\n')
    tin_bot_failure << (VictimLost.failure | IgnoreVictim.failure | SpuriousMovements.failure |
                        GoWrong.failure | NoEscort.failure | Victim404.failure)

    # FIXME: how to model redundancy in fault trees — do wee need 2 copies of the Tin Bot tree?
    all_bad = F('all Tin Bots fail')
    all_bad << (tin_bot_failure & tin_bot_failure)

    failure = T('system failure\n(victim remains in maze)')
    failure << (all_bad | VictimSilent.failure)


if __name__ == '__main__':
    import os.path

    from fault_tree_lib import get_trees, traverse, generate, nodes

    used_nodes = set()

    for tree in get_trees():
        for node in traverse(tree.failure):
            used_nodes.add(node)
        if isinstance(tree.failure, T):
            generate(tree.failure, filename=tree.__name__.lower() + '.eps')

    for node in set(nodes.keys()).difference(used_nodes):
        filename, line = nodes[node]
        print('not used:', repr(node.label), os.path.basename(filename), line)
