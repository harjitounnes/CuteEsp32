#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "StorageModule.h"
#include "LCDModule.h"

using KeyCallback = void (*)(char); 
class KeypadModule {
public:
    explicit KeypadModule(StorageModule* storage, LCDModule* lcd);

    void begin();
    void startTask(UBaseType_t priority = 1);
    void onKey(KeyCallback cb);

private:
    bool loadConfig();
    char scanKey();
    void taskLoop();

    static void taskEntry(void* param);

    StorageModule* _storage;
    LCDModule* _lcd;

    static constexpr uint8_t ROWS = 4;
    static constexpr uint8_t COLS = 4;

    uint8_t _rowPins[ROWS];
    uint8_t _colPins[COLS];
    char    _keymap[ROWS][COLS];

    TaskHandle_t _taskHandle = nullptr;
    void (*_callback)(char) = nullptr;
    KeyCallback _keyCallback = nullptr;
};
