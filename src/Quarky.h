#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "QuarkyPins.h"

enum QuarkyButton : uint8_t {
  BTN_NONE = 0,
  BTN_L    = 1,
  BTN_R    = 2
};

class Quarky {
public:
  Quarky();

  // Initializes all onboard peripherals (safe default LED brightness = 40/255)
  void begin(uint8_t ledBrightness = 40);

  // --- DC Motors (speed: -255 to +255) ---
  void setMotorL(int16_t speed);
  void setMotorR(int16_t speed);
  void drive(int16_t leftSpeed, int16_t rightSpeed);
  void stopMotors(bool brake = false);

  // --- Servos (servoIndex: 1 or 2, angle: 0 to 180) ---
  void setServo(uint8_t servoIndex, uint8_t angle);

  // --- 7x5 RGB LED Matrix ---
  void setBrightness(uint8_t brightness);
  void setPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
  void setPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
  void fillMatrix(uint8_t r, uint8_t g, uint8_t b);
  void clearMatrix();
  void showMatrix();
  Adafruit_NeoPixel& leds() { return _leds; }

  // --- Buttons ---
  uint16_t readButtonRaw();
  QuarkyButton readButton();
  bool isButtonPressed(QuarkyButton btn);

  // --- Capacitive Touch Pads (pad: 1 to 5) ---
  uint16_t readTouch(uint8_t pad);
  bool isTouched(uint8_t pad, uint16_t threshold = 40);

  // --- Speaker (NS4150B on DAC1) ---
  void beep(uint16_t freqHz, uint16_t durationMs = 150);
  void stopTone();
  void writeDAC(uint8_t value);

  // --- IR Sensors ---
  void setIREmitters(bool enabled);
  uint16_t readIRLeft();
  uint16_t readIRRight();

private:
  Adafruit_NeoPixel _leds;

  static void _pwmSetup(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resBits);
  static void _pwmWrite(uint8_t pin, uint8_t channel, uint32_t duty);
  void _setHBridge(uint8_t pin1, uint8_t ch1, uint8_t pin2, uint8_t ch2, int16_t speed);
};
