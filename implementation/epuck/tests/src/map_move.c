#include <assert.h>
#include <stdio.h>
#include <string.h> /* memcmp */

#include "map_heap.h"

#define data_width (4*3)
#define data_height (3)

static unsigned char data_init[] = {
    0x11, 0x12, 0x13,
    0x21, 0x22, 0x23,
    0x31, 0x32, 0x33};
typedef char check_length_data[(sizeof(data_init) / sizeof(data_init[0]) == (data_height * data_width / 4)) ? 1 : -1];

static const size_t data_bytes = sizeof(data_init) / sizeof(data_init[0]);

static unsigned char data_right[] = {
    0x00, 0x11, 0x12,
    0x00, 0x21, 0x22,
    0x00, 0x31, 0x32};
typedef char check_length_right[(sizeof(data_right) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_left[] = {
    0x12, 0x13, 0x00,
    0x22, 0x23, 0x00,
    0x32, 0x33, 0x00};
typedef char check_length_left[(sizeof(data_left) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_up[] = {
    /* Textually, positive y (semantically "up") points to higher line
     * numbers (visually "down").  I'm sorry. */
    0x00, 0x00, 0x00,
    0x11, 0x12, 0x13,
    0x21, 0x22, 0x23};
typedef char check_length_up[(sizeof(data_up) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_down[] = {
    /* Textually, positive y (semantically "up") points to higher line
     * numbers (visually "down").  I'm sorry. */
    0x21, 0x22, 0x23,
    0x31, 0x32, 0x33,
    0x00, 0x00, 0x00};
typedef char check_length_down[(sizeof(data_down) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_empty[] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00};
typedef char check_length_empty[(sizeof(data_empty) == sizeof(data_init)) ? 1 : -1];

static unsigned char data_p11[] = {
    /* Textually, positive y (semantically "up") points to higher line
     * numbers (visually "down").  I'm sorry. */
    0x00, 0x00, 0x00,
    0x00, 0x11, 0x12,
    0x00, 0x21, 0x22};
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
    check_movement(  0,  1, data_up);
    check_movement(  0, -1, data_down);
    check_movement(  4,  1, data_p11);

    check_movement( 12,  0, data_empty);
    check_movement(-12,  0, data_empty);
    check_movement(  0,  3, data_empty);
    check_movement(  0, -3, data_empty);

    check_movement(  4,  4, data_empty);
    check_movement(-16,  1, data_empty);

    printf("map_move: GOOD\n");

    return 0;
}
