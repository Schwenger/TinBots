#include <stddef.h>
#include <stdint.h>

#include "hal.h" /* assert */
#include "map.h"

#ifndef CONFIG_MAP_ASSERT_ANYWAY
#  undef assert
#  define assert(x) (void)sizeof(x)
#endif

typedef char check_neighborhood_map_length[(MAP_PROXIMITY_BUF_SIZE == MAP_INTERNAL_DATA_SIZE(MAP_PROXIMITY_SIZE,MAP_PROXIMITY_SIZE)) ? 1 : -1];
typedef char check_max_is_valid[(0 < MAP_INTERNAL_DATA_SIZE(MAP_MAX_WIDTH,MAP_MAX_HEIGHT)) ? 1 : -1];

/* map_move and map_merge have to be hard-coded: */
typedef char check_bit_per_field[(BIT_PER_FIELD == 2) ? 1 : -1];

/* This implementation is highly optimized, but has lots of assumptions.
 * So I'll just go forward and assume BIT_PER_FIELD = 2.
 * If you change BIT_PER_FIELD, you have to rewrite quite a bit.
 *
 * Also, don't worry: in contrast to LuckySergionattarellaDeuce,
 * I'm intent on getting it perfectly right.
 * [LSD] https://github.com/RemyPorter/TDWTF-BYOC-Casino/tree/master/Week1/ben#all-about-luckysergiomattarelladeuce
 *
 * This function should take 9 instructions.
 * FIXME: Can someone verify that with a disassembler?
 * FIXME: Inline flags?  ('inline' is not C90, so need some clever matlab-checking)
 *
 * The PIC chips throw an exception on unaligned data access!
 * The reads and writes (done by the caller) *must* be properly aligned! */
typedef char check_map_merge_byte_bitlength[(BIT_PER_FIELD == 2) ? 1 : -1];
static uint16_t merge_u16(uint16_t previously, uint16_t input) {
    /* Create a mask of all 'fields' that "are not unknown",
     * which is equivalent to "are not 0b00". */
    const uint16_t mask = input | (input & 0xAAAA) >> 1 | (input & 0x5555) << 1;
    /* Remove information that shall be overwritten. */
    previously &= ~mask;
    /* "or"-in all new bits. */
    return previously | input;
}

/* This implementation, too, is highly optimized.
 * Hard requirements (if any of these is not met, then this doesn't work):
 * - the lower-left corner of the patch is in a "nice" spot (assert)
 *      -> guaranteed by prox_map.c
 * - the patch must fit *wholly* in the map (assert)
 *      -> guaranteed by prox_map.c (t2t.c should assert this, too)
 * - the internal map must be of MAP_MAX_* size (assert)
 * - the patch must be a proximity map (assert)
 * - the internal map must be 2-byte aligned (attempted assert)
 *      -> guaranteed by map_stack.c, map_heap.c
 * - the patch (incoming packet buffer) must be 2-byte aligned (attempted assert)
 *      -> guaranteed by tinpuck/com.c
 *      -> guaranteed by hal/hal_matlab.c
 *      -> guaranteed by tests/mock.c
 * - BIT_PER_FIELD==2 (previous typedef-checks in the file) */
void map_merge(Map* dst, int low_left_x, int low_left_y, Map* patch) {
    uint16_t* dst_data;
    uint16_t* patch_data;

    /* == Assert all assumptions == */
    assert(map_get_width(patch) % 4 == 0);
    /* Lower-left in "nice spot" */
    assert(low_left_x % 4 == 0);
    assert(low_left_y % 2 == 0);
    /* Fit wholly in map: */
    assert(map_get_width(patch) <= map_get_width(dst));
    assert(map_get_height(patch) <= map_get_height(dst));
    assert(low_left_x >= 0);
    assert(low_left_y >= 0);
    assert(low_left_x + map_get_width(patch) <= map_get_width(dst));
    assert(low_left_y + map_get_height(patch) <= map_get_height(dst));
    /* Is *the* map: */
    assert(map_get_width(dst) == MAP_MAX_WIDTH);
    assert(map_get_height(dst) == MAP_MAX_HEIGHT);
    /* Is proximity map: */
    assert(map_get_width(patch) == MAP_PROXIMITY_SIZE);
    assert(map_get_height(patch) == MAP_PROXIMITY_SIZE);
    /* "Alignedness": */
    assert((((ptrdiff_t)(map_serialize(patch))) & 1) == 0);
    assert((((ptrdiff_t)(map_serialize(dst))) & 1) == 0);

    /* == Finally, the actual code == */
    /* We establish a view where the map consists of "blocks" that are precisely
     * 4 fields wide and 2 fields high, and fit precisely into a uin16_t.
     * In this view, *_data behave like "normally" indexed maps, i.e., x+y*w */
    #define PROX_CELL_TWO_ROWS_U16S (MAP_PROXIMITY_SIZE/4)
    assert(4 == PROX_CELL_TWO_ROWS_U16S);
    #define DST_CELL_TWO_ROWS_U16S (MAP_MAX_WIDTH/4)
    /* If fails in testing, check whether you used a different map size: */
    assert(5 == DST_CELL_TWO_ROWS_U16S || 25 == DST_CELL_TWO_ROWS_U16S);
    dst_data = (uint16_t*)map_serialize(dst); /* BLESSED CAST */
    patch_data = (uint16_t*)map_serialize(patch); /* BLESSED CAST */
    dst_data += (low_left_x / 4) + (low_left_y / 2) * DST_CELL_TWO_ROWS_U16S;

    /* Manually unroll LITERALLY EVERYTHING because I expect xc16 to be
     * too dumb for that. */
    dst_data[0 + 0 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 0 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 0 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 0 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 0 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 0 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 0 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 0 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 0 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 0 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 0 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 0 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 1 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 1 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 1 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 1 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 1 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 1 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 1 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 1 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 1 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 1 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 1 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 1 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 2 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 2 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 2 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 2 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 2 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 2 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 2 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 2 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 2 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 2 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 2 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 2 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 3 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 3 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 3 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 3 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 3 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 3 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 3 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 3 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 3 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 3 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 3 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 3 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 4 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 4 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 4 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 4 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 4 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 4 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 4 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 4 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 4 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 4 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 4 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 4 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 5 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 5 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 5 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 5 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 5 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 5 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 5 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 5 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 5 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 5 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 5 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 5 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 6 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 6 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 6 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 6 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 6 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 6 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 6 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 6 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 6 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 6 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 6 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 6 * PROX_CELL_TWO_ROWS_U16S]);

    dst_data[0 + 7 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + 7 * DST_CELL_TWO_ROWS_U16S], patch_data[0 + 7 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + 7 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + 7 * DST_CELL_TWO_ROWS_U16S], patch_data[1 + 7 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + 7 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + 7 * DST_CELL_TWO_ROWS_U16S], patch_data[2 + 7 * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + 7 * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + 7 * DST_CELL_TWO_ROWS_U16S], patch_data[3 + 7 * PROX_CELL_TWO_ROWS_U16S]);
}
