#include "KeypadModule.h"

KeypadModule::KeypadModule(StorageModule* storage, LCDModule* lcd)
    : _storage(storage), _lcd(lcd) {}

void KeypadModule::begin() {
    if (!loadConfig()) {
        return;
    }

    for (uint8_t r = 0; r < ROWS; r++) {
        pinMode(_rowPins[r], OUTPUT);
        digitalWrite(_rowPins[r], HIGH);
    }

    for (uint8_t c = 0; c < COLS; c++) {
        pinMode(_colPins[c], INPUT_PULLUP);
    }
    startTask(0);
}

void KeypadModule::onKey(KeyCallback cb) {
    _keyCallback = cb;
}

bool KeypadModule::loadConfig() {
    ArduinoJson::JsonDocument doc;
    if (!_storage->readJSON("/config.json", doc)) {
        _lcd->setText("Keypad", "Data config tidak ditemukan");
        return false;
    }

    if(!doc["keypad"].is<JsonObject>()) {
        _lcd->setText("Keypad", "Format config salah");
        return false;
    }

    JsonArray rowPinsArr = doc["keypad"]["row_pins"] | JsonArray();
    JsonArray colPinsArr = doc["keypad"]["col_pins"] | JsonArray();
    JsonArray keymapArr = doc["keypad"]["keymap"] | JsonArray();

    if (rowPinsArr.size() != ROWS || colPinsArr.size() != COLS ) {
        _lcd->setText("Keypad", "Data config tidak lengkap." );   
        return false;
    }

    for (uint8_t i = 0; i < ROWS; i++) {
        _rowPins[i] = rowPinsArr[i] | 0;
    }

    for (uint8_t i = 0; i < COLS; i++) {
        _colPins[i] = colPinsArr[i] | 0;
    }

    for (uint8_t r = 0; r < ROWS; r++) {
        JsonArray row = keymapArr[r].as<JsonArray>();

        for (uint8_t c = 0; c < COLS; c++) {
            const char* key = row[c].as<const char*>();
            _keymap[r][c] = (key && key[0]) ? key[0] : '\0';
        }
    }

    return true;
}

void KeypadModule::startTask(UBaseType_t core) {
    xTaskCreatePinnedToCore(
        taskEntry,
        "KeypadTask",
        2048,
        this,
        3,
        &_taskHandle,
        core
    );
}

void KeypadModule::taskEntry(void* param) {
    static_cast<KeypadModule*>(param)->taskLoop();
}

void KeypadModule::taskLoop() {
    char lastKey = 0;
    Serial.println("Keypad task running");

    while (true) {
        char key = scanKey();

        if (key && key != lastKey) {
            if (_keyCallback) {
                _keyCallback(key);
            }
            lastKey = key;
        }

        if (!key) {
            lastKey = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(30)); // debounce + ringan CPU
    }
}

char KeypadModule::scanKey() {
    for (uint8_t r = 0; r < ROWS; r++) {
        digitalWrite(_rowPins[r], LOW);
        for (uint8_t c = 0; c < COLS; c++) {
            if (digitalRead(_colPins[c]) == LOW) {
                digitalWrite(_rowPins[r], HIGH);
                return _keymap[r][c];
            }
        }

        digitalWrite(_rowPins[r], HIGH);
    }
    return 0;
}
