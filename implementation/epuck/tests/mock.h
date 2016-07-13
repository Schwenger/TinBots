#ifndef TESTS_MOCK_H
#define TESTS_MOCK_H

#include "hal.h"

void tests_mock_tick(hal_time amount);

double tests_mock_get_debug(DebugCategory cat);

typedef struct ExpectPackage {
    char command;
    unsigned int length;
    unsigned char* data;
} ExpectPackage;

/* Can be called multiple times, even with static const things. */
void tests_mock_expect_next(const ExpectPackage* pkg);

void tests_mock_expect_assert_done(void);

#endif
