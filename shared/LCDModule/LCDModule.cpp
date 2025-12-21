#include "LCDModule.h"
#include <Arduino.h>

LCDModule::LCDModule(uint8_t addr, uint8_t cols, uint8_t rows)
    : lcd(addr, cols, rows) 
{
    mutex = xSemaphoreCreateMutex();
    rowLength = cols;
}

void LCDModule::begin() {
    lcd.init();
    lcd.backlight(true);
}

void LCDModule::start() {
    xTaskCreate(taskRunner, "LCDTask", 4096, this, 1, NULL);
}

void LCDModule::setText(const String& text1, const String& text2) {
    if (xSemaphoreTake(mutex, 50)) {
        line1 = text1;
        line2 = text2;
        xSemaphoreGive(mutex);
    }
}

void LCDModule::taskRunner(void *param) {
    LCDModule *self = static_cast<LCDModule*>(param);
    self->taskLoop();
}

void LCDModule::taskLoop() {
    int start = 0;

    while (true) {
        lcd.setCursor(0, 0);
        while (line1.length() < int(rowLength))   {
            line1 += ' ';
        }
       
        lcd.print(line1.c_str());

        String displayText;

        if (xSemaphoreTake(mutex, 50)) {
            displayText = line2;
            xSemaphoreGive(mutex);
        }

        int minimumLength = min((int)(start + int(rowLength)), (int)displayText.length());
        String segment = displayText.substring(start, minimumLength);
        while (segment.length() < int(rowLength))   {
            segment += ' ';
        }
        lcd.setCursor(0, 1);
        lcd.print(segment.c_str());

        start++;
        if (start > displayText.length()) start = 0;

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
