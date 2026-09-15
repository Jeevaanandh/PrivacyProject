#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>

#include "apepc.h"

class SensorManager {

private:

    SensorType sensorType;

    uint8_t sensorPin;

    float lastValue;

public:

    SensorManager();

    void begin(SensorType type, uint8_t pin);

    float readSensor();

    float getLastValue();

    SensorType getSensorType();

    float generalizeValue(float value);

    float aggregateValue(float value);

};

#endif