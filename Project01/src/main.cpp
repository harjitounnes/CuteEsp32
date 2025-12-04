#include <Arduino.h>
#include <BlinkModule.h>
#include <WiFiModule.h>
#include <ArduinoOTA.h>
#include <StorageModule.h>
#include <BluetoothModule.h>
#include <OTAUpdateModule.h>
#include <BTCommandHandler.h>
#include "SensorReader.h"
    
BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);
WiFiModule wifi(&storage, &lcd);
BluetoothModule bt("ESP32_Device", &lcd);
OTAUpdateModule ota(&storage, &lcd);

BTCommandHandler cmdHandler(&bt, &storage, &lcd);
SensorReader sensorReader(&cmdHandler, 120000); // Kirim setiap 10000 ms

void setup() {
    Serial.begin(115200);
    delay(10000);
    blink.start();
    lcd.begin();
    lcd.setText("Status: OK", "Ini adalah running text di baris 2");
    lcd.start();
    delay(10000);
    storage.begin();
    wifi.begin();
    bt.begin();
    delay(10000);
    String msg = "Device connected via Bluetooth.";
    cmdHandler.handle(msg);
    ota.begin();
    bt.onMessage([&](const String& msg) {
        cmdHandler.handle(msg);
    });
    delay(10000);
    ArduinoOTA.setPort(3232);
    ArduinoOTA.begin();
    ArduinoOTA.setPassword("otapassword"); 
    delay(10000);
    sensorReader.begin();
    blink.setInterval(200);
}

void loop() {
    ArduinoOTA.handle();
    bt.loop();
    ota.loop();
}
