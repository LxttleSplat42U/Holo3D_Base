#include "RPM_Measurement.h"



void IRAM_ATTR INT_bladePassed(){
  bladePassed = true; 
  nbladePasses = nbladePasses + 1; // inc number of blade passes [Explicit to avoid deprecated warnings]
  digitalWrite(D13, digitalRead(D13) ? LOW : HIGH); // Toggle LED D13
}

void setupRPMMeasurement(){
   pinMode(RPM_SENSOR, INPUT_PULLDOWN); // Set pin as input
   //gpio_set_pull_mode((gpio_num_t)RPM_SENSOR, GPIO_FLOATING); // explicitly disable pull-up/down
   attachInterrupt(digitalPinToInterrupt(RPM_SENSOR), INT_bladePassed, RISING); // attach interrupt to RPM pin
   RPMMeasurementEnabled = true;
   pinMode(D13, OUTPUT); // Debugging link Builtin LED to interrupt
   digitalWrite(D13, LOW);
}

void enableRPMMeasurement(){
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR), INT_bladePassed, RISING); // attach interrupt to RPM pin
  RPMMeasurementEnabled = true;
}

void disableRPMMeasurement(){
  detachInterrupt(RPM_SENSOR);
  RPMMeasurementEnabled = false;
  bladePassed = false;
}

