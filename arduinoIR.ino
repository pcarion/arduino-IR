#include <ArduinoSerial.h>
#include "ZenArduinoIR.h"


// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// Pin 2 is for IR receiver
int pinIRReceiver = 2;

int isOver;

ZenArduinoIR zenArduinoIR(2, HIGH);

// the setup routine runs once when you press reset:
void setup() { 
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(pinIRReceiver, INPUT);
  
  isOver = -1;
  
  if (zenArduinoIR.prepareAcquisition() < 0) {
    msg("Arduino IR preparation failed!");
    return;
  }
  
  // attach interrupt 0 (pin 2 for uno)
  // see: http://arduino.cc/en/Reference/attachInterrupt 
  attachInterrupt(0, interruptHandler, CHANGE);  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
  
  if (isOver < 0) {
    if (zenArduinoIR.isOver() > 0) {
        isOver = 1;
        msg("is OVER");
        msgi("counter is:", zenArduinoIR.counter());
        msgi("errno is:", zenArduinoIR.errno());
        zenArduinoIR.dump();
    } else {
        msgi("... counter is:", zenArduinoIR.counter());
    }
  }
}

void interruptHandler()
{
  zenArduinoIR.interruptHandler();
}

void msg(char * msg) {
  Serial.println(msg);
}

void msgi(char * msg, int i) {
  Serial.print(msg);
  Serial.println(i,DEC);
}
