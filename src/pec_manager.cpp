#include "pec_manager.h"

PECManager::PECManager() : pec(PEC_MAX), lastUpdate(0) {}

void PECManager::begin() {
    pec = PEC_MAX;
    lastUpdate = millis();
}

void PECManager::consume(float privacyCost) {
    pec -= privacyCost;
    if (pec < 0.0f) pec = 0.0f;
}

void PECManager::regenerate() {
    unsigned long now = millis();

    if (lastUpdate == 0) {
        lastUpdate = now;
        return;
    }

    unsigned long elapsed = now - lastUpdate;

    if (elapsed >= 1000) {
        float seconds = elapsed / 1000.0f;
        pec += PEC_REGENERATION_RATE * seconds;
        if (pec > PEC_MAX) pec = PEC_MAX;
        lastUpdate = now;
    }
}

float PECManager::getPEC() {
    return pec;
}

PrivacyState PECManager::getState() {
    return APECP::determineState(pec);
}

void PECManager::reset() {
    pec = PEC_MAX;
    lastUpdate = millis();
}
