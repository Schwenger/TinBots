#include "hal.h"
#include "path-finder.h"
#include "pi.h"
#include "victim-finder.h"

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

static ExactPosition compute_position(double* data);

void vf_reset(VFState* vf) {
    int i;
    vf->locals.state = VF_noinfo;
    vf->found_victim_xy = 0;
    vf->victim_x = -1;
    vf->victim_y = -1;
    for(i = 0; i < 6; ++i) {
        vf->locals.data[i] = 0;
    }
}

static ExactPosition compute_position(double* data) {
    ExactPosition pos;
    int sanity1, sanity2;
    double v1[2], v1_orth[2], v2[2], v2_orth[2], o1[2], o2[2];
    double M[4], b[2], xy[2];
    v1[0] = cos(data[PHI1]);
    v1[1] = sin(data[PHI1]);
    v1_orth[0] = -v1[1];
    v1_orth[1] =  v1[0];
    v2[0] = cos(data[PHI2]);
    v2[1] = sin(data[PHI2]);
    v2_orth[0] = -v2[1];
    v2_orth[1] =  v2[0];
    o1[0] = data[X1];
    o1[1] = data[Y1];
    o2[0] = data[X2];
    o2[1] = data[Y2];

    M[0] = v1_orth[0];
    M[1] = v1_orth[1];
    M[2] = v2_orth[0];
    M[3] = v2_orth[1];
    b[0] = v1_orth[0] * o1[0] + v1_orth[1] * o1[1];
    b[1] = v2_orth[0] * o2[0] + v2_orth[1] * o2[1];

    xy[0] = (b[0] * M[3] - b[1] * M[1]) / (M[0] * M[3] - M[1] * M[2]);
    xy[1] = (b[1] * M[0] - M[2] * b[0]) / (M[0] * M[3] - M[1] * M[2]);

    /*
      % Sanity check
      if v1_orth * ([x; y] - o1) >= 0.1 ...
      || v2_orth * ([x; y] - o2) >= 0.1
      x = -1;
      y = -1;
      end
    */

    pos.x = xy[0];
    pos.y = xy[1];
    sanity1 = v1_orth[0] * (xy[0] - o1[0]) + v1_orth[1] * (xy[1] - o1[1]) < 0.1;
    sanity2 = v2_orth[0] * (xy[0] - o2[0]) + v2_orth[1] * (xy[1] - o2[1]) < 0.1;
    if(!sanity1 || !sanity2){
        pos.x = -1;
        pos.y = -1;
    }
    /*
    printf(
        "Have (%f|%f), which has a distance of %f and %f each.\n",
        xy[0], xy[1],
        v1_orth[0] * (xy[0] - o1[0]) + v1_orth[1] * (xy[1] - o1[1]),
        v2_orth[0] * (xy[0] - o2[0]) + v2_orth[1] * (xy[1] - o2[1]));
    printf(
        "  %08.2f"" %08.2f""     x     %08.2f\n"
        "                     *     =\n"
        "  %08.2f"" %08.2f""     y     %08.2f\n",
        M[0], M[1], b[0],
        M[2], M[3], b[1]);
    printf(
        "  From input data:\n"
        "  %8.2f %8.2f %8.2f\n"
        "  %8.2f %8.2f %8.2f\n",
        data[X1], data[Y1], data[PHI1],
        data[X2], data[Y2], data[PHI2]);
    */
    return pos;
}

void vf_step(VFInputs* inputs, VFState* vf, Sensors* sens) {
    ExactPosition computed_victim;
    switch(vf->locals.state) {
        case VF_noinfo:
            if(inputs->found_victim_phi) {
                vf->locals.data[X1]   = sens->current.x;
                vf->locals.data[Y1]   = sens->current.y;
                vf->locals.data[PHI1] = inputs->victim_angle;
                vf->locals.state = VF_someinfo;
            }
            break;
        case VF_someinfo:
            if(inputs->found_victim_phi == 0) {
                vf->locals.state = VF_waitfornewinfo;
            }
            break;
        case VF_waitfornewinfo:
            if(inputs->found_victim_phi) {
                vf->locals.state = VF_enoughinfo;
                vf->locals.data[X2]   = vf->locals.data[X1];
                vf->locals.data[Y2]   = vf->locals.data[Y1];
                vf->locals.data[PHI2] = vf->locals.data[PHI1];
                vf->locals.data[X1]   = sens->current.x;
                vf->locals.data[Y1]   = sens->current.y;
                vf->locals.data[PHI1] = inputs->victim_angle;
                computed_victim = compute_position(vf->locals.data);
                if(computed_victim.x >= 1 && computed_victim.x <= 200
                        && computed_victim.y >= 1 && computed_victim.y <= 200) {
                    vf->victim_x = computed_victim.x;
                    vf->victim_y = computed_victim.y;
                    vf->found_victim_xy = 1;
                    /* DO NOT send the T2T paket yet.
                     * That's the moderator's job. */
                }
            }
            break;
        case VF_enoughinfo:
            if(inputs->found_victim_phi == 0) {
                vf->locals.state = VF_waitfornewinfo;
            }
            break;
        default:
            hal_print("Invalid state in victim finder. VICTOR, where are you??");
            assert(0);
            vf_reset(vf);
    }
}
