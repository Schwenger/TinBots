#ifndef EPUCK_COMMANDS_H
#define EPUCK_COMMANDS_H

enum Commands {

    CMD_HELLO = 1,
    CMD_START = 2,
    CMD_RESET = 3,
    CMD_PRINT = 4,
    CMD_SET_MODE = 16,
    CMD_UPDATE_LPS = 17,
    CMD_T2T_HEARTBEAT = 48,
    CMD_T2T_VICTIM_PHI = 49,
    CMD_T2T_VICTIM_XY = 50,
    CMD_T2T_UPDATE_MAP = 51,
    CMD_T2T_DOCKED = 52,
    CMD_T2T_COMPLETED = 53,
    CMD_DEBUG_INFO = 96,
    CMD_DEBUG_MOTORS = 97,
    CMD_DEBUG_LED = 98,

};

void register_commands(void);

#endif