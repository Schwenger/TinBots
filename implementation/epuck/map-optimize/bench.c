#include <stddef.h> /* size_t */
#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* clock */

#include "hal.h"
#include "map.h"

hal_time hal_get_time(void);

void hal_set_speed(double left, double right);

double hal_get_speed_left(void);
double hal_get_speed_right(void);

void hal_set_led(unsigned int led, unsigned int value);
void hal_set_front_led(unsigned int value);

void hal_send_put(char* buf, unsigned int length);

void hal_send_done(char command);

void hal_print(const char *message);

void hal_debug_out(DebugCategory key, double value);

void __assert_hal(const char *msg, const char *file, int line) {
    printf("Dafuq, assertion failed: %s:%d: %s\n", file, line, msg);
    abort();
}

int main() {
    static const int iterations = 1000 * 1000;
    Map* accu;
    Map* prox;
    int i;
    clock_t time;

    accu = map_get_accumulated();
    prox = map_get_proximity();
    map_clear(accu);
    map_clear(prox);

    while (1) {
        time = clock();
        for (i = 0; i < iterations; ++i) {
            map_merge(accu, 4, 2, prox);
        }
        time = clock() - time;
        printf("%.3f us/iter\n", time * 1000000.0 / (iterations * CLOCKS_PER_SEC));
    }
}
