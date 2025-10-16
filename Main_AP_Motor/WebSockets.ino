#include <WebSockets.h>

bool Client_Connected = false;

void initWebSocket() {
  ws.onEvent(onWebEvent);  // Attach the WebSocket event handler
  server.addHandler(&ws);  // Add the WebSocket handler to the server
}

void onWebEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  // Handle WebSocket events here
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected, waiting for registration...\n", client->id());
    Client_Connected = true;

  } else if (type == WS_EVT_DISCONNECT) {
    // User Disconnected
    setFanSpeed(0);           // Switch off fan immediately if user disconnected!
    ws.textAll("-1");         // Send shutdown command to all clients
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

    if (msg == "-1") // Shutdown command received
    {
     shutDownSystem(); // Shutdown all fan clients
     shutDown = true;
    } else if (msg == "RESTART"){ // Stop Shutdown routine in main loop
      shutDown = false;      
    } else if (msg.startsWith("MOTOR_SPEED:")) {  //Set motor speed
      String msgValue = msg.substring(strlen("MOTOR_SPEED:"));
      int msgSpeed = msgValue.toInt();  //0 - 255
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

void sendToClient(int clientID, String msg) {

  if (deviceIdToClientId.find(clientID) != deviceIdToClientId.end()) {
    uint32_t wsClientId = deviceIdToClientId[clientID];
    ws.text(wsClientId, msg);
    Serial.printf("Sent to device ID %d (client #%u): %s\n", clientID, wsClientId, msg.c_str());
  } else {
    Serial.printf("Device ID %d not connected\n", clientID);
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

void setFanSpeed(uint8_t speed) {
  ledcWrite(motorPWM, speed);  //Duty Cycle (0-255)
}

// Print all connected devices (For Debugging)
void printConnectedDevices() {
  Serial.println("Connected devices:");
  for (auto const &pair : deviceIdToClientId) {
    Serial.printf("  Device ID: %d (WS Client: %u)\n", pair.first, pair.second);
  }
}