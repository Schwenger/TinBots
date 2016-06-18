#define F_CPU 8000000UL

#include "neatavr.hpp"

#include <util/twi.h>

#define I2C_ADDRESS 0x42

using namespace NeatAVR;

class TWIControl: public Register8<_SFR_ADDR(TWCR)> {
public:
    typedef Bit0 Interrupt;
    typedef Bit2 Enable;
    typedef Bit3 Colision;
    typedef Bit4 StopCondition;
    typedef Bit5 StartCondition;
    typedef Bit6 Acknowledge;
    typedef Bit7 Ready;
};

class TWIAddress: public Register8<_SFR_ADDR(TWAR)> {
public:
    typedef Slice<TWIAddress, 1, 7> Address;
    typedef Bit0 GenericCalls;
};

class TWIStatus: public Register8<_SFR_ADDR(TWSR)> {
public:
    typedef Slice<TWIStatus, 3, 5> Code;
};

typedef Pin14 LED;

int main() {
    LED::output();
    //LED::on();

    TWIAddress::set(I2C_ADDRESS);
    TWIAddress::GenericCalls::enable();

    TWIControl::Enable::on();
    TWIControl::Acknowledge::on();
    TWIControl::Interrupt::enable();

    System::enable_interrupts();

    while(1) { asm volatile ("nop"); }
}

ISR(TWI_vect, ISR_BLOCK) {
    TWIStatus::Code::get();
    LED::on();
}