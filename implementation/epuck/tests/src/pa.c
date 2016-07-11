#include <assert.h>
#include <stdio.h>

#include "pickup-artist.h"

#include "mock.h"

int main() {
    PickupState ps;
    hal_time i;

    pa_reset(&ps);
    assert(!ps.is_dead);
    tests_mock_tick(0xBEEF); /* Should not depend on this value */

    printf("Watch this, I can make the pickup-artist give up:\n-----\n");
    for (i = 0; i < 200; ++i) {
        pa_step(&ps);
        assert(!(ps.is_dead) == !(hal_get_time() - 0xBEEF >= 10000));
        tests_mock_tick(i);
    }
    printf("-----\nTold ya so.\n");

    return 0;
}
