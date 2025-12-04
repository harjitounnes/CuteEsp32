#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WebServer.h>
#include "StorageModule.h"
#include "LCDModule.h"

class OTAUpdateModule {
public:
    OTAUpdateModule(StorageModule* storage, LCDModule* lcd);
    bool begin();                 // Load config + start web routes
    void loop();                  // Handle WebServer loop

    bool checkUpdate();           // Check JSON only
    String getVersion();          // Return current version
    String getEndpoint();         // Return update JSON endpoint

private:
    StorageModule* storage;
    LCDModule* lcd;
    WebServer server{80};
    String latestVersion;
    String firmwareVersion;
    String updateJsonUrl;
    bool updateAvailable = false;
    bool updating = false;
    
    bool loadConfig();
    void handleHomePage();
    void handleProgressUpdate();
    void startOTAUpdate();
    void performOTA();
};    