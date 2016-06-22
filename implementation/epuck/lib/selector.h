#ifndef EPUCK_SELECTOR_H
#define EPUCK_SELECTOR_H

#include "p30F6014A.h"

#include "utils.h"

#define SELECTOR0_STATUS _RG6
#define SELECTOR1_STATUS _RG7
#define SELECTOR2_STATUS _RG8
#define SELECTOR3_STATUS _RG9

#define SELECTOR0_DIR _TRISG6
#define SELECTOR1_DIR _TRISG7
#define SELECTOR2_DIR _TRISG8
#define SELECTOR3_DIR _TRISG9

void init_selector(void) {
    SELECTOR0_DIR = INPUT;
    SELECTOR1_DIR = INPUT;
    SELECTOR2_DIR = INPUT;
    SELECTOR3_DIR = INPUT;
}

unsigned int get_selector(void) {
    return (SELECTOR3_STATUS << 3) | (SELECTOR2_STATUS << 2) | (SELECTOR1_STATUS << 1) | SELECTOR0_STATUS;
}

#endif
