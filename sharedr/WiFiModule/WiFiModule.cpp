#include "WiFiModule.h"
#include "esp_log.h"
#include "cJSON.h"

static const char* TAG = "WiFiModule";

WiFiModule::WiFiModule(StorageModule* storage)
    : storage(storage) {}

// --------------------------------------------------
// Event Handler
// --------------------------------------------------
void WiFiModule::eventHandler(void* arg,
                              esp_event_base_t event_base,
                              int32_t event_id,
                              void* event_data)
{
    WiFiModule* self = static_cast<WiFiModule*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "WiFi started, connecting...");
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        self->connected = false;
        ESP_LOGW(TAG, "WiFi disconnected, reconnecting...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        self->connected = true;
    }
}

// --------------------------------------------------
// BEGIN WiFi
// --------------------------------------------------
void WiFiModule::begin() {
    cJSON* root = storage->readJSON("/config.json");
    if (!root) {
        ESP_LOGW(TAG, "Config not found, skipping WiFi");
        return;
    }

    cJSON* ssidItem = cJSON_GetObjectItem(root, "ssid");
    cJSON* passItem = cJSON_GetObjectItem(root, "password");

    if (!ssidItem || !passItem || !cJSON_IsString(ssidItem) || !cJSON_IsString(passItem)) {
        ESP_LOGE(TAG, "Invalid JSON fields");
        cJSON_Delete(root);
        return;
    }

    std::string ssid = ssidItem->valuestring;
    std::string password = passItem->valuestring;
    cJSON_Delete(root);

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid.c_str());

    // ===== 1. Init TCP/IP stack =====
    ESP_ERROR_CHECK(esp_netif_init());

    // ===== 2. Create default event loop =====
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // ===== 3. Create default STA network interface =====
    esp_netif_create_default_wifi_sta();

    // ===== 4. Init WiFi driver =====
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // ===== 5. Register event handlers =====
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        &WiFiModule::eventHandler, this, NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        &WiFiModule::eventHandler, this, NULL
    ));

    // ===== 6. Set mode dan konfigurasi =====
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // ===== 7. Start WiFi =====
    ESP_ERROR_CHECK(esp_wifi_start());
}

// --------------------------------------------------
// CHECK CONNECTED
// --------------------------------------------------
bool WiFiModule::isConnected() {
    return connected;
}
