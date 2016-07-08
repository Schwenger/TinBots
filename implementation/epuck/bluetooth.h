
#ifndef EPUCK_BLUETOOTH_H
#define EPUCK_BLUETOOTH_H

#include "map.h"

#define T2T_HEARTHBEAT (8) /* 0b1000 */

#define T2T_MAP (9) /* 0b1010 */

#define TIN_PACKAGE_MAX_LENGTH 128

void send_map(Map* map, Position center);

void receive_map(Map* map, Position center, byte* data, unsigned int length);

#endif /* EPUCK_BLUETOOTH_H */
