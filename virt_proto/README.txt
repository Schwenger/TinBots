Tin Bot – Group 6
=================

Welcome to our virtual prototype.

Setup:
------
 1. Start Matlab and navigate to this folder.
 2. Execute `setup()` to setup the simulation environment.

Structure:
----------
 - library: custom Simulink blocks to build Tin Bot simulations
 - classes: Matlab classes used by the simulation
 - software: the "Software" components running on the physical Tin Bot
 - tests: various test cases for different components


Tests:
------
 - drive_test: test physical model of differential drive
 - proximity_test: test physical model of proximity sensor
 - ir_test: test physical model of IR sensor
 - victim_direction_test: test victim direction calculation
 - approximator_test: test approximation of current position and orientation
 - follow_right_hand_test: test right hand rule
 - path_finder_test: test internal map generation and escape path calculation
 - controller_test: test the complete control software
 - traffic_golden_path: test the "golden path" of the traffic cop
