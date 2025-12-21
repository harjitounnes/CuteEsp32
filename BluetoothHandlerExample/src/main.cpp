#include <BlinkModule.h>
#include <StorageModule.h>
#include <LCDModule.h>
#include <BluetoothModule.h>
#include "handler/BTCommandHandler.h"

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);
BluetoothModule bt("ESP32_Device", &lcd);
BTCommandHandler cmdHandler(&bt, &storage, &lcd);   

void setup() {
    Serial.begin(115200);
    blink.start();
    delay(500);
    lcd.begin();
    lcd.setText("Bluetooth Example", "Initializing...");
    lcd.start();
    storage.begin();
    bt.begin();
    lcd.setText("Bluetooth Example", "Ready to pair");
    bt.onMessage([&](const String& msg) {
        cmdHandler.handle(msg);
    });
}   

void loop() {
    bt.loop();
    // put your main code here, to run repeatedly:
}