#include "StorageModule.h"

StorageModule::StorageModule(bool formatOnFail, LCDModule* lcd): 
    formatOnFail(formatOnFail), lcd(lcd) {
}

bool StorageModule::begin() {
    if (!LittleFS.begin(formatOnFail)) {
        Serial.println("[FS] Mount Failed");
        lcd->setText("FS Error", "Mount Failed");
        return false;
    }
    Serial.println("[FS] LittleFS Mounted");
    lcd->setText("FS Mounted", "LittleFS Ready");
    return true;
}

bool StorageModule::exists(const String& path) {
    return LittleFS.exists(path);
}

bool StorageModule::writeJSON(const String& path, const JsonDocument& doc) {
    File file = LittleFS.open(path, "w");
    if (!file) return false;
    serializeJson(doc, file);
    file.close();
    return true;
}

bool StorageModule::readJSON(const String& path, JsonDocument& doc) {
    File file = LittleFS.open(path, "r");
    if (!file) return "";

    DeserializationError err = deserializeJson(doc, file);
    file.close();
    return !err;
}

