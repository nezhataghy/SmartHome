#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char ssid[] = "MySSID";
const char password[] = "MyWifipassword";

const char serverName[] = "52.87.216.189";
const int serverPort = 5000;
const char websocket_server_host[] = "52.87.216.189";
const int websocket_server_port = 8080;

// JWT token for authorization
const char jwtToken[] = "user_jwt_token";

WiFiClient client;
WebSocketsClient webSocket;


void setup() {
  Serial.begin(9600); // Serial connection to Arduino
  Serial1.begin(115200); // Serial connection for debugging

  connectToWiFi();
  connectToWebSocket();

}


void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  // Checking for incoming serial data from Arduino
  if (WiFi.status() == WL_CONNECTED && Serial.available() > 0) {
    String incomingData = Serial.readStringUntil('\n');
    incomingData.trim();
    if (incomingData.startsWith("Update")) {
      sendToDB(incomingData);
    } else {
      sendSensorData(incomingData);
    }
  }

  webSocket.loop();

  // Sending a ping message every 30 seconds
  if (millis() - lastPingTime > 30000) {
    webSocket.sendTXT("ping");
    lastPingTime = millis();
  }
}


void connectToWiFi() {
  Serial1.print("Connecting to ");
  Serial1.println(ssid);

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


void connectToWebSocket() {
  webSocket.begin(websocket_server_host, websocket_server_port, "/");
  webSocket.onEvent(webSocketEvent);
  Serial1.println("Attempting WebSocket connection...");
}


void sendSensorData(String sensorData) {
  HTTPClient http;

  Serial1.println("Connecting to server...");
  Serial1.printf("HTTP POST to: http://%s:%d/sensors\n", serverName, serverPort);

  http.begin(client, String("http://") + serverName + ":" + String(serverPort) + "/sensors");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(jwtToken)); 

  Serial1.print("Sensor Data: ");
  Serial1.println(sensorData);

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

  if ((temperature != 0.0) && (humidity != 0.0) && (humidity != 0.0)) {
  char jsonData[100];
  snprintf(jsonData, sizeof(jsonData), "{\"temperature\":%.1f,\"humidity\":%.1f,\"flameValue\":%d}", temperature, humidity, flameValue);

  Serial1.print("JSON Data: ");
  Serial1.println(jsonData);

  // Sending HTTP POST request
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial1.println(httpResponseCode);
    Serial1.println(response);
  } else {
    Serial1.print("Error on HTTP POST request: ");
    Serial1.println(httpResponseCode);
    Serial1.println("connection failed");
    }
  }

  http.end();
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial1.println("Disconnected from WebSocket server");
      // Attempt to reconnect after a delay
      if (millis() - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = millis();
        connectToWebSocket();
      }
      break;
    case WStype_CONNECTED:
      Serial1.println("Connected to WebSocket server");
      break;
    case WStype_TEXT:
      handleWebSocketMessage(payload, length);
      break;
    case WStype_BIN:
      Serial.println("Binary message received");
      break;
  }
}
    

void handleWebSocketMessage(uint8_t * payload, size_t length) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload, length);

  // Extracting actuator data from JSON
  const char* name = doc["name"];
  const char* status = doc["status"];

  // Sending data to Arduino via Serial
  sendToArduino(name, status);
}


void sendToArduino(const char* name, const char* status) {
  Serial.print(name);
  Serial.print(":");
  Serial.println(status);
}


void sendToDB(String message) {
  String actuator = extractActuator(message);
  String status = extractStatus(message);

  Serial1.print("Actuator: ");
  Serial1.println(actuator);
  Serial1.print("Status: ");
  Serial1.println(status);

  if (actuator != "" && status != "") {
    HTTPClient http;

    String url = String("http://") + serverName + ":" + String(serverPort) + "/actuators/" + actuator;
    Serial1.print("URL: ");
    Serial1.println(url); 
    
    http.begin(client, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(jwtToken)); 

    String payload = "{\"status\": \"" + status + "\"}";
    Serial1.print("Payload: ");
    Serial1.println(payload);

    int httpResponseCode = http.PUT(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial1.println("HTTP Response code: " + String(httpResponseCode));
      Serial1.println("Response: " + response);
    } else {
      Serial1.println("Error on sending PUT request");
    }
    http.end();
  }
}


String extractActuator(String message) {
  int startIndex = message.indexOf(',') + 1;
  int endIndex = message.indexOf(':', startIndex);
  if (startIndex > 0 && endIndex > startIndex) {
    return message.substring(startIndex, endIndex);
  }
  return "";
}


String extractStatus(String message) {
  int startIndex = message.indexOf(':') + 1;
  if (startIndex > 0) {
    return message.substring(startIndex);
  }
  return "";
}
