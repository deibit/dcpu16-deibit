#include "hardware.h"

#ifndef clock_h
#define clock_h

class Clock : public Hardware {
   public:
    Clock();
    ~Clock();
    unsigned interrupt() override;
    unsigned tick() override;
};

#endif /* ifndef clock_h */
