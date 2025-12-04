#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "esp_spp_api.h"
#include "StorageModule.h"

class BluetoothModule {
public:
    BluetoothModule(const std::string& deviceName, StorageModule* fsModule);

    void begin();
    void loop();
    void send(const std::string& msg);

private:
    static void sppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    std::string _name;
    StorageModule* storage;
    static uint32_t _conn_handle;
    static std::vector<std::string> _incoming_msgs;
    static std::mutex _mutex;
};
