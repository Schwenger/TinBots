#include "p30F6014A.h"

#include "utils.h"

#include "adc.h"

#define MODE_ANALOG 0
#define MODE_DIGITAL 1

#define ADC_SCAN 1
#define ADC_NO_SCAN 0

#define PULSE_IR0_STATUS _LATF7	    // pulse IR 0 and 4
#define PULSE_IR1_STATUS _LATF8	    // pulse IR 1 and 5
#define PULSE_IR2_STATUS _LATG0	    // pulse IR 2 and 6
#define PULSE_IR3_STATUS _LATG1     // pulse IR 3 and 7

#define PULSE_IR0_DIR _TRISF7
#define PULSE_IR1_DIR _TRISF8
#define PULSE_IR2_DIR _TRISG0
#define PULSE_IR3_DIR _TRISG1

/* first in front right of robot than turning clockwise */
#define IR0 8   // ir proximity sensor 0 on AD channel 8
#define IR1 9   // ir proximity sensor 1 on AD channel 9
#define IR2 10  // ir proximity sensor 2 on AD channel 10
#define IR3 11  // ir proximity sensor 3 on AD channel 11
#define IR4 12  // ir proximity sensor 4 on AD channel 12
#define IR5 13  // ir proximity sensor 5 on AD channel 13
#define IR6 14  // ir proximity sensor 6 on AD channel 14
#define IR7 15  // ir proximity sensor 7 on AD channel 15

#define SCAN_IR04 ((ADC_SCAN << IR0) | (ADC_SCAN << IR4))
#define SCAN_IR15 ((ADC_SCAN << IR1) | (ADC_SCAN << IR5))
#define SCAN_IR26 ((ADC_SCAN << IR2) | (ADC_SCAN << IR6))
#define SCAN_IR37 ((ADC_SCAN << IR3) | (ADC_SCAN << IR7))

void e_init_adc(void) {
    ADPCFGbits.PCFG8 = MODE_ANALOG;
    ADPCFGbits.PCFG9 = MODE_ANALOG;
    ADPCFGbits.PCFG10 = MODE_ANALOG;
    ADPCFGbits.PCFG11 = MODE_ANALOG;
    ADPCFGbits.PCFG12 = MODE_ANALOG;
    ADPCFGbits.PCFG13 = MODE_ANALOG;
    ADPCFGbits.PCFG14 = MODE_ANALOG;
    ADPCFGbits.PCFG15 = MODE_ANALOG;

    ADCSSL = SCAN_IR04;

    PULSE_IR0_DIR = OUTPUT;
    PULSE_IR1_DIR = OUTPUT;
    PULSE_IR2_DIR = OUTPUT;
    PULSE_IR3_DIR = OUTPUT;

    // set output format to unsigned int
    ADCON1bits.FORM = 0;
    // turn automatic sampling on
    ADCON1bits.ASAM = ON;
    // switch to automatic conversion mode
    ADCON1bits.SSRC = 7;
    // interrupt on 3th sample
    ADCON2bits.SMPI = 3 - 1;
    // scan inputs
    ADCON2bits.CSCNA = ON;
    // 2 cycles between acquisition and conversion
    ADCON3bits.SAMC = 1;
    // set sample rate to 12us
    ADCON3bits.ADCS = 0b111;

    // clear ADC interrupt flag
    IFS0bits.ADIF = OFF;
    // enable ADC interrupt
    IEC0bits.ADIE = ON;

    // enable ADC conversion
    ADCON1bits.ADON = ON;
}

void __attribute__((__interrupt__, auto_psv)) _ADCInterrupt (void) {
    static unsigned int proximity_number = 0;

    IFS0bits.ADIF = 0;

    ADCBUF0;

    proximity_number ^= 1;
    e_set_led(0, proximity_number);
}
