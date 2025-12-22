#include <BlinkModule.h>
#include <DotMatrixModule.h>

#define DOT_MATRIX_DIN 23
#define DOT_MATRIX_CLK 18   
#define DOT_MATRIX_CS 5

BlinkModule blink(2, 500);
DotMatrixModule dotMatrix(DOT_MATRIX_DIN, DOT_MATRIX_CLK, DOT_MATRIX_CS);

void setup() {
    Serial.begin(115200);
    blink.start();
    dotMatrix.begin();
    dotMatrix.start("FREE RTOS  RUNNING TEXT  ");
    dotMatrix.setSpeed(50);
    dotMatrix.setBrightness(8);
}

void loop() {
    // put your main code here, to run repeatedly:
}