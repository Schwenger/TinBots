#include <stdio.h>

#include "hal.h"
#include "map_heap.h"

#define data_width 8
#define data_height 2

/* Just some random stuff */
static FieldType types[] = {
    FIELD_FREE,     FIELD_WALL,     FIELD_UNKNOWN, FIELD_FREE,      FIELD_UNKNOWN,  FIELD_WALL,     FIELD_FREE,    FIELD_WALL,
    FIELD_WALL,     FIELD_WALL,     FIELD_UNKNOWN, FIELD_UNKNOWN,   FIELD_UNKNOWN,  FIELD_UNKNOWN,  FIELD_UNKNOWN, FIELD_WALL};
typedef char check_types_length[(sizeof(types) / sizeof(types[0]) == (data_width * data_height)) ? 1 : -1];

static const unsigned char data_expect[] = {
    /* Single byte is four fields: WXYZ -> 0bzzyyxxww
     * Two bytes is four fields and the four *below* it. */
    0b01001001, /* (0,0)-(3,0) */
    0b00001010, /* (0,1)-(3,1) */
    0b10011000, /* (4,0)-(7,0) */
    0b10000000  /* (4,1)-(7,1) */ };
typedef char check_data_length[(sizeof(data_expect) == MAP_INTERNAL_DATA_SIZE(data_width,data_height)) ? 1 : -1];

int main(void) {
    MapContainer mc;
    unsigned char* data_actual;
    int x, y;
    unsigned int i;

    mc.accu = NULL;
    mc.prox = map_heap_alloc(data_width, data_height);
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
    data_actual[1] = 0x84; /* 0b10000100 */
    {
        /* Own scope because C90.  Sigh. */
        typedef char check_test_two_updated[(data_width == 8 && data_height == 2) ? 1 : -1];
        (void)sizeof(check_test_two_updated);
    }
    types[ 8] = FIELD_UNKNOWN;
    types[ 9] = FIELD_FREE;
    types[10] = FIELD_UNKNOWN;
    types[11] = FIELD_WALL;
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
    map_set_field(mc.prox, 6, 1, FIELD_FREE);
    {
        /* Own scope because C90.  Sigh. */
        typedef char check_test_three_updated[(data_width == 8 && data_height == 2) ? 1 : -1];
        (void)sizeof(check_test_three_updated);
    }
    types[14] = FIELD_FREE;
    for (y = 0; y < data_height; ++y) {
        for (x = 0; x < data_width; ++x) {
            printf(" %d", map_get_field(mc.prox, x, y));
            assert(types[x + y * data_width] == map_get_field(mc.prox, x, y));
        }
    }
    map_heap_free(mc.prox);
    printf(" GOOD (done)\n");

    return 0;
}
