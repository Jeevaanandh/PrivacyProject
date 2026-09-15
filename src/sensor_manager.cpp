#include "sensor_manager.h"
#include <DHT.h>
#include <math.h>

#define DHT_TYPE DHT11

static DHT* dhtSensor = nullptr;

SensorManager::SensorManager()
    : sensorType(SENSOR_PIR), sensorPin(0), lastValue(0.0f) {}

void SensorManager::begin(SensorType type, uint8_t pin) {
    sensorType = type;
    sensorPin = pin;

    if (sensorType == SENSOR_PIR ||
        sensorType == SENSOR_REED ||
        sensorType == SENSOR_LDR) {

        pinMode(sensorPin, INPUT);

    } else if (sensorType == SENSOR_DHT11) {

        // Create DHT11 sensor
        dhtSensor = new DHT(sensorPin, DHT_TYPE);
        dhtSensor->begin();

        // Give DHT11 time to stabilize
        delay(1000);
    }
}

float SensorManager::readSensor() {

    switch (sensorType) {

        // -----------------------------
        // PIR / REED
        // -----------------------------
        case SENSOR_PIR:
        case SENSOR_REED:

            lastValue = digitalRead(sensorPin) ? 1.0f : 0.0f;
            break;


        // -----------------------------
        // LDR
        // -----------------------------
        case SENSOR_LDR: {

            int raw = analogRead(sensorPin);

            // Convert ADC reading to percentage
            lastValue = (raw / 4095.0f) * 100.0f;

            break;
        }


        // -----------------------------
        // DHT11
        // -----------------------------
        case SENSOR_DHT11: {

            if (dhtSensor) {

                float temperature = dhtSensor->readTemperature();

                if (!isnan(temperature)) {

                    // Keep the ACTUAL DHT11 temperature.
                    // Do NOT convert to 0 / 50 / 100.
                    lastValue = temperature;

                } else {

                    // Keep previous valid reading if sensor read fails
                    Serial.println(
                        "[DHT11] Read failed - keeping previous value"
                    );
                }
            }

            break;
        }
    }

    return lastValue;
}


float SensorManager::getLastValue() {
    return lastValue;
}


SensorType SensorManager::getSensorType() {
    return sensorType;
}


// =====================================================
// GENERALIZATION
// =====================================================
// Generalization is used when the PEC state is WARNING.
//
// DHT11 is intentionally NOT converted into categories.
// The actual temperature is preserved.
// =====================================================

float SensorManager::generalizeValue(float value) {

    switch (sensorType) {

        // -----------------------------
        // PIR / REED
        // -----------------------------
        case SENSOR_PIR:
        case SENSOR_REED:

            return value > 0.5f ? 1.0f : 0.0f;


        // -----------------------------
        // LDR
        // -----------------------------
        case SENSOR_LDR:

            if (value < 33.0f)
                return 0.0f;

            if (value < 66.0f)
                return 50.0f;

            return 100.0f;


        // -----------------------------
        // DHT11
        // -----------------------------
        case SENSOR_DHT11:

            // DO NOT GENERALIZE TEMPERATURE.
            // Keep the actual DHT11 value.
            return value;
    }

    return value;
}


// =====================================================
// AGGREGATION
// =====================================================
// IMPORTANT:
//
// This function processes ONE value.
//
// Actual aggregation of MULTIPLE readings is handled
// by TransmissionController.
//
// Therefore, DHT11 is returned unchanged here.
// =====================================================

float SensorManager::aggregateValue(float value) {

    switch (sensorType) {

        // -----------------------------
        // PIR / REED
        // -----------------------------
        case SENSOR_PIR:
        case SENSOR_REED:

            return value;


        // -----------------------------
        // LDR
        // -----------------------------
        case SENSOR_LDR:

            // Reduce precision to nearest 10
            return round(value / 10.0f) * 10.0f;


        // -----------------------------
        // DHT11
        // -----------------------------
        case SENSOR_DHT11:

            // Keep actual temperature.
            // Multiple DHT11 readings are averaged
            // by TransmissionController.
            return value;
    }

    return value;
}