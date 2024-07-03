#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "MySSID";
const char* password = "MyWifipassword";

const char* serverName = "http://server_ip:port/sensors";

// JWT token for authorization
const char* jwtToken = "my_jwt_token";

void setup() {
  Serial.begin(9600); // Serial connection to Arduino
  Serial1.begin(115200); // Serial connection for debugging

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }

  Serial1.println("");
  Serial1.println("WiFi connected");
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED && Serial.available() > 0) {
     HTTPClient http;
    
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(jwtToken));  // Add the JWT token to the header
    
    // Reading sensors data from Arduino
    String sensorData = Serial.readStringUntil('\n');
    sensorData.trim();

    float temperature = 0.0;
    float humidity = 0.0;
    int flameValue = 0;

    int commaIndex1 = sensorData.indexOf(',');
    int commaIndex2 = sensorData.lastIndexOf(',');

    if (commaIndex1 > 0 && commaIndex2 > commaIndex1) {
      temperature = sensorData.substring(0, commaIndex1).toFloat();
      humidity = sensorData.substring(commaIndex1 + 1, commaIndex2).toFloat();
      flameValue = sensorData.substring(commaIndex2 + 1).toInt();
    }

    String jsonData = "{\"temperature\":" + String(temperature, 1) + ",";
    jsonData += "\"humidity\":" + String(humidity, 1) + ",";
    jsonData += "\"flameValue\":" + String(flameValue) + "}";

    // Sending HTTP POST request
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }

  delay(5000); // Delay before sending the next request
}
