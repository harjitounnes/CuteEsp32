#include <BlinkModule.h>
#include <LCDModule.h>

BlinkModule blink(2, 500);
LCDModule lcd(0x27, 16, 2);

void setup() {
    Serial.begin(115200);
    blink.start();
    delay(500);
    lcd.begin();
    lcd.setText("Hello, World!", "LCD I2C Test");
    lcd.start();
}   

void loop() {
    // put your main code here, to run repeatedly:
}