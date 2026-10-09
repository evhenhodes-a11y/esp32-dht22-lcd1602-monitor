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

void setup() {
  Serial.begin(115200);
  Serial.println(F("ESP32 + DHT22 + LCD1602 (I2C) Initialization..."));

  // Initialize I2C bus and LCD
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeChar);

  // Splash screen
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Temp/Humid");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Initialize DHT sensor
  dht.begin();
  delay(2000); // Allow DHT22 to stabilize
  lcd.clear();
}

void loop() {
  // Read humidity and temperature (Celsius)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    lcd.setCursor(0, 1);
    lcd.print("Check DHT22 Wire");
    delay(2000);
    return;
  }

  // Print readings to Serial Monitor
  Serial.print(F("Humidity: "));
  Serial.print(h, 1);
  Serial.print(F("%  Temperature: "));
  Serial.print(t, 1);
  Serial.println(F("°C"));

  // Display Temperature on Line 1
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t, 1);
  lcd.write(0); // Custom degree symbol
  lcd.print("C   ");

  // Display Humidity on Line 2
  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(h, 1);
  lcd.print("%   ");

  // DHT22 sampling interval (minimum 2 seconds)
  delay(2000);
}
