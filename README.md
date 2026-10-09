# ESP32 + DHT22 + LCD 1602 (I2C) Weather Station

This is a PlatformIO project for the **ESP32** microcontroller that reads temperature and humidity data from a **DHT22** sensor and displays the readings on an **LCD 1602** screen equipped with an **I2C interface module (PCF8574)**.

---

## 🔌 Circuit Wiring Diagram

| Component | Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **DHT22** | VCC | `5V` or `3.3V` | Use `3.3V` or `5V` depending on sensor module version |
| **DHT22** | DATA | `GPIO 4` | Add a 10kΩ pull-up resistor between VCC and DATA if using raw 4-pin DHT22 sensor |
| **DHT22** | GND | `GND` | Common Ground |
| **LCD 1602 (I2C)** | VCC | `5V` | I2C LCD modules usually require 5V for bright display/backlight |
| **LCD 1602 (I2C)** | GND | `GND` | Common Ground |
| **LCD 1602 (I2C)** | SDA | `GPIO 21` | Hardware I2C Data Pin |
| **LCD 1602 (I2C)** | SCL | `GPIO 22` | Hardware I2C Clock Pin |

---

## 📂 Project Structure

```text
.
├── platformio.ini         # PlatformIO environment and dependency configuration
├── src/
│   └── main.cpp           # Main application source code
└── README.md              # Project documentation and setup guide
```

---

## 🛠️ Required Libraries (Auto-installed by PlatformIO)

- `adafruit/DHT sensor library`
- `adafruit/Adafruit Unified Sensor`
- `marcoschwartz/LiquidCrystal_I2C`

---

## 🚀 How to Build & Flash

1. Open this directory in **VS Code** with the **PlatformIO** extension installed.
2. Connect your ESP32 board via USB.
3. Click the **PlatformIO Build** (`✓`) button or run:
   ```bash
   pio run
   ```
4. Click **PlatformIO Upload** (`→`) to flash the firmware:
   ```bash
   pio run --target upload
   ```
5. Open the Serial Monitor (`115200` baud rate) to verify outputs:
   ```bash
   pio device monitor
   ```

---

## 🔍 Troubleshooting & Notes

- **I2C Address**: The code defaults to LCD address `0x27`. If your LCD output is blank, run an I2C scanner sketch to verify if your module uses `0x3F` or `0x27`, then update `LiquidCrystal_I2C lcd(0x27, 16, 2);` in `src/main.cpp`.
- **Contrast Adjustment**: Turn the small blue potentiometer on the back of the I2C LCD backpack if text is not visible.
