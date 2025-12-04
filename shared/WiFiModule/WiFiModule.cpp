#include "WiFiModule.h"

WiFiModule::WiFiModule(StorageModule* storage, LCDModule* lcd) : 
    lcd(lcd),
    storage(storage) {}

void WiFiModule::begin() {
    ArduinoJson ::JsonDocument doc;

    if (storage->readJSON("/config.json", doc)) {


        String ssid = doc["ssid"] | "";
        String password = doc["password"] | "";
        lcd->setText("WiFi:", "Connecting to " + ssid);
        Serial.println("[WiFi] Initializing WiFi...");

        Serial.println("[WiFi] Connecting to: " + ssid);
        WiFi.begin(ssid.c_str(), password.c_str());
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 20) {
            delay(500);
            Serial.print(".");
            lcd->setText("WiFi:", "Connecting to " + ssid + " " + String(retries));
            retries++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[WiFi] Connected! IP: " + WiFi.localIP().toString());
            lcd->setText("WiFi Connected", "IP: " + WiFi.localIP().toString());
        } else {
            Serial.println("\n[WiFi] Failed to connect.");
            lcd->setText("WiFi Failed", "Could not connect");
        }
    } else {
        Serial.println("[WiFi] Config not found, skipping WiFi connect");
        lcd->setText("WiFi:", "No config found");
        return;
    }
    
}

bool WiFiModule::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
