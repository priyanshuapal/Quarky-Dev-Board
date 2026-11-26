# AGENTS.md — QuarkyESP32

## Project Overview
This repository is an Arduino/PlatformIO C++ library and hardware pinout for the **STEMpedia Quarky** (ESP32-based robotics board).

## Build & Toolchain
- **Framework:** Arduino on `espressif32` (PlatformIO `board = esp32dev`).
- **Build command:** `pio run`
- **Upload command:** `pio run -t upload`
- **Serial Monitor:** `115200` baud.
- **Dependencies:** `adafruit/Adafruit NeoPixel` (declared in `library.json`).
- **Core compatibility:** Must compile cleanly on both ESP32 Arduino Core 2.x (`ledcSetup`/`ledcAttachPin`) and 3.x (`ledcAttach`).

## Hardware Pinout & Constraints (Strict)
Always include `"Quarky.h"` (which pulls in `"QuarkyPins.h"`). Never hardcode raw GPIO numbers when a `QUARKY_*` macro exists.

- **DC Motors (DRV8833):**
  - Motor R (Ch A): `GPIO 2` (`AIN1`), `GPIO 17` (`AIN2`)
  - Motor L (Ch B): `GPIO 4` (`BIN1`), `GPIO 16` (`BIN2`)
  - **Warning:** `GPIO 2` is an ESP32 strapping pin; do not add external pull-ups or drive it HIGH during boot.
  - Use `bot.drive(left, right)` with speeds from `-255` to `255`.
- **Servos:** `GPIO 22` (Servo 1), `GPIO 23` (Servo 2). Use `bot.setServo(1|2, 0..180)`. Do not pull in third-party servo libraries; hardware LEDC 50 Hz PWM is built in.
- **7×5 RGB LED Matrix:** `GPIO 21`, 35 WS2812B-compatible LEDs, top-left `(0,0)` progressive row-major (`index = y * 7 + x`).
  - **Power constraint:** Keep default brightness <= `50` (out of `255`) when running on battery to prevent brownouts. Always call `bot.showMatrix()` after pixel updates.
- **Buttons (`GPIO 36`, ADC1_CH0):** Input-only resistor ladder (`BTN_L` ≈ 1.3 V, `BTN_R` ≈ 3.3 V). Use `bot.readButton()` or `bot.isButtonPressed()`. Never call `pinMode(36, INPUT_PULLUP)` (GPIO 36 has no internal pull-up).
- **Capacitive Touch Pads (1–5):** `GPIO 15, 13, 12, 14, 27` (Pad 5 TBD).
  - **Warning:** `GPIO 12` is a strapping pin (MTDI); do not pull HIGH at boot.
- **Audio (`GPIO 25` → NS4150B amp):** Connected to ESP32 `DAC1`. Use `bot.beep(freq, ms)` for tones or `bot.writeDAC(0..255)` for 8-bit PCM audio.
- **IR Sensors:** `GPIO 34` (Left), `GPIO 35` (Right), `GPIO 5` (Emitter enable, TBD). Input-only ADC1 pins.
- **Breakout Headers:** `D1=18`, `D2=19`, `D3=26`, `A1=33`, `A2=32`, `A3=39` (input-only).

## Coding Guidelines
- Always call `bot.begin()` in `setup()` before using any peripheral.
- Prefer non-blocking `millis()` timers over long `delay()` calls in `loop()` so button/touch polling stays responsive.
- Keep Wi-Fi/BLE compatible: all onboard analog inputs use `ADC1`; do not move analog reads to `ADC2` pins.
