#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import time

import vrep


connection = vrep.simxStart('127.0.0.1', 19997, True, True, 5000, 5)
assert connection >= 0


tin_bot = vrep.simxGetObjectHandle(connection, 'tinBot',
                                   vrep.simx_opmode_oneshot_wait)[1]
motor_left = vrep.simxGetObjectHandle(connection, 'ePuck_leftJoint',
                                      vrep.simx_opmode_oneshot_wait)[1]
motor_right = vrep.simxGetObjectHandle(connection, 'ePuck_rightJoint',
                                       vrep.simx_opmode_oneshot_wait)[1]

print(tin_bot, motor_left, motor_right)

vrep.simxStartSimulation(connection, vrep.simx_opmode_oneshot_wait)


try:
    while True:
        vrep.simxSetJointTargetVelocity(connection, motor_left, 5.0,
                                        vrep.simx_opmode_streaming)
        vrep.simxSetJointTargetVelocity(connection, motor_right, 5.0,
                                        vrep.simx_opmode_streaming)
        time.sleep(1)
        vrep.simxSetJointTargetVelocity(connection, motor_left, -5.0,
                                        vrep.simx_opmode_streaming)
        vrep.simxSetJointTargetVelocity(connection, motor_right, -5.0,
                                        vrep.simx_opmode_streaming)
        time.sleep(1)
finally:
    vrep.simxFinish(connection)
