#include "Quarky.h"

// LEDC channel assignments (used on ESP32 Arduino Core 2.x)
static constexpr uint8_t CH_MOTOR_R1 = 0;
static constexpr uint8_t CH_MOTOR_R2 = 1;
static constexpr uint8_t CH_MOTOR_L1 = 2;
static constexpr uint8_t CH_MOTOR_L2 = 3;
static constexpr uint8_t CH_SERVO_1  = 4;
static constexpr uint8_t CH_SERVO_2  = 5;

static constexpr uint32_t MOTOR_PWM_FREQ = 20000; // 20 kHz (silent)
static constexpr uint8_t  MOTOR_PWM_RES  = 8;     // 0–255

static constexpr uint32_t SERVO_PWM_FREQ = 50;    // 50 Hz
static constexpr uint8_t  SERVO_PWM_RES  = 16;    // 0–65535

static const uint8_t TOUCH_PINS[5] = {
  QUARKY_TOUCH_1, QUARKY_TOUCH_2, QUARKY_TOUCH_3, QUARKY_TOUCH_4, QUARKY_TOUCH_5
};

Quarky::Quarky()
  : _leds(QUARKY_LED_COUNT, QUARKY_LED_PIN, NEO_GRB + NEO_KHZ800) {}

void Quarky::_pwmSetup(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resBits) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, freq, resBits);
#else
  ledcSetup(channel, freq, resBits);
  ledcAttachPin(pin, channel);
#endif
}

void Quarky::_pwmWrite(uint8_t pin, uint8_t channel, uint32_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pin, duty);
#else
  ledcWrite(channel, duty);
#endif
}

void Quarky::begin(uint8_t ledBrightness) {
  analogReadResolution(12);

  // Motors
  _pwmSetup(QUARKY_MOTOR_R_IN1, CH_MOTOR_R1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  _pwmSetup(QUARKY_MOTOR_R_IN2, CH_MOTOR_R2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  _pwmSetup(QUARKY_MOTOR_L_IN1, CH_MOTOR_L1, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  _pwmSetup(QUARKY_MOTOR_L_IN2, CH_MOTOR_L2, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  stopMotors(false);

  // Servos
  _pwmSetup(QUARKY_SERVO_1, CH_SERVO_1, SERVO_PWM_FREQ, SERVO_PWM_RES);
  _pwmSetup(QUARKY_SERVO_2, CH_SERVO_2, SERVO_PWM_FREQ, SERVO_PWM_RES);

  // LED Matrix
  _leds.begin();
  _leds.setBrightness(ledBrightness);
  _leds.clear();
  _leds.show();

  // Buttons & IR
  pinMode(QUARKY_BTN_PIN, INPUT);
  pinMode(QUARKY_IR_L_PIN, INPUT);
  pinMode(QUARKY_IR_R_PIN, INPUT);
  if (QUARKY_IR_EN_PIN >= 0) {
    pinMode(QUARKY_IR_EN_PIN, OUTPUT);
    digitalWrite(QUARKY_IR_EN_PIN, LOW);
  }
}

// --- Motors ---
void Quarky::_setHBridge(uint8_t pin1, uint8_t ch1, uint8_t pin2, uint8_t ch2, int16_t speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    _pwmWrite(pin1, ch1, speed);
    _pwmWrite(pin2, ch2, 0);
  } else if (speed < 0) {
    _pwmWrite(pin1, ch1, 0);
    _pwmWrite(pin2, ch2, -speed);
  } else {
    _pwmWrite(pin1, ch1, 0);
    _pwmWrite(pin2, ch2, 0);
  }
}

void Quarky::setMotorL(int16_t speed) {
  _setHBridge(QUARKY_MOTOR_L_IN1, CH_MOTOR_L1, QUARKY_MOTOR_L_IN2, CH_MOTOR_L2, speed);
}

void Quarky::setMotorR(int16_t speed) {
  _setHBridge(QUARKY_MOTOR_R_IN1, CH_MOTOR_R1, QUARKY_MOTOR_R_IN2, CH_MOTOR_R2, speed);
}

void Quarky::drive(int16_t leftSpeed, int16_t rightSpeed) {
  setMotorL(leftSpeed);
  setMotorR(rightSpeed);
}

void Quarky::stopMotors(bool brake) {
  uint8_t level = brake ? 255 : 0;
  _pwmWrite(QUARKY_MOTOR_L_IN1, CH_MOTOR_L1, level);
  _pwmWrite(QUARKY_MOTOR_L_IN2, CH_MOTOR_L2, level);
  _pwmWrite(QUARKY_MOTOR_R_IN1, CH_MOTOR_R1, level);
  _pwmWrite(QUARKY_MOTOR_R_IN2, CH_MOTOR_R2, level);
}

// --- Servos ---
void Quarky::setServo(uint8_t servoIndex, uint8_t angle) {
  angle = constrain(angle, 0, 180);
  uint32_t pulseUs = map(angle, 0, 180, 500, 2400);
  uint32_t duty    = (pulseUs * 65535UL) / 20000UL;

  if (servoIndex == 1) {
    _pwmWrite(QUARKY_SERVO_1, CH_SERVO_1, duty);
  } else if (servoIndex == 2) {
    _pwmWrite(QUARKY_SERVO_2, CH_SERVO_2, duty);
  }
}

// --- 7x5 LED Matrix ---
void Quarky::setBrightness(uint8_t brightness) {
  _leds.setBrightness(brightness);
}

void Quarky::setPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
  if (x >= QUARKY_LED_WIDTH || y >= QUARKY_LED_HEIGHT) return;
  uint8_t idx = y * QUARKY_LED_WIDTH + x; // Top-left, progressive row-major
  _leds.setPixelColor(idx, _leds.Color(r, g, b));
}

void Quarky::setPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= QUARKY_LED_COUNT) return;
  _leds.setPixelColor(index, _leds.Color(r, g, b));
}

void Quarky::fillMatrix(uint8_t r, uint8_t g, uint8_t b) {
  _leds.fill(_leds.Color(r, g, b));
}

void Quarky::clearMatrix() {
  _leds.clear();
}

void Quarky::showMatrix() {
  _leds.show();
}

// --- Buttons ---
uint16_t Quarky::readButtonRaw() {
  return analogRead(QUARKY_BTN_PIN);
}

QuarkyButton Quarky::readButton() {
  uint16_t val = analogRead(QUARKY_BTN_PIN);
  if (val > 3000) return BTN_R; // ~3.3V
  if (val > 900)  return BTN_L; // ~1.3V
  return BTN_NONE;
}

bool Quarky::isButtonPressed(QuarkyButton btn) {
  return readButton() == btn;
}

// --- Capacitive Touch Pads ---
uint16_t Quarky::readTouch(uint8_t pad) {
  if (pad < 1 || pad > 5) return 0;
  return touchRead(TOUCH_PINS[pad - 1]);
}

bool Quarky::isTouched(uint8_t pad, uint16_t threshold) {
  if (pad < 1 || pad > 5) return false;
  return touchRead(TOUCH_PINS[pad - 1]) < threshold;
}

// --- Speaker ---
void Quarky::beep(uint16_t freqHz, uint16_t durationMs) {
  tone(QUARKY_SPEAKER_PIN, freqHz, durationMs);
}

void Quarky::stopTone() {
  noTone(QUARKY_SPEAKER_PIN);
}

void Quarky::writeDAC(uint8_t value) {
  dacWrite(QUARKY_SPEAKER_PIN, value);
}

// --- IR Sensors ---
void Quarky::setIREmitters(bool enabled) {
  if (QUARKY_IR_EN_PIN >= 0) {
    digitalWrite(QUARKY_IR_EN_PIN, enabled ? HIGH : LOW);
  }
}

uint16_t Quarky::readIRLeft() {
  return analogRead(QUARKY_IR_L_PIN);
}

uint16_t Quarky::readIRRight() {
  return analogRead(QUARKY_IR_R_PIN);
}
