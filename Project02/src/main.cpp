#include <Arduino.h>
#include <BlinkModule.h>
#include <LCDModule.h>
#include <StorageModule.h>
#include <BluetoothModule.h>
#include <KeypadModule.h>

#include "handler/BTCommandHandler.h"
#include "handler/KeypadCommandHandler.h"

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);
BluetoothModule bt("ESP32_Device", &lcd);
BTCommandHandler cmdHandler(&bt, &storage, &lcd);
KeypadModule keypad(&storage, &lcd);
KeypadCommandHandler keypadCmdHandler(&lcd);

void setup() {
    Serial.begin(115200);
    blink.start();
    delay(500);
    lcd.begin();
    lcd.setText("Status: OK", "Ini adalah running text di baris 2");
    lcd.start();
    storage.begin();
    bt.begin();
    String msg = "Device connected via Bluetooth.";
    cmdHandler.handle(msg);

    bt.onMessage([&](const String& msg) {
        cmdHandler.handle(msg);
    });

    delay(500);
    keypad.begin();
    keypad.onKey([](char key) {
        keypadCmdHandler.onKey(key);
    });
    keypadCmdHandler.onCommand([](const String& cmd) {
        cmdHandler.handle(cmd);
    });
}

void loop() {
    bt.loop();
}
