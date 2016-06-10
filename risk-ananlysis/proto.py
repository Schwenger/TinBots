# -*- coding: utf-8 -*-

"""
Failures in Communication
"""

# TODO: add documentation for components (needed for risk documentation) (use python docstrings)

from fault_tree_lib import Tree, Failure as F, Primary as P, Secondary as S

from common import software_bug

import hw


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

    # extension board failure includes WAU failure
    receiver = hw.ExtBoard.failure

    failure = F('SOS failure')
    failure << (medium | sender | receiver)


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for component in get_trees():
        generate(component.failure, filename='com_' + component.__name__.lower() + '.eps')
