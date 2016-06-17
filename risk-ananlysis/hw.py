# -*- coding: utf-8 -*-

"""
Failures in Hardware Components
"""

from fault_tree_lib import Tree, Failure as F, Primary as P, Secondary as S


class Bluetooth(Tree):
    sender = P('bluetooth module failure\n(sender)', failure_rate=1e-5)
    receiver = P('bluetooth module failure\n(receiver)', failure_rate=1e-5)

    medium = P('medium failure\n(noise, interference, ...)', failure_rate=1e-3)

    failure = F('bluetooth communication failure')
    failure << (medium | sender | receiver)


class Power(Tree):
    supply = P('faulty power supply', failure_rate=1e-6)
    grid = P('electricity grid outage', failure_rate=2.25e-5)
    wiring = S('power not\nconnected')

    failure = F('power supply failure')
    failure << (supply | grid | wiring)


class Battery(Tree):
    defect = P('primary battery defect', failure_rate=1e-8)
    not_charged = S('battery not charged')
    switch = P('primary power switch failure', failure_rate=1e-6)

    # Q: Why don't we include 'wiring'?
    # A: Because that's already handled in things such as 'controller board' etc.

    failure = F('power failure')
    failure << (defect | not_charged | switch)

    failure_no_switch = F('power failure')
    failure_no_switch << (defect() | not_charged())


class Raspberry(Tree):
    board = P('primary hardware failure', failure_rate=1e-6)
    os = P('operating system failure', failure_rate=2.94e-5)

    external_module_bug = P('bug in\nexternal module', failure_rate=0)
    internal_module_bug = P('bug in LPS software\n(blob recognition, ...)', failure_rate=0)

    software_failure = F('software failure')
    software_failure << (os | external_module_bug | internal_module_bug)

    failure = F('raspberry pi failure')
    failure << (board | Power.failure | software_failure)


class Camera(Tree):
    failure = P('primary camera failure', failure_rate=1e-4)


class EPuck(Tree):
    board = P('primary controller board failure', failure_rate=1e-10)
    controller = P('primary controller failure', failure_rate=1e-12)

    not_turned_on = S('user did not turn\non the E-Puck')

    memory_fault = P('memory fault', failure_rate=1e-11)

    failure = F('E-Puck failure')
    failure << (board | controller | Battery.failure | not_turned_on |
                memory_fault)


class Motor(Tree):
    failure = P('primary motor fault', failure_rate=1e-5)


class ExtBoard(Tree):
    board = P('primary circuit failure', failure_rate=1e-10)
    controller = P('primary microcontroller\nfailure', failure_rate=1e-12)

    ir_sensor = P('primary IR\nsensor defect', failure_rate=1e-5)

    i2c = P('primary I2C\nbus failure', failure_rate=1e-5)

    failure = F('extension board failure')
    failure << (i2c | board | controller | ir_sensor)


class Victim(Tree):
    ir_led = P('primary IR LED failure', failure_rate=1e-5)
    controller = P('primary microcontroller\nfailure', failure_rate=1e-12)

    with F('circuit failure') as circuit:
        transistor = P('primary transistor defect', failure_rate=1e-12)
        resistor = P('primary resistor defect', failure_rate=1e-9)

        circuit << (transistor | resistor)

    failure = F('victim failure')
    # Copy Battery.failure as it's a different battery
    failure << (ir_led | controller | circuit | Battery.failure_no_switch)


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for component in get_trees():
        generate(component.failure, filename='hw_' + component.__name__.lower() + '.eps')
