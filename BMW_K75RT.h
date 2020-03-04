#ifndef __bmw_K75RT
#define __bmw_K75RT

#include "Arduino.h"
#include "PCF8574.h"
#include "AdvancedPin.h"

using namespace std;

class BMW_K75RT {
  private:
    PCF8574 * pcf8574_1;
    PCF8574 * pcf8574_2;
    
  public:
    //INPUTS
    AnalogicPin * lightSensorPinIn;
    AnalogicPin * temperatureSensorPinIn;     //OK
    AnalogicPin * oilPressureSensorPinIn;     //OK
    AnalogicPin * fuelSensorPinIn;            //OK
    
    DigitalPin * mUnitLightOutputPinIn;
    DigitalPin * rpmPinIn;
    DigitalPin * speedPinIn_wheel;            //OK TODO : Choisir
    DigitalPin * speedPinIn_abs;              //OK TODO : Choisir
    DigitalPin * lightSwitchPosition1PinIn;   //OK
    DigitalPin * lightSwitchPosition2PinIn;   //OK
    DigitalPin * gearBox1PinIn;               //OK
    DigitalPin * gearBox2PinIn;               //OK
    DigitalPin * gearBox3PinIn ;              //OK
    
    //OUTPUTS
    AnalogicPin * fuelIndicatorPinOut;        //OK
    
    DigitalPin * ledRingPinOut;
    DigitalPin * headlightPinOut; 
    DigitalPin * speedIdicatorPinOut;
    DigitalPin * rpmPinOut;                   //OK
    DigitalPin * fanPinOut;                   //OK
    DigitalPin * warningPinOut;
    
    DigitalPin * neutralPinOut;               //OK
    DigitalPin * gear1PinOut;                 //OK
    DigitalPin * gear2PinOut;                 //OK
    DigitalPin * gear3PinOut;                 //OK           
    DigitalPin * gear4PinOut;                 //OK
    DigitalPin * gear5PinOut;                 //OK

    int gear;
    
    const float OIL_PRESURE_THESHOLD_MIN = 0.33;

  public:
    bool oilPressureWarning = false;
    bool globalWarning = false;
    
  public: 
    BMW_K75RT();
    void setup();
    void loopInit();
    void sayHello();
    bool isOilPresureOK();
    
   
};

#endif
