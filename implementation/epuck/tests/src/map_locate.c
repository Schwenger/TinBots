#include <stdio.h>

#include "hal.h"
#include "map.h"

static int report(unsigned long rhs_val) {
    printf("rhs value was %lu instead!\n", rhs_val);
    return 0;
}

#define assert_eq(A,B) assert((A == (B)) || report(B))

int main() {
    assert_eq(BIT_PER_FIELD * 0, map_internal_locate(0, 0, 4, 2));
    assert_eq(BIT_PER_FIELD * 0, map_internal_locate(0, 0, 4, 4));
    assert_eq(BIT_PER_FIELD * 0, map_internal_locate(0, 0, 8, 2));
    assert_eq(BIT_PER_FIELD * 1, map_internal_locate(1, 0, 4, 2));
    assert_eq(BIT_PER_FIELD * 1, map_internal_locate(1, 0, 4, 4));
    assert_eq(BIT_PER_FIELD * 1, map_internal_locate(1, 0, 8, 2));
    assert_eq(BIT_PER_FIELD * 2, map_internal_locate(2, 0, 4, 2));
    assert_eq(BIT_PER_FIELD * 2, map_internal_locate(2, 0, 4, 4));
    assert_eq(BIT_PER_FIELD * 2, map_internal_locate(2, 0, 8, 2));
    assert_eq(BIT_PER_FIELD * 3, map_internal_locate(3, 0, 4, 2));
    assert_eq(BIT_PER_FIELD * 3, map_internal_locate(3, 0, 4, 4));
    assert_eq(BIT_PER_FIELD * 3, map_internal_locate(3, 0, 8, 2));
    assert_eq(BIT_PER_FIELD * 4, map_internal_locate(0, 1, 4, 2));
    assert_eq(BIT_PER_FIELD * 4, map_internal_locate(0, 1, 4, 4));
    assert_eq(BIT_PER_FIELD * 4, map_internal_locate(0, 1, 8, 2));
    assert_eq(BIT_PER_FIELD * 7, map_internal_locate(3, 1, 4, 2));
    assert_eq(BIT_PER_FIELD * 7, map_internal_locate(3, 1, 4, 4));
    assert_eq(BIT_PER_FIELD * 7, map_internal_locate(3, 1, 8, 2));

    assert_eq(BIT_PER_FIELD * 8, map_internal_locate(4, 0, 8, 2));
    assert_eq(BIT_PER_FIELD * 8, map_internal_locate(0, 2, 4, 4));
    assert_eq(BIT_PER_FIELD * 15, map_internal_locate(7, 1, 8, 2));
    assert_eq(BIT_PER_FIELD * 15, map_internal_locate(3, 3, 4, 4));

    printf("\t=> done (GOOD)\n");

    return 0;
}
