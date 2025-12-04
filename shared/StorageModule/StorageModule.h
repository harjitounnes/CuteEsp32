#ifndef STORAGE_MODULE_H
#define STORAGE_MODULE_H

#include <LittleFS.h>
#include <ArduinoJson.h> 
#include <LCDModule.h>

class StorageModule {
public:
    StorageModule(bool formatOnFail = true, LCDModule* lcd = nullptr);

    bool begin();
    bool exists(const String& path);

    bool writeJSON(const String& path, const JsonDocument& doc);
    bool readJSON(const String& path, JsonDocument& doc);

private:
    bool formatOnFail;
    LCDModule* lcd;
};

#endif
