# ESP32 + DHT22 + LCD 1602 (FreeRTOS) Weather Station

This is a PlatformIO project for the ESP32 microcontroller using native FreeRTOS tasks and mutexes instead of blocking delay() calls. It reads temperature and humidity data from a DHT22 sensor and displays the readings on an LCD 1602 screen (PCF8574 I2C interface).

---

## FreeRTOS Architecture

- **SensorTask (Core 0)**: Reads the DHT22 sensor every 2,000 ms using non-blocking vTaskDelayUntil().
- **DisplayTask (Core 1)**: Updates the LCD 1602 screen every 1,000 ms independently.
- **Mutex (xDataMutex)**: Ensures thread-safe reading/writing of shared sensor data between tasks across dual cores.

---

## Circuit Wiring Diagram

| Component | Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **DHT22** | VCC | `3.3V` or `5V` | Use `3.3V` or `5V` depending on sensor module version |
| **DHT22** | DATA | `GPIO 4` | Add a 10kΩ pull-up resistor between VCC and DATA if using raw 4-pin DHT22 sensor |
| **DHT22** | GND | `GND` | Common Ground |
| **LCD 1602 (I2C)** | VCC | `5V` | I2C LCD modules usually require 5V for bright display/backlight |
| **LCD 1602 (I2C)** | GND | `GND` | Common Ground |
| **LCD 1602 (I2C)** | SDA | `GPIO 21` | Hardware I2C Data Pin |
| **LCD 1602 (I2C)** | SCL | `GPIO 22` | Hardware I2C Clock Pin |

---

## Project Structure

```text
.
├── platformio.ini         # PlatformIO environment and dependency configuration
├── src/
│   └── main.cpp           # Main FreeRTOS application source code
└── README.md              # Project documentation and setup guide
```

---

## Required Libraries (Auto-installed by PlatformIO)

- `adafruit/DHT sensor library`
- `adafruit/Adafruit Unified Sensor`
- `marcoschwartz/LiquidCrystal_I2C`

---

## How to Build & Flash

1. Open this directory in **VS Code** with the **PlatformIO** extension installed.
2. Connect your ESP32 board via USB.
3. Click **PlatformIO Build** (`✓`) or run:
   ```bash
   pio run
   ```
4. Click **PlatformIO Upload** (`→`) to flash the firmware:
   ```bash
   pio run --target upload
   ```
5. Open the Serial Monitor (`115200` baud rate):
   ```bash
   pio device monitor
   ```
