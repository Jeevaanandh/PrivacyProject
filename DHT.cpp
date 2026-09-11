//Use the ESP32 Dev Module in Arduino IDE
#include <DHT.h>

#define DHTPIN 5        // D5 = GPIO5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);

    dht.begin();

    Serial.println("DHT11 started");
}

void loop() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if reading failed
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT11!");
        delay(2000);
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.println("--------------------");

    // DHT11 should not be read too frequently
    delay(2000);
}
