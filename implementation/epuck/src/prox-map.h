#ifndef EPUCK_PROX_MAP_H
#define EPUCK_PROX_MAP_H

#include "map.h"
#include "sensors.h"

typedef struct ProxMapState {
    /* Might be a few cm off from the current position to avoid flickering */
    Position lower_left;
} ProxMapState;

void proximity_reset(ProxMapState* prox_map, Sensors* sens);

void proximity_step(ProxMapState* prox_map, Sensors* sens);

void proximity_send(ProxMapState* prox_map);

#endif
