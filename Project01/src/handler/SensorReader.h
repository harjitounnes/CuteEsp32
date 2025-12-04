#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <Arduino.h>
#include "BTCommandHandler.h"   // handler milik kamu
#include <SPI.h>
#include <max6675.h>

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
    int thermoDO = 19;
    int thermoCS = 5;
    int thermoCLK = 18;
    MAX6675 thermocouple = MAX6675(thermoCLK, thermoCS, thermoDO);
};

#endif
