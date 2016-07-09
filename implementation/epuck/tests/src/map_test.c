#include <assert.h>
#include <stdio.h>

#include "map_heap.h"

#define data_width 3
#define data_height 4

/* Just some random stuff */
static FieldType types[] = {
    FIELD_FREE, FIELD_WALL, FIELD_UNKNOWN, FIELD_FREE,
    FIELD_UNKNOWN, FIELD_WALL, FIELD_FREE, FIELD_WALL,
    FIELD_WALL, FIELD_WALL, FIELD_UNKNOWN, FIELD_UNKNOWN};
typedef char check_types_length[(sizeof(types) / sizeof(types[0]) == (data_width * data_height)) ? 1 : -1];

static const unsigned char data_expect[] = {
    /* WXYZ -> 0bzzyyxxww
     * 0b01001001, 0b10011000, 0b00001010 */
    0x49, 0x98, 0x0A};
typedef char check_data_length[(sizeof(data_expect) == MAP_INTERNAL_DATA_SIZE(data_width,data_height)) ? 1 : -1];

int main(void) {
    MapContainer mc;
    unsigned char* data_actual;
    int x, y;
    unsigned int i;

    mc.accu = NULL;
    mc.prox = map_heap_alloc(3, 4);
    map_heap_container = &mc;
    assert(mc.prox == map_get_proximity());
    assert(NULL == map_get_accumulated());
    for (y = 0; y < data_height; ++y) {
        for (x = 0; x < data_width; ++x) {
            map_set_field(mc.prox, x, y, types[x + y * data_width]);
        }
    }

    /* First test: is the serialized data correct? */
    data_actual = map_serialize(mc.prox);
    assert(data_actual == mc.prox->data);
    printf("map_test_ser:");
    for(i = 0; i < sizeof(data_expect); ++i) {
        printf(" 0x%02x", data_actual[i]);
        assert(data_expect[i] == data_actual[i]);
    }
    printf(" GOOD\n");

    /* Second test: if I change something and deserialize it (but this is just a
     * view, so there's no 'deserialize' necessary), does it work? */
    data_actual[2] = 0x84; /* 0b10000100 */
    types[2 + 2 * data_width] = FIELD_UNKNOWN;
    types[0 + 3 * data_width] = FIELD_FREE;
    /* types[0 + 3 * data_width] stays FIELD_UNKNOWN */
    types[2 + 3 * data_width] = FIELD_WALL;
    printf("map_test_deser:");
    for (y = 0; y < data_height; ++y) {
        for (x = 0; x < data_width; ++x) {
            printf(" %d", map_get_field(mc.prox, x, y));
            assert(types[x + y * data_width] == map_get_field(mc.prox, x, y));
        }
    }
    printf(" GOOD\n");

    /* Third test: is map_set_field really only locally modifying things? */
    printf("map_test_set:");
    types[2 + 0 * data_width] = FIELD_FREE;
    map_set_field(mc.prox, 2, 0, FIELD_FREE);
    for (y = 0; y < data_height; ++y) {
        for (x = 0; x < data_width; ++x) {
            printf(" %d", map_get_field(mc.prox, x, y));
            assert(types[x + y * data_width] == map_get_field(mc.prox, x, y));
        }
    }
    printf(" GOOD (done)\n");

    return 0;
}
