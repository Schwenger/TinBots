#define F_CPU 8000000UL

#include "neatavr.hpp"

#include "neatavr/serial.hpp"
#include "neatavr/twi.hpp"

#include <stdio.h>
#include <string.h>

#include <util/delay.h>

#include <util/twi.h>

#define I2C_ADDRESS 0x42

using namespace NeatAVR;


typedef Pin14 LED;


// IR Detector
template<typename Pin> class IRDetector {
private:
    volatile uint16 counter;

    volatile uint8 state;
    volatile uint8 pulses;
public:
    volatile uint8 active;

    void init() {
        Pin::input();
    }

    void detect() {
        uint8 next_state = Pin::read();
        if (state == next_state) {
            counter++;
            if (counter > 4000) {
                counter = 0;
                active = 0;
            }
        } else {
            if (counter > 15 || counter < 4) {
                pulses = 0;
            } else {
                pulses++;
            }
            if (pulses > 6) {
                active = 1;
            }
            counter = 0;
        }
        state = next_state;
    }
};


IRDetector<Pin12> ir0;
IRDetector<Pin11> ir1;
IRDetector<Pin10> ir2;
IRDetector<Pin9>  ir3;
IRDetector<Pin16> ir4;
IRDetector<Pin15> ir5;


volatile uint8 sensor_data = 0;


int main() {
    Serial::init();

    Serial::printline("\n\nTin Bot Extension Board v0.1");
    Serial::printline("Initializing");

    LED::output();

    Timer::init(Timer::Prescaler::DIV_8);
    Timer::WaveGeneration::set(Timer::WaveGeneration::CTC);

    Timer::ChannelA::Interrupt::enable();
    Timer::ChannelA::Compare::set(50);

    TWI::Address::set(I2C_ADDRESS);

    TWI::Control::Enable::on();
    TWI::Control::Acknowledge::on();
    TWI::Control::Interrupt::enable();

    ir0.init();
    ir1.init();
    ir2.init();
    ir3.init();
    ir4.init();
    ir5.init();

    System::enable_interrupts();

    Serial::printline("Running");

    char message[64];

    while (1) {
        _delay_ms(200);

        memset(message, 0, 64);
        sprintf(message, "Sensors: %i %i %i %i %i %i", ir0.active, ir1.active, ir2.active, ir3.active, ir4.active, ir5.active);
        Serial::printline(message);
    }
}


INTERRUPT_ROUTINE(TIMER_COMPARE_A_INTRRUPT) {
    // executed every 50us
    ir0.detect();
    ir1.detect();
    ir2.detect();
    ir3.detect();
    ir4.detect();
    ir5.detect();
    sensor_data = ((ir5.active << 5) | (ir4.active << 4) | (ir3.active << 3) |
                   (ir2.active << 2) | (ir1.active << 1) | ir0.active);
    LED::write(sensor_data);
}


// very simple ISR only suitable for transmitting one byte
INTERRUPT_ROUTINE(TWI_INTERRUPT) {
    switch (TWI::status()) {
        case TW_SR_SLA_ACK:
            TWI::acknowledge();
            //position = 0;
            //available = 0;
            break;
        case TW_SR_DATA_ACK:
            //buffer[position] = (char) TWI::get();
            //position++;
            TWI::acknowledge();
            break;
        case TW_ST_SLA_ACK:
        case TW_ST_DATA_ACK:
            TWI::put(sensor_data);
            TWI::acknowledge();
            //LED::on();
            break;
        case TW_ST_DATA_NACK:
            TWI::acknowledge();
            break;
        default:
            //buffer[position] = 0;
            //available = 1;
            TWI::acknowledge();
    }
}