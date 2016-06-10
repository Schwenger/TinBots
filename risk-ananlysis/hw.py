# -*- coding: utf-8 -*-

"""
Failures in Hardware Components
"""

# TODO: add documentation for components (needed for risk documentation) (use python docstrings)

from fault_tree_lib import Component, Failure as F, Primary as P, Secondary as S


class Bluetooth(Component):
    module = P('bluetooth module failure')
    medium = P('medium failure (noise, interference, ...)')

    failure = F('bluetooth communication failure')
    failure << (module | medium)


class Power(Component):
    supply = P('faulty power supply')
    grid = P('electricity grid outage')
    wiring = S('wiring failure')

    failure = P('power supply failure')
    failure << (supply | grid | wiring)


class Battery(Component):
    defect = P('primary battery defect')
    not_charged = S('battery not charged')

    wiring = F('failure in wiring')

    failure = F('battery failure')
    failure << (defect | not_charged | wiring)


class Raspberry(Component):
    board = P('primary hardware failure')

    failure = F('raspberry failure')
    failure << (board | Power.failure)


class Camera(Component):
    failure = P('primary camera failure')


class EPuck(Component):
    board = P('primary board failure')

    failure = F('E-Puck failure')
    failure << (board | Battery.failure)


class ExtBoard(Component):
    board = P('primary circuit failure')
    controller = P('primary mikrocontroller defect')

    ir_sensor = P('primary IR sensor defect')

    failure = F('extension board failure')
    failure << (board | controller | ir_sensor)


class TinBot(Component):
    i2c = P('primary bus I2C failure')

    failure = F('complete Tin Bot failure')
    failure << (EPuck.failure | ExtBoard.failure | i2c)


class Victim(Component):
    ir_led = P('primary IR LED failure')
    controller = P('primary mikrocontroller defect')

    with F('circuit failure') as circuit:
        transistor = P('primary transistor defect')
        resistor = P('primary resistor defect')

        circuit << (transistor | resistor)

    failure = F('victim failure')
    failure << (ir_led | controller | Battery.failure)


if __name__ == '__main__':
    from fault_tree_lib import get_components, generate

    for component in get_components():
        generate(component.failure, filename='hw_' + component.__name__.lower() + '.eps')
