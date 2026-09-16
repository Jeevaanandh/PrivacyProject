#include "transmission_controller.h"

TransmissionController::TransmissionController()
    : delayedUntil(0),
      aggregationBuffer(0.0f),
      aggregationCount(0),
      lastAggregationTime(0) {
}

void TransmissionController::begin() {
    delayedUntil = 0;

    aggregationBuffer = 0.0f;

    aggregationCount = 0;

    lastAggregationTime = millis();
}


// =====================================================
// DETERMINE TRANSMISSION ACTION
// =====================================================

TransmissionAction TransmissionController::determineAction(
    PrivacyState state) {

    switch (state) {

        case STATE_NORMAL:
            return ACTION_NORMAL;

        case STATE_WARNING:
            return ACTION_GENERALIZE;

        case STATE_HIGH:
            return ACTION_AGGREGATE;

        case STATE_CRITICAL:
            return ACTION_SUPPRESS;
    }

    return ACTION_SUPPRESS;
}


// =====================================================
// CHECK WHETHER TRANSMISSION IS ALLOWED
// =====================================================

bool TransmissionController::shouldTransmit(
    TransmissionAction action) {

    unsigned long now = millis();

    switch (action) {

        // -----------------------------------------
        // NORMAL
        // -----------------------------------------
        case ACTION_NORMAL:
            return true;


        // -----------------------------------------
        // GENERALIZATION
        // -----------------------------------------
        case ACTION_GENERALIZE:
            return true;


        // -----------------------------------------
        // AGGREGATION
        // -----------------------------------------
        case ACTION_AGGREGATE:

            // Aggregation is transmitted only
            // after the complete time window.
            if (aggregationCount == 0) {
                return false;
            }

            return (now - lastAggregationTime)
                   >= AGGREGATION_INTERVAL;


        // -----------------------------------------
        // DELAY
        // -----------------------------------------
        case ACTION_DELAY:
            return now >= delayedUntil;


        // -----------------------------------------
        // SUPPRESSION
        // -----------------------------------------
        case ACTION_SUPPRESS:
            return false;
    }

    return false;
}


// =====================================================
// GENERALIZATION
// =====================================================

float TransmissionController::applyGeneralization(
    float value,
    SensorManager& sensor) {

    return sensor.generalizeValue(value);
}


// =====================================================
// ADD READING TO AGGREGATION BUFFER
// =====================================================

void TransmissionController::addToAggregation(float value) {

    aggregationBuffer += value;

    aggregationCount++;
}


// =====================================================
// GET AGGREGATED VALUE
// =====================================================

float TransmissionController::getAggregatedValue() {

    if (aggregationCount == 0) {
        return 0.0f;
    }

    return aggregationBuffer /
           static_cast<float>(aggregationCount);
}


// =====================================================
// CLEAR AGGREGATION BUFFER
// =====================================================

void TransmissionController::clearAggregation() {

    aggregationBuffer = 0.0f;

    aggregationCount = 0;

    // Start a fresh aggregation window
    lastAggregationTime = millis();
}


// =====================================================
// GET NUMBER OF BUFFERED READINGS
// =====================================================

uint16_t TransmissionController::getAggregationCount() {

    return aggregationCount;
}


// =====================================================
// CHECK AGGREGATION WINDOW
// =====================================================

bool TransmissionController::aggregationReady() {

    if (aggregationCount == 0) {
        return false;
    }

    return (millis() - lastAggregationTime)
           >= AGGREGATION_INTERVAL;
}


// =====================================================
// CHECK DELAY
// =====================================================

bool TransmissionController::delayExpired() {

    return millis() >= delayedUntil;
}