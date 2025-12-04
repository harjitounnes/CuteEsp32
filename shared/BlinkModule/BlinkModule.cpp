#include "BlinkModule.h"

BlinkModule::BlinkModule(uint8_t pin, uint16_t intervalMs): 
    _pin(pin), _interval(intervalMs) {}
void BlinkModule::task(void *param) {

    BlinkModule *self = static_cast<BlinkModule*>(param);

    pinMode(self->_pin, OUTPUT);

    while (true) {
        if(self->paused) {
            digitalWrite(self->_pin, LOW);
            continue;
        }
        digitalWrite(self->_pin, HIGH);
        vTaskDelay(self->_interval / portTICK_PERIOD_MS);
        digitalWrite(self->_pin, LOW);
        vTaskDelay(self->_interval / portTICK_PERIOD_MS);
    }
}

void BlinkModule::stop() {
    paused = true;
}

void BlinkModule::setInterval(uint16_t intervalMs) {
    _interval = intervalMs;
}

void BlinkModule::start() {
    xTaskCreate(
        BlinkModule::task,
        "BlinkTask",
        2048,
        this,
        1,
        NULL
    );
}
