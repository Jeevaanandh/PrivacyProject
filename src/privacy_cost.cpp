#include "privacy_cost.h"
#include <time.h>

float PrivacyCost::getBaseSensitivity(SensorType sensorType) {

    switch (sensorType) {

        case SENSOR_PIR:
            return 0.85f;

        case SENSOR_LDR:
            return 0.65f;

        case SENSOR_DHT11:
            return 0.55f;

        case SENSOR_REED:
            return 0.90f;
    }

    return 0.50f;
}


float PrivacyCost::getTemporalFactor() {

    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
        return 0.70f;

    int hour = timeinfo.tm_hour;

    if (hour >= 23 || hour < 6)
        return 1.00f;

    if (hour >= 18 && hour < 23)
        return 0.85f;

    if (hour >= 8 && hour < 18)
        return 0.65f;

    return 0.75f;
}


float PrivacyCost::getFrequencyFactor(
    uint32_t transmissionCount) {

    if (transmissionCount <= 1)
        return 0.40f;

    if (transmissionCount <= 5)
        return 0.55f;

    if (transmissionCount <= 10)
        return 0.70f;

    if (transmissionCount <= 20)
        return 0.85f;

    return 1.00f;
}


float PrivacyCost::getCorrelationFactor(
    float localValue,
    const APECPPacket* neighborPackets,
    uint8_t neighborCount) {

    (void)localValue;

    if (neighborCount == 0)
        return 0.30f;

    uint8_t correlated = 0;

    for (uint8_t i = 0; i < neighborCount; i++) {

        if (neighborPackets[i].nodeID == 255)
            continue;

        uint32_t age =
            millis() - neighborPackets[i].timestamp;

        if (age <= APECP_CORRELATION_WINDOW)
            correlated++;
    }

    if (correlated == 0)
        return 0.30f;

    if (correlated == 1)
        return 0.50f;

    if (correlated == 2)
        return 0.75f;

    return 1.00f;
}