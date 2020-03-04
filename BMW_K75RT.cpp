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
this->pcf8574_1                  = new PCF8574(0x20);
this->pcf8574_2                  = new PCF8574(0x21);

//INPUTS
this->lightSensorPinIn           = new  AnalogicPin(INPUT, A0);
this->temperatureSensorPinIn     = new AnalogicPin(INPUT, A1); //OK
this->oilPressureSensorPinIn     = new AnalogicPin(INPUT, A2); //OK
this->fuelSensorPinIn            = new AnalogicPin(INPUT, A3); //OK

this->mUnitLightOutputPinIn      = new DigitalPin(INPUT, 2);
this->rpmPinIn                   = new DigitalPin(INPUT, 3);
this->speedPinIn_wheel           = new DigitalPin(INPUT, 4);  //OK TODO : Choisir
this->speedPinIn_abs             = new DigitalPin(INPUT, 5);  //OK TODO : Choisir
this->lightSwitchPosition1PinIn  = new DigitalPin(INPUT, 6);  //OK
this->lightSwitchPosition2PinIn  = new DigitalPin(INPUT, 7);  //OK
this->gearBox1PinIn              = new DigitalPin(INPUT, 8);  //OK
this->gearBox2PinIn              = new DigitalPin(INPUT, 9);  //OK
this->gearBox3PinIn              = new DigitalPin(INPUT, 10); //OK

//OUTPUTS
this->fuelIndicatorPinOut       = new AnalogicPin(OUTPUT, A6); //OK

this->ledRingPinOut             = new DigitalPin(OUTPUT, pcf8574_1, 0);
this->headlightPinOut           = new DigitalPin(OUTPUT, pcf8574_1, 1); 
this->speedIdicatorPinOut       = new DigitalPin(OUTPUT, pcf8574_1, 2);
this->rpmPinOut                 = new DigitalPin(OUTPUT, pcf8574_1, 3); //OK
this->fanPinOut                 = new DigitalPin(OUTPUT, pcf8574_1, 4); //OK
this->warningPinOut             = new DigitalPin(OUTPUT, pcf8574_1, 5);

this->neutralPinOut             = new DigitalPin(OUTPUT, pcf8574_2, 0); //OK
this->gear1PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 1); //OK
this->gear2PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 2); //OK
this->gear3PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 3); //OK           
this->gear4PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 4); //OK
this->gear5PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 5); //OK
}

void BMW_K75RT::setup () {
  //INPUTS
  lightSensorPinIn          ->setup();
  temperatureSensorPinIn    ->setup();
  oilPressureSensorPinIn    ->setup();
  fuelSensorPinIn           ->setup();
  mUnitLightOutputPinIn     ->setup();
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

  gear = -1;
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
