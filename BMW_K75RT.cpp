#include "Arduino.h"
#include "PCF8574.h"
#include "AdvancedPin.h"
#include "BMW_K75RT.h"

using namespace std;

BMW_K75RT::BMW_K75RT() {
/*
  ANALOGS A0 -> A7 (!) A4 + A5 -> SDA + SCL
  DIGITALS ARDUINO 2 -> 13
  DIGITALS PCF8574 0 -> 7
*/

//EXTENDERS
* pcf8574_1                  = PCF8574(0x20);
* pcf8574_2                  = PCF8574(0x21);

//INPUTS
* lightSensorPinIn           = AnalogicPin(INPUT, A0);
* temperatureSensorPinIn     = AnalogicPin(INPUT, A1); //OK
* oilPressureSensorPinIn     = AnalogicPin(INPUT, A2); //OK
* fuelSensorPinIn            = AnalogicPin(INPUT, A3); //OK

mUnitLightOutputPinIn      =  DigitalPin(INPUT, 2);
* rpmPinIn                   = DigitalPin(INPUT, 3);
* speedPinIn_wheel           = DigitalPin(INPUT, 4);  //OK TODO : Choisir
* speedPinIn_abs             = DigitalPin(INPUT, 5);  //OK TODO : Choisir
* lightSwitchPosition1PinIn  = DigitalPin(INPUT, 6);  //OK
* lightSwitchPosition2PinIn  = DigitalPin(INPUT, 7);  //OK
* gearBox1PinIn              = DigitalPin(INPUT, 8);  //OK
* gearBox2PinIn              = DigitalPin(INPUT, 9);  //OK
* gearBox3PinIn              = DigitalPin(INPUT, 10); //OK

//OUTPUTS
* fuelIndicatorPinOut       = AnalogicPin(OUTPUT, A6); //OK

* ledRingPinOut             = DigitalPin(OUTPUT, pcf8574_1, 0);
* headlightPinOut           = DigitalPin(OUTPUT, pcf8574_1, 1); 
* speedIdicatorPinOut       = DigitalPin(OUTPUT, pcf8574_1, 2);
* rpmPinOut                 = DigitalPin(OUTPUT, pcf8574_1, 3); //OK
* fanPinOut                 = DigitalPin(OUTPUT, pcf8574_1, 4); //OK
* warningPinOut             = DigitalPin(OUTPUT, pcf8574_1, 5);

* neutralPinOut             = DigitalPin(OUTPUT, pcf8574_2, 0); //OK
* gear1PinOut               = DigitalPin(OUTPUT, pcf8574_2, 1); //OK
* gear2PinOut               = DigitalPin(OUTPUT, pcf8574_2, 2); //OK
* gear3PinOut               = DigitalPin(OUTPUT, pcf8574_2, 3); //OK           
* gear4PinOut               = DigitalPin(OUTPUT, pcf8574_2, 4); //OK
* gear5PinOut               = DigitalPin(OUTPUT, pcf8574_2, 5); //OK
}

void BMW_K75RT::setup () {
  //INPUTS
  lightSensorPinIn          ->setup();
  temperatureSensorPinIn    ->setup();
  oilPressureSensorPinIn    ->setup();
  fuelSensorPinIn           ->setup();
  mUnitLightOutputPinIn     .setup();
  rpmPinIn                  ->setup();
  speedPinIn_wheel          ->setup();
  speedPinIn_abs            ->setup();
  lightSwitchPosition1PinIn ->setup();
  lightSwitchPosition2PinIn ->setup();
  gearBox1PinIn             ->setup();
  gearBox2PinIn             ->setup();
  gearBox3PinIn             ->setup();
  
  //OUTPUTS
  fuelIndicatorPinOut       ->setup();
  ledRingPinOut             ->setup();
  headlightPinOut           ->setup();
  speedIdicatorPinOut       ->setup();
  rpmPinOut                 ->setup();
  fanPinOut                 ->setup();
  warningPinOut             ->setup();
  neutralPinOut             ->setup();
  gear1PinOut               ->setup();
  gear2PinOut               ->setup();
  gear3PinOut               ->setup();
  gear4PinOut               ->setup();
  gear5PinOut               ->setup();

  pcf8574_1                 ->begin();
  pcf8574_2                 ->begin();
}

void BMW_K75RT::loopInit() {
  globalWarning = false;
}
void BMW_K75RT::sayHello() {
  Serial.println("BMW_K75RT says : Hi :)");
}

bool BMW_K75RT::isOilPresureOK() { 
  float oilPressure = oilPressureSensorPinIn->value();
  oilPressureWarning = oilPressure <= OIL_PRESURE_THESHOLD_MIN ? true : false;
  globalWarning = globalWarning || oilPressureWarning;
  return oilPressureWarning;
}
