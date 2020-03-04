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


    /*********************************
    TIME MANAGEMENT
    *********************************/

    /**********************************
    LIGHTS
    **********************************/
    const int LIGHT_SWITCH_POSITION_OFF = 0;
    const int LIGHT_SWITCH_POSITION_PARKING_LIGHT = 1;
    const int LIGHT_SWITCH_POSITION_LOW_BEAM = 2;
    const float LIGHT_SENSOR_THRESHOLD_MIN = 0.33;
    const float LIGHT_SENSOR_THRESHOLD_MAX = 0.66;
    /**********************************
    OIL
    **********************************/
    const float OIL_PRESURE_THESHOLD_MIN = 0.33;

    /*********************************
    RPM & SHIFTLIGHT
    *********************************/  
    const int SHIFT_LIGHT_THERSHOLD_MAX = 8600; //67HZ + 2 Segments
    const int SHIFT_LIGHT_THERSHOLD_MIN = 8400; // LA ZONE SUR LE COMPTEUR BMX EST ENTRE 8500 ET 9000

    /**********************************
    SPEED
    **********************************/


    /*********************************
    TEMPERATURE MOTEUR ET VENTILATION
    *********************************/
    /* POST #23 http://bmist.forumpro.fr/t100118-probleme-sonde-temperature-ldr-k75?highlight=sonde+temp%E9rature
    *  "le ventilo se déclenchait vers 29,5 ce qui doit correspondre à 101°c valeur normale"
    */
    const float TEMPERATURE_VENTILATION_THRESHOLD = 103;  //TODO : FIND MATCHING VALUES //103° = 175 OHMS
    const float TEMPERATURE_VENTILATION_HYSTERESIS = 5; //5° TODO : Verifier si cela nous convient
    const float TEMPERATURE_WARNING_THRESHOLD = 111;  //TODO : FIND MATCHING VALUES // 111° = 143 OHMS
    const float TEMPERATURE_WARNING_HYSTERESIS = 5; //TODO : Verifier si cela nous convient

    /**********************************
    GEARBOX
    **********************************/
    
    /**********************************
    FUEL
    **********************************/
    const int FUEL_LEVEL_THERSHOLD_MAX = 0.15; //S'allume en dessous de 10%
    const int FUEL_LEVEL_THERSHOLD_MIN = 0.10; //S'éteint au dessus de 15%
    const int FUEL_LEVEL_HIGH = 1.0; //S'éteint au dessus de 15%
    const int FUEL_LEVEL_LOW = 0.0; //S'éteint au dessus de 15%

    /**********************************
    WARNINGS
    **********************************/

    /**********************************
    DISPLAY
    **********************************/

  public: //TODO : ENCAPSULATE INTO A DATA STRUCTURE
    bool globalWarning;

    bool oilPressureWarning;

    bool currentRPMState;
    bool previousRPMState;
    unsigned long int timeSpentAtPreviousRPMState;
    float rpm;
    bool rpmWarning = false;

    bool currentSpeedState;
    bool previousSpeedState;
    unsigned long int timeSpentAtPreviousSpeedState;
    float kmh;

    float engineTemperatureSensorValue;
    float engineTemperature;
    bool fanOn;
    bool engineTemperatureWarning;
    
    bool gearBox1Value;
    bool gearBox2Value;
    bool gearBox3Value;
    int engagedGear;
    bool gearWarning;

    float fuelLevel;

    unsigned long int processed;
    unsigned long int lastPrintedProcessed;

    Stopwatch * stopwatch;
    
  public: 
    BMW_K75RT();
    void setup();
    void loopInit();
    void sayHello();
    void updateLights();
    bool isOilPresureOK();
    void updateRPM();
    void updateSpeed();
    void updateVentialtion();
    void updateEngagedGear();
    void updateFuelLevel();
    bool isGlobalWarning();
    String toString();
};

#endif
