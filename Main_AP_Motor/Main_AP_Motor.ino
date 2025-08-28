#include <Arduino.h>

#include <WiFi.h> //Used to enable WiFi functionality
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

//Motor Control
const int motorPWM = 8;
//PWM Settings
const int freq = 8000; //Frequency [Hz]
const int resolution = 8; //bits

//User command variables
uint8_t msgSpeed = 0; //Fan Speed 0 - 100


const int FAN_ID = 1; //Set fan ID
const int Monitor_LED = D13;
bool Monitor_LED_State;
int tblink = 0; 
bool Client_Connected = false;

//WiFi Name and Password
const char* ssid = "Holo3D"; 
AsyncWebServer server(80); //Create a server on port 80
IPAddress staticIP(192, 168, 4, 1); // IP address of the ESP32 
IPAddress gateway(192, 168, 4, 1);  // Same as local_IP for AP 
IPAddress subnet(255, 255, 255, 0); // Subnet mask

//Setup WebSocket
AsyncWebSocket ws("/ws"); //Create a WebSocket object

// put function declarations here:
void initWiFiAP(); //Initialize WiFi
void initWebSocket(); //Initialize WebSocket
void LED_Blink(); //Used to check if board is reponsive/active by monitoriung the built in D13 LED flashing
void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len); //WebSocket Event
void setFanSpeed(uint8_t speed);

void setup() {
  //PWM & Motor Control Setup
  ledcAttach(motorPWM, freq, resolution);

  // put your setup code here, to run once:
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
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

  if (!Client_Connected){
  LED_Blink();
  }
  
}

// put function definitions here:
void initWiFiAP(){
//Create WiFi AP
  WiFi.softAPConfig(staticIP, gateway, subnet); //Set static IP, gateway and subnet mask
  WiFi.softAP(ssid); //add password parameter if needed
  IPAddress IP = WiFi.softAPIP(); //Get the IP address of the AP
}

void initWebSocket(){
  ws.onEvent(onWebEvent); //Attach the WebSocket event handler
  server.addHandler(&ws); //Add the WebSocket handler to the server
}

void LED_Blink(){
if (millis() - tblink > 500) {
    tblink = millis();
    Monitor_LED_State = !Monitor_LED_State;
    digitalWrite(Monitor_LED, Monitor_LED_State ? HIGH : LOW);
}
}

void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  //Handle WebSocket events here
  if (type == WS_EVT_CONNECT) {
    Serial.println("WebSocket client connected");
    Client_Connected = true;
    ws.textAll("Connected");
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("WebSocket client disconnected");
    //User Disconnected
    setFanSpeed(0); //Switch off fan immediately if user disconnected!

    Client_Connected = false;
  } else if (type == WS_EVT_DATA) {
    Serial.printf("WebSocket data received: %.*s\n", len, data);
    // Process incoming data
    String msg = "";
    for (size_t i = 0; i < len; i++) {
      msg += (char) data[i];
    }

    if (msg == "LED_ON"){
      Serial.println("Turning ON");
      ws.textAll("Turning ON");
      digitalWrite(Monitor_LED, HIGH);
    } else if (msg == "LED_OFF"){
      Serial.println("Turning OFF");
      ws.textAll("Turning OFF");
      digitalWrite(Monitor_LED, LOW);
    } else if (msg.startsWith("MOTOR_SPEED:") ){
      String msgValue = msg.substring(strlen("MOTOR_SPEED:"));
      msgSpeed = msgValue.toInt(); //0 - 255
      setFanSpeed(msgSpeed);
    }
  }
}

void setFanSpeed(uint8_t speed){
  ledcWrite(motorPWM, speed); //Duty Cycle (0-255)
}