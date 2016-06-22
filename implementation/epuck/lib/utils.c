#include "utils.h"

void e_enable_interrupts() {
    ENABLE_INTERRUPTS();
}

void e_disable_interrupts() {
    DISABLE_INTERRUPTS();
}

void __e_cleanup(unsigned char* _) {
    ENABLE_INTERRUPTS();
}
