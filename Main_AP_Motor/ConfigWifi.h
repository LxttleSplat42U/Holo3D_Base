#pragma once
#include <WiFi.h>  //Used to enable WiFi functionalityconst char *ssid
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

//WiFi Name and Password
extern const char *ssid;
AsyncWebServer server(80);           //Create a server on port 80
IPAddress staticIP(192, 168, 4, 1);  // IP address of the ESP32
IPAddress gateway(192, 168, 4, 1);   // Same as local_IP for AP
IPAddress subnet(255, 255, 255, 0);  // Subnet mask

void initWiFiAP();