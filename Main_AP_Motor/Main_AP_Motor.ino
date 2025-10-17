#include <HardwareSoftwareConfig.h>
#include <RPM_Measurement.h>
#include <PWMConfig.h>
#include <WiFiConfig.h>
#include <WebSockets.h>

// Built in LED mainly used for debuging purposes
extern const int Monitor_LED;

// Timer to report/relay information to connected clients
extern const int tReportInterval;
unsigned long tReportConnectedClients = 0;  // Keeps track of last report timestamp

void setup() {
  // Enable serial for debugging
  Serial.begin(115200); 

  //PWM & Motor Control Setup
  setupPWM();
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<!DOCTYPE html><html><body><h2>ESP32 WebSocket</h2><script>const socket = new WebSocket('ws://' + location.hostname + '/ws'); socket.onopen = () => { console.log('Connected'); socket.send('Hi from browser'); };</script></body></html>");
  });
  // Setup WiFi AP and WebSocket server based on configuration
  initWiFiAP();
  initWebSocket();

  //Set initial Built
  pinMode(Monitor_LED, OUTPUT);
  digitalWrite(Monitor_LED, LOW);
  Serial.println("Test:");
  server.begin(); // Begin websocket servers

  // Setup RPM Measurement interrupts and timers
  setupRPMMeasurement();
  setupRPMTimer();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - tReportConnectedClients > tReportInterval) {
    tReportConnectedClients = millis();
    if (avgRPMCount >= 1){
      avgRPM = sumRPM / avgRPMCount;
      avgRPMCount = 0;
      sumRPM = 0;
    } else {
      avgRPM = 0;
    }
    
    //printConnectedDevices();
    // sendToClient(11, "Analog: " + String(analogRead(RPM_SENSOR)) + "RPM: " + String(nbladePasses));
    sendToClient(11, "RPM: " + String(avgRPM) + ":");
    sendToClient(userID, "11:RPM: " + String(avgRPM) + ":");
    // sendToClient(11, "min: " + String(rpm_min) + "max: " + String(rpm_max));
  }

  if (shutDown){  // If system is in shutdown mode, repeat sending shutdown commands to prevent system from restarting
  if (millis() - tshutdownTimer > tshutDownRepeatInterval) {
    tshutdownTimer = millis();
    shutDownSystem(); 
  }
  }

}



