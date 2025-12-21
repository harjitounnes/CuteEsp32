#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <Arduino.h>
#include <Wire.h>

class LCD_I2C {
public:
    LCD_I2C(uint8_t address, uint8_t cols, uint8_t rows);

    void init();
    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row);
    void print(const char* str);
    void backlight(bool on);

private:
    uint8_t _addr;
    uint8_t _cols, _rows;
    uint8_t _backlight;

    void send(uint8_t value, uint8_t mode);
    void write4bits(uint8_t value);
    void pulseEnable(uint8_t value);
    void expanderWrite(uint8_t data);
};

#endif
