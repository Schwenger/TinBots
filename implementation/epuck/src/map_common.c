#include <assert.h>
#include <string.h> /* memcpy */

#include "map_detail.h"

/* ===== Common implementation of map.h ===== */

/* Check that constant in map-h */
typedef char check_neighborhood_map_length[(MAP_PROXIMITY_BUF_SIZE == MAP_INTERNAL_DATA_SIZE(MAP_PROXIMITY_SIZE,MAP_PROXIMITY_SIZE)) ? 1 : -1];

typedef unsigned long FieldPtr;

static FieldPtr locate(int x, int y, int w, int h) {
    FieldPtr bit_offset;

    assert(w >= 1 && h >= 1);
    /* I don't see why this should ever be violated */
    assert(w <= 256 && h <= 256);
    assert(x >= 0 && y >= 0 && x < w && y < h);
    bit_offset = (FieldPtr)(x + y * w);
    bit_offset *= BIT_PER_FIELD;
    return bit_offset;
}

/* Assume that a field always fits wholly into a byte: */
typedef char check_bit_length[((8 / BIT_PER_FIELD) * BIT_PER_FIELD == 8) ? 1 : -1];

static const unsigned int FIELD_MASK = (1 << BIT_PER_FIELD) - 1;

FieldType map_get_field(Map* map, int x, int y) {
    unsigned char* data;
    FieldPtr p;
    unsigned char raw_char;
    unsigned int bit_idx;
    FieldType result;
    data = map_serialize(map);
    p = locate(x, y, map_get_width(map), map_get_height(map));
    raw_char = data[p >> 3];
    bit_idx = p & 0x7; /* 0b111 */
    assert((bit_idx % BIT_PER_FIELD) == 0);
    result = (raw_char >> bit_idx) & FIELD_MASK;
    return result;
}

void map_set_field(Map* map, int x, int y, FieldType type) {
    unsigned char* data;
    FieldPtr p;
    unsigned char raw_char;
    unsigned int bit_idx;
    assert((type & ~FIELD_MASK) == 0);
    data = map_serialize(map);
    p = locate(x, y, map_get_width(map), map_get_height(map));
    raw_char = data[p >> 3];
    bit_idx = p & 0x7; /* 0b111 */
    assert((bit_idx % BIT_PER_FIELD) == 0);
    /* Clear old value: */
    raw_char &= ~(FIELD_MASK << bit_idx);
    /* Write new value: */
    raw_char |= type << bit_idx;
    /* Update array: */
    data[p >> 3] = raw_char;
}
