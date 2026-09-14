# APECP ESP32 Prototype

Adaptive Privacy Exposure Capacity Protocol for 3 ESP32 IoT nodes.

## Nodes

- ESP32 #1: PIR
- ESP32 #2: LDR
- ESP32 #3: DHT22

## Open in VS Code

1. Install VS Code.
2. Install the PlatformIO extension.
3. Open this `APECP` folder.
4. Connect an ESP32.
5. Change `NODE_ID` and `SENSOR` in `src/main.cpp`.
6. Build and upload.

## Configuration

ESP32 #1:
```cpp
#define NODE_ID 1
#define SENSOR SENSOR_PIR
```

ESP32 #2:
```cpp
#define NODE_ID 2
#define SENSOR SENSOR_LDR
```

ESP32 #3:
```cpp
#define NODE_ID 3
#define SENSOR SENSOR_DHT22
```

Pins:
- PIR: GPIO 27
- LDR: GPIO 34
- DHT22: GPIO 4
- LED: GPIO 2

## APECP flow

Sensor -> Privacy Cost -> PEC -> Privacy State -> Adaptive Action -> ESP-NOW

States:
- NORMAL: normal transmission
- WARNING: generalization
- HIGH: aggregation
- CRITICAL: suppression

PEC regenerates gradually over time.
