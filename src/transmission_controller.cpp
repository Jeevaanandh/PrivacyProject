#include "transmission_controller.h"

TransmissionController::TransmissionController()
    : delayedUntil(0), aggregationBuffer(0), aggregationCount(0) {}

void TransmissionController::begin() {
    delayedUntil = 0;
    aggregationBuffer = 0;
    aggregationCount = 0;
}

TransmissionAction TransmissionController::determineAction(
    PrivacyState state) {
    switch (state) {
        case STATE_NORMAL: return ACTION_NORMAL;
        case STATE_WARNING: return ACTION_GENERALIZE;
        case STATE_HIGH: return ACTION_AGGREGATE;
        case STATE_CRITICAL: return ACTION_SUPPRESS;
    }
    return ACTION_SUPPRESS;
}

bool TransmissionController::shouldTransmit(
    TransmissionAction action) {
    unsigned long now = millis();

    switch (action) {
        case ACTION_NORMAL:
        case ACTION_GENERALIZE:
            return true;

        case ACTION_AGGREGATE:
            return (now % 10000UL) < 100UL;

        case ACTION_DELAY:
            return now >= delayedUntil;

        case ACTION_SUPPRESS:
            return false;
    }
    return false;
}

float TransmissionController::applyGeneralization(
    float value, SensorManager& sensor) {
    return sensor.generalizeValue(value);
}

float TransmissionController::applyAggregation(
    float value, SensorManager& sensor) {
    return sensor.aggregateValue(value);
}

void TransmissionController::addToAggregation(float value) {
    aggregationBuffer += value;
    aggregationCount++;
}

float TransmissionController::getAggregatedValue() {
    if (aggregationCount == 0) return 0;
    return aggregationBuffer / aggregationCount;
}

void TransmissionController::clearAggregation() {
    aggregationBuffer = 0;
    aggregationCount = 0;
}

bool TransmissionController::delayExpired() {
    return millis() >= delayedUntil;
}
