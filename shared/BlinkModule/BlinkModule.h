#ifndef BLINK_MODULE_H
#define BLINK_MODULE_H

#include <Arduino.h>

class BlinkModule {
public:
    BlinkModule(uint8_t pin = 2, uint16_t intervalMs = 500);

    void start();
    void stop();
    void setInterval(uint16_t intervalMs);

private:
    uint8_t _pin;
    uint16_t _interval;
    static void task(void *param);
    bool paused = false;
};

#endif
