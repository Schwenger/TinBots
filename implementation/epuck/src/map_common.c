#include <stdint.h>
#include <stdlib.h> /* abs */
#include <string.h> /* memset */

#include "hal.h" /* assert */
#include "map.h"

/* ===== Common implementation of map.h ===== */

/* Layout in memory:
 *
 * ABCDIJKLQRST..
 * EFGHMNOPUVWX..
 *
 * Or in words: groups of four to positive x, then groups of 2 to
 * positive y, then to the end in positive x direction, then to the
 * end in positive y direction. */

/* Check the constants in map.h */
typedef char check_neighborhood_map_length[(MAP_PROXIMITY_BUF_SIZE == MAP_INTERNAL_DATA_SIZE(MAP_PROXIMITY_SIZE,MAP_PROXIMITY_SIZE)) ? 1 : -1];
typedef char check_bit_length[(NUM_FIELD <= (1 << BIT_PER_FIELD)) ? 1 : -1];
typedef char check_max_is_valid[(0 < MAP_INTERNAL_DATA_SIZE(MAP_MAX_WIDTH,MAP_MAX_HEIGHT)) ? 1 : -1];
/* We don't have space for a 256x256 map (16 KiB).
 * In case you're from the future and need larger maps:
 * - I *think* no code uses the 256-constraint though.
 * - Do the E-Pucks finally have sanely-sized memory? :D
 * - MAP_MAX_WIDTH and MAP_MAX_HEIGHT are still necessary for fast
 *   interaction between map_merge/prox_map, so don't overdo it. */
typedef char check_max_width[(MAP_MAX_WIDTH < 256) ? 1 : -1];
typedef char check_max_height[(MAP_MAX_HEIGHT < 256) ? 1 : -1];

unsigned long map_internal_locate(int x, int y, int w, int h) {
    unsigned long bit_offset;
    assert(w >= 1 && h >= 1);
    assert(w <= MAP_MAX_WIDTH && h <= MAP_MAX_HEIGHT);
    assert(x >= 0 && y >= 0 && x < w && y < h);
    assert(w % 4 == 0);
    assert(h % 2 == 0);
    x = (int)((unsigned int)(x & ~0x3) << 1) + (x & 0x3);
    bit_offset = (unsigned long)(x + (y >> 1) * 2 * w + (y & 1) * 4);
    bit_offset *= BIT_PER_FIELD;
    return bit_offset;
}

/* Assume that a field always fits wholly into a byte: */
typedef char check_fields_fit_byte[((8 / BIT_PER_FIELD) * BIT_PER_FIELD == 8) ? 1 : -1];

static const unsigned int FIELD_MASK = (1 << BIT_PER_FIELD) - 1;

FieldType map_get_field(Map* map, int x, int y) {
    unsigned char* data;
    unsigned long p;
    unsigned char raw_char;
    unsigned int bit_idx;
    FieldType result;
    data = map_serialize(map);
    p = map_internal_locate(x, y, map_get_width(map), map_get_height(map));
    raw_char = data[p >> 3];
    bit_idx = p & 0x7; /* 0b111 */
    assert((bit_idx % BIT_PER_FIELD) == 0);
    result = (raw_char >> bit_idx) & FIELD_MASK;
    return result;
}

void map_set_field(Map* map, int x, int y, FieldType type) {
    unsigned char* data;
    unsigned long p;
    unsigned char raw_char;
    unsigned int bit_idx;
    assert((type & ~FIELD_MASK) == 0);
    data = map_serialize(map);
    p = map_internal_locate(x, y, map_get_width(map), map_get_height(map));
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

void map_clear(Map* map) {
    memset(map_serialize(map), 0, (unsigned long)MAP_INTERNAL_DATA_SIZE(map_get_width(map),map_get_height(map)));
}

/* ===== map_move ===== */

/* map_move is hard-coded: */
typedef char check_bit_per_field[(BIT_PER_FIELD == 2) ? 1 : -1];

void map_move(Map* map, int by_x, int by_y) {
    int w, h, two_rows_bytes;
    unsigned char* data;

    /* == Initialization.  Nothing fancy. == */
    w = map_get_width(map);
    h = map_get_height(map);
    assert(by_x % 4 == 0);
    assert(by_y % 2 == 0);
    assert(w % 4 == 0);
    assert(h % 2 == 0);
    two_rows_bytes = w / 2;
    assert(two_rows_bytes > 0);
    assert(two_rows_bytes % 2 == 0);
    assert(h * two_rows_bytes / 2 == MAP_INTERNAL_DATA_SIZE(w,h));
    data = map_serialize(map);

    /* Do it in two separate steps, because that's easier,
     * and should be only marginally slower than combined action.
     * (And we don't actually care about performance here,
     * I just wanted this algorithm to be simple, and there's
     * not enough memory for a second buffer.) */

    /* == shift along X-axis == */
    assert(FIELD_UNKNOWN == 0);
    if (abs(by_x) >= w) {
        map_clear(map);
        /* Returning early is just an optimization,
         * and does not change behavior: */
        return;
    } else if (by_x < 0) {
        int y;
        const unsigned int by_x_bytes = (unsigned int)(-by_x/2);
        assert(by_x_bytes < (unsigned int)two_rows_bytes);
        memmove(data, data + by_x_bytes, (unsigned int)(h * two_rows_bytes / 2) - by_x_bytes);
        for (y = 0; y < h; y += 2) {
            memset(data + (y / 2 + 1) * two_rows_bytes - by_x_bytes, 0, by_x_bytes);
        }
    } else if (by_x > 0) {
        int y;
        const unsigned int by_x_bytes = (unsigned int)(by_x/2);
        assert(by_x_bytes < (unsigned int)two_rows_bytes);
        memmove(data + by_x_bytes, data, (unsigned int)(h * two_rows_bytes / 2) - by_x_bytes);
        for (y = 0; y < h; y += 2) {
            memset(data + (y / 2) * two_rows_bytes, 0, by_x_bytes);
        }
    }

    /* == shift along Y-axis == */
    assert(FIELD_UNKNOWN == 0);
    if (abs(by_y) >= h) {
        map_clear(map);
    } else if (by_y < 0) {
        memmove(data,
                data + two_rows_bytes * (-by_y) / 2,
                (unsigned int)((h - (-by_y)) * two_rows_bytes / 2));
        memset(data + (h - (-by_y)) * two_rows_bytes / 2,
               0,
               (unsigned int)(-by_y * two_rows_bytes / 2));
    } else if (by_y > 0) {
        memmove(data + two_rows_bytes * by_y / 2,
                data,
                (unsigned int)((h - by_y) * two_rows_bytes / 2));
        memset(data,
               0,
               (unsigned int)(by_y * two_rows_bytes / 2));
    }
}
