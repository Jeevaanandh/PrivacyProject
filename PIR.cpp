//Use ESP32 DEV Module

#define PIR_PIN 4

void setup() {
    Serial.begin(115200);

    pinMode(PIR_PIN, INPUT);

    Serial.println("PIR sensor started");
}

void loop() {

    int motion = digitalRead(PIR_PIN);

    if (motion == HIGH) {
        Serial.println("Motion detected!");
    } 
    else {
        Serial.println("No motion");
    }

    delay(5000);
}
