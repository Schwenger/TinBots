/*
 * MOCK Hardware Abstraction Layer
 *
 * The name of this file appears quite often in the Makefile.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* memcpy */

#include "hal.h"
#include "pi.h"
#include "sensors.h" /* Want to define ir_sensors_angles */
#include "mock.h"

const double ir_sensor_angle[6] = {
    0*M_PI/3,
    1*M_PI/3,
    2*M_PI/3,
    3*M_PI/3,
    4*M_PI/3,
    5*M_PI/3
};

static double mot_l;
static double mot_r;

static hal_time global_clock = 0;

void tests_mock_tick(hal_time amount) {
    global_clock += amount;
}

hal_time hal_get_time() {
    return global_clock;
}

void hal_set_speed(double left, double right) {
    mot_l = left;
    mot_r = right;
}

double hal_get_speed_right(void) {
    return mot_r;
}

double hal_get_speed_left(void) {
    return mot_l;
}

void hal_set_led(unsigned int led, unsigned int value) {
    /* Nothing to do here. */
    (void)led;
    (void)value;
}
void hal_set_front_led(unsigned int value) {
    /* Nothing to do here. */
    (void)value;
}

typedef struct ExpectPackageList {
    unsigned char* data;
    unsigned char* data_orig;
    struct ExpectPackageList* next;
    unsigned int length;
    char command;
    int is_broadcast;
} ExpectPackageList;

static int expect_enabled = 1;
/* Needs to be a deep copy */
static ExpectPackageList* expect_list;

void hal_send_put(char* buf, unsigned int length) {
    int cmp;
    unsigned int i;

    if (!expect_enabled) {
        return;
    }

    assert(expect_list);
    assert(expect_list->length >= length);
    assert(length > 0);
    assert(expect_list->data);
    cmp = memcmp(expect_list->data, buf, length);
    if (cmp) {
        printf("hal_send_put() used with unexpected data:\n");
        printf("\tExpected: 0x");
        for (i = 0; i < length; ++i) {
            printf("%02x", expect_list->data[i]);
        }
        printf("\n\tActual:   0x");
        for (i = 0; i < length; ++i) {
            printf("%02x", buf[i]);
        }
        printf("\n\tAlready received: %lu bytes\n\tTrailing (not printed): %u bytes\n",
            (unsigned long)(expect_list->data - expect_list->data_orig),
            expect_list->length - length);
        assert(0);
    }
    expect_list->data += length;
    expect_list->length -= length;
}

void hal_send_done(char command, int is_broadcast) {
    ExpectPackageList* next;

    if (!expect_enabled) {
        return;
    }

    assert(expect_list);
    assert(expect_list->length == 0); /* Remaining data size */
    assert(expect_list->command == command);
    assert(expect_list->is_broadcast == is_broadcast);
    free(expect_list->data_orig);
    next = expect_list->next;
    free(expect_list);
    expect_list = next;
}

void hal_print(const char* message) {
    printf("%s\n", message);
}

static double debug_info[DEBUG_CAT_NUM] = {0};

void hal_debug_out(DebugCategory key, double value) {
    assert(key < DEBUG_CAT_NUM);
    debug_info[key] = value;
}

double tests_mock_get_debug(DebugCategory cat) {
    assert(cat < DEBUG_CAT_NUM);
    return debug_info[cat];
}

void tests_mock_expect_next(const ExpectPackage* pkg) {
    ExpectPackageList* last;
    ExpectPackageList* next;

    assert(expect_enabled);
    /* Use malloc everywhere so that valgrind can find errors. */
    next = malloc(sizeof(ExpectPackageList));
    next->command = pkg->command;
    next->is_broadcast = pkg->is_broadcast;
    assert(pkg->length < 128); /* TIN_PACKAGE_MAX_LENGTH */
    next->length = pkg->length;
    next->next = NULL;
    if (pkg->length) {
        /* This buffer will be read by other tinbots, and therefore "needs" to be
         * 2-byte aligned.  However, x86 is pretty permissive, meaning the access
         * will be a bit slower, in the unlikely event that malloc() gives us
         * a 2-bytes-unaligned address. */
        next->data_orig = malloc(pkg->length);
        assert(pkg->data);
        memcpy(next->data_orig, pkg->data, pkg->length);
        next->data = next->data_orig;
    } else {
        assert(!pkg->data);
        next->data = next->data_orig = NULL;
    }

    if (!expect_list) {
        expect_list = next;
    } else {
        last = expect_list;
        while (last->next) {
            last = last->next;
        }
        last->next = next;
    }
}

void tests_mock_expect_set_enabled(int is_enabled) {
    assert(expect_enabled != is_enabled);
    tests_mock_expect_assert_done();
    expect_enabled = is_enabled;
}

void tests_mock_expect_assert_done(void) {
    assert(!expect_list);
}

void __assert_hal(const char *msg, const char *file, int line) {
    printf("%s:%d: Assertion failed: %s\n", file, line, msg);
    abort();
}
