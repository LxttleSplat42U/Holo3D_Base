// Change the following based on the hardware and desired software configuration
const int Monitor_LED = D13;          // Built in microcontroller LED
const int motorPWM = 8;               // PWM Motor Control PIN
const int RPM_SENSOR = 1;             // Pin speed/rpm sensor or circuit is connected to

// WiFi and WebSocket config
const char *ssid = "Holo3D";          // Wifi AP Name
const char *password = NULL;          // Password of AP [NULL = none]
// **Note the rest of the server IP and ports are configured in WiFiConfig.h**

// RPM Measurement
const int tRPMDebounceTime = 1000;    // [µs] Debounce time for RPM_sensor 
const int RPMTimerPrescaler = 80;     // Prescaler value to divide 80 Mhz PLL clock [ensure no decimal for precise timing] [<41.67ms based on 1440rpm (60/1440)]
const int RPM_MEASUREMENT_TIME = 1;  // Desired timer frequency at which to calculate the fan rpm [ensure no decimal for precise timing]

//PWM Settings
const int freq = 8000;                // Frequency of the PWM pin used to control the motor speed
const int resolution = 8;             // Resolution of the PWM pin used to control the motor speed
const int tReportInterval = 2000;      // [ms] Interval at which connected clients are reported to [Ex. reporting fan RPM, etc.]

// Shutdown
bool shutDown = false;
int tshutdownTimer = 0;
int tshutDownRepeatInterval = 500; 

void shutDownSystem(){
  setFanSpeed(0); // Shutdown fan motors
  ws.textAll("-1"); // Forward to all connected clients for shutdown
}
