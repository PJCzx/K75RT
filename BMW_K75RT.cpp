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
  this->rpmPinIn                   = new DigitalPin(INPUT_PULLUP, 3);
  this->speedPinIn_wheel           = new DigitalPin(INPUT_PULLUP, 4);  //OK TODO : Choisir //TODO : Cosidérer tout passer en INPUT_PULLUP
  this->speedPinIn_abs             = new DigitalPin(INPUT, 5);  //OK TODO : Choisir
  this->lightSwitchPosition1PinIn  = new DigitalPin(INPUT, 6);  //OK
  this->lightSwitchPosition2PinIn  = new DigitalPin(INPUT, 7);  //OK
  this->gearBox1PinIn              = new DigitalPin(INPUT, 8);  //OK
  this->gearBox2PinIn              = new DigitalPin(INPUT, 9);  //OK
  this->gearBox3PinIn              = new DigitalPin(INPUT, 10); //OK

  //OUTPUTS
  this->faker                     = new DigitalPin(OUTPUT, 11);

  this->fuelIndicatorPinOut       = new AnalogicPin(OUTPUT, A6); //OK

  this->ledRingPinOut             = new DigitalPin(OUTPUT, pcf8574_1, 0);
  this->headlightPinOut           = new DigitalPin(OUTPUT, pcf8574_1, 1); 
  this->speedPinOut               = new DigitalPin(OUTPUT, 12);//DigitalPin(OUTPUT, pcf8574_1, 2);
  this->rpmPinOut                 = new DigitalPin(OUTPUT, 13);//DigitalPin(OUTPUT, pcf8574_1, 3); //OK
  this->fanPinOut                 = new DigitalPin(OUTPUT, pcf8574_1, 4); //OK
  this->warningPinOut             = new DigitalPin(OUTPUT, pcf8574_1, 5);

  this->neutralPinOut             = new DigitalPin(OUTPUT, pcf8574_2, 0); //OK
  this->gear1PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 1); //OK
  this->gear2PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 2); //OK
  this->gear3PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 3); //OK           
  this->gear4PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 4); //OK
  this->gear5PinOut               = new DigitalPin(OUTPUT, pcf8574_2, 5); //OK

  this->stopwatch                 = new Stopwatch();
  //TODO DATA INIT

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
  faker                     ->setup();

  fuelIndicatorPinOut       ->setup();
  ledRingPinOut             ->setup();
  headlightPinOut           ->setup();
  speedPinOut               ->setup();
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
  processed++;
  stopwatch->run();
}

void BMW_K75RT::sayHello() {
  Serial.println("BMW_K75RT says : Hi :)");
}

void BMW_K75RT::updateLights() {
  bool mUnitLightVal = mUnitLightOutputPinIn->state(); 
  
  if(mUnitLightVal == HIGH) {
        // IN THIS CASE, M-UNIT ASK LIGHS TOBE OFF AND WILL TRIGGER HIGHBEAM
        //Blynk_mUnitLed.off();
        //Blynk_highBeamLed.on();
        
        //switch headlight OFF
        headlightPinOut->low();
        //Blynk_headlightLed.off();
        
        //switch ledring OFF
        ledRingPinOut->low();
        //Blynk_ledRingLed.off();
        
        
      } else {
        //High beam should be off according to M-Unit
        //Blynk_mUnitLed.on();
        //Blynk_highBeamLed.off();
        
        //Light SWITCH value
        int lightSwitchPosition = 0;
        
        if (lightSwitchPosition1PinIn->state() == HIGH) {
          if (lightSwitchPosition2PinIn->state() == HIGH) {
            lightSwitchPosition = 2;
            
            } else {
              lightSwitchPosition = 1;
          }
        } else {
          lightSwitchPosition = 0;
        }

        //Light sensor value
        float lightSensorValue = lightSensorPinIn->value();
        
        if(lightSwitchPosition == LIGHT_SWITCH_POSITION_PARKING_LIGHT) {
            //Blynk.virtualWrite(V1, "PARKING_LIGHT");
            
            //switch headlight OFF
            headlightPinOut->low();
            //Blynk_headlightLed.off();
            
            //switch ledring ON
            ledRingPinOut->high();
            //Blynk_ledRingLed.on();

         
          } else if(lightSwitchPosition == LIGHT_SWITCH_POSITION_LOW_BEAM) {
            //Blynk.virtualWrite(V1, "LOW_BEAM");

            //switch headlight ON
            headlightPinOut->high();
            //Blynk_headlightLed.on();
            
            //switch ledring OFF
            ledRingPinOut->low();
            //Blynk_ledRingLed.off();
         
          } else if(lightSwitchPosition == LIGHT_SWITCH_POSITION_OFF) {
            //Blynk.virtualWrite(V1, "OFF");

            if(lightSensorValue >= LIGHT_SENSOR_THRESHOLD_MAX) {
              //switch headlight OFF
              headlightPinOut->low();
              //Blynk_headlightLed.off();
              
              //switch ledring ON
              ledRingPinOut->high();
              //Blynk_ledRingLed.on();
              
            } else if(lightSensorValue <= LIGHT_SENSOR_THRESHOLD_MIN) {
              //switch headlight ON
              headlightPinOut->high();
              //Blynk_headlightLed.on();
              
              //switch ledring OFF
              ledRingPinOut->low();
              //Blynk_ledRingLed.off();

            }    
          }
        }
  
}

bool BMW_K75RT::isOilPresureOK() { 
  float oilPressure = oilPressureSensorPinIn->value();
  oilPressureWarning = oilPressure <= OIL_PRESURE_THESHOLD_MIN ? true : false;
  globalWarning = globalWarning || oilPressureWarning;
  return oilPressureWarning;
}

void BMW_K75RT::updateRPM() {
  //Get tachymeter state
  currentRPMState = rpmPinIn->state();
  
  //if state changed since last check
  if(currentRPMState == previousRPMState) {
    timeSpentAtPreviousRPMState_IN += stopwatch->currentMillis - stopwatch->previousMillis;
  } else {
    previousRPMState = currentRPMState;
    int segments = 2;
    float secondesParSegments = (float)timeSpentAtPreviousRPMState_IN*2/1000;
    rpm = 60.0/secondesParSegments*segments;

    if(rpm >= RPM_THERSHOLD_MAX){ //TODO : Define another threshold ?
      rpmWarning = true;
    } else if(rpm <= RPM_THERSHOLD_MIN){
      rpmWarning = false;
    }

    timeSpentAtPreviousRPMState_IN = 0;
  }
  //TODO : VERIFIER SI CE BOUT DE CODE FONCTIONNE
  float timeAtState = (1.0/rpm) * RPM_PULSE_RATIO;
  
  if(timeSpentAtPreviousRPMState_OUT > timeAtState) {
    timeSpentAtPreviousRPMState_OUT = 0;
    this->rpmPinOut->toggle();
  }
  timeSpentAtPreviousRPMState_OUT += stopwatch->timeSpentFromLastRun();
  if(timeSpentAtPreviousRPMState_OUT > 1000) timeSpentAtPreviousRPMState_OUT = 0;
}

void BMW_K75RT::updateSpeed() {

  //Get speed state
  currentSpeedState = speedPinIn_wheel->state(); //TODO : choisir avec ABS
  
  //if state changed since last check
  if(currentSpeedState == previousSpeedState) {
    timeSpentAtPreviousSpeedState_IN += stopwatch->timeSpentFromLastRun();
  } else {
    //TODO : Simplifier les calculs
    //TODO : Faire attention avec les millisec et les calculs qui ne sont pas unsigned long et donc qui peuvent un jour déconner
    previousSpeedState = currentSpeedState;

    float circonferenceMM = 17*25.4*PI; //TODO : Définir taille de la roue (17 pouces -> MM * PI)
    float segments = 1.0; // TODO : Définir nb de segments 
    float distanceEnMMParcouruePourUnSegment = circonferenceMM/segments/2; // /2 parce qu'on HIGH + LOW = 1 segment

    float millisecondesParSegments = (float)timeSpentAtPreviousSpeedState_IN;

    float vitesseEnMMparMilisec = distanceEnMMParcouruePourUnSegment/millisecondesParSegments;
    float vitesseEnMMparHeures = vitesseEnMMparMilisec*1000.0*60.0*60.0;
    float vitesseEnKMparHeures = vitesseEnMMparHeures / 1000.0 / 1000.0;
    
    kmh = vitesseEnKMparHeures;

    timeSpentAtPreviousSpeedState_IN = 0;
  }

  //TODO : VERIFIER SI CE BOUT DE CODE FONCTIONNE
  float timeAtState = (1.0 / kmh) * KMH_PULSE_RATIO;
  if(timeSpentAtPreviousSpeedState_OUT > timeAtState) {
    timeSpentAtPreviousSpeedState_OUT = 0;
    this->speedPinOut->toggle();
  }
  
  timeSpentAtPreviousSpeedState_OUT += stopwatch->timeSpentFromLastRun();
}


void BMW_K75RT::updateVentialtion() {

  engineTemperatureSensorValue = temperatureSensorPinIn->value();
  
  if(engineTemperature > TEMPERATURE_VENTILATION_THRESHOLD) fanOn = true;
  if(engineTemperature < TEMPERATURE_VENTILATION_THRESHOLD - TEMPERATURE_VENTILATION_HYSTERESIS) fanOn = false;
  
  if(engineTemperature > TEMPERATURE_WARNING_THRESHOLD) engineTemperatureWarning =  true;
  if(engineTemperature < TEMPERATURE_WARNING_THRESHOLD - TEMPERATURE_WARNING_HYSTERESIS) engineTemperatureWarning =  false;

  fanPinOut->set(fanOn ? HIGH : LOW); 

  engineTemperature = mapf(engineTemperatureSensorValue,0.0,1.0,TEMPERATURE_MIN_VAL_TEMP,TEMPERATURE_MAX_VAL_TEMP);
}

void BMW_K75RT::updateEngagedGear() {
  gearBox1Value = gearBox1PinIn->state();
  gearBox2Value = gearBox2PinIn->state();
  gearBox3Value = gearBox3PinIn->state();

  //conversion en numéro de vitesse
  gearWarning = false;
  engagedGear = -1;
  
       if(gearBox1Value == LOW   && gearBox2Value == LOW  && gearBox3Value == LOW)   engagedGear = 0;
  else if(gearBox1Value == HIGH  && gearBox2Value == LOW  && gearBox3Value == LOW)   engagedGear = 1;
  else if(gearBox1Value == LOW   && gearBox2Value == HIGH && gearBox3Value == LOW)   engagedGear = 2;
  else if(gearBox1Value == HIGH  && gearBox2Value == HIGH && gearBox3Value == LOW)   engagedGear = 3;
  else if(gearBox1Value == LOW   && gearBox2Value == LOW  && gearBox3Value == HIGH ) engagedGear = 4;
  else if(gearBox1Value == HIGH  && gearBox2Value == LOW  && gearBox3Value == HIGH ) engagedGear = 5;
  else if(gearBox1Value == LOW   && gearBox2Value == HIGH && gearBox3Value == HIGH ) engagedGear = 6;
  else if(gearBox1Value == HIGH  && gearBox2Value == HIGH && gearBox3Value == HIGH ) engagedGear = 7;

  else { engagedGear = -1; gearWarning = true; }

  neutralPinOut->set(engagedGear == 0 ? HIGH : LOW);
  gear1PinOut->set(engagedGear == 1 ? HIGH : LOW);
  gear2PinOut->set(engagedGear == 2 ? HIGH : LOW);
  gear3PinOut->set(engagedGear == 3 ? HIGH : LOW);
  gear4PinOut->set(engagedGear == 4 ? HIGH : LOW);
  gear5PinOut->set(engagedGear == 5 ? HIGH : LOW);
}

void BMW_K75RT::updateFuelLevel() {
   
  fuelLevelSensorValue = fuelSensorPinIn->value();

  if(fuelLevelSensorValue <= FUEL_LEVEL_THERSHOLD_MIN) {
    fuelWarning = true; 
  } else if(fuelLevelSensorValue >= FUEL_LEVEL_THERSHOLD_MAX) {
    fuelWarning = false; 
  }
  fuelIndicatorPinOut->set(fuelWarning);

  fuelLevel = mapf(fuelLevelSensorValue, 0.0, 1.0, FUEL_MIN_VAL_LEVEL, FUEL_MAX_VAL_LEVEL);
}

bool BMW_K75RT::isGlobalWarning() {

  globalWarning = oilPressureWarning || gearWarning || engineTemperatureWarning || rpmWarning || fuelWarning;
  warningPinOut->set(globalWarning ? HIGH : LOW); 
  return globalWarning;
}


String BMW_K75RT::toString() {

    String text = "";
    text+= "eps : ";
    text += processed - lastPrintedProcessed;
    lastPrintedProcessed = processed;
    text += "\t";
    //LIGHTS
      //INPUTS
      //OUTPUTS
    
    //OIL
      //INPUTS
      //OUTPUTS
          
    //RPM
    text += "RPM: ";
    text += rpm;
    text += "(";
    text += timeSpentAtPreviousRPMState_IN;
    text += ") W(";
    text += rpmWarning ? "X" : "-";
    text += ")\t";
          
    //SPEED
    text += "SPEED: ";
    text += kmh;
    text += "(";
    text += timeSpentAtPreviousSpeedState_IN;
    text += ")\t";
    
         
    //TEMPERATURE MOTEUR ET VENTILATION
      text += (float)(engineTemperature);
      text += "°C (";
      text += (float)(engineTemperatureSensorValue);
      text += " F/W: ";
      text += fanOn ? "X/" : "-/";
      text += engineTemperatureWarning ? "X" : "-";
      text += ")\t";

    //GEARBOX
    text += "Gear: ";
      text += (int) engagedGear;
      text += " (";
      text += gearBox1Value;
      text += gearBox2Value;
      text += gearBox3Value;
      text += ")\t";
      
    //FUEL
      text += (float)(fuelLevel);
      text += "% (";
      text += (float)(fuelLevelSensorValue);
      text += " W: ";
      text += fuelWarning ? "X" : "-";
      text += ")\t";
          
    //ALERTE
      //INPUTS
      //OUTPUTS
      text += "/!\\(";
      text += globalWarning ? "X)" : "-)";

      return text;
}
