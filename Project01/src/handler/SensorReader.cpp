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
        double sum = 0;
        double offset = -2.5; // Kalibrasi offset suhu
        for (int i=0; i<10; i++){
            sum += self->thermocouple.readCelsius();
            delay(100);
        }
        double tempC = sum / 10 + offset;
        self->message = "Sensor Data: " + String(tempC) + " C"; 
        self->btHandler->send(self->message);
        vTaskDelay(self->sendInterval / portTICK_PERIOD_MS);
    }
}
