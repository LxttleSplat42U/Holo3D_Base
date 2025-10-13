#include <Arduino.h>
#include <RPM_Measurement.h>
#include <ConfigWifi.h>
#include <WebSockets.h>

#include "esp_err.h"  //  ensures ESP_ERROR_CHECK is defined otherwise an error occurs for some reason

const int RPM_LOW = 2500;
const int RPM_HIGH = 3000;


// Change based on hardware configuration
const int motorPWM = 8;       // PWM Motor Control PIN
const char *ssid = "Holo3D";  // Wifi AP Name
const int RPM_SENSOR = 1;
volatile int nbladePasses = 0;
volatile bool bladePassed = false;
volatile bool RPMMeasurementEnabled = false;
int rpm_max = -10;
int rpm_min = 8000;

//Speed RPM [Position Sensor]
hw_timer_t *TIM0;  //Setup hardware timer for accurate and precise timing



//PWM Settings
const int freq = 8000;     //Frequency [Hz]
const int resolution = 8;  //bits

const int Monitor_LED = D13;
bool Monitor_LED_State;
unsigned long tReportConnectedClients = 0;


// put function declarations here:                                                          //Initialize WebSocket
void toggleBuiltInLED();

void setup() {
  // put your setup code here, to run once:
  //PWM & Motor Control Setup
  pinMode(motorPWM, OUTPUT);  //Ensure Motor is OFF
  digitalWrite(motorPWM, LOW);
  ledcAttach(motorPWM, freq, resolution);

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

  // setupRPMMeasurement();
  pinMode(RPM_SENSOR, INPUT_PULLDOWN); // Set pin as input
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - tReportConnectedClients > 500) {
    tReportConnectedClients = millis();
    //printConnectedDevices();
    // sendToClient(11, "Analog: " + String(analogRead(RPM_SENSOR)) + "RPM: " + String(nbladePasses));
    // sendToClient(11, "RPM: " + String(nbladePasses));
    sendToClient(11, "min: " + String(rpm_min) + "max: " + String(rpm_max));
    rpm_max = -10;
    rpm_min = 8000;
  }

  // Disable interrupt until blade finsihed passing
  // if (bladePassed && RPMMeasurementEnabled){
  //   disableRPMMeasurement();
  // } else if ((!bladePassed) && (analogRead(RPM_SENSOR) <= 60) && !RPMMeasurementEnabled) {
  //   enableRPMMeasurement();
  // }

  // if (!bladePassed) {
  //   if (analogRead(RPM_SENSOR) >= RPM_HIGH) {
  //     bladePassed = true;
  //     nbladePasses++;
  //     toggleBuiltInLED();
  //   }
  // } else if (analogRead(RPM_SENSOR) <= RPM_LOW) {
  //   bladePassed = false;
  //   toggleBuiltInLED();
  // }

  int sensorR = analogRead(RPM_SENSOR);
  if (sensorR >= rpm_max)
  {
    rpm_max = sensorR;
  } else if (sensorR <= rpm_min)
  {
    rpm_min = sensorR;
  }
}

void toggleBuiltInLED() {
  Monitor_LED_State = !Monitor_LED_State;
  digitalWrite(Monitor_LED, Monitor_LED_State ? HIGH : LOW);
}
