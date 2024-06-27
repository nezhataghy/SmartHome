#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

int LDR = A0;
const int FAN = 3;
const int LED = 11;
LiquidCrystal_I2C lcd(0x27, 16, 2); // the LCD address and dimensions

bool isDark = false; // store sthe current lighting condition

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LDR, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(LED, OUTPUT);
  lcd.init();                      // initialize the LCD
  lcd.backlight();                
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("By Nezha");
  delay(5000);
  lcd.clear();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (temperature > 30) {
    digitalWrite(FAN, HIGH); 
  } else {
    digitalWrite(FAN, LOW); 
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);

  int LDRValue = analogRead(LDR);
  Serial.print("sensor = ");
  Serial.println(LDRValue);

  // Checking if the lighting condition has changed
  if (LDRValue < 150 && !isDark) {
    digitalWrite(LED, HIGH); 
    Serial.println(" It's Dark Outside; Light ON");
    isDark = true; 
  } else if (LDRValue >= 150 && isDark) {
    digitalWrite(LED, LOW); 
    Serial.println(" It's Bright Outside; Light OFF");
    isDark = false; 
  }

  delay(200); 
}
