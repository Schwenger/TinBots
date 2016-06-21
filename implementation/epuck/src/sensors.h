#ifndef SENSORS_H
#define SENSORS_H

typedef struct Sensors {
    double proximity[8];
    int ir[6];
    int victim_attached;
    struct {
        double x;
        double y;
        double direction;
    } current;
} Sensors;

#endif
