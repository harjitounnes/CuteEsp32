#include "BlinkModule.h"
#include "esp_log.h"

static const char *TAG = "BlinkModule";

BlinkModule::BlinkModule(uint8_t pin, uint16_t intervalMs) {
    _pin = pin;
    _interval = intervalMs;
}

void BlinkModule::task(void *param) {
    BlinkModule *self = static_cast<BlinkModule*>(param);

    // Konfigurasi GPIO sebagai output
    gpio_reset_pin((gpio_num_t)self->_pin);
    gpio_set_direction((gpio_num_t)self->_pin, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Blink task started on GPIO %d", self->_pin);

    while (true) {
        gpio_set_level((gpio_num_t)self->_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(self->_interval));

        gpio_set_level((gpio_num_t)self->_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(self->_interval));
    }
}

void BlinkModule::start() {
    xTaskCreate(
        BlinkModule::task,
        "BlinkTask",
        2048,     // stack size
        this,     // param
        1,        // priority
        NULL
    );
}
