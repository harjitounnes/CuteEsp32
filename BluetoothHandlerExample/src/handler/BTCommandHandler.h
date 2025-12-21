#pragma once
#include <Arduino.h>
#include <StorageModule.h>
#include "BluetoothModule.h"
#include <WiFi.h>

class BTCommandHandler {
    public:
    BTCommandHandler(BluetoothModule* bt, StorageModule* storage, LCDModule* lcd);
    void handle(const String& msg);
    #include <LCDModule.h>
    void send(const String& msg);

private:
    BluetoothModule* bt;
    StorageModule* storage;
    LCDModule* lcd;
    void saveConfig(const char* path, const char* key, const String& value);
};
