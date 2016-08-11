#include "clock.h"

Clock::Clock()
    : Hardware("Generic Clock (compatible)", 0x12d0b402, 0x1, 0xff) {}

Clock::~Clock() {}

unsigned Clock::interrupt(word msg) { return 0; }
unsigned Clock::interrupt() { return 0; }
unsigned Clock::tick() { return 0; }
