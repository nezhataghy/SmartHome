#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11

struct SensorData {
  float temperature;
  float humidity;
};

DHT dht(DHTPIN, DHTTYPE);

SensorData data;
int LDR = A0;
const int FAN = 3;
const int LED = 11;
LiquidCrystal_I2C lcd(0x27, 16, 2); // the LCD address and dimensions

bool LedON = false;


unsigned long previousMillis = 0;  // Stores the last time the function was executed
const long interval = 2000;        // Interval in milliseconds (2 seconds)
const int numReadings = 20;        // Number of readings to average

// Debouncing parameters
const int debounceCount = 3;       // Number of stable readings needed to confirm a change
int stableReadingCount = 0;

SensorData sendSensor() {
  SensorData result;
  
  result.temperature = dht.readTemperature();
  result.humidity = dht.readHumidity();

  return result;
}

int readLDR() {
  long sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(LDR);
    delay(10); // Small delay between readings
  }
  return sum / numReadings;
}

void setup() {
  Serial.begin(9600);
  pinMode(LDR, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(LED, OUTPUT);
  dht.begin();
  lcd.init();                      // initializing the LCD
  lcd.backlight();                
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("By Nezha");
  delay(5000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time

  // Checking if it's time to execute the function
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    data = sendSensor(); // Update sensor data
  }

  // Controling the fan based on temperature
  if (data.temperature > 30) {
    digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW); 
  }

  // Displaying sensor data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(data.temperature, 1); // Display temperature with 1 decimal place
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(data.humidity, 1); // Display humidity with 1 decimal place
  lcd.print("%");

  // Reading LDR value and control LED based on lighting condition
  int LDRValue = readLDR();
  Serial.print("LDR sensor value = ");
  Serial.println(LDRValue);

    if ((LDRValue < 100) && (LedON == false)) { // Adjust threshold if needed
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(LED, HIGH);
      LedON = true;
      Serial.println("It's Dark Outside; Light ON");
      stableReadingCount = 0; // Reset counter
    }
  } else if ((LDRValue >= 100) && (LedON == true)) { // Adjust threshold if needed
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(LED, LOW);
      LedON = false;
      Serial.println("It's Bright Outside; Light OFF");
      stableReadingCount = 0; // Reset counter
    }
  } else {
    stableReadingCount = 0; // Reset counter if reading is inconsistent
  }

  delay(2000);
}
