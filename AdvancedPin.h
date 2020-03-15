#ifndef __digitalpin
#define __digitalpin

#include "Arduino.h"
#include "Helper.h"
#include "PCF8574.h"

using namespace std;

class DigitalPin {
  protected:   
    int ioType;
    PCF8574* pcf8574 = NULL;
    int pin;
    bool currentState;
    
  public:
    DigitalPin(int io_type, int pin);
    DigitalPin(int io_type, PCF8574* pcf8574, int pin);
    
    void setup(int setupValue = -1);

    void sayHello();

    bool state();

    void set(bool value); 
    void high();
    void low();
    bool toggle();
};

class AnalogicPin {
  private:   
    int ioType;
    int pin;
  public: 
    AnalogicPin(int io_type, int pin);
    
    void setup(int setupValue = -1);

    float value();
    void set(float value); 

};

#endif
