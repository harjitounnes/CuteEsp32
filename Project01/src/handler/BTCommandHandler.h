#pragma once
#include <Arduino.h>
#include <StorageModule.h>
#include <LCDModule.h>
#include "BluetoothModule.h"

class BTCommandHandler {
public:
    BTCommandHandler(BluetoothModule* bt, StorageModule* storage, LCDModule* lcd);
    void handle(const String& msg);
    void send(const String& msg);

private:
    BluetoothModule* bt;
    StorageModule* storage;
    LCDModule* lcd;
    void saveConfig(const char* path, const char* key, const String& value);
};
