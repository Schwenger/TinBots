#ifndef TESTS_STUB_H
#define TESTS_STUB_H

#include "hal.h"

void tests_stub_tick(hal_time amount);

double tests_stub_get_debug(DebugCategory cat);

typedef struct ExpectPackage {
    char command;
    unsigned int length;
    char* data;
} ExpectPackage;

/* Can be called multiple times, even with static const things. */
void tests_stub_expect_next(const ExpectPackage* pkg);

void tests_stub_expect_assert_done(void);

#endif
