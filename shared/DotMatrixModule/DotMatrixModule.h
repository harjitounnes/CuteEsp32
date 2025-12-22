#pragma once

#include <Arduino.h>

class DotMatrixModule {
public:
    DotMatrixModule(uint8_t din, uint8_t clk, uint8_t cs);

    void begin();
    void setBrightness(uint8_t level);
    void start(const char* text = "HELLO ESP32!");
    void setText(const char* text);      // 🔥 update teks runtime
    void setSpeed(uint16_t ms); 

private:
    uint8_t _din, _clk, _cs;

    uint16_t _speedMs = 60; 
    uint8_t _level  =  8;
    QueueHandle_t _textQueue = nullptr;
    
    static void taskEntry(void* pv);
    void taskRun();
    void clear();
    void send(uint8_t reg, uint8_t data);
    void renderCharToColumns(char c, uint8_t cols[6]);
    void displayFrame(uint8_t frame[8]);
};
