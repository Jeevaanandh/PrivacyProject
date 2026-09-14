#include <Arduino.h>
#include <WiFi.h>

#include "apepc.h"
#include "privacy_cost.h"
#include "pec_manager.h"
#include "espnow_manager.h"
#include "sensor_manager.h"
#include "transmission_controller.h"

// =====================================================
// APECP - UNIFIED 3-NODE MAIN
// SAME CODE IS UPLOADED TO ALL THREE ESP32s
// =====================================================

// =====================================================
// NODE MAC ADDRESSES
// =====================================================

// PIR
const char* PIR_MAC = "B0:CB:D8:E8:FB:10";

// LDR
const char* LDR_MAC = "A4:F0:0F:6F:9C:B8";

// DHT22
const char* DHT_MAC = "48:9D:31:2F:9B:0C";

// =====================================================
// SENSOR PINS
// =====================================================

#define PIR_PIN 4
#define LDR_PIN 34
#define DHT_PIN 5

// =====================================================
// TIMING
// =====================================================

#define SENSOR_INTERVAL 2000UL
#define DEBUG_INTERVAL 5000UL

// =====================================================
// NODE CONFIGURATION
// =====================================================

uint8_t NODE_ID = 0;

SensorType sensorType;

uint8_t SENSOR_PIN = 0;

const char* SENSOR_NAME = "UNKNOWN";

// =====================================================
// APECP OBJECTS
// =====================================================

PECManager pecManager;
ESPNowManager espNowManager;
SensorManager sensorManager;
TransmissionController transmissionController;

// =====================================================
// TIMERS
// =====================================================

unsigned long lastSensorRead = 0;
unsigned long lastDebug = 0;

// =====================================================
// TRANSMISSION INFORMATION
// =====================================================

uint32_t transmissionCount = 0;
uint16_t sequenceNumber = 0;

// =====================================================
// IDENTIFY NODE FROM MAC
// =====================================================

bool identifyNode() {

    String mac = WiFi.macAddress();

    mac.toUpperCase();

    Serial.print("Detected MAC: ");
    Serial.println(mac);

    // -------------------------------------------------
    // NODE 1 - PIR
    // -------------------------------------------------

    if (mac == PIR_MAC) {

        NODE_ID = 1;

        sensorType = SENSOR_PIR;

        SENSOR_PIN = PIR_PIN;

        SENSOR_NAME = "PIR";

        return true;
    }

    // -------------------------------------------------
    // NODE 2 - LDR
    // -------------------------------------------------

    if (mac == LDR_MAC) {

        NODE_ID = 2;

        sensorType = SENSOR_LDR;

        SENSOR_PIN = LDR_PIN;

        SENSOR_NAME = "LDR";

        return true;
    }

    // -------------------------------------------------
    // NODE 3 - DHT22
    // -------------------------------------------------

    if (mac == DHT_MAC) {

        NODE_ID = 3;

        sensorType = SENSOR_DHT22;

        SENSOR_PIN = DHT_PIN;

        SENSOR_NAME = "DHT22";

        return true;
    }

    // -------------------------------------------------
    // UNKNOWN NODE
    // -------------------------------------------------

    return false;
}

// =====================================================
// APECP TRANSMISSION
// =====================================================

void performAPECPTransmission(
    float rawValue,
    float privacyCost,
    float sensitivity,
    float temporalFactor,
    float frequencyFactor,
    float correlationFactor,
    TransmissionAction action
) {

    float transmittedValue = rawValue;

    bool generalized = false;
    bool aggregated = false;
    bool delayed = false;
    bool suppressed = false;

    // -------------------------------------------------
    // NORMAL TRANSMISSION
    // -------------------------------------------------

    if (action == ACTION_NORMAL) {

        transmittedValue = rawValue;
    }

    // -------------------------------------------------
    // GENERALIZATION
    // -------------------------------------------------

    else if (action == ACTION_GENERALIZE) {

        transmittedValue =
            sensorManager.generalizeValue(rawValue);

        generalized = true;
    }

    // -------------------------------------------------
    // AGGREGATION
    // -------------------------------------------------

    else if (action == ACTION_AGGREGATE) {

        transmissionController.addToAggregation(rawValue);

        transmittedValue =
            transmissionController.getAggregatedValue();

        aggregated = true;
    }

    // -------------------------------------------------
    // DELAY
    // -------------------------------------------------

    else if (action == ACTION_DELAY) {

        delayed = true;
    }

    // -------------------------------------------------
    // SUPPRESSION
    // -------------------------------------------------

    else if (action == ACTION_SUPPRESS) {

        suppressed = true;

        Serial.println();
        Serial.println("[APECP] TRANSMISSION SUPPRESSED");
        Serial.println();

        return;
    }

    // -------------------------------------------------
    // CREATE PACKET
    // -------------------------------------------------

    APECPPacket packet{};

    packet.version = APECP_VERSION;

    packet.nodeID = NODE_ID;

    packet.sensorType = sensorType;

    packet.timestamp = millis();

    packet.sensorValue = transmittedValue;

    packet.privacyCost = privacyCost;

    packet.pecRemaining =
        pecManager.getPEC();

    packet.sensitivity =
        sensitivity;

    packet.temporalFactor =
        temporalFactor;

    packet.frequencyFactor =
        frequencyFactor;

    packet.correlationFactor =
        correlationFactor;

    packet.privacyState =
        pecManager.getState();

    packet.transmissionAction =
        action;

    packet.sequence =
        sequenceNumber++;

    packet.generalized =
        generalized;

    packet.aggregated =
        aggregated;

    packet.delayed =
        delayed;

    packet.suppressed =
        suppressed;

    // -------------------------------------------------
    // SEND PACKET
    // -------------------------------------------------

    if (espNowManager.sendPacket(packet)) {

        transmissionCount++;

        Serial.println();
        Serial.println("--------------------------------");
        Serial.println("APECP TRANSMISSION");
        Serial.println("--------------------------------");

        Serial.print("Node: ");
        Serial.println(NODE_ID);

        Serial.print("Sensor: ");
        Serial.println(SENSOR_NAME);

        Serial.print("Raw Value: ");
        Serial.println(rawValue, 2);

        Serial.print("Sent Value: ");
        Serial.println(transmittedValue, 2);

        Serial.print("Privacy Cost: ");
        Serial.println(privacyCost, 2);

        Serial.print("PEC: ");
        Serial.println(
            pecManager.getPEC(),
            2
        );

        Serial.print("State: ");
        Serial.println(
            APECP::stateToString(
                pecManager.getState()
            )
        );

        Serial.print("Action: ");
        Serial.println(
            APECP::actionToString(
                action
            )
        );

        Serial.print("Sequence: ");
        Serial.println(
            packet.sequence
        );

        Serial.println("--------------------------------");
    }

    else {

        Serial.println(
            "[APECP] ESP-NOW transmission failed"
        );
    }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

    Serial.begin(115200);

    delay(1000);

    // -------------------------------------------------
    // INITIAL WIFI MODE
    // -------------------------------------------------

    WiFi.mode(WIFI_STA);

    WiFi.disconnect();

    delay(100);

    // -------------------------------------------------
    // HEADER
    // -------------------------------------------------

    Serial.println();
    Serial.println();

    Serial.println("========================================");
    Serial.println("       APECP IoT PRIVACY PROTOCOL");
    Serial.println("========================================");

    // -------------------------------------------------
    // IDENTIFY BOARD
    // -------------------------------------------------

    if (!identifyNode()) {

        Serial.println();
        Serial.println("ERROR: UNKNOWN ESP32 MAC ADDRESS!");

        Serial.print("Detected MAC: ");
        Serial.println(
            WiFi.macAddress()
        );

        Serial.println();
        Serial.println("This ESP32 is not registered.");
        Serial.println("System halted.");

        while (true) {
            delay(1000);
        }
    }

    // -------------------------------------------------
    // DISPLAY NODE INFORMATION
    // -------------------------------------------------

    Serial.println();

    Serial.println("NODE IDENTIFIED");

    Serial.println("--------------------------------");

    Serial.print("Node ID: ");
    Serial.println(NODE_ID);

    Serial.print("MAC: ");
    Serial.println(
        WiFi.macAddress()
    );

    Serial.print("Sensor: ");
    Serial.println(
        SENSOR_NAME
    );

    Serial.print("Sensor Pin: ");
    Serial.println(
        SENSOR_PIN
    );

    Serial.println("--------------------------------");

    // -------------------------------------------------
    // SENSOR INITIALIZATION
    // -------------------------------------------------

    sensorManager.begin(
        sensorType,
        SENSOR_PIN
    );

    // -------------------------------------------------
    // PEC INITIALIZATION
    // -------------------------------------------------

    pecManager.begin();

    // -------------------------------------------------
    // TRANSMISSION INITIALIZATION
    // -------------------------------------------------

    transmissionController.begin();

    // -------------------------------------------------
    // ESP-NOW INITIALIZATION
    // -------------------------------------------------

    if (!espNowManager.begin(NODE_ID)) {

        Serial.println();
        Serial.println("ESP-NOW ERROR!");

        while (true) {
            delay(1000);
        }
    }

    // -------------------------------------------------
    // READY
    // -------------------------------------------------

    Serial.println();

    Serial.println("========================================");
    Serial.println("          APECP NODE READY");
    Serial.println("========================================");

    Serial.print("Node: ");
    Serial.println(NODE_ID);

    Serial.print("Sensor: ");
    Serial.println(SENSOR_NAME);

    Serial.println();
}

// =====================================================
// LOOP
// =====================================================

void loop() {

    unsigned long now = millis();

    // -------------------------------------------------
    // PEC REGENERATION
    // -------------------------------------------------

    pecManager.regenerate();

    // -------------------------------------------------
    // SENSOR READING
    // -------------------------------------------------

    if (
        now - lastSensorRead >=
        SENSOR_INTERVAL
    ) {

        lastSensorRead = now;

        float sensorValue =
            sensorManager.readSensor();

        // -------------------------------------------------
        // PRIVACY SENSITIVITY
        // -------------------------------------------------

        float sensitivity =
            PrivacyCost::getBaseSensitivity(
                sensorType
            );

        // -------------------------------------------------
        // TEMPORAL FACTOR
        // -------------------------------------------------

        float temporalFactor =
            PrivacyCost::getTemporalFactor();

        // -------------------------------------------------
        // FREQUENCY FACTOR
        // -------------------------------------------------

        float frequencyFactor =
            PrivacyCost::getFrequencyFactor(
                transmissionCount
            );

        // -------------------------------------------------
        // CORRELATION FACTOR
        // -------------------------------------------------

        float correlationFactor =
            PrivacyCost::getCorrelationFactor(
                sensorValue,
                espNowManager.getNeighborPackets(),
                espNowManager.getNeighborCount()
            );

        // -------------------------------------------------
        // CALCULATE PRIVACY COST
        // -------------------------------------------------

        float cost =
            APECP::calculatePrivacyCost(
                sensitivity,
                temporalFactor,
                frequencyFactor,
                correlationFactor
            );

        // -------------------------------------------------
        // CONSUME PEC
        // -------------------------------------------------

        pecManager.consume(cost);

        // -------------------------------------------------
        // PRIVACY STATE
        // -------------------------------------------------

        PrivacyState state =
            pecManager.getState();

        // -------------------------------------------------
        // TRANSMISSION ACTION
        // -------------------------------------------------

        TransmissionAction action =
            transmissionController.determineAction(
                state
            );

        // -------------------------------------------------
        // TRANSMIT
        // -------------------------------------------------

        if (
            transmissionController.shouldTransmit(
                action
            )
        ) {

            performAPECPTransmission(
                sensorValue,
                cost,
                sensitivity,
                temporalFactor,
                frequencyFactor,
                correlationFactor,
                action
            );
        }

        else {

            Serial.println(
                "[APECP] Packet delayed"
            );
        }
    }

    // -------------------------------------------------
    // DEBUG STATUS
    // -------------------------------------------------

    if (
        now - lastDebug >=
        DEBUG_INTERVAL
    ) {

        lastDebug = now;

        Serial.println();

        Serial.println(
            "========== APECP STATUS =========="
        );

        Serial.print("Node ID: ");
        Serial.println(NODE_ID);

        Serial.print("Sensor: ");
        Serial.println(SENSOR_NAME);

        Serial.print("MAC: ");
        Serial.println(
            WiFi.macAddress()
        );

        Serial.print("Sensor Pin: ");
        Serial.println(SENSOR_PIN);

        Serial.print("PEC: ");
        Serial.println(
            pecManager.getPEC(),
            2
        );

        Serial.print("Privacy State: ");
        Serial.println(
            APECP::stateToString(
                pecManager.getState()
            )
        );

        Serial.print("Transmissions: ");
        Serial.println(
            transmissionCount
        );

        Serial.print("Neighbors: ");
        Serial.println(
            espNowManager.getNeighborCount()
        );

        espNowManager.printNeighbors();

        Serial.println(
            "================================="
        );
    }
}