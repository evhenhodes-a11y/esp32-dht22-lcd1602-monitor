#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Pin definitions
#define DHTPIN 4          // GPIO pin connected to DHT22 DATA
#define DHTTYPE DHT22     // DHT sensor model

#define I2C_SDA 21        // ESP32 default SDA pin
#define I2C_SCL 22        // ESP32 default SCL pin

// LCD setup: address 0x27 (or 0x3F), 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT setup
DHT dht(DHTPIN, DHTTYPE);

// FreeRTOS task handles
TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xDisplayTaskHandle = NULL;

// FreeRTOS Mutex for thread-safe shared data access
SemaphoreHandle_t xDataMutex = NULL;

// Shared sensor data structure
struct SensorData {
  float temperature = 0.0f;
  float humidity = 0.0f;
  bool isValid = false;
} gSensorData;

// Custom degree symbol character
byte degreeChar[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// Task 1: Read DHT22 Sensor (runs every 2000ms)
void TaskReadSensor(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000);

  for (;;) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdTRUE) {
      if (isnan(h) || isnan(t)) {
        gSensorData.isValid = false;
        Serial.println(F("[SensorTask] Error reading DHT22!"));
      } else {
        gSensorData.temperature = t;
        gSensorData.humidity = h;
        gSensorData.isValid = true;
        Serial.printf("[SensorTask] Temp: %.1f°C | Hum: %.1f%%\n", t, h);
      }
      xSemaphoreGive(xDataMutex);
    }

    // Precise periodic delay (doesn't drift over time)
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// Task 2: Update LCD Display (runs every 1000ms)
void TaskUpdateDisplay(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);

  for (;;) {
    SensorData localData;

    if (xSemaphoreTake(xDataMutex, portMAX_DELAY) == pdTRUE) {
      localData = gSensorData;
      xSemaphoreGive(xDataMutex);
    }

    if (!localData.isValid) {
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error!   ");
      lcd.setCursor(0, 1);
      lcd.print("Check DHT22 Wire");
    } else {
      // Line 1: Temperature
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(localData.temperature, 1);
      lcd.write(0); // Custom degree symbol
      lcd.print("C   ");

      // Line 2: Humidity
      lcd.setCursor(0, 1);
      lcd.print("Hum:  ");
      lcd.print(localData.humidity, 1);
      lcd.print("%   ");
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("ESP32 + DHT22 + LCD1602 (FreeRTOS) Initializing..."));

  // Create Mutex for shared sensor data protection
  xDataMutex = xSemaphoreCreateMutex();

  // Initialize I2C bus and LCD
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeChar);

  // Splash screen
  lcd.setCursor(0, 0);
  lcd.print("ESP32 FreeRTOS");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // Initialize DHT sensor
  dht.begin();
  vTaskDelay(pdMS_TO_TICKS(2000)); // Non-blocking RTOS startup delay
  lcd.clear();

  // Create FreeRTOS Tasks pinned to ESP32 dual cores
  // TaskReadSensor pinned to Core 0
  xTaskCreatePinnedToCore(
    TaskReadSensor,
    "SensorTask",
    3072,
    NULL,
    1,
    &xSensorTaskHandle,
    0
  );

  // TaskUpdateDisplay pinned to Core 1
  xTaskCreatePinnedToCore(
    TaskUpdateDisplay,
    "DisplayTask",
    3072,
    NULL,
    1,
    &xDisplayTaskHandle,
    1
  );
}

void loop() {
  // FreeRTOS handles task execution. Empty loop deletes the idle main loop task.
  vTaskDelete(NULL);
}
