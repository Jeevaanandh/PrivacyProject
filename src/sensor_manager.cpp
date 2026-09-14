#include "sensor_manager.h"
#include <DHT.h>

#define DHT_TYPE DHT22
static DHT* dhtSensor = nullptr;

SensorManager::SensorManager()
    : sensorType(SENSOR_PIR), sensorPin(0), lastValue(0) {}

void SensorManager::begin(SensorType type, uint8_t pin) {
    sensorType = type;
    sensorPin = pin;

    if (sensorType == SENSOR_PIR || sensorType == SENSOR_REED ||
        sensorType == SENSOR_LDR) {
        pinMode(sensorPin, INPUT);
    } else if (sensorType == SENSOR_DHT22) {
        dhtSensor = new DHT(sensorPin, DHT_TYPE);
        dhtSensor->begin();
    }
}

float SensorManager::readSensor() {
    switch (sensorType) {
        case SENSOR_PIR:
        case SENSOR_REED:
            lastValue = digitalRead(sensorPin) ? 1.0f : 0.0f;
            break;

        case SENSOR_LDR: {
            int raw = analogRead(sensorPin);
            lastValue = (raw / 4095.0f) * 100.0f;
            break;
        }

        case SENSOR_DHT22:
            if (dhtSensor) {
                float temperature = dhtSensor->readTemperature();
                if (!isnan(temperature)) lastValue = temperature;
            }
            break;
    }
    return lastValue;
}

float SensorManager::getLastValue() {
    return lastValue;
}

SensorType SensorManager::getSensorType() {
    return sensorType;
}

float SensorManager::generalizeValue(float value) {
    switch (sensorType) {
        case SENSOR_PIR:
        case SENSOR_REED:
            return value > 0.5f ? 1.0f : 0.0f;

        case SENSOR_LDR:
            if (value < 33.0f) return 0.0f;
            if (value < 66.0f) return 50.0f;
            return 100.0f;

        case SENSOR_DHT22:
            if (value < 18.0f) return 0.0f;
            if (value < 28.0f) return 50.0f;
            return 100.0f;
    }
    return value;
}

float SensorManager::aggregateValue(float value) {
    switch (sensorType) {
        case SENSOR_PIR:
        case SENSOR_REED:
            return value;
        case SENSOR_LDR:
            return round(value / 10.0f) * 10.0f;
        case SENSOR_DHT22:
            return round(value);
    }
    return value;
}
