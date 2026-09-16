#ifndef TRANSMISSION_CONTROLLER_H
#define TRANSMISSION_CONTROLLER_H

#include <Arduino.h>

#include "apepc.h"
#include "sensor_manager.h"


// =====================================================
// AGGREGATION INTERVAL
// =====================================================
//
// Readings are collected for 10 seconds.
// After 10 seconds, their average is transmitted.
//
// 10000 ms = 10 seconds
//
// =====================================================

#define AGGREGATION_INTERVAL 10000UL


class TransmissionController {

private:

    // Used if ACTION_DELAY is ever used
    unsigned long delayedUntil;


    // =================================================
    // AGGREGATION VARIABLES
    // =================================================

    // Sum of readings in the current window
    float aggregationBuffer;

    // Number of readings collected
    uint16_t aggregationCount;

    // Start time of current aggregation window
    unsigned long lastAggregationTime;


public:

    TransmissionController();

    void begin();


    // =================================================
    // DETERMINE ACTION
    // =================================================

    TransmissionAction determineAction(
        PrivacyState state
    );


    // =================================================
    // TRANSMISSION CONTROL
    // =================================================

    bool shouldTransmit(
        TransmissionAction action
    );


    // =================================================
    // GENERALIZATION
    // =================================================

    float applyGeneralization(
        float value,
        SensorManager& sensor
    );


    // =================================================
    // AGGREGATION
    // =================================================

    // Add one sensor reading
    void addToAggregation(
        float value
    );

    // Calculate average of buffered readings
    float getAggregatedValue();

    // Clear buffer and start a new window
    void clearAggregation();

    // Number of readings currently buffered
    uint16_t getAggregationCount();

    // Check whether 10-second window is complete
    bool aggregationReady();


    // =================================================
    // DELAY
    // =================================================

    bool delayExpired();
};

#endif