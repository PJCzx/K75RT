#include "Arduino.h"
#include "PCF8574.h" // https://github.com/xreef/PCF8574_library
#include "AdvancedPin.h"
#include "BMW_K75RT.h"
#include "string.h"
/*
#define BLYNK_USE_DIRECT_CONNECT
#include <SoftwareSerial.h>
#define BLYNK_PRINT DebugSerial
#include <BlynkSimpleSerialBLE.h>

SoftwareSerial DebugSerial(0, 1); // RX, TX
char auth[] = "lsqhnFAbstIc_xXw6CN1VfxBQ2J8XYBx";
*/
Stopwatch stopwatch = Stopwatch();
BMW_K75RT k75 = BMW_K75RT();

float temperatureSensorValue = 0;  
float oilPresureSensorValue = 0;
float gearBox1Value = 0;
float gearBox2Value = 0;
float gearBox3Value = 0;
unsigned int serialDelay = 500;
int serialLastSent = 0;
unsigned long int timeSpentAtPreviousRPMState;
bool currentRPMState;
bool previousRPMState;
float rpm;
unsigned long int timeSpentAtPreviousSpeedState;
bool currentSpeedState;
bool previousSpeedState;
float kmh;
boolean fanOn = false;
boolean gearWarning = false;
boolean temperatureWarning = false;
boolean rpmWarning = false;
unsigned int executionPerSeconds = 0;
unsigned int secondsElapsed = 0;

 /*
 * POST #23 http://bmist.forumpro.fr/t100118-probleme-sonde-temperature-ldr-k75?highlight=sonde+temp%E9rature
 * "le ventilo se déclenchait vers 29,5 ce qui doit correspondre à 101°c valeur normale"
 */
const float TEMPERATURE_VENTILATION_THRESHOLD = 103;  //TODO : FIND MATCHING VALUES //103° = 175 OHMS
const float TEMPERATURE_VENTILATION_HYSTERESIS = 5; //5° TODO : Verifier si cela nous convient
const float TEMPERATURE_WARNING_THRESHOLD = 111;  //TODO : FIND MATCHING VALUES // 111° = 143 OHMS
const float TEMPERATURE_WARNING_HYSTERESIS = 5; //TODO : Verifier si cela nous convient

const int SHIFT_LIGHT_THERSHOLD_MAX = 8600; //67HZ + 2 Segments
const int SHIFT_LIGHT_THERSHOLD_MIN = 8400; // LA ZONE SUR LE COMPTEUR BMX EST ENTRE 8500 ET 9000

const int FUEL_LEVEL_THERSHOLD_MAX = 0.15; //S'allume en dessous de 10%
const int FUEL_LEVEL_THERSHOLD_MIN = 0.10; //S'éteint au dessus de 15%
const int FUEL_LEVEL_HIGH = 1.0; //S'éteint au dessus de 15%
const int FUEL_LEVEL_LOW = 0.0; //S'éteint au dessus de 15%

const int LIGHT_SWITCH_POSITION_OFF = 0;
const int LIGHT_SWITCH_POSITION_PARKING_LIGHT = 1;
const int LIGHT_SWITCH_POSITION_LOW_BEAM = 2;
const float LIGHT_SENSOR_THRESHOLD_MIN = 0.33;
const float LIGHT_SENSOR_THRESHOLD_MAX = 0.66;
//const float OIL_PRESURE_THESHOLD_MIN = 0.33;

void setup() {
  secondsElapsed = 0;
  Serial.begin(115200);
  Serial.println("Setup");
  /*
  //DebugSerial.begin(9600);
  //DebugSerial.println("Waiting for connections...");
  //Serial.begin(9600);
  //Blynk.begin(Serial, auth);
  */
  k75.setup();
    
  for (int i = 1; i <=13; i++) digitalWrite(i, LOW);
    
  currentRPMState = LOW;
  previousRPMState = LOW;
  timeSpentAtPreviousRPMState = 0;
  rpm = 0;
  rpmWarning = false;

  currentSpeedState = LOW;
  previousSpeedState = LOW;
  timeSpentAtPreviousSpeedState = 0;
  kmh = 0.0;
}


/*
int readCount = 0;
BLYNK_READ(V31)
{
  Blynk.virtualWrite(V31, readCount++); 
}
BLYNK_READ(V30)
{
  Blynk.virtualWrite(V30, "N"); 
}
int aValue = 0;
BLYNK_WRITE(V0) {
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V1 Slider value is: ");
  Serial.println(pinValue);
  aValue = pinValue;
  analogWrite(1, aValue);
  int input0 = analogRead(0);
  Blynk.virtualWrite(V5, input0); 
}

BLYNK_READ(V5)
{
  int input0 = analogRead(0);
  Blynk.virtualWrite(V5, input0); 
}
*/

void loop(){
  k75.loopInit();

  if(millis()/1000 > secondsElapsed) {
      secondsElapsed = millis()/1000;
      Serial.print("executionPerSeconds : ");
      Serial.println(executionPerSeconds);
      k75.sayHello();
      k75.mUnitLightOutputPinIn.sayHello();
      executionPerSeconds = 0;
    }
    executionPerSeconds++;
    stopwatch.run();
  }

void ORIGINAL_loop() {
  
  k75.loopInit();

  //TODO : Utiliser le stopwatch ?
  
  if(millis()/1000 > secondsElapsed) {
    secondsElapsed = millis()/1000;
    Serial.print("executionPerSeconds : ");
    Serial.println(executionPerSeconds);
    executionPerSeconds = 0;
  }
  executionPerSeconds++;
   
  //TODO : Etudier s'il est intelligent de regrouper toute les rectures de PIN en 1 seule fois pour mettre toutes les variables à jours et éxécuter le code ensuite ?
  
  //Blynk.run();
  
 /*********************************
  TIME MANAGEMENT
  *********************************/
  stopwatch.run();
  
  /**********************************
  LIGHTS
  **********************************/
  
  bool mUnitLightVal = k75.mUnitLightOutputPinIn.state(); 
  
  if(mUnitLightVal == HIGH) {
        // IN THIS CASE, M-UNIT ASK LIGHS TOBE OFF AND WILL TRIGGER HIGHBEAM
        //Blynk_mUnitLed.off();
        //Blynk_highBeamLed.on();
        
        //switch headlight OFF
        k75.headlightPinOut->low();
        //Blynk_headlightLed.off();
        
        //switch ledring OFF
        k75.ledRingPinOut->low();
        //Blynk_ledRingLed.off();
        
        
      } else {
        //High beam should be off according to M-Unit
        //Blynk_mUnitLed.on();
        //Blynk_highBeamLed.off();
        
        //Light SWITCH value
        int lightSwitchPosition = 0;
        
        if (k75.lightSwitchPosition1PinIn->state() == HIGH) {
          if (k75.lightSwitchPosition2PinIn->state() == HIGH) {
            lightSwitchPosition = 2;
            
            } else {
              lightSwitchPosition = 1;
          }
        } else {
          lightSwitchPosition = 0;
        }

        //Light sensor value
        float lightSensorValue = k75.lightSensorPinIn->value();
        
        if(lightSwitchPosition == LIGHT_SWITCH_POSITION_PARKING_LIGHT) {
            //Blynk.virtualWrite(V1, "PARKING_LIGHT");
            
            //switch headlight OFF
            k75.headlightPinOut->low();
            //Blynk_headlightLed.off();
            
            //switch ledring ON
            k75.ledRingPinOut->high();
            //Blynk_ledRingLed.on();

         
          } else if(lightSwitchPosition == LIGHT_SWITCH_POSITION_LOW_BEAM) {
            //Blynk.virtualWrite(V1, "LOW_BEAM");

            //switch headlight ON
            k75.headlightPinOut->high();
            //Blynk_headlightLed.on();
            
            //switch ledring OFF
            k75.ledRingPinOut->low();
            //Blynk_ledRingLed.off();
         
          } else if(lightSwitchPosition == LIGHT_SWITCH_POSITION_OFF) {
            //Blynk.virtualWrite(V1, "OFF");

            if(lightSensorValue >= LIGHT_SENSOR_THRESHOLD_MAX) {
              //switch headlight OFF
              k75.headlightPinOut->low();
              //Blynk_headlightLed.off();
              
              //switch ledring ON
              k75.ledRingPinOut->high();
              //Blynk_ledRingLed.on();
              
            } else if(lightSensorValue <= LIGHT_SENSOR_THRESHOLD_MIN) {
              //switch headlight ON
              k75.headlightPinOut->high();
              //Blynk_headlightLed.on();
              
              //switch ledring OFF
              k75.ledRingPinOut->low();
              //Blynk_ledRingLed.off();

            }    
          }
        }
  
  /**********************************
  OIL
  **********************************/
  k75.isOilPresureOK();

 /*********************************
  RPM & SHIFTLIGHT
  *********************************/  
  
  //Get tachymeter state
  currentRPMState = k75.rpmPinIn->state();
  
  //if state changed since last check
  if(currentRPMState == previousRPMState) {
    timeSpentAtPreviousRPMState += stopwatch.currentMillis - stopwatch.previousMillis;
  } else {
    previousRPMState = currentRPMState;
    int segments = 2;
    float secondesParSegments = (float)timeSpentAtPreviousRPMState*2/1000;
    rpm = 60.0/secondesParSegments*segments;
    if(rpm >= SHIFT_LIGHT_THERSHOLD_MAX){ //TODO : Define another threshold ?
      rpmWarning = true;
    } else if(rpm <= SHIFT_LIGHT_THERSHOLD_MIN){
      rpmWarning = false;
    }
    timeSpentAtPreviousRPMState = 0;
  }

  //TODO : Ecrire le pulse sur rpmPinOut
  
  /**********************************
  SPEED
  **********************************/
  
  //Get speed state
  currentRPMState = k75.speedPinIn_wheel->state(); //TODO : choisir avec ABS
  
  //if state changed since last check
  if(currentSpeedState == previousSpeedState) {
    timeSpentAtPreviousSpeedState += (stopwatch.currentMillis - stopwatch.previousMillis);
  } else {
    previousSpeedState = currentSpeedState;

    //x2 parce qu'on mesure qu'un seul etat
    float millisecondesParSegments = (float)timeSpentAtPreviousSpeedState*2/1000;
    
    //PREFERE NOMBRE DE SEGMENT POUR 1 tour de roue

    int circonferenceMM = 1000;
    int segments = 1;

    float distancePourUnSegment = circonferenceMM/segments;
    float vitesseEnMMparMilisec = distancePourUnSegment/millisecondesParSegments;
    
    //vitesseEnMMparMilisec/1000 > Metre
    //vitesseEnMMparMilisec/1000000 > vitesseEnKMparMilisec
    //vitesseEnKMparMilisec*1000*60*60 > Heure
    
    kmh = vitesseEnMMparMilisec/1000/1000*1000*60*60;

    //TODO : Write pulse on speedIdicatorPinOut

    timeSpentAtPreviousSpeedState = 0;
  }
  
  
  /*********************************
  TEMPERATURE MOTEUR ET VENTILATION
  *********************************/
   
  temperatureSensorValue = k75.temperatureSensorPinIn->value();
  
  float temperature = mapf(temperatureSensorValue,0.0,1.0,0.0,1.0);

  if(temperature > TEMPERATURE_VENTILATION_THRESHOLD) fanOn = true;
  if(temperature < TEMPERATURE_VENTILATION_THRESHOLD - TEMPERATURE_VENTILATION_HYSTERESIS) fanOn = false;
  
  if(temperature > TEMPERATURE_WARNING_THRESHOLD) temperatureWarning =  true;
  if(temperature < TEMPERATURE_WARNING_THRESHOLD - TEMPERATURE_WARNING_HYSTERESIS) temperatureWarning =  false;

  k75.fanPinOut->set(fanOn ? HIGH : LOW); 

  
  
  /**********************************
  GEARBOX
  **********************************/
  
  gearBox1Value = k75.gearBox1PinIn->state();
  gearBox2Value = k75.gearBox2PinIn->state();
  gearBox3Value = k75.gearBox3PinIn->state();

  //conversion en numéro de vitesse
  gearWarning = false;
  int gear;
       if(gearBox1Value == LOW   && gearBox2Value == LOW  && gearBox3Value == LOW)   gear = 0;
  else if(gearBox1Value == HIGH  && gearBox2Value == LOW  && gearBox3Value == LOW)   gear = 1;
  else if(gearBox1Value == LOW   && gearBox2Value == HIGH && gearBox3Value == LOW)   gear = 2;
  else if(gearBox1Value == HIGH  && gearBox2Value == HIGH && gearBox3Value == LOW)   gear = 3;
  else if(gearBox1Value == LOW   && gearBox2Value == LOW  && gearBox3Value == HIGH ) gear = 4;
  else if(gearBox1Value == HIGH  && gearBox2Value == LOW  && gearBox3Value == HIGH ) gear = 5;
  else if(gearBox1Value == HIGH  && gearBox2Value == HIGH && gearBox3Value == HIGH ) gear = 6;
  else { gear = -1; gearWarning = true; }

  k75.neutralPinOut->set(gear == 0 ? HIGH : LOW);
  k75.gear1PinOut->set(gear == 1 ? HIGH : LOW);
  k75.gear2PinOut->set(gear == 2 ? HIGH : LOW);
  k75.gear3PinOut->set(gear == 3 ? HIGH : LOW);
  k75.gear4PinOut->set(gear == 4 ? HIGH : LOW);
  k75.gear5PinOut->set(gear == 5 ? HIGH : LOW);
  
  /**********************************
  FUEL
  **********************************/
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float fuelSensor = k75.fuelSensorPinIn->value();

  if(fuelSensor <= FUEL_LEVEL_THERSHOLD_MIN) {
    k75.fuelIndicatorPinOut->set(FUEL_LEVEL_HIGH); 
  } else if(fuelSensor >= FUEL_LEVEL_THERSHOLD_MAX) {
    k75.fuelIndicatorPinOut->set(FUEL_LEVEL_LOW); 
  }
  
  /**********************************
  ALERTE
  **********************************/
  
  k75.globalWarning = k75.oilPressureWarning || gearWarning || temperatureWarning || rpmWarning;

  k75.warningPinOut->set(k75.globalWarning ? HIGH : LOW); 
  
  

  /*********************************************
  SERIAL WRITE
  *********************************************/
  
   
  if (abs(stopwatch.currentMillis - serialLastSent) >= serialDelay) {
    serialLastSent = stopwatch.currentMillis;

     String text = "";
    text+= "executionPerSeconds : ";
    text += executionPerSeconds;
    //LIGHTS
      //INPUTS
      //OUTPUTS
    
    //OIL
      //INPUTS
      //OUTPUTS
          
    //RPM & SHIFTLIGHT
      //INPUTS
      //OUTPUTS
          
    //SPEED
       //INPUTS
       //OUTPUTS
         
    //TEMPERATURE MOTEUR ET VENTILATION
    text += "/tTS:/t";
      //INPUTS
      text += (float)(temperatureSensorValue);

      text += "\tΩ: ";
      text += (float)(temperature);
      
      //OUTPUTS
      text += "\tState/Warning: ";
      text += fanOn ? "X/" : "-/";
      text += temperatureWarning ? "X" : "-";
      
    //GEARBOX
    text += "\tGear:\t";
      //INPUTS
      text += (int)(gearBox1Value);
      text += " ";
      text += (int)(gearBox2Value);
      text += " ";
      text += (int)(gearBox3Value);
      text += "\t";
      
      //OUTPUTS
      text += (int)(gear);
    
          
    //FUEL
      //INPUTS
      //OUTPUTS
          
    //ALERTE
      //INPUTS
      //OUTPUTS
      text += k75.globalWarning ? "X" : "-";
    
   


    //Serial.begin(9600);
    //Serial.println(text);  
    //Serial.end();
  }
}
