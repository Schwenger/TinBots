#ifndef EXT_DETECTOR_HPP
#define EXT_DETECTOR_HPP

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

#endif
