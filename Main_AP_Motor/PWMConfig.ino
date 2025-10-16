void setupPWM(){
  pinMode(motorPWM, OUTPUT);              // Set pin to OUPUT
  digitalWrite(motorPWM, LOW);            // Ensure Motor is OFF
  ledcAttach(motorPWM, freq, resolution); // Setup PWM frequency and resolution
}