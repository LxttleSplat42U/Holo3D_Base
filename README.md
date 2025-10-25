# Holo3D_Base
Main Fan Base ESP32 (C6)

# Video Showcase #1
https://github.com/user-attachments/assets/fd77e25b-0bb4-49c2-8a1c-dbb2ccc4506e

# Video Showcase #2: Safety E-Stop
The mobile companion application includes an Emergency Safety Stop, which can be enabled by pressing the "Enable Fan E-Stop" button on the mobile application.

Once enabled, if enough device movement is detected, the fan system will shut down immediately (by sending a "-1" shut-off command to the WebSocket server), after which the fan system can be re-enabled by pressing the "Restart Fan System" button to send the "RESTART" command.

## Safety E-Stop Distant

https://github.com/user-attachments/assets/e4aefa24-4d65-431b-95dd-5887364afc17


## Safety E-Stop Close
Here the safety E-Stop shows the speed at which the shut-down occurs, which will help prevent accidental injury if someone accidentally falls or trips towards the fans.

A small snippet at the beginning also shows how the interactive custom circle image updates in real time as the user interacts with the slider and colour wheel for each fan individually.

https://github.com/user-attachments/assets/4482ef85-0fa0-46a5-928b-66956f27236e



# Steps on how to use
### Setting up the board
Arduino preferences board manager URL: https://espressif.github.io/arduino-esp32/package_esp32_index.json

### Tools Settings
  Enable CDC on boot (enables flashing without needing to press the physical boot button)
### Libraries used
AsyncTCP by ESP32Async 3.4.9

ESPAsyncWebServer by ESP32Async 3.8.1


# Default configurations used (can be changed if needed, please see the final heading)
## Hardware
![Hardware configuration Final_page](https://github.com/user-attachments/assets/4b55402b-b4bf-4689-ad56-7bb81c7a7709)


## Software
Creates a open WiFi access point "Holo3D" with a WebSockets server accessible at 192.168.4.1 on port 80. (can be changed in "HardwareSoftwareConfig.ino" or "WiFiConfig.h" for more advanced functionality)

# If a different hardware configuration is being used
Edit the "HardwareSoftwareConfig.ino" file and change the pin assignement etc. to match the new hardware configuration.
Ex. If the speed/Hall-effect sensor is connected to a different pin other than pin 1.

This file can also be used to change the WiFi access point name (ssid) and password if needed.

If more configuration is required, such as changing the WebSockets IP address and port/URL. "WiFiConfig.h" can be edited to the new required values.




