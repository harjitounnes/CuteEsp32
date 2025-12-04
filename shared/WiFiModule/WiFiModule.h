#pragma once
#include <WiFi.h>
#include <StorageModule.h>
#include <LCDModule.h>

class WiFiModule {
public:
    WiFiModule(StorageModule* storage, LCDModule* lcd);

    void begin();
    bool isConnected();

private:
    StorageModule* storage;
    LCDModule* lcd;
};
