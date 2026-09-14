#ifndef TRANSMISSION_CONTROLLER_H
#define TRANSMISSION_CONTROLLER_H

#include <Arduino.h>
#include "apepc.h"
#include "sensor_manager.h"

class TransmissionController {
private:
    unsigned long delayedUntil;
    float aggregationBuffer;
    uint16_t aggregationCount;

public:
    TransmissionController();
    void begin();
    TransmissionAction determineAction(PrivacyState state);
    bool shouldTransmit(TransmissionAction action);
    float applyGeneralization(float value, SensorManager& sensor);
    float applyAggregation(float value, SensorManager& sensor);
    void addToAggregation(float value);
    float getAggregatedValue();
    void clearAggregation();
    bool delayExpired();
};

#endif
