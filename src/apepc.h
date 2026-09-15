#ifndef APECP_H
#define APECP_H

#include <Arduino.h>

enum SensorType {

    SENSOR_PIR = 0,

    SENSOR_LDR = 1,

    SENSOR_DHT11 = 2,

    SENSOR_REED = 3

};

enum PrivacyState {

    STATE_NORMAL = 0,

    STATE_WARNING = 1,

    STATE_HIGH = 2,

    STATE_CRITICAL = 3

};

enum TransmissionAction {

    ACTION_NORMAL = 0,

    ACTION_GENERALIZE = 1,

    ACTION_AGGREGATE = 2,

    ACTION_DELAY = 3,

    ACTION_SUPPRESS = 4

};

struct APECPPacket {

    uint8_t version;

    uint8_t nodeID;

    uint8_t sensorType;

    uint32_t timestamp;

    float sensorValue;

    float privacyCost;

    float pecRemaining;

    float sensitivity;

    float temporalFactor;

    float frequencyFactor;

    float correlationFactor;

    uint8_t privacyState;

    uint8_t transmissionAction;

    uint16_t sequence;

    bool generalized;

    bool aggregated;

    bool delayed;

    bool suppressed;

};

#define APECP_VERSION 1

#define PEC_MAX 100.0f

#define PEC_WARNING_THRESHOLD 70.0f

#define PEC_HIGH_THRESHOLD 40.0f

#define PEC_CRITICAL_THRESHOLD 15.0f

#define PEC_REGENERATION_RATE 0.08f

#define APECP_CORRELATION_WINDOW 15000UL

class APECP {

public:

    static float calculatePrivacyCost(
        float sensitivity,
        float temporalFactor,
        float frequencyFactor,
        float correlationFactor
    );

    static PrivacyState determineState(float pec);

    static const char* stateToString(PrivacyState state);

    static const char* actionToString(TransmissionAction action);

};

#endif