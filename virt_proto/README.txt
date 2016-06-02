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
    Quick, atomic
 - proximity_test: test physical model of proximity sensor
    Quick, atomic
 - ir_test: test physical model of IR sensor
    Quick, atomic
 - traffic_blind_test: test the "blind" part of the traffic cop
    Quick, atomic
 - victim_direction_test: test victim direction calculation
    Medium time, tests environment and bare-bone software component
 - path_finder_test: test internal map generation and escape path calculation
    BROKEN!  Inverted y (see Slack, pf_broken_proof.png)
    Medium time, awesome visualization
 - approximator_test: test approximation of current position and orientation
    Slow-ish
    UNTESTED (I'm not sure how to read the outputs)
 - follow_right_hand_test: test right hand rule
    Very slow, nearly holistic
 - controller_test: test the complete control software
    BROKEN!  Map triggers unhandled corner-case ("stroke" mode)
    Very slow, nearly holistic
