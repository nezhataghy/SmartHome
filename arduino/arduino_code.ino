#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

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
const int PIR_PIN = 4;
const int OutdoorLED = 22;
Servo myServo;
const int servoPin = 13;


LiquidCrystal_I2C lcd(0x27, 16, 2); // the LCD address and dimensions

bool LedON = false;
int pirState = LOW;


unsigned long previousMillis = 0;  // Stores the last time the function was executed
const long interval = 2000;        // Interval in milliseconds (2 seconds)
const int numReadings = 20;        // Number of readings to average

// Debouncing parameters
const int debounceCount = 3;       // Number of stable readings needed to confirm a change
int stableReadingCount = 0;


// Keypad setup
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputPassword = "";
const String password = "1998";

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

void openDoor() {
  myServo.write(90);
  delay(5000);
  myServo.write(0);
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(PIR_PIN, INPUT);
  pinMode(FAN, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(OutdoorLED, OUTPUT);

  
  myServo.attach(servoPin);
  myServo.write(0); // Initial position

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

  // Reading PIR sensor
  pirState = digitalRead(PIR_PIN);
  // Controling the fan based on temperature and motion detection
  if (data.temperature > 30 && pirState == HIGH) {
    digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW); 
  }

  // Displaying sensor data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(data.temperature, 1); // Display temperature with 1 decimal place
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humi: ");
  lcd.print(data.humidity, 1); // Display humidity with 1 decimal place
  lcd.print(" %");

  // Reading LDR value and control LED based on lighting condition
  int LDRValue = readLDR();
  Serial.print("LDR sensor value = ");
  Serial.println(LDRValue);

    if ((LDRValue < 100) && (LedON == false)) { 
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(OutdoorLED, HIGH);
      LedON = true;
      Serial.println("It's Dark Outside; Light ON");
      stableReadingCount = 0; // Reseting counter
    }
  } else if ((LDRValue >= 100) && (LedON == true)) { 
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(OutdoorLED, LOW);
      LedON = false;
      Serial.println("It's Bright Outside; Light OFF");
      stableReadingCount = 0; // Reseting counter
    }
  } else {
    stableReadingCount = 0; // Reseting counter if reading is inconsistent
  }

  // Reading keypad input
  char key = keypad.getKey();
  if (key) {
    if (key == '#') { // Checking password
      if (inputPassword == password) {

        lcd.clear();
        Serial.println("Password correct");
        lcd.print("Access Granted");
        openDoor();
      } else {
        Serial.println("Password incorrect");
        lcd.clear();
        lcd.print("Access Denied");
        delay(1000);
      }
      inputPassword = ""; // Reseting input password
    } else if (key == '*') { 
      inputPassword = "";
      Serial.println("Input reset");
    } else { // Adding key to password
      inputPassword += key;
      Serial.print("Current input: ");
      Serial.println(inputPassword);
    }
  }

  delay(200);
}
