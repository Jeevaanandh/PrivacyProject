#ifndef PEC_MANAGER_H
#define PEC_MANAGER_H

#include <Arduino.h>
#include "apepc.h"

class PECManager {
private:
    float pec;
    unsigned long lastUpdate;

public:
    PECManager();
    void begin();
    void consume(float privacyCost);
    void regenerate();
    float getPEC();
    PrivacyState getState();
    void reset();
};

#endif
