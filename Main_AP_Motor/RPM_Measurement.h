#pragma once
extern const int RPM_SENSOR; // Pin for Hall-effect sensor
extern volatile int nbladePasses; // Used to count the number of blade passes in the interrupt
extern volatile bool bladePassed;
extern volatile bool RPMMeasurementEnabled;

void IRAM_ATTR INT_bladePassed();
void setupRPMMeasurement();
void enableRPMMeasurement();
void disableRPMMeasurement();
void MeasureRPM();

