#ifndef EPUCK_UTILS_H
#define EPUCK_UTILS_H

#include "p30F6014A.h"

#include "stdbool.h"

#define ON 1
#define OFF 0

#define OUTPUT 0
#define INPUT 1

// shamelessly stolen from original library
#define FOSC   7.3728e6     // 7.3728MHz crystal in XTL mode
#define PLL    8.0       	// 8x PLL

#define FCY         ((FOSC*PLL)/(4.0))	// instruction cycle frequency
#define MILLISEC    (FCY/1.0e3)		    // 1ms delay constant
#define MICROSEC    (FCY/1.0e6)		    // 1us delay constant
#define NANOSEC     (FCY/1.0e9)		    // 1ns delay constant

#define DISABLE_INTERRUPTS() { __asm__ volatile ("disi #10000"); }
#define ENABLE_INTERRUPTS() { __asm__ volatile ("disi #2"); }

#endif
