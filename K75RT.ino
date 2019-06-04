int debug = 1; //0 OFF, 1 SERIAL, 2 LED, 3 SERIAL + LED

int temperatureSensorPin  = A0;
int gearBox1Pin           = A5;
int gearBox2Pin           = A6;
int gearBox3Pin           = A7;

int fanPin                = 2;
int warningPin            = 3;
int neutralPin            = 7;
int gear1Pin              = 8;
int gear2Pin              = 9;
int gear3Pin              = 10;            
int gear4Pin              = 11;
int gear5Pin              = 12;
int ledPin                = 13;      // select the pin for the LED

float temperatureSensorValue = 0;  // variable to store the value coming from the sensor
float gearBox1Value = 0;
float gearBox2Value = 0;
float gearBox3Value = 0;

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
const float WARNING_THRESHOLD_OHMS = 143;

void setup() {
  //Entrées
  pinMode(temperatureSensorPin, INPUT);
  pinMode(gearBox1Pin, INPUT);
  pinMode(gearBox2Pin, INPUT);
  pinMode(gearBox3Pin, INPUT);

  //Sories
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(warningPin, OUTPUT);
  pinMode(neutralPin, OUTPUT);
  pinMode(gear1Pin, OUTPUT);
  pinMode(gear2Pin, OUTPUT);
  pinMode(gear3Pin, OUTPUT);
  pinMode(gear4Pin, OUTPUT);
  pinMode(gear5Pin, OUTPUT);

  for (int i = 1; i <=13; i++) digitalWrite(i, LOW);
  
  if (debug) Serial.begin(9600);
  
}

boolean analogToDigital(int value) {
       if (value < 1023*0.3) return LOW;
  else if (value > 1023*0.7) return HIGH;
  else return NULL;
}

float mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
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
  boolean fanOn = temperatureSensorResistance < VENTILATION_THRESHOLD_OHMS ? true : false;
  boolean warningOn = temperatureSensorResistance < WARNING_THRESHOLD_OHMS ? true : false;
  digitalWrite(fanPin, fanOn ? HIGH : LOW); 
  digitalWrite(warningPin, warningOn ? HIGH : LOW); 


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
  int gear;
       if(gearBox1Active == LOW   && gearBox2Active == LOW  && gearBox3Active == LOW)   gear = 0;
  else if(gearBox1Active == HIGH  && gearBox2Active == LOW  && gearBox3Active == LOW)   gear = 1;
  else if(gearBox1Active == LOW   && gearBox2Active == HIGH && gearBox3Active == LOW)   gear = 2;
  else if(gearBox1Active == HIGH  && gearBox2Active == HIGH && gearBox3Active == LOW)   gear = 3;
  else if(gearBox1Active == LOW   && gearBox2Active == LOW  && gearBox3Active == HIGH ) gear = 4;
  else if(gearBox1Active == HIGH  && gearBox2Active == LOW  && gearBox3Active == HIGH ) gear = 5;
  else if(gearBox1Active == HIGH  && gearBox2Active == HIGH && gearBox3Active == HIGH ) gear = 6;
  else { gear = -1; warningOn = true; }

  //Allumage en fonction de la vitesse choisie
  digitalWrite(neutralPin, gear == 0 ? HIGH : LOW);
  digitalWrite(gear1Pin, gear == 1 ? HIGH : LOW);
  digitalWrite(gear2Pin, gear == 2 ? HIGH : LOW);
  digitalWrite(gear3Pin, gear == 3 ? HIGH : LOW);
  digitalWrite(gear4Pin, gear == 4 ? HIGH : LOW);
  digitalWrite(gear5Pin, gear == 5 ? HIGH : LOW);
      
  if (debug == 1 || debug == 3 ) {

    String text = "";
    text += "TS: ";
    text += (float)(temperatureSensorValue);
    text += "\tVoltage: ";
    text += (float)(temperatureSensorVoltage);
    text += "\tΩ: ";
    text += (float)(temperatureSensorResistance);
    text += "\tFan/Warning: ";
    text += fanOn ? "X/" : "-/";
    text += warningOn ? "X" : "-";
    text += "\tGear: ";
    text += (int)(gear);
    text += "\t";
    text += (int)(gearBox1Value);
    text += "/";
    text += (int)(gearBox2Value);
    text += "/";
    text += (int)(gearBox3Value);

    Serial.println(text);  
    
  }
  if (debug == 2 || debug == 3) {
   
      for (int i = 0; i < gear; i++) {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(200);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(200);
        if (i == gear-1) delay(1000);
      }
  
      if (fanOn) {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);
      }
  
       if (warningOn) {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(3000);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(3000);
      }
       
      delay(2000); 
    }        
  
  // delay in between reads for stability
  delay(1); 
}
