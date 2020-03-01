#include "PCF8574.h"
/*
#define BLYNK_USE_DIRECT_CONNECT
#include <SoftwareSerial.h>
#define BLYNK_PRINT DebugSerial
#include <BlynkSimpleSerialBLE.h>

SoftwareSerial DebugSerial(0, 1); // RX, TX
char auth[] = "lsqhnFAbstIc_xXw6CN1VfxBQ2J8XYBx";
*/

//ANALOGS A0 -> A7 

int lightSensorPinIn          = A0; 
int temperatureSensorPinIn    = A1; //OK
int oilPressureSensorPinIn    = A2; //OK
int fuelSensorPinIn           = A3; //OK
// A4 + A5 -> SDA + SCL
int fuelIndicatorPinOut       = A6; //OK
int speedIdicatorPinOut       = A7; // TODO : ANALOG | DIGITAL ?


//DIGITALS 2 -> 13

int mUnitLightOutputPinIn = 2;
int rpmPinIn              = 3; // TODO trouver une entrée
int rpmPinOut             = 3; //OK
int fanPinOut             = 4; //OK
int speedPinIn_wheel      = 5; //OK TODO : Choisir
int speedPinIn_abs        = 5; //OK TODO : Choisir

int neutralPinOut         = 7;  //OK
int gear1PinOut           = 8; //OK
int gear2PinOut           = 9; //OK
int gear3PinOut           = 10; //OK           
int gear4PinOut           = 11; //OK
int gear5PinOut           = 12; //OK
int warningPinOut         = 13;

//( A4 : SDA / A5 : SCL)
PCF8574 pcf8574_1(0x20); //P0 -> P7

int lightSwitchPosition1PinIn = 0; //OK
int lightSwitchPosition2PinIn = 1; //OK
int ledRingPinOut             = 2;
int headlightPinOut           = 3; 
int gearBox1PinIn             = 4; //OK
int gearBox2PinIn             = 5; //OK
int gearBox3PinIn             = 6; //OK
//P7 AVAILABLE

float temperatureSensorValue = 0;  
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
  /*
  //DebugSerial.begin(9600);
  //DebugSerial.println("Waiting for connections...");
  //Serial.begin(9600);
  //Blynk.begin(Serial, auth);
  */

  //TODO : REVOIR TOUTES LES I/O
  
  //Entrées
  pinMode(temperatureSensorPinIn, INPUT);
  pinMode(oilPressureSensorPinIn, INPUT);
  pinMode(fuelSensorPinIn, INPUT);
  pinMode(gearBox1PinIn, INPUT);
  pinMode(gearBox2PinIn, INPUT);
  pinMode(gearBox3PinIn, INPUT);
  pinMode(mUnitLightOutputPinIn, INPUT);
  pinMode(lightSensorPinIn, INPUT);

  //Sorties
  pinMode(fanPinOut, OUTPUT);
  pinMode(warningPinOut, OUTPUT);
  pinMode(neutralPinOut, OUTPUT);
  pinMode(fuelIndicatorPinOut, OUTPUT);
  pinMode(gear1PinOut, OUTPUT);
  pinMode(gear2PinOut, OUTPUT);
  pinMode(gear3PinOut, OUTPUT);
  pinMode(gear4PinOut, OUTPUT);
  pinMode(gear5PinOut, OUTPUT);
  pinMode(ledRingPinOut, OUTPUT);
  pinMode(headlightPinOut, OUTPUT);

  pcf8574_1.pinMode(lightSwitchPosition1PinIn, INPUT);
  pcf8574_1.pinMode(lightSwitchPosition2PinIn, INPUT);
  pcf8574_1.pinMode(ledRingPinOut, OUTPUT);
  pcf8574_1.pinMode(headlightPinOut, OUTPUT);
  pcf8574_1.pinMode(gearBox1PinIn, INPUT);
  pcf8574_1.pinMode(gearBox2PinIn, INPUT);
  pcf8574_1.pinMode(gearBox3PinIn, INPUT);
  pcf8574_1.pinMode(P7, OUTPUT);

  pcf8574_1.begin();
 
  for (int i = 1; i <=13; i++) digitalWrite(i, LOW);
  
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

void loop() {
  //Blynk.run();
  
 /*********************************
  TIME MANAGEMENT
  *********************************/
  
  ///If code run over 50 days : Manage time overflow
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
  
  bool mUnitLightVal = digitalRead(mUnitLightOutputPinIn); 
  
  if(mUnitLightVal == HIGH) {
        // IN THIS CASE, M-UNIT ASK LIGHS TOBE OFF AND WILL TRIGGER HIGHBEAM
        //Blynk_mUnitLed.off();
        //Blynk_highBeamLed.on();
        
        //switch headlight OFF
        pcf8574_1.digitalWrite(headlightPinOut, LOW);
        //Blynk_headlightLed.off();
        
        //switch ledring OFF
        pcf8574_1.digitalWrite(ledRingPinOut, LOW);
        //Blynk_ledRingLed.off();
        
        
      } else {
        //High beam should be off according to M-Unit
        //Blynk_mUnitLed.on();
        //Blynk_highBeamLed.off();
        
        //Light swith value
        int lightSwitchPosition = 0;
        
        if (pcf8574_1.digitalRead(lightSwitchPosition1PinIn) == HIGH) {
          if (pcf8574_1.digitalRead(lightSwitchPosition2PinIn) == HIGH) {
            lightSwitchPosition = 2;
            
            } else {
              lightSwitchPosition = 1;
          }
        } else {
          lightSwitchPosition = 0;
        }

        //Light sensor value
        int lightSensorValue = analogRead(lightSensorPinIn);
        
        if(lightSwitchPosition == LIGHT_SWITH_POSITION_PARKING_LIGHT) {
            //Blynk.virtualWrite(V1, "PARKING_LIGHT");
            
            //switch headlight OFF
            pcf8574_1.digitalWrite(headlightPinOut, LOW);
            //Blynk_headlightLed.off();
            
            //switch ledring ON
            pcf8574_1.digitalWrite(ledRingPinOut, HIGH);
            //Blynk_ledRingLed.on();

         
          } else if(lightSwitchPosition == LIGHT_SWITH_POSITION_LOW_BEAM) {
            //Blynk.virtualWrite(V1, "LOW_BEAM");

            //switch headlight ON
            pcf8574_1.digitalWrite(headlightPinOut, HIGH);
            //Blynk_headlightLed.on();
            
            //switch ledring OFF
            pcf8574_1.digitalWrite(ledRingPinOut, LOW);
            //Blynk_ledRingLed.off();
         
          } else if(lightSwitchPosition == LIGHT_SWITH_POSITION_OFF) {
            //Blynk.virtualWrite(V1, "OFF");

            if(lightSensorValue >= LIGHT_SENSOR_THRESHOLD_MAX) {
              //switch headlight OFF
              pcf8574_1.digitalWrite(headlightPinOut, LOW);
              //Blynk_headlightLed.off();
              
              //switch ledring ON
              pcf8574_1.digitalWrite(ledRingPinOut, HIGH);
              //Blynk_ledRingLed.on();
              
            } else if(lightSensorValue <= LIGHT_SENSOR_THRESHOLD_MIN) {
              //switch headlight ON
              pcf8574_1.digitalWrite(headlightPinOut, HIGH);
              //Blynk_headlightLed.on();
              
              //switch ledring OFF
              pcf8574_1.digitalWrite(ledRingPinOut, LOW);
              //Blynk_ledRingLed.off();

            }    
          }
        }
  
  /**********************************
  OIL
  **********************************/
  
  float val = analogRead(oilPressureSensorPinIn);
  float oilPressure = mapf(val, 0, 1023,0,1); 
  oilPressureWarning = oilPressure <= OIL_PRESURE_THESHOLD_MIN ? true : false;
  

 /*********************************
  RPM & SHIFTLIGHT
  *********************************/  
  
  //Get tachymeter state
  currentRPMState = digitalRead(rpmPinIn);
  
  //if state changed since last check
  if(currentRPMState == previousRPMState) {
    timeSpentAtPreviousRPMState += currentMillis - previousMillis;
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
  currentRPMState = digitalRead(speedPinIn_wheel); //TODO : choisir avec ABS
  
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
  
  //Mesure de la valeur de la sonde température moteur
  
  temperatureSensorValue = analogRead(temperatureSensorPinIn);
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float temperatureSensorVoltage = mapf(temperatureSensorValue, 0, 1023,0,5);

  //Calcul de la résistance en fonction de la tension mesurée  
  float temperatureSensorResistance = mapf(temperatureSensorVoltage,0,5,3000,0);

  //Conversion de la résistance en température
  //TODO (utile ?) 
  // 103° > Ventilateur ON
  // 111° > Warning ON
   
  
  //Application des mesures nécessaire vis-à-vis de la résitance mesurés
  if(temperatureSensorResistance < VENTILATION_THRESHOLD_OHMS) fanOn = true;
  if(temperatureSensorResistance > VENTILATION_THRESHOLD_OHMS + VENTILATION_HYSTERESIS) fanOn = false;
  
  if(temperatureSensorResistance < WARNING_THRESHOLD_OHMS) temperatureWarning =  true;
  if(temperatureSensorResistance > WARNING_THRESHOLD_OHMS + WARNING_HYSTERESIS) temperatureWarning =  false;

  digitalWrite(fanPinOut, fanOn ? HIGH : LOW); 

  
  
  /**********************************
  GEARBOX
  **********************************/
  
  gearBox1Value = pcf8574_1.digitalRead(gearBox1PinIn);
  gearBox2Value = pcf8574_1.digitalRead(gearBox2PinIn);
  gearBox3Value = pcf8574_1.digitalRead(gearBox3PinIn);

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

  digitalWrite(neutralPinOut, gear == 0 ? HIGH : LOW);
  digitalWrite(gear1PinOut, gear == 1 ? HIGH : LOW);
  digitalWrite(gear2PinOut, gear == 2 ? HIGH : LOW);
  digitalWrite(gear3PinOut, gear == 3 ? HIGH : LOW);
  digitalWrite(gear4PinOut, gear == 4 ? HIGH : LOW);
  digitalWrite(gear5PinOut, gear == 5 ? HIGH : LOW);
  
  /**********************************
  FUEL
  **********************************/
  
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float fuelSensor = mapf(analogRead(fuelSensorPinIn), 0, 1023, 0, 100);

  if(fuelSensor <= FUEL_LEVEL_THERSHOLD_MIN) {
    analogWrite(fuelIndicatorPinOut, 255); 
  } else if(fuelSensor >= FUEL_LEVEL_THERSHOLD_MAX) {
    analogWrite(fuelIndicatorPinOut, 0); 
  }
  
  /**********************************
  ALERTE
  **********************************/
  
  globalWarning = oilPressureWarning || gearWarning || temperatureWarning || rpmWarning;

  digitalWrite(warningPinOut, globalWarning ? HIGH : LOW); 
  
  

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
}
