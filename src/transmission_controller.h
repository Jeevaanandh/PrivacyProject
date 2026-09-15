#ifndef TRANSMISSION_CONTROLLER_H
#define TRANSMISSION_CONTROLLER_H

#include <Arduino.h>

#include "apepc.h"
#include "sensor_manager.h"


// =====================================================
// AGGREGATION INTERVAL
// =====================================================
// Readings are collected for 10 seconds and then
// their average is transmitted.
//
// Change this to 5000 for 5 seconds if required.
// =====================================================

#define AGGREGATION_INTERVAL 10000UL


class TransmissionController {

private:

    unsigned long delayedUntil;

    // Sum of all readings collected during
    // the current aggregation window
    float aggregationBuffer;

    // Number of readings collected
    uint16_t aggregationCount;

    // Time at which the current aggregation
    // window started
    unsigned long lastAggregationTime;


public:

    TransmissionController();

    void begin();

    // Decide action based on PEC state
    TransmissionAction determineAction(PrivacyState state);

    // Decide whether data should currently
    // be transmitted
    bool shouldTransmit(TransmissionAction action);

    // Apply generalization
    float applyGeneralization(
        float value,
        SensorManager& sensor
    );

    // Add one reading to aggregation buffer
    void addToAggregation(float value);

    // Return average of buffered readings
    float getAggregatedValue();

    // Clear buffer and start new aggregation window
    void clearAggregation();

    // Return number of readings currently buffered
    uint16_t getAggregationCount();

    // Check whether aggregation interval is complete
    bool aggregationReady();

    // Check delay status
    bool delayExpired();
};

#endif