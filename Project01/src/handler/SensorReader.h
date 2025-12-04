#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <Arduino.h>
#include "BTCommandHandler.h"   // handler milik kamu

class SensorReader {
public:
    SensorReader(BTCommandHandler* handler, uint32_t intervalMs = 10000);

    void begin();                       // mulai task
    void setInterval(uint32_t ms);      // ubah interval
    static void taskLoop(void* param);  // loop RTOS

private:
    BTCommandHandler* btHandler;
    uint32_t sendInterval;
    String message;
    TaskHandle_t taskHandle;
};

#endif
