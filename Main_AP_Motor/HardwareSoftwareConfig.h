#include "RPM_Measurement.h"
#pragma once
// Change the following based on the hardware and desired software configuration
extern const int userID;                        // Mobile companion app ID
extern const int Monitor_LED;           // Built in microcontroller LED
extern const int motorPWM;              // PWM Motor Control PIN
extern const int RPM_SENSOR;            // Pin speed/rpm sensor or circuit is connected to

// WiFi and WebSocket config
extern const char *ssid;                // Wifi AP Name
extern const char *password;            // Password of AP [NULL = none]
// **Note the rest of the server IP and ports are configured in WiFiConfig.h**

// RPM Measurement
extern const int tRPMDebounceTime;      // [µs] Debounce time for RPM_sensor 
extern const int RPMTimerPrescaler;     // Prescaler value to divide 80 Mhz PLL clock [ensure no decimal for precise timing] [<41.67ms based on 1440rpm (60/1440)]
extern const int RPM_MEASUREMENT_TIME;  // Desired timer frequency at which to calculate the fan rpm [ensure no decimal for precise timing]
extern const int tReportInterval;       // [ms] Interval at which connected clients are reported to [Ex. reporting fan RPM, etc.]

//PWM Settings
extern const int freq;                  // Frequency of the PWM pin used to control the motor speed
extern const int resolution;            // Resolution of the PWM pin used to control the motor speed

// Shutdown
extern bool shutDown;
extern int tshutdownTimer;
extern int tshutDownRepeatInterval; 

void shutDownSystem();
