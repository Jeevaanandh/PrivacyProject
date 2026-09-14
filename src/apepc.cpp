#include "apepc.h"

float APECP::calculatePrivacyCost(float sensitivity, float temporalFactor,
                                  float frequencyFactor, float correlationFactor) {
    sensitivity = constrain(sensitivity, 0.0f, 1.0f);
    temporalFactor = constrain(temporalFactor, 0.0f, 1.0f);
    frequencyFactor = constrain(frequencyFactor, 0.0f, 1.0f);
    correlationFactor = constrain(correlationFactor, 0.0f, 1.0f);

    return constrain(
        sensitivity * temporalFactor * frequencyFactor * correlationFactor * 100.0f,
        0.0f, 100.0f
    );
}

PrivacyState APECP::determineState(float pec) {
    if (pec <= PEC_CRITICAL_THRESHOLD) return STATE_CRITICAL;
    if (pec <= PEC_HIGH_THRESHOLD) return STATE_HIGH;
    if (pec <= PEC_WARNING_THRESHOLD) return STATE_WARNING;
    return STATE_NORMAL;
}

const char* APECP::stateToString(PrivacyState state) {
    switch (state) {
        case STATE_NORMAL: return "NORMAL";
        case STATE_WARNING: return "WARNING";
        case STATE_HIGH: return "HIGH";
        case STATE_CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

const char* APECP::actionToString(TransmissionAction action) {
    switch (action) {
        case ACTION_NORMAL: return "NORMAL";
        case ACTION_GENERALIZE: return "GENERALIZE";
        case ACTION_AGGREGATE: return "AGGREGATE";
        case ACTION_DELAY: return "DELAY";
        case ACTION_SUPPRESS: return "SUPPRESS";
    }
    return "UNKNOWN";
}
