#define F_CPU 8000000UL

#include "neatavr.hpp"

#include <avr/delay.h>

using namespace NeatAVR;

typedef Pin9 IREmitter;

volatile uint8 carrier = 1;

void squawk() {
    for (uint8 counter = 0; counter < 6; counter++) {
        carrier = 1;
        _delay_us(600);
        carrier = 0;
        _delay_us(600);
    }
}

int main() {
    IREmitter::output();

    Timer::init(Timer::Prescaler::DIV_8);

    Timer::Interrupt::enable();
    Timer::WaveGeneration::set(Timer::WaveGeneration::CTC);

    Timer::ChannelA::Interrupt::enable();
    Timer::ChannelA::Compare::set(14);

    System::enable_interrupts();

    while (1) {
        _delay_ms(500);
        squawk();
    }
}

INTERRUPT_ROUTINE(TIMER_COMPARE_A_INTRRUPT) {
    if (carrier) {
        IREmitter::toggle();
    } else {
        IREmitter::off();
    }
}