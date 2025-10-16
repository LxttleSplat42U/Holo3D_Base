#pragma once
#include <WiFiConfig.h>
#include <map>                              // Used to map all websockets to their clients efficiently

extern const int motorPWM;                  // Motor Control PIN
extern bool Client_Connected;               // Check if user is connected
std::map<int, uint32_t> deviceIdToClientId; // Maps device ID to WebSocket client ID
AsyncWebSocket ws("/ws");                   // Create a WebSocket object

void initWebSocket();
void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);  //WebSocket Event
void sendToClient(int clientID, String msg);
void sendToAllMotorClients(String msg);
void setFanSpeed(uint8_t speed);
void printConnectedDevices();