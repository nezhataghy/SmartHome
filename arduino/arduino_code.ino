#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

struct SensorData {
  float temperature;
  float humidity;
  int flameValue;
};

SensorData data;

int LDR = A0; // Analog output pin of the LDR sensor
const int FLAME_SENSOR_AO = A1; // Analog output pin of the flame sensor
const int FAN = 3;
const int PIR_PIN = 4;
const int ALARM = 5;
const int TRIG_PIN = 6; 
const int ECHO_PIN = 7; 
Servo myServo;
const int Door = 13;
const int OutdoorLED = 22;
const int ULTRASONIC_LED = 23; // LED controlled by ultrasonic sensor

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
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {40, 41, 42, 43};
byte colPins[COLS] = {44, 45, 46, 47};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputPassword = "";
const String password = "9898";


void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
  dht.begin();

  pinMode(ECHO_PIN, INPUT);
  pinMode(FLAME_SENSOR_AO, INPUT);
  pinMode(LDR, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(ALARM, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(OutdoorLED, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_LED, OUTPUT);
  
  myServo.attach(Door);
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
  unsigned long currentMillis = millis();

  // Checking if it's time to execute the function
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    data = sendSensor();
    // Sending sensor data over Serial to ESP8266
    Serial.print(data.temperature);
    Serial.print(",");
    Serial.print(data.humidity);
    Serial.print(",");
    Serial.println(data.flameValue);
  }

  sendToESP("668887687566298c4fb550ec", "LOCKED");

  // Reading PIR sensor
  pirState = digitalRead(PIR_PIN);
  // Controling the fan based on temperature and motion detection
  if (data.temperature > 25 && pirState == HIGH) {
    digitalWrite(FAN, HIGH);
    sendToESP("6688872c7566298c4fb550dc", "ON");
  } else {
    digitalWrite(FAN, LOW); 
    sendToESP("6688872c7566298c4fb550dc", "OFF");
  }

    // Flame sensor logic
  if (data.flameValue < 500) { 
    digitalWrite(ALARM, HIGH); 
    Serial1.println("Flame detected! ALARM ON");
    sendToESP("668887397566298c4fb550e0", "ON");
    lcd.clear();
    lcd.print("Flame detected!");
    delay(500);
  } else {
    digitalWrite(ALARM, LOW);  
    sendToESP("668887397566298c4fb550e0", "OFF");
    Serial1.println("No flame detected.");
  }

    // Ultrasonic sensor logic
    long distance = readUltrasonicDistance();
    Serial1.print("Distance: ");
    Serial1.print(distance);
    Serial1.println(" cm");

    if (distance < 20) { 
      digitalWrite(ULTRASONIC_LED, HIGH);
      Serial1.println("Object detected! LED ON");
      sendToESP("668887567566298c4fb550e8", "ON");
    } else {
      digitalWrite(ULTRASONIC_LED, LOW);
      Serial1.println("No object detected. LED OFF");
      sendToESP("668887567566298c4fb550e8", "OFF");
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
  Serial1.print("LDR sensor value = ");
  Serial1.println(LDRValue);

    if ((LDRValue < 100) && (LedON == false)) { 
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(OutdoorLED, HIGH);
      sendToESP("668887487566298c4fb550e4", "ON");
      LedON = true;
      Serial1.println("It's Dark Outside; Light ON");
      stableReadingCount = 0; // Reseting counter
    }
  } else if ((LDRValue >= 100) && (LedON == true)) { 
    stableReadingCount++;
    if (stableReadingCount >= debounceCount) {
      digitalWrite(OutdoorLED, LOW);
      LedON = false;
      Serial1.println("It's Bright Outside; Light OFF");
      sendToESP("668887487566298c4fb550e4", "OFF");
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
        Serial1.println("Password correct");
        lcd.print("Access Granted");
        openDoor();
      } else {
        Serial1.println("Password incorrect");
        lcd.clear();
        lcd.print("Access Denied");
        delay(1000);
      }
      inputPassword = ""; // Reseting input password
    } else if (key == '*') { 
      inputPassword = "";
      Serial1.println("Input reset");
    } else { // Adding key to password
      inputPassword += key;
      Serial1.print("Current input: ");
      Serial1.println(inputPassword);
    }
  }

  delay(2000);
}


SensorData sendSensor() {
  SensorData result;
  
  result.temperature = dht.readTemperature();
  result.humidity = dht.readHumidity();
  result.flameValue = analogRead(FLAME_SENSOR_AO);

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
  sendToESP("668887687566298c4fb550ec", "LOCKED");
}


long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = (duration / 2) / 29.1;
  return distance;
}


void controlActuator(String command) {
  // Spliting command into ID and status
  int splitIndex = command.indexOf(':');
  if (splitIndex == -1) {
    Serial1.println("Invalid command format");
    return;
  }
  String id = command.substring(0, splitIndex);
  String status = command.substring(splitIndex + 1);

  status.toUpperCase();

  // Controling actuators based on ID and status
  if (id == "FAN") {
    digitalWrite(FAN, status == "ON" ? HIGH : LOW);
  } else if (id == "ALARM") {
    digitalWrite(ALARM, status == "ON" ? HIGH : LOW);
  } else if (id == "OutdoorLED") {
    digitalWrite(OutdoorLED, status == "ON" ? HIGH : LOW);
  } else if (id == "ULTRASONIC_LED") {
    digitalWrite(ULTRASONIC_LED, status == "ON" ? HIGH : LOW);
  } else if (id == "Door") {
    if (status == "UNLOCKED")
      openDoor();
  } else {
    Serial1.println("Unknown actuator ID");
  }
}


void sendToESP(const char* id, const char* status) {
  Serial.print("Update");
  Serial.print(",");
  Serial.print(id);
  Serial.print(":");
  Serial.println(status);
}
