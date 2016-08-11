#include "hardware.h"

#pragma once

class Clock : public Hardware {
   public:
    Clock();
    ~Clock();
    unsigned interrupt() override;
    unsigned tick() override;
};
