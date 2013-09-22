#include <Arduino.h>
#include <ArduinoSerial.h>
#include "ZenArduinoIR.h"

#define E_OVERFLOW 10
#define E_NO_HIGHLO 20
#define E_GLITCH_AT_0 30

// TODO
// - what if the micros() value is close to overflow?
// - what if we have a glitch for the first slice
// - what if we have 2 glitches in a row

ZenArduinoIR::ZenArduinoIR(int pin, int state0)
{
  m_pin = pin;
  m_state0 = state0;
}

int ZenArduinoIR::prepareAcquisition() {
  Serial.print("Hello from ZenArduinoIR acquisition on pin:");
  Serial.println(m_pin);
  
  m_errno = 0;
  
  for(int i = 0; i < ZA_IR_NB_TIME_SLICES; i++) {
    m_timeSlice[i] = 0;
  }
  
  // store current state
  m_lastValue = digitalRead(m_pin);
  m_time0 = micros();
  m_prevTime0 = m_time0;
  m_indexSlice = 0;
  
  // we expect the value of the pin to be in state0
  if (m_lastValue != m_state0) {
    return -1;
  }
  
  return 0;
}

void ZenArduinoIR::interruptHandler() {
  // we read value of pin and time
  int value = digitalRead(m_pin);
  unsigned long time = micros();

  // the value of the pin should have changed 
  if (m_lastValue == value) {
    setErrno(E_NO_HIGHLO);
    return;
  }

  // duration of the change of value
  long delta = time - m_time0;
  
  // we test if we have a glitch
  // a glitch is a change value which last a very small amount of time
  if (delta < ZA_IR_GLITCH) {
    if (m_indexSlice > 0) {
      // we reset time0 so that we accumulate in previous slice
      m_time0 = m_prevTime0;
      m_indexSlice --; 
      m_lastValue = value;
      return;
    } else {
      delta = 0;
      setErrno(E_GLITCH_AT_0);
    }
  }
  
  // we have a valid slice here
  
  // test for overflow
  if (m_indexSlice >= ZA_IR_NB_TIME_SLICES) {
    setErrno(E_OVERFLOW);
    return;
  }
  m_timeSlice[m_indexSlice] = delta;
  
  // prepare for next slice
  m_indexSlice++;
  m_prevTime0 = m_time0;
  m_time0 = time;
  m_lastValue = value;
}

int ZenArduinoIR::isOver() {
  // if still on first slice, we can't be over
  if (m_indexSlice == 0) {
    return -1;
  }

  // we measure time sice last change of state  
  unsigned long time = micros();
  long delta = time - m_time0;
  
  if (delta > ZA_IR_ACQUISITION_TIMEOUT) {
    return 1;
  } else {
    return -1;
  }
}

void ZenArduinoIR::setErrno(int errno) {
  if (m_errno == 0) {
    m_errno = errno;
  }
}

int ZenArduinoIR::errno() {
  return m_errno;
}

int ZenArduinoIR::counter() {
  return m_indexSlice;
}

void ZenArduinoIR::dump() {
  for(int i = 0 ; i < m_indexSlice; i++) {
      //Serial.print(i,DEC);
      //Serial.print(":");
      Serial.println(m_timeSlice[i],DEC);
  }
}
