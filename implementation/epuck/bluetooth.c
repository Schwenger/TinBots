#include <assert.h>

#include "bluetooth.h"
#include "hal.h"

void receive_map(Position center, unsigned char* data) {
    (void)center;
    (void)data;
    /* FIXME: Needs to be torn apart, to separate e-puck specifics (should be only hal_send_message()) from general methods (e.g. assembling the packet)
void receive_map(Map* map, Position center, byte* data, unsigned int length) {
    int diameter = 2 * RADIUS;
    int buffer[fields_transmitted];
    unsigned int x,y;
    map_deserialize(buffer, data, length);

    for(x = 0; x < diameter; ++x) {
        for(y = 0; y < diameter; ++y) {
            map->occupancy[center.x - RADIUS + x][center.y - RADIUS + y] = buffer[x + RADIUS * y];
        }
    }
    */
}

void send_map(Map* map, Position center) {
    (void)map;
    (void)center;
    /* FIXME: Needs to be torn apart, to separate e-puck specifics (should be none) from general methods (e.g. parsing the packet)
void send_map(Map* m, Position center) {
    unsigned int diameter = 2 * RADIUS;
    unsigned int fields_to_transmit = diameter * diameter;
    unsigned int bytes_needed = fields_to_transmit * BIT_PER_FIELD / 8;
    int x, y;
    int** map = m->occupancy;
    int occ[TIN_PACKAGE_MAX_LENGTH];
    static byte data[TIN_PACKAGE_MAX_LENGTH];

    for(x = 0; x < diameter; ++x) {
        for(y = 0; y < diameter; ++y) {
            occ[x * diameter + y] = map[center.x - RADIUS + x][center.y - RADIUS + y];
        }
    }
    assert(bytes_needed < TIN_PACKAGE_MAX_LENGTH);
    map_serialize(occ, data, fields_to_transmit);
    hal_send_msg(0, data, bytes_needed);
    */
}
