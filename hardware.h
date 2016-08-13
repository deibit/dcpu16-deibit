#include "cpu.h"
#include <string>

#pragma once

using word = uint16_t;

class Hardware {
   public:
    Hardware(std::string, uint32_t, uint16_t, uint16_t);
    virtual ~Hardware();

    std::string name;
    uint32_t ID;
    uint16_t version;
    uint16_t manufacturer;
    CPU* cpu;

    virtual unsigned interrupt(word) = 0;
    virtual unsigned interrupt() = 0;
    virtual unsigned tick() = 0;

    void query();
    void setcpu(CPU*);
};
