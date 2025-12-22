#include "DotMatrixModule.h"
#include <string.h>

/* =========================================================
   FONT ASCII 5x7 (32–126)
   ========================================================= */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00},
    {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
    // (font DIPERTAHANKAN SAMA seperti milikmu)
};

/* =========================================================
   CONSTRUCTOR & INIT
   ========================================================= */
DotMatrixModule::DotMatrixModule(uint8_t din, uint8_t clk, uint8_t cs)
    : _din(din), _clk(clk), _cs(cs), _speedMs(60), _textQueue(nullptr) {}

void DotMatrixModule::begin() {
    pinMode(_din, OUTPUT);
    pinMode(_clk, OUTPUT);
    pinMode(_cs, OUTPUT);

    send(0x09, 0x00);
    send(0x0A, 0x08);
    send(0x0B, 0x07);
    send(0x0C, 0x01);
    send(0x0F, 0x00);

    clear();
}

/* =========================================================
   LOW LEVEL
   ========================================================= */
void DotMatrixModule::send(uint8_t reg, uint8_t data) {
    digitalWrite(_cs, LOW);
    for (int i = 7; i >= 0; i--) {
        digitalWrite(_clk, LOW);
        digitalWrite(_din, (reg >> i) & 1);
        digitalWrite(_clk, HIGH);
    }
    for (int i = 7; i >= 0; i--) {
        digitalWrite(_clk, LOW);
        digitalWrite(_din, (data >> i) & 1);
        digitalWrite(_clk, HIGH);
    }
    digitalWrite(_cs, HIGH);
}

void DotMatrixModule::clear() {
    for (uint8_t i = 1; i <= 8; i++) send(i, 0x00);
}

void DotMatrixModule::setBrightness(uint8_t level) {
    if (level > 15) level = 15;
    send(0x0A, level);
}

/* =========================================================
   FONT RENDER
   ========================================================= */
void DotMatrixModule::renderCharToColumns(char c, uint8_t cols[6]) {
    if (c < 32 || c > 126) c = ' ';
    const uint8_t* glyph = font5x7[c - 32];

    for (int i = 0; i < 5; i++) cols[i] = glyph[i];
    cols[5] = 0x00;
}

void DotMatrixModule::displayFrame(uint8_t frame[8]) {
    for (uint8_t row = 0; row < 8; row++) {
        send(row + 1, frame[row]);
    }
}

/* =========================================================
   FREERTOS CONTROL
   ========================================================= */
void DotMatrixModule::start(const char* text) {
    if (_textQueue) return; // cegah start 2x

    _textQueue = xQueueCreate(1, sizeof(char[64]));

    char buf[64];
    strncpy(buf, text, sizeof(buf) - 1);
    buf[63] = '\0';
    xQueueOverwrite(_textQueue, buf);

    xTaskCreatePinnedToCore(
        taskEntry,
        "DotMatrixTask",
        4096,
        this,
        2,
        nullptr,
        1
    );
}

void DotMatrixModule::setText(const char* text) {
    if (!_textQueue) return;

    char buf[64];
    strncpy(buf, text, sizeof(buf) - 1);
    buf[63] = '\0';

    xQueueOverwrite(_textQueue, buf);
}

void DotMatrixModule::setSpeed(uint16_t ms) {
    _speedMs = ms;
}

/* =========================================================
   TASK
   ========================================================= */
void DotMatrixModule::taskEntry(void* pv) {
    static_cast<DotMatrixModule*>(pv)->taskRun();
}

void DotMatrixModule::taskRun() {
    uint8_t frame[8] = {0};
    uint8_t cols[6];
    char text[64] = " ";

    for (;;) {
        xQueueReceive(_textQueue, &text, 0);

        const char* p = text;
        while (*p) {
            renderCharToColumns(*p++, cols);

            for (int col = 0; col < 6; col++) {
                for (int row = 0; row < 8; row++) {
                    frame[row] <<= 1;
                    if (cols[col] & (1 << row)) frame[row] |= 1;
                }

                displayFrame(frame);
                vTaskDelay(pdMS_TO_TICKS(_speedMs));
            }
        }
    }
}
