# SmartHome

This project is a SmartHome system that allows users to control and monitor various sensors and actuators to enhance home automation, and improve safety through a web application. The system consists of a backend server, a frontend application, and an Arduino setup for controlling actuators and collecting sensor data.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Backend Setup](#backend-setup)
- [Frontend Setup](#frontend-setup)
- [Arduino Setup](#arduino-setup)
- [Running the Project](#running-the-project)
- [Usage](#usage)
- [License](#license)

## Prerequisites
- Node.js (version 14 or higher)
- MongoDB
- Arduino IDE
- ESP8266 WiFi module

## Backend Setup
1. Clone the repository:
    ```sh
    git clone https://github.com/nezhataghy/SmartHome
    cd SmartHome
    ```
2. Install dependencies:
    ```sh
    npm install
    ```
3. Setup environment variables:
    Create a `.env` file in the root directory with the following content:
    ```env
    MONGODB_URI=<your_mongodb_uri>
    ```
4. Run the backend server:
    ```sh
    npm start
    ```
    The backend server should now be running on [http://localhost:5000](http://localhost:5000).

## Frontend Setup
1. Navigate to the frontend directory:
    ```sh
    cd frontend
    ```
2. Install dependencies:
    ```sh
    npm install
    ```
3. Update API URL:
    Ensure that the API requests in the frontend code point to the correct backend server URL (e.g., `http://52.87.216.189:5000`).

4. Run the frontend application:
    ```sh
    npm start
    ```
    The frontend application should now be running on [http://localhost:3000](http://localhost:3000).

## Arduino Setup

### Arduino Mega Setup
1. Install necessary libraries:
    In the Arduino IDE, install the following libraries:
    - DHT
    - Wire
    - LiquidCrystal_I2C
    - Keypad
    - Servo

2. Upload the Arduino Mega code:
    - Connect your Arduino Mega.
    - Open the `arduino_code.ino` file in the Arduino IDE.
    - Upload the code to your Arduino Mega.

### ESP8266 Setup
1. Install necessary libraries:
    In the Arduino IDE, install the following libraries:
    - ESP8266WiFi
    - ESP8266HTTPClient

2. Upload the ESP8266 code:
    - Connect your ESP8266 module.
    - Open the `ESP8266_code.ino` file in the Arduino IDE.
    - Update the WiFi credentials in the code:
      ```cpp
      const char* ssid = "your_SSID";
      const char* password = "your_PASSWORD";
      ```
    - Update the server host and port:
      ```cpp
      const char* http_server_host = "52.87.216.189";
      const int http_server_port = 5000;
      ```
    - Upload the code to your ESP8266 module.

## Running the Project
Ensure that the backend server, frontend application, and Arduino are all running. Open a web browser and navigate to [http://localhost:3000](http://localhost:3000) to access the frontend application. Use the login and registration forms to authenticate. Access the dashboard to monitor and control sensors and actuators.

## Usage
- **Authentication:** Users can register and log in using their username or email and password.
- **Dashboard:** After logging in, users can view sensor data and control actuators in real-time.
- **Arduino Integration:** The Arduino setup will send sensor data to the backend server and respond to actuator control commands from the server.
