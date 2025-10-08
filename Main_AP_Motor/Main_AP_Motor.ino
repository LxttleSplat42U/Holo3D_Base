#include <Arduino.h>

#include <WiFi.h>  //Used to enable WiFi functionality
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include "driver/pulse_cnt.h"  //For PCNT
#include "esp_err.h"           //  ensures ESP_ERROR_CHECK is defined otherwise an error occurs for some reason
#include <map>                 //Used to map Client efficiently

// Map device ID to WebSocket client ID
std::map<int, uint32_t> deviceIdToClientId;

const int FAN_ID = 1;  //Set fan ID

//Pin definitions
uint8_t SpeedSensorPin = 1;

//Speed RPM [Position Sensor]
#define SPEED_SENSOR_PIN 1       // Speed Sensor pin number
#define SAMPLE_INTERVAL_US 1000  // 1 ms = 1000 Hz sampling
const int LOW_TH = 3100;         // ADC level if no blade passed
const int HIGH_TH = 3120;        // ADC level if blade passed

volatile bool bladePassed = false;
volatile int nBladePasses = 0;  //Count number of times the blade has passed

hw_timer_t *TIM0;      //Setup hardware timer for accurate and precise timing
int currFanSpeed = 0;  //rpm of the fan currently


//Motor Control
const int motorPWM = 8;

//PWM Settings
const int freq = 8000;     //Frequency [Hz]
const int resolution = 8;  //bits

//User command variables
uint8_t msgSpeed = 0;  //Fan Speed 0 - 100


const int Monitor_LED = D13;
bool Monitor_LED_State;
int tblink = 0;
unsigned long tReport = 0;
bool Client_Connected = false;

//WiFi Name and Password
const char *ssid = "Holo3D";
AsyncWebServer server(80);           //Create a server on port 80
IPAddress staticIP(192, 168, 4, 1);  // IP address of the ESP32
IPAddress gateway(192, 168, 4, 1);   // Same as local_IP for AP
IPAddress subnet(255, 255, 255, 0);  // Subnet mask

//Setup WebSocket
AsyncWebSocket ws("/ws");  //Create a WebSocket object

// put function declarations here:
void SetupInterrupts();

void initWiFiAP();                                                                                                               //Initialize WiFi
void initWebSocket();                                                                                                            //Initialize WebSocket
void LED_Blink();                                                                                                                //Used to check if board is reponsive/active by monitoriung the built in D13 LED flashing
void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);  //WebSocket Event
void setFanSpeed(uint8_t speed);
void handleBladeInterrupt();
void handleTIM0();
void sendToClient(int clientID, String msg);
void printConnectedDevices();
void sendToAllMotorClients(String msg);

void SetupInterrupts() {

  //Timer
  TIM0 = timerBegin(25600);                //3125 Hz timer (Timer_number, prescaler, upcount?)
  timerAttachInterrupt(TIM0, handleTIM0);  //Timer, function to call
  timerAlarm(TIM0, 3125, true, 0);         //Timer, ARR, ARR_Enabled?, number of times? (0 = infinite)
}


void setup() {
  // put your setup code here, to run once:
  //PWM & Motor Control Setup
  pinMode(motorPWM, OUTPUT);  //Ensure Motor is OFF
  digitalWrite(motorPWM, LOW);
  ledcAttach(motorPWM, freq, resolution);

  //Enable Timer and Interrupts
  SetupInterrupts();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<!DOCTYPE html><html><body><h2>ESP32 WebSocket</h2><script>const socket = new WebSocket('ws://' + location.hostname + '/ws'); socket.onopen = () => { console.log('Connected'); socket.send('Hi from browser'); };</script></body></html>");
  });

  Serial.begin(115200);
  initWiFiAP();
  initWebSocket();

  //Set initial Built-in LED state
  pinMode(Monitor_LED, OUTPUT);
  digitalWrite(Monitor_LED, LOW);
  Monitor_LED_State = false;

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - tReport > 10000) {
    tReport = millis();
    printConnectedDevices();
  }
}

// put function definitions here:
void initWiFiAP() {
  //Create WiFi AP
  WiFi.softAPConfig(staticIP, gateway, subnet);  //Set static IP, gateway and subnet mask
  WiFi.softAP(ssid);                             //add password parameter if needed
  IPAddress IP = WiFi.softAPIP();                //Get the IP address of the AP
}

void initWebSocket() {
  ws.onEvent(onWebEvent);  //Attach the WebSocket event handler
  server.addHandler(&ws);  //Add the WebSocket handler to the server
}

void LED_Blink() {
  Monitor_LED_State = !Monitor_LED_State;
  digitalWrite(Monitor_LED, Monitor_LED_State ? HIGH : LOW);
}

void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  //Handle WebSocket events here
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected, waiting for registration...\n", client->id());
    Client_Connected = true;

  } else if (type == WS_EVT_DISCONNECT) {
    //User Disconnected
    setFanSpeed(0);    //Switch off fan immediately if user disconnected!
    ws.textAll("-1");  //Send shutdown command to all clients
    Client_Connected = false;

    Serial.printf("WebSocket client #%u disconnected\n", client->id());

    // Remove from device map
    for (auto it = deviceIdToClientId.begin(); it != deviceIdToClientId.end(); ++it) {
      if (it->second == client->id()) {
        Serial.printf("Device ID %d disconnected\n", it->first);
        deviceIdToClientId.erase(it);
        break;
      }
    }



  } else if (type == WS_EVT_DATA) {
    Serial.printf("WebSocket data received: %.*s\n", len, data);
    // Process incoming data
    String msg = "";
    for (size_t i = 0; i < len; i++) {
      msg += (char)data[i];
    }

    if (msg.startsWith("MOTOR_SPEED:")) {  //Set motor speed
      String msgValue = msg.substring(strlen("MOTOR_SPEED:"));
      msgSpeed = msgValue.toInt();  //0 - 255
      setFanSpeed(msgSpeed);

      //Forward to all motor ESPs (ID ends with 0, Ex. second fan will be ID=20)
      sendToAllMotorClients(msg);

    } else if (msg.startsWith("REGISTER:")) {  // Handle device registration
      int deviceId = msg.substring(9).toInt();
      deviceIdToClientId[deviceId] = client->id();
      Serial.printf("Device ID %d registered with WebSocket client #%u\n", deviceId, client->id());

      // Send confirmation back to this specific client
      client->text("REGISTERED:" + String(deviceId));
      return;
    } else {
      // Split message ID:ACTION:VALUE
      int firstColon = msg.indexOf(':');
      int secondColon = msg.indexOf(':', firstColon + 1);

      //Check if ID listed
      if (firstColon > 0) {
        int targetClientID = msg.substring(0, firstColon).toInt();

        if (targetClientID > 0) {
          // This is a targeted message - send only to specific device
          Serial.printf("Forwarding message to device ID %d: %s\n", targetClientID, msg.c_str());
          sendToClient(targetClientID, msg);
          return;
        }
      }
    }

  }
}

void setFanSpeed(uint8_t speed) {
  ledcWrite(motorPWM, speed);  //Duty Cycle (0-255)
}

//Function to calculate the current fan speed at consistent/known intervals
void handleTIM0() {

  int sensorValue = analogRead(SPEED_SENSOR_PIN);

  if (!bladePassed && sensorValue >= HIGH_TH) {
    bladePassed = true;
    nBladePasses++;
  } else if (bladePassed && sensorValue <= LOW_TH) {
    bladePassed = false;
  }

  if (!Client_Connected) {  //Check to see if timer initialised correctly
    LED_Blink();
  }

  currFanSpeed = nBladePasses;  //get the fan speed (divide by time 1s)
  //nBladePasses = 0; //Reset counter
}

void sendToClient(int clientID, String msg) {

  if (deviceIdToClientId.find(clientID) != deviceIdToClientId.end()) {
    uint32_t wsClientId = deviceIdToClientId[clientID];
    ws.text(wsClientId, msg);
    Serial.printf("Sent to device ID %d (client #%u): %s\n", clientID, wsClientId, msg.c_str());
  } else {
    Serial.printf("Device ID %d not connected\n", clientID);
  }
}

// Print all connected devices (For Debugging)
void printConnectedDevices() {
  Serial.println("Connected devices:");
  for (auto const &pair : deviceIdToClientId) {
    Serial.printf("  Device ID: %d (WS Client: %u)\n", pair.first, pair.second);
  }
}

// Send message to all devices with ID ending in 0 (X0)
void sendToAllMotorClients(String msg) {
  int count = 0;
  for(auto const& pair : deviceIdToClientId) {
    if(pair.first % 10 == 0) {  // Check if ID ends in 0
      ws.text(pair.second, msg);
      Serial.printf("Sent to device ID %d (client #%u): %s\n", 
                    pair.first, pair.second, msg.c_str());
      count++;
    }
  }
  if(count == 0) {
    Serial.println("No motor clients (X0) connected");
  } else {
    Serial.printf("Sent to %d motor clients\n", count);
  }
}