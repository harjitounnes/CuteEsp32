#include <BlinkModule.h>
#include <LCDModule.h>
#include <StorageModule.h>
#include <WiFiModule.h>
#include <MqttModule.h>

WiFiClient wifiClient;

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);
WiFiModule wifi(&storage, &lcd);
MqttModule mqtt(&wifiClient, &wifi, &storage, &lcd);

void setup() {
    Serial.begin(115200);
    blink.start();
    storage.begin();
    lcd.begin();
    lcd.setText("Status: OK", "MQTT Example");
    lcd.start();
    wifi.begin();
    delay(2000);
    mqtt.begin();

    mqtt.onMessage([](const char* topic, const char* payload) {
        Serial.printf("Received message on topic %s: %s\n", topic, payload);
    });

}

void loop() {
    mqtt.publish("esp32/publish", "Hello from ESP32!");
    Serial.println("Published message");
    delay(10000);
}