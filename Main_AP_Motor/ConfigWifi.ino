#include <ConfigWifi.h>

void initWiFiAP() {
  //Create WiFi AP
  WiFi.softAPConfig(staticIP, gateway, subnet);  //Set static IP, gateway and subnet mask
  WiFi.softAP(ssid);                             //add password parameter if needed
  IPAddress IP = WiFi.softAPIP();                //Get the IP address of the AP
}