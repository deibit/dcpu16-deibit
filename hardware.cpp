#include "hardware.h"
#include "cpu.h"

Hardware::Hardware(std::string n, uint32_t id, uint16_t ver, uint16_t man) :
    name(n), ID(id), version(ver), manufacturer(man), cpu{nullptr} {}

Hardware::~Hardware() {
        cpu = nullptr;
    }

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
