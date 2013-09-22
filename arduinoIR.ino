#include <ArduinoSerial.h>

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// Pin 2 is for IR receiver
int pinIRReceiver = 2;

unsigned long time0 = -1;
unsigned long timeFirstAcquisition = -1;

int acquisitionDone = -1;

// if pin doesn't change during this period... the acquisition is considered over
// 5 seconds here:
#define ACQUISITION_TIMEOUT  (5 * 1000 * 1000) 

// last value read on IR pin
int state = -1;

// storage of times for each IR pin change of state
#define NB_TIMES_SLICES 132
long times[NB_TIMES_SLICES]; 

// current slide being read
int counter = 0;

// the setup routine runs once when you press reset:
void setup() { 
  Serial.begin(9600);
  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(pinIRReceiver, INPUT);
  
  // initialize state of IR pin
  state = digitalRead(pinIRReceiver);
  time0 = micros();
  
  // we store HIGH duration in 'even' indexed slots
  if (state == HIGH) {
    counter = 0;
  } else {
    times[0] = 0;
    counter = 1;
  }
  timeFirstAcquisition = -1;
  // boolean to indicate that the acquisition was done
  acquisitionDone = -1;
  
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
  
  if (acquisitionDone > 0) {
    for(int i = 0 ; i < counter; i++) {
        Serial.print(i,DEC);
        Serial.print(":");
        Serial.println(times[i],DEC);
    }
    Serial.println("DONE");
    acquisitionDone = 0;
  }
  
  if (acquisitionDone < 0) {
      msgi("counter is:", counter);
  }
}

void interruptHandler()
{
  int val = digitalRead(pinIRReceiver);
  unsigned long time = micros();
  
  if (state != val) {
    if (counter < NB_TIMES_SLICES) {
      times[counter] = time - time0;
      time0 = time;
      counter++;
    }
    if (timeFirstAcquisition < 0) {
      timeFirstAcquisition = time;
    }
  }
  
  if (acquisitionDone < 0) {
    if (((time - timeFirstAcquisition) > ACQUISITION_TIMEOUT) && (counter > 10)) {
      // we need at least 10 measurement to declare acquisition done
      acquisitionDone = 1;
    }
  }
  
  state = val;
}

void msg(char * msg) {
  Serial.println(msg);
}

void msgi(char * msg, int i) {
  Serial.print(msg);
  Serial.println(i,DEC);
}
