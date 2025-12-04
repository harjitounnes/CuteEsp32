#include "BluetoothModule.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "cJSON.h"
#include <string.h>

static const char* TAG = "BluetoothModule";

uint32_t BluetoothModule::_conn_handle = 0;
std::vector<std::string> BluetoothModule::_incoming_msgs;
std::mutex BluetoothModule::_mutex;

BluetoothModule::BluetoothModule(const std::string& deviceName, StorageModule* fsModule)
    : _name(deviceName), storage(fsModule) {}

void BluetoothModule::begin() {
    esp_err_t ret = esp_spp_register_callback(&BluetoothModule::sppCallback);
    ESP_ERROR_CHECK(ret);
    esp_spp_init(ESP_SPP_MODE_CB);
    ESP_LOGI(TAG, "Bluetooth started: %s", _name.c_str());
}

void BluetoothModule::send(const std::string& msg) {
    if (_conn_handle != 0) {
        esp_spp_write(_conn_handle, msg.size(), (uint8_t*)msg.c_str());
    }
}

void BluetoothModule::loop() {
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto& msg : _incoming_msgs) {
        ESP_LOGI(TAG, "[BT RX] %s", msg.c_str());

        if (msg == "read-config") {
            std::string json;
            if (storage->readJSON("/config.json", json)) {
                send(json + "\n");
                ESP_LOGI(TAG, "[BT TX] config sent");
            } else {
                send("{\"error\":\"failed to read config\"}\n");
            }
        }
        else if (msg == "get-ip") {
            esp_netif_t* sta_if = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
            esp_netif_ip_info_t ip_info;
            esp_netif_get_ip_info(sta_if, &ip_info);
            char buf[64];
            snprintf(buf, sizeof(buf), "{\"info\":\"%u.%u.%u.%u\"}\n",
                     IP2STR(&ip_info.ip));
            send(buf);
            ESP_LOGI(TAG, "[BT] IP sent: %s", buf);
        }
        else if (msg == "restart") {
            send("{\"info\":\"restarting ESP32\"}\n");
            ESP_LOGI(TAG, "[BT] Restarting ESP32");
            vTaskDelay(pdMS_TO_TICKS(100));
            esp_restart();
        }
        else if (msg.find("add-config") == 0) {
            // ambil bagian JSON dari command
            size_t jsonStart = msg.find('{');
            if (jsonStart == std::string::npos) {
                send("{\"error\":\"invalid format\"}\n");
                continue;
            }

            std::string jsonPart = msg.substr(jsonStart);
            cJSON* incoming = cJSON_Parse(jsonPart.c_str());
            if (!incoming) {
                send("{\"error\":\"invalid json\"}\n");
                continue;
            }

            cJSON* keyItem = cJSON_GetObjectItem(incoming, "key");
            cJSON* valueItem = cJSON_GetObjectItem(incoming, "value");
            if (!cJSON_IsString(keyItem) || !cJSON_IsString(valueItem)) {
                send("{\"error\":\"missing key or value\"}\n");
                cJSON_Delete(incoming);
                continue;
            }

            std::string key = keyItem->valuestring;
            std::string value = valueItem->valuestring;
            cJSON_Delete(incoming);

            // baca file config lama
            std::string jsonFile;
            storage->readJSON("/config.json", jsonFile);

            cJSON* root = nullptr;
            if (!jsonFile.empty()) {
                root = cJSON_Parse(jsonFile.c_str());
            }
            if (!root) root = cJSON_CreateObject();

            // update key
            cJSON* item = cJSON_GetObjectItem(root, key.c_str());
            if (item) cJSON_SetValuestring(item, value.c_str());
            else cJSON_AddStringToObject(root, key.c_str(), value.c_str());

            // tulis kembali ke SPIFFS
            char* out = cJSON_PrintUnformatted(root);
            storage->writeJSON("/config.json", out);
            send("{\"success\":\"config updated\"}\n");
            ESP_LOGI(TAG, "[BT TX] config updated: %s", key.c_str());

            cJSON_Delete(root);
            free(out);
        }
        else {
            send("Unknown command: " + msg + "\n");
        }
    }

    _incoming_msgs.clear();
}

// --------------------------------------------------
// Callback SPP
// --------------------------------------------------
void BluetoothModule::sppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
    switch(event) {
        case ESP_SPP_SRV_OPEN_EVT:
            _conn_handle = param->srv_open.handle;
            ESP_LOGI(TAG, "SPP client connected");
            break;

        case ESP_SPP_DATA_IND_EVT: {
            std::string msg((char*)param->data_ind.data, param->data_ind.len);
            std::lock_guard<std::mutex> lock(_mutex);
            _incoming_msgs.push_back(msg);
            break;
        }

        default:
            break;
    }
}
