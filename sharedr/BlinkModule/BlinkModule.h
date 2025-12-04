#ifndef BLINK_MODULE_H
#define BLINK_MODULE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdint.h>

class BlinkModule {
public:
    BlinkModule(uint8_t pin, uint16_t intervalMs);
    void start();

private:
    static void task(void *param);

    uint8_t _pin;
    uint16_t _interval;
};

#endif
