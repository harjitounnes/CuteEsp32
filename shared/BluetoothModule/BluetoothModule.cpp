#include "BluetoothModule.h"

BluetoothModule::BluetoothModule(const String& deviceName, LCDModule* lcdModule):
name(deviceName), lcd(lcdModule) {}

void BluetoothModule::begin() {
    BT.begin(name);
    Serial.println("[BT] Bluetooth started: " + name);
    lcd->setText("BT", "Started: " + name);
}

void BluetoothModule::onMessage(BTMessageCallback cb) {
    messageCallback = cb;
}

void BluetoothModule::send(const String& msg) {
    BT.print(msg);
}

void BluetoothModule::loop() {
    if (BT.available()) {
        String msg = BT.readStringUntil('\n');
        msg.trim();
        Serial.println("[BT RX] " + msg);
        lcd->setText("BT RX: ", msg);

        if (messageCallback) {
            messageCallback(msg);
        }

    }
}
