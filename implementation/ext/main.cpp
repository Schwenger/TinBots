#define F_CPU 8000000UL

#include "neatavr.hpp"

#include <stdio.h>
#include <util/twi.h>
#include <string.h>

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

volatile unsigned int status;
volatile unsigned int counter = 0;

volatile char buffer[64];
volatile unsigned int position;
volatile unsigned char available = 0;


int main() {
    Serial::init();
    Serial::printline("Tin Bot Extension Board v0.1");
    Serial::printline("Startup...");

    LED::output();

    TWIAddress::set(I2C_ADDRESS);
    TWIAddress::GenericCalls::enable();

    TWIControl::Enable::on();
    TWIControl::Acknowledge::on();
    TWIControl::Interrupt::enable();

    System::enable_interrupts();




    unsigned int previous = 0;

    while (1) {
        if (status != previous) {
            Serial::printline("Status:");
            char data[10];
            memset(data, 0, 10);
            sprintf(data, "%i", status);
            previous = status;
            Serial::printline(data);
        }
        if (available) {
            System::disable_interrupts();
            Serial::printline("Message:");
            Serial::write((char*) buffer, strlen((char*) buffer));
            Serial::write("\n", 1);
            available = 0;
            System::enable_interrupts();
        }
    }
}

ISR(TWI_vect, ISR_BLOCK) {
    status = TW_STATUS;
    switch (TW_STATUS) {
        case TW_SR_SLA_ACK:
            TWIControl::Acknowledge::on();
            position = 0;
            available = 0;
            break;
        case TW_SR_DATA_ACK:
            buffer[position] = (char) TWDR;
            position++;
            TWIControl::Acknowledge::on();
            break;
        case TW_ST_SLA_ACK:
        case TW_ST_DATA_ACK:
            TWDR = 0x05;
            TWIControl::Acknowledge::on();
            LED::on();
            break;
        case TW_ST_DATA_NACK:
            TWIControl::Acknowledge::on();
            break;
        default:
            buffer[position] = 0;
            available = 1;
            TWIControl::Acknowledge::on();
    }
}