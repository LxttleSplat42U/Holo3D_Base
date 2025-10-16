# Holo3D_Base
Main Fan Base ESP32 (C6)

# Steps on how to use
### Setting up the board
Arduino preferences board manager URL: https://espressif.github.io/arduino-esp32/package_esp32_index.json

### Tools Settings
  Enable CDC on boot (enables flashing without needing to press the physical boot button)
### Libraries used
AsyncTCP by ESP32Async 3.4.9

ESPAsyncWebServer by ESP32Async 3.8.1
# Video Showcase #1
https://github.com/user-attachments/assets/fd77e25b-0bb4-49c2-8a1c-dbb2ccc4506e

# Default configurations used (can be changed if needed, please see the final heading)
## Hardware
<img width="993" height="831" alt="image" src="https://github.com/user-attachments/assets/cc09d603-2ad0-4773-a3cb-e7febf1d5e96" />
## Software
Creates a open WiFi access point "Holo3D" with a WebSockets server accessible at 192.168.4.1 on port 80. (can be changed in "HarwareSoftwareConfig.ino" or "WiFiConfig.h" for more advanced functionality)

# If a different hardware configuration is being used
Edit the "HarwareSoftwareConfig.ino" file and change the pin assignement etc. to match the new hardware configuration.
Ex. If the speed/Hall-effect sensor is connected to a diferent pin than pin 1.

This file can also be used to change the WiFi access point name (ssid) and password if needed.

If more configuration is required, such s changing the WebSockets IP address and port/ URL. "WiFiConfig.h" can be edited to the new required values.




