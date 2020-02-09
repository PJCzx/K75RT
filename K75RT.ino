#include <AUnit.h>
using namespace aunit;

#include "BMW_K75.h"

int temperatureSensorPin      = A0;
int oilPressureSensorPin      = A1;
int fuelSensorPin             = A2;

int lightSwitchPosition1PinIn = A3;
int lightSwitchPosition2PinIn = A4;
int ledRingPinOut             = 0; // TO BE FOUND
int headlightPinOut           = 0; // TO BE FOUND
int lightSensorPinIn          = 0; // TO BE FOUND
int lightInPinIn              = 0; // TO BE FOUND

int gearBox1Pin               = A5;
int gearBox2Pin               = A6;
int gearBox3Pin               = A7;

int rpmPin                = 2;
int fanPin                = 3;
int speedPin              = 4;
int fuelIndicatorPin      = 5;

int neutralPin            = 7;
int gear1Pin              = 8;
int gear2Pin              = 9;
int gear3Pin              = 10;            
int gear4Pin              = 11;
int gear5Pin              = 12;
int warningPin            = 13;

float temperatureSensorValue = 0;  // variable to store the value coming from the sensor
float oilPresureSensorValue = 0;
float gearBox1Value = 0;
float gearBox2Value = 0;
float gearBox3Value = 0;
unsigned long int currentMillis;
unsigned long int previousMillis;

int serialDelay = 500;
int serialLastSent = 0;

unsigned long int timeSpentAtPreviousRPMState;
bool currentRPMState;
bool previousRPMState;
float rpm;

unsigned long int timeSpentAtPreviousSpeedState;
bool currentSpeedState;
bool previousSpeedState;
float kmh;

 //Constantes
  //Définition de seuils
   /*
   * POST #23 http://bmist.forumpro.fr/t100118-probleme-sonde-temperature-ldr-k75?highlight=sonde+temp%E9rature
   * "le ventilo se déclenchait vers 29,5 ce qui doit correspondre à 101°c valeur normale"
   */
 /* 
 * 103° > Ventilateur ON (175 OHMS)
 * 111° > Warning ON (143 OHMS)
 */
const float VENTILATION_THRESHOLD_OHMS = 175;
const float VENTILATION_HYSTERESIS = 1000;
const float WARNING_THRESHOLD_OHMS = 143;
const float WARNING_HYSTERESIS = 100;

const int SHIFT_LIGHT_THERSHOLD_MAX = 8600; //67HZ + 2 Segments
const int SHIFT_LIGHT_THERSHOLD_MIN = 8400; // LA ZONE SUR LE COMPTEUR BMX EST ENTRE 8500 ET 9000

const int FUEL_LEVEL_THERSHOLD_MAX = 15; //S'allume en dessous de 10%
const int FUEL_LEVEL_THERSHOLD_MIN = 10; //S'éteint au dessus de 15%

const int LIGHT_SWITH_POSITION_OFF = 0;
const int LIGHT_SWITH_POSITION_PARKING_LIGHT = 1;
const int LIGHT_SWITH_POSITION_LOW_BEAM = 2;
const float LIGHT_SENSOR_THRESHOLD_MIN = 0.33;
const float LIGHT_SENSOR_THRESHOLD_MAX = 0.66;
const float OIL_PRESURE_THESHOLD_MIN = 0.33;

boolean fanOn = false;

boolean globalWarning = false;
boolean oilPressureWarning = false;
boolean gearWarning = false;
boolean temperatureWarning = false;
boolean rpmWarning = false;

float mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

boolean analogToDigital(int value) {
       if (value < 1023*0.3) return LOW;
  else if (value > 1023*0.7) return HIGH;
  else return NULL;
}

void setup() {
  //Entrées
  pinMode(temperatureSensorPin, INPUT);
  pinMode(oilPressureSensorPin, INPUT);
  pinMode(fuelSensorPin, INPUT);
  pinMode(gearBox1Pin, INPUT);
  pinMode(gearBox2Pin, INPUT);
  pinMode(gearBox3Pin, INPUT);

  //Sorties
  pinMode(fanPin, OUTPUT);
  pinMode(warningPin, OUTPUT);
  pinMode(neutralPin, OUTPUT);
  pinMode(fuelIndicatorPin, OUTPUT);
  pinMode(gear1Pin, OUTPUT);
  pinMode(gear2Pin, OUTPUT);
  pinMode(gear3Pin, OUTPUT);
  pinMode(gear4Pin, OUTPUT);
  pinMode(gear5Pin, OUTPUT);


  for (int i = 1; i <=13; i++) digitalWrite(i, LOW);
  
  Serial.begin(9600);

  currentMillis = 0;
  previousMillis = 0;
  
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

void loop() {
  //aunit::TestRunner::run();
  
 /*********************************
  TIME MANAGEMENT
  *********************************/
  //If code run over 50 days : Manage time overflow
  if(currentMillis < previousMillis) {
    previousMillis = currentMillis;
  }
  
  //Backup before update
  previousMillis = currentMillis;
  
  //update
  currentMillis = millis();

  /**********************************
  LIGHTS
  **********************************/
  bool mUnitLightOutput = false; //TODO : get M-Unit light output
  if(mUnitLightOutput == false) {
        //TODO : switch headlight ON
        //TODO : switch ledring OFF
      } else {
        //Light swith & Light sensor value
        int lightSwitchPosition = 0; //TODO : get switch position
        int lightSensorValue = 0; //TODO : get lights sensor value
        
        if(lightSwitchPosition == LIGHT_SWITH_POSITION_PARKING_LIGHT) {
            //TODO : switch headlight OFF
            //TODO : switch ledring ON
         
          } else if(lightSwitchPosition == LIGHT_SWITH_POSITION_LOW_BEAM) {
            //TODO : switch headlight ON
            //TODO : switch ledring OFF
         
          } else if(lightSwitchPosition == LIGHT_SWITH_POSITION_OFF) {
            if(lightSensorValue >= LIGHT_SENSOR_THRESHOLD_MAX) {
            //TODO : switch headlight OFF
            //TODO : switch ledring ON
              
            } else if(lightSensorValue <= LIGHT_SENSOR_THRESHOLD_MIN) {
            //TODO : switch headlight ON
            //TODO : switch ledring OFF

            }    
          } else {
            //ERROR CASE
            //TODO : switch headlight ON
            //TODO : switch ledring ON
 
          } 
        }

  /**********************************
  OIL
  **********************************/
  float val = analogRead(oilPressureSensorPin);
  float oilPressure = mapf(val, 0, 1023,0,1); 
  oilPressureWarning = oilPressure <= OIL_PRESURE_THESHOLD_MIN ? true: false;

 /*********************************
  RPM & SHIFTLIGHT
  *********************************/  
  //Get tachymeter state
  currentRPMState = digitalRead(rpmPin);
  
  //if state changed since last check
  if(currentRPMState == previousRPMState) {
    timeSpentAtPreviousRPMState += currentMillis - previousMillis;
  } else {
    previousRPMState = currentRPMState;
    int segments = 2;
    float secondesParSegments = (float)timeSpentAtPreviousRPMState*2/1000;
    rpm = 60.0/secondesParSegments*segments;
    if(rpm >= SHIFT_LIGHT_THERSHOLD_MAX){
      rpmWarning = true;
    } else if(rpm <= SHIFT_LIGHT_THERSHOLD_MIN){
      rpmWarning = false;
    }
    timeSpentAtPreviousRPMState = 0;
  }

  /**********************************
  SPEED
  **********************************/
  //Get speed state
  currentRPMState = digitalRead(speedPin);
  
  //if state changed since last check
  if(currentSpeedState == previousSpeedState) {
    timeSpentAtPreviousSpeedState += (currentMillis - previousMillis);
  } else {
    previousSpeedState = currentSpeedState;

    //x2 parce qu'on mesure qu'un seul etat
    float millisecondesParSegments = (float)timeSpentAtPreviousSpeedState*2/1000;
    
    //PREFERE NOMBRE DE SEGMENT POUR 1 tour de roue

    int circonferenceMM = 1000;
    int segments = 1;

    float distancePourUnSegment = circonferenceMM/segments;
    float vitesseEnMMparMilisec = distancePourUnSegment/millisecondesParSegments;
    /*
    vitesseEnMMparMilisec/1000 > Metre
    vitesseEnMMparMilisec/1000000 > vitesseEnKMparMilisec
    vitesseEnKMparMilisec*1000*60*60 > Heure
    */
    kmh = vitesseEnMMparMilisec/1000/1000*1000*60*60;
    

    timeSpentAtPreviousSpeedState = 0;
  }
  
  
  /*********************************
  TEMPERATURE MOTEUR ET VENTILATION
  *********************************/

  //Mesure de la valeur de la sonde température moteur
  
  temperatureSensorValue = analogRead(temperatureSensorPin);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float temperatureSensorVoltage = mapf(temperatureSensorValue, 0, 1023,0,5);

  //Calcul de la résistance en fonction de la tension mesurée  
  float temperatureSensorResistance = mapf(temperatureSensorVoltage,0,5,3000,0);// minOhms + ((maxOhms-minOhms) - (temperatureSensorVoltage/5)*(maxOhms-minOhms)) ; //TODO

  //Conversion de la résistance en température
  //TODO (utile ?)
   /* 
   * 103° > Ventilateur ON
   * 111° > Warning ON
   */
  
  //Application des mesures nécessaire vis-à-vis de la résitance mesurés
  if(temperatureSensorResistance < VENTILATION_THRESHOLD_OHMS) fanOn = true;
  if(temperatureSensorResistance > VENTILATION_THRESHOLD_OHMS + VENTILATION_HYSTERESIS) fanOn = false;
  
  if(temperatureSensorResistance < WARNING_THRESHOLD_OHMS) temperatureWarning =  true;
  if(temperatureSensorResistance > WARNING_THRESHOLD_OHMS + WARNING_HYSTERESIS) temperatureWarning =  false;

  digitalWrite(fanPin, fanOn ? HIGH : LOW); 

  
  /**********************************
  GEARBOX
  **********************************/
  gearBox1Value = analogRead(gearBox1Pin);
  gearBox2Value = analogRead(gearBox2Pin);
  gearBox3Value = analogRead(gearBox3Pin);

  //mesure des valeurs des cables de sortie de boite
  boolean gearBox1Active = analogToDigital(gearBox1Value);
  boolean gearBox2Active = analogToDigital(gearBox2Value);
  boolean gearBox3Active = analogToDigital(gearBox3Value);

  //conversion en numéro de vitesse
  gearWarning = false;
  int gear;
       if(gearBox1Active == LOW   && gearBox2Active == LOW  && gearBox3Active == LOW)   gear = 0;
  else if(gearBox1Active == HIGH  && gearBox2Active == LOW  && gearBox3Active == LOW)   gear = 1;
  else if(gearBox1Active == LOW   && gearBox2Active == HIGH && gearBox3Active == LOW)   gear = 2;
  else if(gearBox1Active == HIGH  && gearBox2Active == HIGH && gearBox3Active == LOW)   gear = 3;
  else if(gearBox1Active == LOW   && gearBox2Active == LOW  && gearBox3Active == HIGH ) gear = 4;
  else if(gearBox1Active == HIGH  && gearBox2Active == LOW  && gearBox3Active == HIGH ) gear = 5;
  else if(gearBox1Active == HIGH  && gearBox2Active == HIGH && gearBox3Active == HIGH ) gear = 6;
  else { gear = -1; gearWarning = true; }

  digitalWrite(neutralPin, gear == 0 ? HIGH : LOW);
  digitalWrite(gear1Pin, gear == 1 ? HIGH : LOW);
  digitalWrite(gear2Pin, gear == 2 ? HIGH : LOW);
  digitalWrite(gear3Pin, gear == 3 ? HIGH : LOW);
  digitalWrite(gear4Pin, gear == 4 ? HIGH : LOW);
  digitalWrite(gear5Pin, gear == 5 ? HIGH : LOW);

  /**********************************
  FUEL
  **********************************/
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float fuelSensor = mapf(analogRead(fuelSensorPin), 0, 1023, 0, 100);

  if(fuelSensor <= FUEL_LEVEL_THERSHOLD_MIN) {
    digitalWrite(fuelIndicatorPin, HIGH); 
  } else if(fuelSensor >= FUEL_LEVEL_THERSHOLD_MAX) {
    digitalWrite(fuelIndicatorPin, LOW); 
  }

  /**********************************
  ALERTE
  **********************************/

  globalWarning = oilPressureWarning || gearWarning || temperatureWarning || rpmWarning;

  digitalWrite(warningPin, globalWarning ? HIGH : LOW); 

  

  /*********************************************
  SERIAL WRITE
  *********************************************/
  if (abs(currentMillis - serialLastSent) >= serialDelay) {
    serialLastSent = currentMillis;

     String text = "";

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
      text += "\tVoltage: ";
      text += (float)(temperatureSensorVoltage);
      text += "\tΩ: ";
      text += (float)(temperatureSensorResistance);
      
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
      text += globalWarning ? "X" : "-";
    
   


    Serial.begin(9600);
    Serial.println(text);  
    Serial.end();
  }
    
  // delay in between reads for stability
  delay(1); 
}
