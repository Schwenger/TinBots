#include <stdio.h>
#include <string.h> /* memcmp */

#include "hal.h"
#include "map_heap.h"

/* Use '3' to detect 'direction' errors.
 * Because 2-1 == 1 but 3-1 != 1. */
#define data_width (4*3)
#define data_height (2*3)

static unsigned char data_init[] = {
    /* 'Named' according to their semantical position. */
    0x11,0x21, 0x15,0x25, 0x19,0x29,
    0x31,0x41, 0x35,0x45, 0x39,0x49,
    0x51,0x61, 0x55,0x65, 0x59,0x69};
typedef char check_length_data[(sizeof(data_init) / sizeof(data_init[0]) == (data_height * data_width / 4)) ? 1 : -1];

static const size_t data_bytes = sizeof(data_init) / sizeof(data_init[0]);

static unsigned char data_right[] = {
    0x00,0x00, 0x11,0x21, 0x15,0x25,
    0x00,0x00, 0x31,0x41, 0x35,0x45,
    0x00,0x00, 0x51,0x61, 0x55,0x65,};
typedef char check_length_right[(sizeof(data_right) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_left[] = {
    0x15,0x25, 0x19,0x29, 0x00,0x00,
    0x35,0x45, 0x39,0x49, 0x00,0x00,
    0x55,0x65, 0x59,0x69, 0x00,0x00};
typedef char check_length_left[(sizeof(data_left) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_up[] = {
    /* Textually, positive y (semantically "up") points to higher line
     * numbers (visually "down").  I'm sorry. */
    0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x11,0x21, 0x15,0x25, 0x19,0x29,
    0x31,0x41, 0x35,0x45, 0x39,0x49};
typedef char check_length_up[(sizeof(data_up) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_down[] = {
    /* Textually, positive y (semantically "down") points to higher line
     * numbers (visually "up").  I'm sorry. */
    0x31,0x41, 0x35,0x45, 0x39,0x49,
    0x51,0x61, 0x55,0x65, 0x59,0x69,
    0x00,0x00, 0x00,0x00, 0x00,0x00};
typedef char check_length_down[(sizeof(data_down) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_empty[] = {
    0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x00};
typedef char check_length_empty[(sizeof(data_empty) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_p11[] = {
    /* Textually, positive y (semantically "up") points to higher line
     * numbers (visually "down").  I'm sorry. */
    0x00,0x00, 0x00,0x00, 0x00,0x00,
    0x00,0x00, 0x11,0x21, 0x15,0x25,
    0x00,0x00, 0x31,0x41, 0x35,0x45};
typedef char check_length_p11[(sizeof(data_p11) == sizeof(data_init)) ? 1 : -1];

static void check_movement(int by_x, int by_y, unsigned char* data_expect) {
    unsigned char* data_actual;
    size_t i = 0;
    Map* map = map_get_proximity();
    data_actual = map_serialize(map);
    memcpy(data_actual, data_init, data_bytes);
    map_move(map, by_x, by_y);
    if (0 == memcmp(data_actual, data_expect, data_bytes)) {
        return;
    }
    printf("Mismatch for (%d,%d):\n\tExpect = 0x", by_x, by_y);
    for (i = 0; i < data_bytes; ++i) {
        printf("%02x", data_expect[i]);
    }
    printf("\n\tActual = 0x");
    for (i = 0; i < data_bytes; ++i) {
        printf("%02x", data_actual[i]);
    }
    printf("\n");
    assert(0);
}

int main(void) {
    MapContainer mc;
    assert(data_width % 4 == 0);

    mc.accu = NULL;
    mc.prox = map_heap_alloc(data_width, data_height);
    map_heap_container = &mc;
    assert(mc.prox == map_get_proximity());

    check_movement( 0,  0, data_init);

    check_movement(  4,  0, data_right);
    check_movement(- 4,  0, data_left);
    check_movement(  0,  2, data_up);
    check_movement(  0, -2, data_down);
    check_movement(  4,  2, data_p11);

    check_movement( data_width,            0, data_empty);
    check_movement(-data_width,            0, data_empty);
    check_movement(          0,  data_height, data_empty);
    check_movement(          0, -data_height, data_empty);

    assert(4 < data_width);
    check_movement(  4,  8, data_empty);
    assert(2 < data_height);
    check_movement(-16,  2, data_empty);

    printf("map_move: GOOD\n");

    return 0;
}
