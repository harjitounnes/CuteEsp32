#include "SensorReader.h"

SensorReader::SensorReader(BTCommandHandler* handler, uint32_t intervalMs):
    btHandler(handler), sendInterval(intervalMs), message("Hello"), taskHandle(nullptr){}

void SensorReader::begin() {
    xTaskCreatePinnedToCore(
        SensorReader::taskLoop,
        "BT_Sender_Task",
        4096,
        this,
        1,
        &taskHandle,
        1   // core 1
    );
}

void SensorReader::setInterval(uint32_t ms) {
    sendInterval = ms;
}


void SensorReader::taskLoop(void* param) {
    SensorReader* self = static_cast<SensorReader*>(param);

    for (;;) {
        self->message = "Sensor Data: " + String(random(0, 100)); 
        self->btHandler->send(self->message);
        vTaskDelay(self->sendInterval / portTICK_PERIOD_MS);
    }
}
