#include "hardware.h"

#pragma once

using word = uint16_t;

class Clock : public Hardware {
   public:
    Clock();
    ~Clock();
    unsigned interrupt(word msg) override;
    unsigned interrupt() override;
    unsigned tick() override;
};
