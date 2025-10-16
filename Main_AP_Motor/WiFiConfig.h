#pragma once
#include <WiFi.h>  
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// WiFi Name (ssid) and Password
extern const char *ssid;
extern const char *password;

// WebSocket IP and port
IPAddress staticIP(192, 168, 4, 1);  // IP address of the ESP32
IPAddress gateway(192, 168, 4, 1);   // Same as local_IP for AP
IPAddress subnet(255, 255, 255, 0);  // Subnet mask
AsyncWebServer server(80);           // Create a server on port 80

void initWiFiAP();