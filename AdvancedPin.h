#ifndef __digitalpin
#define __digitalpin

#include "Helper.h"
#include "PCF8574.h"

using namespace std;

class DigitalPin {
  private:   
    int pin;
    PCF8574* pcf8574 = NULL;
    int ioType;
    
  public: 
    DigitalPin(int io_type, int pin);
    DigitalPin(int io_type, PCF8574* pcf8574, int pin);
    
    void setup(int setupValue = -1);

    bool state();

    void set(bool value); 
    void high();
    void low();
};

class AnalogicPin {
  private:   
    int pin;
    int ioType;
    
  public: 
    AnalogicPin(int io_type, int pin);
    
    void setup(int setupValue = -1);

    float value();
    void set(float value); 

};

#endif
