#ifndef _BMW_K75_
#define _BMW_K75_

#include <AUnit.h>
using namespace aunit;



class BMWK75 {
private:
    bool lightIn = true;
    float lightSensorLevelIn = 0;
    int lightSwitchPosition = LIGHT_SWITH_POSITION_LOW_BEAM;

    bool ledRingOut = true;
    bool headlightOut = true;
    
public:
  BMWK75 (int whatever){}

    const int LIGHT_SWITH_POSITION_OFF = 0;
    const int LIGHT_SWITH_POSITION_PARKING_LIGHT = 1;
    const int LIGHT_SWITH_POSITION_LOW_BEAM = 2;

    const float LIGHT_SENSOR_THRESHOLD = 0.5;

    //GETTERS
    bool getLightIn () { return lightIn; }
    bool getHeadlightOut () { return headlightOut; }
    bool getLedRingOut () { return ledRingOut; }
    float getLightSensorLevelIn () { return lightSensorLevelIn;}
    int getLightSwitchPosition() {return lightSwitchPosition;}

    //SETTERS
    void setLightIn (bool value) {
      lightIn = value;
    }

    void setHeadlightOut (bool val) { headlightOut = val; }
    void setLedRingOut (bool val) { ledRingOut = val; }
    void setLightSensorLevelIn (float val) {
       if(val >= this->LIGHT_SENSOR_THRESHOLD && this->getLightSwitchPosition() == this->LIGHT_SWITH_POSITION_OFF && this->getLightIn() == true) {
              setHeadlightOut (false);
              setLedRingOut (true);
        } else if(val < this->LIGHT_SENSOR_THRESHOLD && getLightSwitchPosition() == this->LIGHT_SWITH_POSITION_OFF && this->getLightIn() == true) {
              setHeadlightOut (true);
              setLedRingOut (false);
        }
      lightSensorLevelIn = val;
    }
    
    void setLightSwitchPosition (int value) {
      if(this->getLightIn() == true)
      {
          if(value == LIGHT_SWITH_POSITION_OFF) {
            if(this->getLightSensorLevelIn() >= this->LIGHT_SENSOR_THRESHOLD) {
                setHeadlightOut (false);
                setLedRingOut (true);
            } else {
                setHeadlightOut (true);
                setLedRingOut (false);
            }
            this->lightSwitchPosition = value;
          
          } else if(value == LIGHT_SWITH_POSITION_PARKING_LIGHT) {
            setHeadlightOut (false);
            setLedRingOut (true);
            this->lightSwitchPosition = value;
          
          } else if(value == LIGHT_SWITH_POSITION_LOW_BEAM) {
            setHeadlightOut (true);
            setLedRingOut (false);
            this->lightSwitchPosition = value;

          } else {
            //TODO: Handle error
          }
      } else {
        setHeadlightOut (false);
        setLedRingOut (false);
      }

    }

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

    
};


BMWK75 tested (1);

test(LightsModes) {
  //Default
  assertEqual(tested.getHeadlightOut(), true);
  assertEqual(tested.getLedRingOut(), true);

  //Light On
  tested.setLightIn(true);
 
  //POSITION OFF
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_OFF);
  for (float i = 0; i < tested.LIGHT_SENSOR_THRESHOLD; i=i+0.1) {
        tested.setLightSensorLevelIn(i);
        assertEqual(tested.getHeadlightOut(), true);
        assertEqual(tested.getLedRingOut(), false);
  }
  for (float i = tested.LIGHT_SENSOR_THRESHOLD; i <= 1; i=i+0.1) {
      tested.setLightSensorLevelIn(i);
      assertEqual(tested.getHeadlightOut(), false);
      assertEqual(tested.getLedRingOut(), true);
  }

  //POSITION PARKING_LIGHT
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_PARKING_LIGHT);
  assertEqual(tested.getHeadlightOut(), false);
  assertEqual(tested.getLedRingOut(), true);
  
  //POSITION LOW_BEAM
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_LOW_BEAM);
  assertEqual(tested.getHeadlightOut(), true);
  assertEqual(tested.getLedRingOut(), false);
  
  
  //Light Off
  tested.setLightIn(false);
 
  //POSITION OFF
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_OFF);
  for (float i = 0; i < tested.LIGHT_SENSOR_THRESHOLD; i=i+0.1) {
        tested.setLightSensorLevelIn(i);
        assertEqual(tested.getHeadlightOut(), false);
        assertEqual(tested.getLedRingOut(), false);
  }
  for (float i = tested.LIGHT_SENSOR_THRESHOLD; i <= 1; i=i+0.1) {
      tested.setLightSensorLevelIn(i);
      assertEqual(tested.getHeadlightOut(), false);
      assertEqual(tested.getLedRingOut(), false);
  }

  //POSITION PARKING_LIGHT
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_PARKING_LIGHT);
  assertEqual(tested.getHeadlightOut(), false);
  assertEqual(tested.getLedRingOut(), false);
  
  //POSITION LOW_BEAM
  tested.setLightSwitchPosition(tested.LIGHT_SWITH_POSITION_LOW_BEAM);
  assertEqual(tested.getHeadlightOut(), false);
  assertEqual(tested.getLedRingOut(), false);

}


#endif
