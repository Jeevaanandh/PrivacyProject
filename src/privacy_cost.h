#ifndef PRIVACY_COST_H
#define PRIVACY_COST_H

#include <Arduino.h>
#include "apepc.h"

class PrivacyCost {
public:
    static float getBaseSensitivity(SensorType sensorType);
    static float getTemporalFactor();
    static float getFrequencyFactor(uint32_t transmissionCount);
    static float getCorrelationFactor(float localValue,
                                      const APECPPacket* neighborPackets,
                                      uint8_t neighborCount);
};

#endif
