#include "clock.h"

Clock::Clock()
    : Hardware("Generic Clock (compatible)", 0x12d0b402, 0x1, 0xff),
      divisor(0),
      elapsed_ticks(0),
      ticks(0),
      stop(false) {}

Clock::~Clock() {}

unsigned Clock::interrupt(word msg) {
    switch (msg) {
        case 0:
            if (cpu) {
                uint16_t reg_B = cpu->context()[B];
                if (reg_B == 0) {
                    stop = true;
                    ticks = 0;
                    divisor = 0;
                } else {
                    divisor = reg_B;
                    stop = false;
                    ticks = 0;
                }
            } else {
                return 0;
            }
        case 1:
            if (cpu) {
                cpu->context()[C] = elapsed_ticks;
            }
        case 2:
        default:
            return 0;
    }

    return 0;
}
unsigned Clock::interrupt() { return 0; }
unsigned Clock::tick() { return 0; }
