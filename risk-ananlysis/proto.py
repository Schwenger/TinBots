# -*- coding: utf-8 -*-

"""
Failures in Communication
"""

from fault_tree_lib import Tree, Failure as F, Primary as P, Secondary as S

import hw


class CBP(Tree):
    medium = S('environment failure (not enough light,\nno clear line of sight, ...)')
    sender = S('missing color blob')
    receiver = hw.Camera.failure

    failure = F('color blob\nprotocol failure')
    failure << (medium | sender | receiver)


class LPS(Tree):
    medium = hw.Bluetooth.medium
    sender = hw.Raspberry.failure
    receiver = hw.Bluetooth.receiver

    with F('LPP link down') as link_down:
        service_outage = S('simulated LPS\nservice outage (NR1)')

        # Raspberry board failure includes Bluetooth sender failure
        link_down << (service_outage | medium | receiver)

    with F('LPS sends no data') as no_data:
        no_data << (CBP.failure | sender)

    failure = F('LPP failure')
    failure << (link_down | no_data)


class SOS(Tree):
    medium = P('primary medium failure\n(interference, ...)', failure_rate=1e-4)

    sender = hw.Victim.failure.as_leaf()

    # extension board failure includes WAU failure
    receiver = hw.ExtBoard.failure.as_leaf()

    failure = F('SOS failure')
    failure << (medium | sender | receiver)


if __name__ == '__main__':
    from fault_tree_lib import get_trees, generate

    for component in get_trees():
        generate(component.failure, filename='com_' + component.__name__.lower() + '.eps')
