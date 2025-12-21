#pragma once
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
// #include <string>
#include <LCD_I2C.h>


class LCDModule {
public:
    LCDModule(uint8_t addr, uint8_t cols, uint8_t rows);

    void begin();
    void start();
    void setText(const String& text1, const String& text2);

private:
    LCD_I2C lcd;
    SemaphoreHandle_t mutex;

    static void taskRunner(void *param);
    String line1;
    String line2;
    uint8_t rowLength = 16;
    void taskLoop();
};
