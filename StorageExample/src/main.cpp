#include <BlinkModule.h>
#include <LCDModule.h>
#include <StorageModule.h>

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);
StorageModule storage(true, &lcd);

void setup() {
    Serial.begin(115200);
    blink.start();
    delay(500);
    lcd.begin();
    lcd.setText("Storage Example", "Initializing...");
    lcd.start();
    storage.begin();
    lcd.setText("Storage Example", "Ready");
}

void loop() {
    // put your main code here, to run repeatedly:
}