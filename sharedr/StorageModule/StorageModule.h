#ifndef STORAGE_MODULE_H
#define STORAGE_MODULE_H

#include "esp_spiffs.h"
#include "esp_log.h"
#include <sys/stat.h>
#include <string>
#include "cJSON.h"

class StorageModule {
public:
    StorageModule(bool formatOnFail = false);

    bool begin();
    bool exists(const std::string& path);
    cJSON* readJSON(const std::string& path);
    bool writeJSON(const std::string& path, cJSON* root);

private:
    bool _formatOnFail;
    std::string basePath = "/spiffs";

    std::string fullPath(const std::string& path);
};

#endif
