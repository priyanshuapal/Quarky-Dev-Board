# QuarkyESP32

Reverse-engineered GPIO pinout, hardware notes, and Arduino/PlatformIO C++ library for the **STEMpedia Quarky** (ESP32).

> **Status:** Work in Progress. Most peripherals are verified; IR sensors are pending final hardware verification.

---

## Installation

### AI Agent
Download the AGENTS.md file and give it to your preferred agent of choice.


### Arduino IDE
1. Download this repository as a `.zip` file.
2. In Arduino IDE, go to **Sketch → Include Library → Add .ZIP Library...** and select the downloaded file.
3. Install **Adafruit NeoPixel** from the Library Manager.


### PlatformIO (Recommended)
Add the repository URL to `lib_deps` in your `platformio.ini`:

```ini
[env:quarky]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/priyanshuapal/Quarky-Dev-Board.git
```

*(PlatformIO automatically pulls `adafruit/Adafruit NeoPixel` via `library.json`.)*

---
## Hardware Overview

<img width="1832" height="2987" alt="IMG_20261126_212912 jpg" src="https://github.com/user-attachments/assets/3a5e04ea-4400-4bbd-a7eb-8e7997b2f995" />


| Subsystem | Component / Spec | Notes |
| :--- | :--- | :--- |
| **MCU** | ESP32 (Dual-Core) | Standard ESP32-WROOM/WROVER architecture |
| **USB-to-Serial** | CH340G | USB-C interface for flashing and serial monitor |
| **Motor Driver** | DRV8833 | Dual H-Bridge for 2× DC motors |
| **Display** | 7×5 RGB LED Matrix | 35 addressable LEDs, top-left origin, progressive (row-major) |
| **Audio** | NS4150B + Speaker | Driven via ESP32 internal DAC1 (`GPIO 25`) |
| **Power & Charging** | TP4056 + Boost Converter | 1S Li-ion charging via USB-C; internal step-up for battery operation |
| **Controls** | 2× Tactile Buttons, 5× Touch Pads, 1× Reset | L/R buttons share a resistor ladder on `GPIO 36` |
| **Sensors** | 2× IR Proximity/Line Sensors | Pinout TBD |

---

## GPIO Pinout Map

<img width="1639" height="2600" alt="IMG_20261126_212926 jpg" src="https://github.com/user-attachments/assets/bfc96a42-a10c-411e-abd0-935574d99682" />


| GPIO | Board Label / Peripheral | Function / Type | Notes |
| :---: | :--- | :--- | :--- |
| **2** | Motor R — `AIN1` | Output (PWM) | DRV8833 Channel A; strapping pin (keep LOW/floating at boot) |
| **4** | Motor L — `BIN1` | Output (PWM) | DRV8833 Channel B |
| **5** | *Unconfirmed* | TBD | Candidate for IR emitter enable |
| **12** | Touch Pad 3 | Capacitive Touch (`T5`) | Strapping pin (MTDI); avoid pulling HIGH at boot |
| **13** | Touch Pad 2 | Capacitive Touch (`T4`) | |
| **14** | Touch Pad 4 | Capacitive Touch (`T6`) | |
| **15** | Touch Pad 1 | Capacitive Touch (`T3`) | |
| **16** | Motor L — `BIN2` | Output (PWM) | DRV8833 Channel B |
| **17** | Motor R — `AIN2` | Output (PWM) | DRV8833 Channel A |
| **18** | Header `D1` | Digital I/O | General-purpose breakout |
| **19** | Header `D2` | Digital I/O | General-purpose breakout |
| **21** | 7×5 LED Matrix | Data Out | Addressable RGB (WS2812B-compatible), 35 LEDs |
| **22** | Servo 1 | Output (PWM) | Dedicated servo header |
| **23** | Servo 2 | Output (PWM) | Dedicated servo header |
| **25** | Speaker (`NS4150B`) | Analog Out (`DAC1`) | Supports `dacWrite()` and I2S-to-DAC audio |
| **26** | Header `D3` | Digital I/O / `DAC2` | General-purpose breakout |
| **27** | Touch Pad 5 *(TBD)* | Capacitive Touch (`T7`) | Likely Pad 5 — needs verification |
| **32** | Header `A2` | Analog In (`ADC1_CH4`) | General-purpose breakout |
| **33** | Header `A1` | Analog In (`ADC1_CH5`) | General-purpose breakout |
| **34** | IR Sensor L *(TBD)* | Input Only (`ADC1_CH6`) | Suspected IR phototransistor |
| **35** | IR Sensor R *(TBD)* | Input Only (`ADC1_CH7`) | Suspected IR phototransistor |
| **36** | Buttons `L` & `R` | Input Only (`ADC1_CH0`) | Resistor ladder: `L` ≈ 1.3 V, `R` ≈ 3.3 V |
| **39** | Header `A3` | Input Only (`ADC1_CH3`) | General-purpose breakout (no internal pull-up/down) |

---

## Hardware Notes

* **Buttons (`GPIO 36`):** Idle `~0 V` (ADC ≈ `0`), Button L `~1.3 V` (ADC ≈ `1500–1650`), Button R `~3.3 V` (ADC ≈ `4095`).
* **7×5 LED Matrix (`GPIO 21`):** 35 LEDs, progressive row-major from top-left (`0` to `34`). Keep brightness capped (e.g., `30–60 / 255`) on battery to avoid brownouts.
* **DC Motors (`DRV8833`):** Motor L on Channel B (`GPIO 4`, `16`), Motor R on Channel A (`GPIO 2`, `17`).
* **Audio (`GPIO 25`):** Wired to ESP32 `DAC1`, supporting both `tone()` and raw 8-bit DAC output.

---



## Quick Start

```cpp
#include <Arduino.h>
#include "Quarky.h"

Quarky bot;

void setup() {
  bot.begin(40); // LED brightness (0–255)
  bot.beep(1000, 100);
  bot.setPixel(3, 2, 0, 255, 0); // Center pixel green
  bot.showMatrix();
}

void loop() {
  if (bot.isButtonPressed(BTN_L)) {
    bot.drive(180, 180);
  } else if (bot.isButtonPressed(BTN_R)) {
    bot.drive(-180, -180);
  } else {
    bot.stopMotors();
  }
  delay(20);
}
```

---

## API Reference

### Initialization
| Method | Description |
| :--- | :--- |
| `begin(uint8_t ledBrightness = 40)` | Initializes PWM, LED matrix, ADC, and I/O pins. |

### DC Motors (`DRV8833`)
Speed ranges from `-255` (full reverse) to `+255` (full forward).

| Method | Description |
| :--- | :--- |
| `setMotorL(int16_t speed)` | Sets Left motor (Channel B) speed and direction. |
| `setMotorR(int16_t speed)` | Sets Right motor (Channel A) speed and direction. |
| `drive(int16_t left, int16_t right)` | Sets both motors simultaneously. |
| `stopMotors(bool brake = false)` | Coasts (`false`) or actively brakes (`true`) both motors. |

### Servos
| Method | Description |
| :--- | :--- |
| `setServo(uint8_t index, uint8_t angle)` | Moves Servo `1` or `2` to `0–180°` (50 Hz hardware PWM). |

### 7×5 RGB LED Matrix
Origin `(0, 0)` is top-left; `x` is `0–6`, `y` is `0–4`. Call `showMatrix()` to push updates.

| Method | Description |
| :--- | :--- |
| `setBrightness(uint8_t brightness)` | Sets global brightness (`0–255`). |
| `setPixel(x, y, r, g, b)` | Sets pixel at `(x, y)` to RGB color. |
| `setPixel(index, r, g, b)` | Sets pixel by linear index (`0–34`). |
| `fillMatrix(r, g, b)` | Fills all 35 LEDs with a single color. |
| `clearMatrix()` | Clears the pixel buffer. |
| `showMatrix()` | Writes the buffer to the LEDs. |
| `leds()` | Returns the underlying `Adafruit_NeoPixel&` object. |

### Buttons & Touch Pads
| Method | Description |
| :--- | :--- |
| `readButton()` | Returns `BTN_NONE`, `BTN_L`, or `BTN_R`. |
| `isButtonPressed(QuarkyButton btn)` | Returns `true` if the specified button is pressed. |
| `readButtonRaw()` | Returns raw 12-bit ADC value (`0–4095`) on `GPIO 36`. |
| `readTouch(uint8_t pad)` | Returns raw capacitive touch value for pad `1–5`. |
| `isTouched(uint8_t pad, uint16_t threshold = 40)` | Returns `true` when touch reading drops below `threshold`. |

### Audio (`NS4150B` on DAC1)
| Method | Description |
| :--- | :--- |
| `beep(uint16_t freqHz, uint16_t durationMs = 150)` | Plays a tone on `GPIO 25`. |
| `stopTone()` | Stops active tone output. |
| `writeDAC(uint8_t value)` | Writes an 8-bit analog sample (`0–255`) directly to DAC1. |

### IR Sensors
| Method | Description |
| :--- | :--- |
| `setIREmitters(bool enabled)` | Toggles the IR emitter enable pin (`QUARKY_IR_EN_PIN`). |
| `readIRLeft()` / `readIRRight()` | Returns 12-bit ADC reading (`0–4095`) from the Left/Right IR sensor. |

---

## Overriding Pins
Unconfirmed and breakout pins in `QuarkyPins.h` use `#ifndef` guards. Override them via `build_flags` in `platformio.ini` without editing the library:

```ini
build_flags =
    -D QUARKY_TOUCH_5=27
    -D QUARKY_IR_L_PIN=34
    -D QUARKY_IR_R_PIN=35
    -D QUARKY_IR_EN_PIN=5
```

---

## License
MIT License — see `LICENSE` for details.
