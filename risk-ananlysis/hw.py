# -*- coding: utf-8 -*-

"""
Failures in Hardware Components
"""

# TODO: add documentation for components (needed for risk documentation) (use python docstrings)

from fault_tree_lib import Tree, Failure as F, Primary as P, Secondary as S


class Bluetooth(Tree):
    sender = P('bluetooth module failure\n(sender)')
    receiver = P('bluetooth module failure\n(receiver)')

    medium = P('medium failure (noise, interference, ...)')

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

    wiring = F('failure in wiring')

    failure = F('battery failure')
    failure << (defect | not_charged | wiring)


class Raspberry(Tree):
    board = P('primary hardware failure')

    failure = F('raspberry failure')
    failure << (board | Power.failure)


class Camera(Tree):
    failure = P('primary camera failure')


class EPuck(Tree):
    board = P('primary board failure')

    not_turned_on = S('user did not turn\non the E-Puck')

    failure = F('E-Puck failure')
    failure << (board | Battery.failure | not_turned_on)


class Motor(Tree):
    failure = P('primary motor fault')


class ExtBoard(Tree):
    board = P('primary circuit failure')
    controller = P('primary mikrocontroller defect')

    ir_sensor = P('primary IR sensor defect')

    i2c = P('primary I2C bus failure')

    failure = F('extension board failure')
    failure << (i2c | board | controller | ir_sensor)


class Victim(Tree):
    ir_led = P('primary IR LED failure')
    controller = P('primary mikrocontroller defect')
    not_turned_on = S('user did not turn\non the victim')

    with F('circuit failure') as circuit:
        transistor = P('primary transistor defect')
        resistor = P('primary resistor defect')

        circuit << (transistor | resistor)

    failure = F('victim failure')
    failure << (ir_led | controller | not_turned_on | Battery.failure)


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for component in get_trees():
        generate(component.failure, filename='hw_' + component.__name__.lower() + '.eps')
