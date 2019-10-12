#ifndef _BMW_K75_
#define _BMW_K75_

#include <AUnit.h>
using namespace aunit;

class BMWK75Helper {
  public :
    boolean analogToDigital(int value) {
           if (value < 1023*0.3) return LOW;
      else if (value > 1023*0.7) return HIGH;
      else return NULL;
    }
    
    float mapf(double val, double in_min, double in_max, double out_min, double out_max) {
        return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

BMWK75Helper helper;

class BMWK75 {
  private:
    
    //UNPLUGGED lightSwitchPosition0Pin;
    int lightSwitchPosition1PinIn;
    int lightSwitchPosition2PinIn;
    int lightSwitchPosition = LIGHT_SWITH_POSITION_LOW_BEAM;

    int ledRingPinOut;
    bool ledRing = true;

    int headlightPinOut;
    bool headlight = true;
    
    int lightSensorPinIn;
    float lightSensor = 0;

    int lightInPinIn;
    bool lightIn = true;

        
  protected:
    bool isTestInstance = false;
    
  public:

    const int LIGHT_SWITH_POSITION_OFF = 0;
    const int LIGHT_SWITH_POSITION_PARKING_LIGHT = 1;
    const int LIGHT_SWITH_POSITION_LOW_BEAM = 2;
    const float LIGHT_SENSOR_THRESHOLD_MIN = 0.33;
    const float LIGHT_SENSOR_THRESHOLD_MAX = 0.66;


    BMWK75 (bool isTestInstance) {
      this->isTestInstance = isTestInstance;
    }


    void setup() {
      //Entrées
      //pinMode(temperatureSensorPin, INPUT);
      
      //Sorties
      //pinMode(ledPin, OUTPUT);
    }

    //GETTERS
    bool getLightIn () {
      if (this->isTestInstance != true) {
        int val = analogRead(lightInPinIn);
        lightIn = helper.analogToDigital(val) == HIGH ? true : false;
      }
      return lightIn;
    }
    
    bool getHeadlight () {
      return headlight;
    }
    
    bool getLedRing () {
      return ledRing;
    }
    
    float getLightSensor () {
      return lightSensor;
    }
    
    int getLightSwitchPosition() {
      return lightSwitchPosition;
    }

    //SETTERS
    void setLightIn (bool value) {
      lightIn = value;
      updateLights();
    }

    void setHeadlight (bool val) {
      //TODO : UPDATED GPIO
      headlight = val;
    }
    void setLedRing (bool val) {
      //TODO : UPDATED GPIO
      ledRing = val;
    }
    void setLightSensor (float val) {
      lightSensor = val;
      updateLights();
    }

    void updateLights() {
      //Light input
      if(this->getLightIn() == false) {
        setHeadlight (false);
        setLedRing (false);
      } else {
        //Light swith & Light sensor value
        int sp = this->getLightSwitchPosition();
        if(sp == LIGHT_SWITH_POSITION_PARKING_LIGHT) {
            setHeadlight (false);
            setLedRing (true); 
                     
          } else if(sp == LIGHT_SWITH_POSITION_LOW_BEAM) {
            setHeadlight (true);
            setLedRing (false);
            
          } else if(sp == LIGHT_SWITH_POSITION_OFF) {
            if(this->getLightSensor() >= this->LIGHT_SENSOR_THRESHOLD_MAX) {
                setHeadlight (false);
                setLedRing (true);
                
            } else if(this->getLightSensor() <= this->LIGHT_SENSOR_THRESHOLD_MIN) {
                setHeadlight (true);
                setLedRing (false);
            }    
          } else {
            //ERROR CASE
            setHeadlight (true);
            setLedRing (true);    
          } 
        }
    }
    
    void setLightSwitchPosition (int value) {
      this->lightSwitchPosition = value;
      updateLights();
    }

//NOOOOON
    //UPDATERS
    void getGpioIn() {
       //GET ALL INs AND SET THE CLASS VALUES
    }

    void setGpioOuts() {
       //SET ALL OUTs FROM THE CLASS VALUES
    }

    void refreshGPIO() {
      getGpioIn();
      setGpioOuts();
    }
//NOOOOON
    
};


BMWK75 tested (true);

test(DefaultLightConfiguration) {
  assertEqual(tested.getHeadlight(), true);
  assertEqual(tested.getLedRing(), true);
}

test(M_UNIT_LIGHT_IN_HIGH_AND_LIGHT_SWICTH_PARKING_LIGHT) {
  tested.setLightIn(true);
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_PARKING_LIGHT);
  assertEqual(tested.getHeadlight(), false);
  assertEqual(tested.getLedRing(), true);
}

test(M_UNIT_LIGHT_IN_HIGH_AND_LIGHT_SWICTH_LOW_BEAM) {
  tested.setLightIn(true);
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_LOW_BEAM);
  assertEqual(tested.getHeadlight(), true);
  assertEqual(tested.getLedRing(), false);
}

test(M_UNIT_LIGHT_IN_HIGH_AND_LIGHT_SWICTH_OFF_AUTO) {
  tested.setLightIn(true);
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_OFF);
  
  for (float i = 0; i <= 1; i=i+0.01) {
        tested.setLightSensor(i);
        if (i < tested.LIGHT_SENSOR_THRESHOLD_MAX) {
          assertEqual(tested.getHeadlight(), true);
          assertEqual(tested.getLedRing(), false);
        } else {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), true);
        }
  }
  for (float i = 1; i >= 0; i=i-0.01) {
        tested.setLightSensor(i);
        if (i > tested.LIGHT_SENSOR_THRESHOLD_MIN) {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), true);
        } else {
          assertEqual(tested.getHeadlight(), true);
          assertEqual(tested.getLedRing(), false);
        }
  }
}
  
 test(M_UNIT_LIGHT_IN_LOW_AND_LIGHT_SWICTH_PARKING_LIGHT) { 
  //Light Off
  tested.setLightIn(false);
 
  //POSITION PARKING_LIGHT
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_PARKING_LIGHT);
  assertEqual(tested.getHeadlight(), false);
  assertEqual(tested.getLedRing(), false);
 }

test(M_UNIT_LIGHT_IN_LOW_AND_LIGHT_SWICTH_LOW_BEAM) {
  tested.setLightIn(false);
   
  //POSITION LOW_BEAM
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_LOW_BEAM);
  assertEqual(tested.getHeadlight(), false);
  assertEqual(tested.getLedRing(), false);
 }

test(M_UNIT_LIGHT_IN_LOW_AND_LIGHT_SWICTH_OFF_AUTO) {
  tested.setLightIn(false);
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_OFF);

  for (float i = 0; i <= 1; i=i+0.01) {
        tested.setLightSensor(i);
        if (i < tested.LIGHT_SENSOR_THRESHOLD_MAX) {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), false);
        } else {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), false);
        }
  }
  for (float i = 1; i >= 0; i=i-0.01) {
        tested.setLightSensor(i);
        if (i > tested.LIGHT_SENSOR_THRESHOLD_MIN) {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), false);
        } else {
          assertEqual(tested.getHeadlight(), false);
          assertEqual(tested.getLedRing(), false);
        }
  }
}

#endif
