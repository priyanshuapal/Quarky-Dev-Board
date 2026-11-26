#include <Arduino.h>
#include "Quarky.h"

Quarky bot;

void setup() {
  Serial.begin(115200);
  bot.begin(40);

  // Startup chime + green smile check
  bot.beep(880, 100);
  delay(120);
  bot.beep(1320, 150);

  bot.fillMatrix(0, 80, 255);
  bot.showMatrix();
  delay(500);
  bot.clearMatrix();
  bot.showMatrix();

  bot.setServo(1, 90);
  bot.setServo(2, 90);
}

void loop() {
  QuarkyButton btn = bot.readButton();

  if (btn == BTN_L) {
    bot.drive(180, 180);
    bot.fillMatrix(0, 255, 0);
    bot.showMatrix();
  } else if (btn == BTN_R) {
    bot.drive(-180, -180);
    bot.fillMatrix(255, 0, 0);
    bot.showMatrix();
  } else {
    bot.stopMotors();
    bot.clearMatrix();

    // Light up bottom row columns 1–5 when touch pads 1–5 are touched
    for (uint8_t pad = 1; pad <= 5; pad++) {
      if (bot.isTouched(pad, 40)) {
        bot.setPixel(pad, 4, 255, 180, 0);
      }
    }
    bot.showMatrix();
  }

  delay(30);
}
