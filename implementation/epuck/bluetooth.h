#ifndef EPUCK_BLUETOOTH_H
#define EPUCK_BLUETOOTH_H

#include "map.h"

void send_map(Map* map, Position center);

/* 'data' must be exactly MAP_PROXIMITY_BUF_SIZE long. */
void receive_map(Position center, unsigned char* data);

#endif /* EPUCK_BLUETOOTH_H */
