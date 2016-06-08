#!/usr/bin/env python3
# -*- coding:utf-8 -*-

from fault_tree_lib import Failure, graphviz


escort_no_led = Failure(r'escorting,\nbut no led')


see_no_led = Failure(r'clear line of sight,\nbut no LED')


victim_lost = Failure(r'victim lost while escorting')


standing_still = Failure(r'standing still')
no_initial_lps = Failure(r'no initial position from LPS')
software_init = Failure(r'software initialization failure')

standing_still << (no_initial_lps | software_init)


print(graphviz(escort_no_led, see_no_led, victim_lost, standing_still))
