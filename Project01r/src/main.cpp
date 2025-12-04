#include "esp_log.h"
#include "nvs_flash.h"

#include "StorageModule.h"
#include "WiFiModule.h"
#include "BlinkModule.h"
// #include "BluetoothModule.h"

static const char* TAG = "MAIN";

extern "C" void app_main() {
    ESP_LOGI(TAG, "System starting...");

    // --- INIT NVS (required before WiFi or SPIFFS)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // --- INIT FILESYSTEM
    static StorageModule storage(true);
    if (!storage.begin()) {
        ESP_LOGE(TAG, "Filesystem init failed!");
    }

    // --- INIT WIFI
    static WiFiModule wifi(&storage);
    wifi.begin();

    // static BluetoothModule bt("ESP32_Device", &storage);
    // bt.begin();
    // bt.loop();
    // --- START BLINK
    static BlinkModule blink(2, 500);   // GPIO 2, interval 500ms
    blink.start();

    ESP_LOGI(TAG, "Main done — tasks running.");
}
