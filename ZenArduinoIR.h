#ifndef ZenArduinoIR_h
#define ZenArduinoIR_h

// configuration parameters of the class

// number of time slices used during acquisition
#define ZA_IR_NB_TIME_SLICES 132

// if a slice is less than this duration, it is considered
// as a glitch and is ignored
#define ZA_IR_GLITCH 250


// if pin doesn't change during this period... the acquisition is considered over
// 2 seconds here:
#define ZA_IR_ACQUISITION_TIMEOUT  (2 * 1000 * 1000)

class ZenArduinoIR {
  public:
    ZenArduinoIR(int pin, int state0);
    
    int prepareAcquisition();
    
    void interruptHandler();
    
    int isOver();
    
    int errno();
    
    int counter();
    
    void dump();
    
  private:
    int m_pin; // pin used to read the state of the IR receiver
    
    int m_state0; // value of the pin when no IR signal received
    
    // storage of acquisition data
    unsigned long m_timeSlice[ZA_IR_NB_TIME_SLICES];
    int m_indexSlice;
    
    int m_lastValue; // last value of the pin
    unsigned long m_time0; // time (us) of last change of state 
    unsigned long m_prevTime0; // value used to account for glitches
    
    // used to track potential errors
    // (if > 0, then debugging is required!)
    int m_errno;
    
    void setErrno(int errno);
};

#endif
