#ifndef ESPNOW_MANAGER_H
#define ESPNOW_MANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "apepc.h"

#define MAX_NEIGHBORS 5

class ESPNowManager {
private:
    APECPPacket receivedPackets[MAX_NEIGHBORS];
    uint8_t packetCount;
    uint8_t localNodeID;
    static ESPNowManager* instance;

   static void onReceive(const uint8_t* mac,
                      const uint8_t* data, int len);

public:
    ESPNowManager();
    bool begin(uint8_t nodeID);
    bool sendPacket(APECPPacket& packet);
    void processReceivedPacket(const uint8_t* data, int len);
    uint8_t getNeighborCount();
    const APECPPacket* getNeighborPackets();
    void printNeighbors();
};

#endif
