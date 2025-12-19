#include <Arduino.h>
#include <BlinkModule.h>
#include <LCDModule.h>
#include <StorageModule.h>
#include <BluetoothModule.h>
#include <KeypadModule.h>
// #include <WiFiModule.h>
// #include <ArduinoOTA.h>
// #include <OTAUpdateModule.h>
// #include <CamModule.h>
// #include "esp_system.h"

#include "handler/BTCommandHandler.h"
#include "handler/KeypadCommandHandler.h"
// #include "handler/SensorReader.h"

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);
BluetoothModule bt("ESP32_Device", &lcd);
BTCommandHandler cmdHandler(&bt, &storage, &lcd);
KeypadModule keypad(&storage, &lcd);
KeypadCommandHandler keypadCmdHandler(&lcd);
// WiFiModule wifi(&storage, &lcd);
// // OTAUpdateModule ota(&storage, &lcd);
// SensorReader sensorReader(&cmdHandler, 10000); // Kirim setiap 120000 ms

// CamModule cam(80, "ICON+", "M12@n0@mrq@15@r@");

// void onKeyPressed(char key) {
    
// }

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
    // wifi.begin();
    // delay(3000);

    // // ota.begin();
    // delay(10000);
    // ArduinoOTA.setPort(3232);
    // ArduinoOTA.begin();
    // ArduinoOTA.setPassword("1234"); 
    // delay(10000);
    // sensorReader.begin();
    // blink.setInterval(200);
}

void loop() {
    // cam.loop();
    // ArduinoOTA.handle();
    bt.loop();
    // ota.loop();
}

// uint8_t rowPins[4] = {12, 14, 27, 26};
// uint8_t colPins[4] = {25, 33, 32, 13};

// void setup() {
//     Serial.begin(115200);

//     for (int r = 0; r < 4; r++) {
//         pinMode(rowPins[r], OUTPUT);
//         digitalWrite(rowPins[r], HIGH);
//     }

//     for (int c = 0; c < 4; c++) {
//         pinMode(colPins[c], INPUT_PULLUP);
//     }
// }

// void loop() {
//     for (int r = 0; r < 4; r++) {
//         digitalWrite(rowPins[r], LOW);

//         for (int c = 0; c < 4; c++) {
//             if (digitalRead(colPins[c]) == LOW) {
//                 Serial.printf("Pressed R%d C%d\n", r, c);
//                 delay(300);
//             }
//         }

//         digitalWrite(rowPins[r], HIGH);
//     }
// }
