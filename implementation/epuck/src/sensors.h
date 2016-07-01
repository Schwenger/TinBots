#ifndef SENSORS_H
#define SENSORS_H

#include "pi.h"

enum Proximity_Order {
    /* If you change this, please also change:
     * - rhr.c -> find_wall -> sense_angles */
    PROXIMITY_M_20,
    PROXIMITY_M_45,
    PROXIMITY_M_90,
    PROXIMITY_M_150,
    PROXIMITY_P_150,
    PROXIMITY_P_90,
    PROXIMITY_P_45,
    PROXIMITY_P_20,
    NUM_PROXIMITY
};

enum IR_Order {
    /* If you change this, please also change:
     * - victim-direction.c -> TODO (not pushed yet) */
    IR_315,
    IR_270,
    IR_225,
    IR_135,
    IR_90,
    IR_45,
    NUM_IR /* ?? */
};

typedef struct LPS_Data {
    double x, y ,phi;
} LPS_Data;

extern const double ir_sensor_angle[6];

typedef struct Sensors {
    double proximity[NUM_PROXIMITY];
    int ir[NUM_IR];
    int victim_attached;
    struct {
        double x;
        double y;
        double direction;
    } current;
    LPS_Data lps;
} Sensors;

#endif
