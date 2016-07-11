#include <assert.h>
#include <stdio.h>

#include "commands.h"
#include "map_heap.h"
#include "mock.h"
#include "t2t-parse.h"

static void test_heartbeat(void) {
    ExpectPackage pkg = {CMD_T2T_HEARTBEAT, 0, NULL};
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_heartbeat();
    tests_mock_expect_assert_done();
}

/* Untestable due to new format
static void test_found_phi1(void) {
    char data[4] = {42, 7, 0, 0};
    ExpectPackage pkg = {CMD_T2T_VICTIM_PHI, 4, NULL};
    pkg.data = data;
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_found_phi(42, 7, 0);
    tests_mock_expect_assert_done();
}

static void test_found_phi2(void) {
    char data[4] = {0, 99, 0, 1};
    ExpectPackage pkg = {CMD_T2T_VICTIM_PHI, 4, NULL};
    pkg.data = data;
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_found_phi(0, 99, 256.001 / 1000.0);
    tests_mock_expect_assert_done();
}
*/

static void test_found_xy(void) {
    char data[4] = {12, 34, 56};
    ExpectPackage pkg = {CMD_T2T_VICTIM_XY, 3, NULL};
    pkg.data = data;
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_found_xy(12, 34, 56);
    tests_mock_expect_assert_done();
}

static void test_update_map(void) {
    Map* m;
    char data[2 + MAP_PROXIMITY_BUF_SIZE] = {0};
    ExpectPackage pkg = {CMD_T2T_UPDATE_MAP, 2 + MAP_PROXIMITY_BUF_SIZE, NULL};
    pkg.data = data;
    tests_mock_expect_assert_done();

    data[0] = 99;
    data[1] = 3;
    /* Positions (4,3)-(7,3) are stored in the byte at offset 11 of the map. */
    data[2 + 11] = (char)0x89; /* 0b10 00 10 01 */
    m = map_heap_alloc(MAP_PROXIMITY_SIZE, MAP_PROXIMITY_SIZE);
    map_set_field(m, 4 + 0, 3, 1);
    map_set_field(m, 4 + 1, 3, 2);
    map_set_field(m, 4 + 2, 3, 0);
    map_set_field(m, 4 + 3, 3, 2);

    tests_mock_expect_next(&pkg);
    t2t_send_update_map(99, 3, m);
    tests_mock_expect_assert_done();

    map_heap_free(m);
}

static void test_docked(void) {
    ExpectPackage pkg = {CMD_T2T_DOCKED, 0, NULL};
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_docked();
    tests_mock_expect_assert_done();
}

static void test_completed(void) {
    ExpectPackage pkg = {CMD_T2T_COMPLETED, 0, NULL};
    tests_mock_expect_assert_done();

    tests_mock_expect_next(&pkg);
    t2t_send_completed();
    tests_mock_expect_assert_done();
}

#define RUN(x) x(); printf(#x " passed\n")

int main() {
    RUN(test_heartbeat);
    /*
    RUN(test_found_phi1);
    RUN(test_found_phi2);
    */
    RUN(test_found_xy);
    RUN(test_update_map);
    RUN(test_docked);
    RUN(test_completed);
    printf("\t=> GOOD\n");
    return 0;
}
