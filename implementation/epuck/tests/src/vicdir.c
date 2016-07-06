#include <assert.h>
#include <stdio.h>

#include "pi.h"
#include "stub.h"
#include "victim-direction.h"

#define IR_COMPLETION_TIME (16651UL)
#define IR_PASS_TIME ((hal_time)((IR_COMPLETION_TIME*40)/360))

static const double true_victim_phi = 10 * M_PI / 180;

static int cyclic_within_p(hal_time begin, hal_time now, hal_time end) {
    /* while (end < begin) {
        end += IR_COMPLETION_TIME;
    } */
    while (now < begin) {
        /* 'mod' might be faster, but whatever. */
        now += IR_COMPLETION_TIME;
    }
    return now <= end;
}

int main() {
    VDState vds;
    Sensors sens;
    int i;
    hal_time core_time[NUM_IR];

    /* Initialization */
    vd_reset(&vds);
    sens.current.x = 0;
    sens.current.y = 0;
    sens.current.direction = 0;
    for (i = 0; i < NUM_IR; ++i) {
        const double core_angle = true_victim_phi + 2 * M_PI - ir_sensor_angle[i];
        core_time[i] = (hal_time)( core_angle * IR_COMPLETION_TIME / (2 * M_PI) );
    }
    assert(6 == NUM_IR);
    printf("Core times: %07lu %07lu %07lu %07lu %07lu %07lu\n",
        core_time[0], core_time[1], core_time[2], core_time[3], core_time[4], core_time[5]);
    printf("Pass time:  %07lu\n", IR_PASS_TIME);

    /* Running */
    while (hal_get_time() < 20000) {
        for (i = 0; i < NUM_IR; ++i) {
            sens.ir[i] = cyclic_within_p(core_time[i] - IR_PASS_TIME, hal_get_time(), core_time[i] + IR_PASS_TIME);
        }
        vd_step(&vds, &sens);
        tests_stub_tick(10);
        if (hal_get_time() % 200 == 0) {
            printf("Debug data:");
            for (i = 0; i < DEBUG_CAT_NUM; ++i) {
                printf(" %7.2f", tests_stub_get_debug((DebugCategory)i));
            }
            printf("\n");
        }
    }

    printf("Hello world!\n");
    return 0;
}
