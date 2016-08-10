#include "hardware.h"
#include "cpu.h"

void Hardware::setcpu(CPU* cpu) {
    this->cpu = cpu;
}

void Hardware::query() {
    if (cpu) {
        cpu->context()[A] = ID;
        cpu->context()[B] = ID << 16;
        cpu->context()[C] = version;
        cpu->context()[X] = manufacturer;
        cpu->context()[Y] = manufacturer << 16;
    }
}
