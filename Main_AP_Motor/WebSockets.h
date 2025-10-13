#pragma once
#include <WiFi.h>  //Used to enable WiFi functionality
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <map>                 //Used to map Client efficiently

//Motor Control PIN
extern const int motorPWM;

// bool if user not connected
bool Client_Connected = false;
// Map device ID to WebSocket client ID
std::map<int, uint32_t> deviceIdToClientId;

AsyncWebSocket ws("/ws");  //Create a WebSocket object
void initWebSocket();
void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);  //WebSocket Event
void sendToClient(int clientID, String msg);
void sendToAllMotorClients(String msg);
void setFanSpeed(uint8_t speed);
void printConnectedDevices();