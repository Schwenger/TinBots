#include <stdio.h>

#include "hal.h"
#include "pi.h"
#include "victim-finder.h"

typedef struct Measurement {
    double x, y, phi;
} Measurement;

typedef struct Test {
    Measurement m1;
    Measurement m2;
    struct {double x; double y;} res;
} Test;

int main() {
    VFState vfs;
    VFInputs in;
    Sensors sens;
    const Test the_test = {
        {45, 50, 0*M_PI/2}, {50, 45, 1*M_PI/2}, {50, 50}
    };

    vf_reset(&vfs);

    in.found_victim_phi = 1;
    sens.current.x = the_test.m1.x;
    sens.current.y = the_test.m1.y;
    in.victim_angle = the_test.m1.phi;
    vf_step(&in, &vfs, &sens);
    assert(!vfs.found_victim_xy);
    in.found_victim_phi = 0;
    vf_step(&in, &vfs, &sens);
    assert(!vfs.found_victim_xy);

    in.found_victim_phi = 1;
    sens.current.x = the_test.m2.x;
    sens.current.y = the_test.m2.y;
    in.victim_angle = the_test.m2.phi;
    vf_step(&in, &vfs, &sens);
    assert(vfs.found_victim_xy);
    in.found_victim_phi = 0;
    vf_step(&in, &vfs, &sens);
    assert(vfs.found_victim_xy);
    printf("Actual %.1f,%.1f expected %.1f,%.1f\n",
        vfs.victim_x, vfs.victim_y, the_test.res.x, the_test.res.y);
    if (fabs(vfs.victim_x - the_test.res.x) < 0.01
        || fabs(vfs.victim_y - the_test.res.y) < 0.01) {
        printf("\t=> and that's GOOD!\n");
    } else {
        printf("\t=> and that's BAD!\n");
        return -1;
    }

    return 0;
}
