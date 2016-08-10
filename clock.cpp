#include "clock.h"

Clock::Clock() {
        name = "Generic Clock (compatible)";
        ID = 0x12d0b402;
        version = 0x1;
        manufacturer = 0xff;  // Not specified
}

Clock::~Clock() {

}

unsigned Clock::interrupt() { return 0; }
unsigned Clock::tick() { return 0; }
