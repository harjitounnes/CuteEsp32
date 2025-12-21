#include <BlinkModule.h>

BlinkModule blink(2, 500);

void setup() {
    Serial.begin(115200);
    blink.start();
}

void loop() {
    // put your main code here, to run repeatedly:
}