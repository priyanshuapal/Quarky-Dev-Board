#pragma once

// --- General Purpose Headers ---
#ifndef QUARKY_D1
  #define QUARKY_D1 18
#endif
#ifndef QUARKY_D2
  #define QUARKY_D2 19
#endif
#ifndef QUARKY_D3
  #define QUARKY_D3 26
#endif

#ifndef QUARKY_A1
  #define QUARKY_A1 33
#endif
#ifndef QUARKY_A2
  #define QUARKY_A2 32
#endif
#ifndef QUARKY_A3
  #define QUARKY_A3 39
#endif

// --- DRV8833 DC Motor Driver ---
// Channel A = Motor R, Channel B = Motor L
#define QUARKY_MOTOR_R_IN1 2
#define QUARKY_MOTOR_R_IN2 17
#define QUARKY_MOTOR_L_IN1 4
#define QUARKY_MOTOR_L_IN2 16

// --- Servo Headers ---
#define QUARKY_SERVO_1 22
#define QUARKY_SERVO_2 23

// --- 7x5 Addressable LED Matrix ---
#define QUARKY_LED_PIN    21
#define QUARKY_LED_WIDTH  7
#define QUARKY_LED_HEIGHT 5
#define QUARKY_LED_COUNT  (QUARKY_LED_WIDTH * QUARKY_LED_HEIGHT)

// --- Audio (NS4150B Amp on DAC1) ---
#define QUARKY_SPEAKER_PIN 25

// --- Tactile Buttons (Resistor Ladder on ADC1_CH0) ---
// L ≈ 1.3V (~1610), R ≈ 3.3V (~4095)
#define QUARKY_BTN_PIN 36

// --- Capacitive Touch Pads (1-5) ---
#define QUARKY_TOUCH_1 15
#define QUARKY_TOUCH_2 13
#define QUARKY_TOUCH_3 12
#define QUARKY_TOUCH_4 14
#ifndef QUARKY_TOUCH_5
  #define QUARKY_TOUCH_5 27 // TBD: verify on hardware
#endif

// --- IR Proximity / Line Sensors (TBD: verify on hardware) ---
#ifndef QUARKY_IR_L_PIN
  #define QUARKY_IR_L_PIN 34
#endif
#ifndef QUARKY_IR_R_PIN
  #define QUARKY_IR_R_PIN 35
#endif
#ifndef QUARKY_IR_EN_PIN
  #define QUARKY_IR_EN_PIN 5 // Set to -1 if emitters are hardwired ON
#endif
