#include <Arduino.h>
#include <ArduinoSerial.h>
#include "ZenArduinoIR.h"


ZenArduinoIR::ZenArduinoIR()
{
}

void ZenArduinoIR::prepareAcquisition() {
  Serial.println("Hello from ZenArduinoIR acquisition!!");
}

