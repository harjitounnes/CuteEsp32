#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include "StorageModule.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>

class WiFiModule {
public:
    WiFiModule(StorageModule* storage);
    void begin();
    bool isConnected();

private:
    StorageModule* storage;
    bool connected = false;

    static void eventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data);
};

#endif
