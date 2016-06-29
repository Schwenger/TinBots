#include <hal/hal.h>
#include <assert.h>
#include <pi.h>
#include "victim-finder.h"
#include "pathfinder.h"

#define X1   0
#define Y1   1
#define PHI1 2
#define X2   3
#define Y2   4
#define PHI2 5

enum {
    VF_noinfo,
    VF_someinfo,
    VF_waitfornewinfo,
    VF_enoughinfo
};

static Position compute_position(double* data);

void vf_reset(VFState* vf) {
    int i;
    vf->found_victim_xy = 0;
    vf->victim_x = -1;
    vf->victim_y = -1;
    for(i = 0; i < 6; ++i) {
        vf->locals.data[i] = 0;
    }
}

static Position compute_position(double* data) {
    double v1_x, v1_y, v1_orth_x, v1_orth_y;
    double v2_x, v2_y, v2_orth_x, v2_orth_y;
    double o1_x, o1_y, o2_x, o2_y;
    double M[4];
    double b_x, b_y, x, y;
    v1_x = cos(data[PHI1]);
    v1_y = sin(data[PHI1]);
    v1_orth_x = -v1_y;
    v1_orth_y =  v1_x;
    v2_x = cos(data[PHI2]);
    v2_y = sin(data[PHI2]);
    v2_orth_x = -v2_y;
    v2_orth_y =  v2_x;
    o1_x = data[X1];
    o1_y = data[Y1];
    o2_x = data[X2];
    o2_y = data[Y2];

    M[0] = v1_orth_x;
    M[1] = v1_orth_y;
    M[2] = v2_orth_x;
    M[3] = v2_orth_y;

//    M = [v1_orth; v2_orth];
//    b = [v1_orth*o1; v2_orth*o2];
//    xy = mldivide(M, b); % Solve equation system: M * xy = b
//    x = xy(1);
//    y = xy(2);
//
//    % Sanity check
//    if v1_orth * ([x; y] - o1) >= 0.1 ...
//    || v2_orth * ([x; y] - o2) >= 0.1
//    x = -1;
//    y = -1;
//    end
}

void vf_step(VFInputs* inputs, VFState* vf, Sensors* sens) {
    Position computed_victim;
    switch(vf->state) {
        case VF_noinfo:
            if(inputs->found_victim_phi) {
                vf->locals.data[X1]   = sens->current.x;
                vf->locals.data[Y1]   = sens->current.y;
                vf->locals.data[PHI1] = sens->current.direction;
                vf->state = VF_someinfo;
            }
            break;
        case VF_someinfo:
            if(inputs->found_victim_phi == 0) {
                vf->state = VF_waitfornewinfo;
            }
            break;
        case VF_waitfornewinfo:
            if(inputs->found_victim_phi) {
                vf->state = VF_enoughinfo;
                vf->locals.data[X2]   = vf->locals.data[X1];
                vf->locals.data[Y2]   = vf->locals.data[Y1];
                vf->locals.data[PHI2] = vf->locals.data[PHI1];
                vf->locals.data[X1]   = sens->current.x;
                vf->locals.data[Y1]   = sens->current.y;
                vf->locals.data[PHI1] = sens->current.direction;
                computed_victim = compute_position(vf->locals.data);
                if(computed_victim.x >= 1 && computed_victim.x <= 200
                        && computed_victim.y >= 1 && computed_victim.y <= 200) {
                    vf->victim_x = computed_victim.x;
                    vf->victim_y = computed_victim.y;
                    vf->found_victim_xy = 1;
                }
            }
            break;
        case VF_enoughinfo:
            if(inputs->found_victim_phi == 0) {
                vf->state = VF_waitfornewinfo;
            }
            break;
        default:
            hal_print("Invalid state in victim finder. VICTOR, where are you??");
            assert(0);
            vf_reset(vf);
    }
}
