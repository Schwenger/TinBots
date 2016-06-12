# -*- coding: utf-8 -*-

"""
Failures in Hardware Components
"""

# TODO: add documentation for components (needed for risk documentation) (use python docstrings)

from fault_tree_lib import Tree, Failure as F, Primary as P, Secondary as S


class Bluetooth(Tree):
    sender = P('bluetooth module failure\n(sender)')
    receiver = P('bluetooth module failure\n(receiver)')

    medium = P('medium failure\n(noise, interference, ...)')

    failure = F('bluetooth communication failure')
    failure << (medium | sender | receiver)


class Power(Tree):
    supply = P('faulty power supply')
    grid = P('electricity grid outage')
    wiring = S('wiring failure')

    failure = P('power supply failure')
    failure << (supply | grid | wiring)


class Battery(Tree):
    defect = P('primary battery defect')
    not_charged = S('battery not charged')

    wiring = P('failure in wiring')

    failure = F('power failure')
    failure << (defect | not_charged | wiring)


class Raspberry(Tree):
    board = P('primary hardware failure')
    os = P('operating system failure')

    external_module_bug = P('bug in\nexternal module')
    internal_module_bug = P('bug in LPS software\n(blob recognition, ...)')

    software_failure = F('software failure')
    software_failure << (os | external_module_bug | internal_module_bug)

    failure = F('raspberry pi failure')
    failure << (board | Power.failure | software_failure)


class Camera(Tree):
    failure = P('primary camera failure')


class EPuck(Tree):
    board = P('primary board failure')

    not_turned_on = S('user did not turn\non the E-Puck')

    memory_fault = P('memory fault')

    failure = F('E-Puck failure')
    failure << (board | Battery.failure | not_turned_on)


class Motor(Tree):
    failure = P('primary motor fault')


class ExtBoard(Tree):
    board = P('primary circuit failure')
    controller = P('primary microcontroller\nfailure')

    ir_sensor = P('primary IR\nsensor defect')

    i2c = P('primary I2C\nbus failure')

    failure = F('extension board failure')
    failure << (i2c | board | controller | ir_sensor)


class Victim(Tree):
    ir_led = P('primary IR LED failure')
    controller = P('primary microcontroller\nfailure')
    not_turned_on = S('user did not turn\non the victim')

    with F('circuit failure') as circuit:
        transistor = P('primary transistor defect')
        resistor = P('primary resistor defect')

        circuit << (transistor | resistor)

    failure = F('victim failure')
    # Copy Battery.failure as it's a different battery
    failure << (ir_led | controller | not_turned_on | Battery.failure())


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for component in get_trees():
        generate(component.failure, filename='hw_' + component.__name__.lower() + '.eps')
