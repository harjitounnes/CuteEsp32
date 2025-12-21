#include "LCD_I2C.h"

// LCD commands
#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ENTRYMODE   0x04
#define LCD_DISPLAYCTL  0x08
#define LCD_FUNCTIONSET 0x20

#define LCD_2LINE       0x08
#define LCD_4BITMODE    0x00
#define LCD_DISPLAYON   0x04

#define LCD_CMD  0
#define LCD_DATA 1

LCD_I2C::LCD_I2C(uint8_t address, uint8_t cols, uint8_t rows)
    : _addr(address), _cols(cols), _rows(rows), _backlight(0x08) {}

void LCD_I2C::init() {
    Wire.begin();
    delay(50);

    // Init 4-bit mode
    write4bits(0x30);
    delay(5);
    write4bits(0x30);
    delay(5);
    write4bits(0x30);
    delay(5);
    write4bits(0x20);

    send(LCD_FUNCTIONSET | LCD_2LINE | LCD_4BITMODE, LCD_CMD);
    send(LCD_DISPLAYCTL | LCD_DISPLAYON, LCD_CMD);
    send(LCD_CLEAR, LCD_CMD);
    delay(2);
    send(LCD_ENTRYMODE | 0x02, LCD_CMD);
}

void LCD_I2C::clear() {
    send(LCD_CLEAR, LCD_CMD);
    delay(2);
}

void LCD_I2C::home() {
    send(LCD_HOME, LCD_CMD);
    delay(2);
}

void LCD_I2C::setCursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    send(0x80 | (col + row_offsets[row]), LCD_CMD);
}

void LCD_I2C::print(const char* str) {
    while (*str) {
        send(*str++, LCD_DATA);
    }
}

void LCD_I2C::backlight(bool on) {
    _backlight = on ? 0x08 : 0x00;
    expanderWrite(0);
}

void LCD_I2C::send(uint8_t value, uint8_t mode) {
    uint8_t high = value & 0xF0;
    uint8_t low  = (value << 4) & 0xF0;

    write4bits(high | (mode ? 0x01 : 0x00));
    write4bits(low  | (mode ? 0x01 : 0x00));
}

void LCD_I2C::write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void LCD_I2C::pulseEnable(uint8_t value) {
    expanderWrite(value | 0x04);
    delayMicroseconds(1);
    expanderWrite(value & ~0x04);
    delayMicroseconds(50);
}

void LCD_I2C::expanderWrite(uint8_t data) {
    Wire.beginTransmission(_addr);
    Wire.write(data | _backlight);
    Wire.endTransmission();
}
