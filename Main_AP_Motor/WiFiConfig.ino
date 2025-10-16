#include <WiFiConfig.h>

void initWiFiAP() {
  //Create WiFi AP
  WiFi.softAPConfig(staticIP, gateway, subnet);  // Set static IP, gateway and subnet mask
  WiFi.softAP(ssid, password);                   // Create netword with ssid and password 
  IPAddress IP = WiFi.softAPIP();                // Get the IP address of the AP
}