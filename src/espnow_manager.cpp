#include "espnow_manager.h"

ESPNowManager* ESPNowManager::instance = nullptr;

static uint8_t broadcastAddress[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

ESPNowManager::ESPNowManager()
    : packetCount(0), localNodeID(0) {
    instance = this;
    for (int i = 0; i < MAX_NEIGHBORS; i++)
        receivedPackets[i].nodeID = 255;
}

bool ESPNowManager::begin(uint8_t nodeID) {
    localNodeID = nodeID;

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.printf("Node ID: %u\n", localNodeID);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR: ESP-NOW initialization failed");
        return false;
    }

    esp_now_register_recv_cb(ESPNowManager::onReceive);

    esp_now_peer_info_t peerInfo{};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(broadcastAddress)) {
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            Serial.println("ERROR: Failed to add broadcast peer");
            return false;
        }
    }

    Serial.println("ESP-NOW READY");
    return true;
}

bool ESPNowManager::sendPacket(APECPPacket& packet) {
    esp_err_t result = esp_now_send(
        broadcastAddress,
        reinterpret_cast<uint8_t*>(&packet),
        sizeof(APECPPacket)
    );

    if (result == ESP_OK) return true;

    Serial.printf("ESP-NOW send failed: %d\n", result);
    return false;
}

void ESPNowManager::onReceive(const uint8_t* mac,
                              const uint8_t* data, int len) {
    (void)mac;
    if (instance) instance->processReceivedPacket(data, len);
}

void ESPNowManager::processReceivedPacket(const uint8_t* data, int len) {
    if (len != sizeof(APECPPacket)) return;

    APECPPacket packet{};
    memcpy(&packet, data, sizeof(APECPPacket));

    if (packet.nodeID == localNodeID) return;
    if (packet.version != APECP_VERSION) return;

    int existingIndex = -1;
    for (int i = 0; i < packetCount; i++) {
        if (receivedPackets[i].nodeID == packet.nodeID) {
            existingIndex = i;
            break;
        }
    }

    if (existingIndex >= 0) {
        receivedPackets[existingIndex] = packet;
        return;
    }

    if (packetCount < MAX_NEIGHBORS) {
        receivedPackets[packetCount++] = packet;
    } else {
        int oldest = 0;
        for (int i = 1; i < MAX_NEIGHBORS; i++) {
            if (receivedPackets[i].timestamp <
                receivedPackets[oldest].timestamp)
                oldest = i;
        }
        receivedPackets[oldest] = packet;
    }
}

uint8_t ESPNowManager::getNeighborCount() {
    return packetCount;
}

const APECPPacket* ESPNowManager::getNeighborPackets() {
    return receivedPackets;
}

void ESPNowManager::printNeighbors() {
    Serial.println("--- NEIGHBOR PRIVACY STATES ---");

    for (int i = 0; i < packetCount; i++) {
        Serial.printf(
            "Node %u | PEC=%.2f | Cost=%.2f | State=%s\n",
            receivedPackets[i].nodeID,
            receivedPackets[i].pecRemaining,
            receivedPackets[i].privacyCost,
            APECP::stateToString(
                static_cast<PrivacyState>(
                    receivedPackets[i].privacyState
                )
            )
        );
    }
}
