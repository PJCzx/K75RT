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
BMW_K75RT k75 = BMW_K75RT();

unsigned long int lastDispalyedSecond = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
  /*
  //DebugSerial.begin(9600);
  //DebugSerial.println("Waiting for connections...");
  //Serial.begin(9600);
  //Blynk.begin(Serial, auth);
  */
  k75.setup();    
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
  k75.loopInit();           // PERF AFTER ACTIVATION : 23 327
  k75.updateLights();       // PERF AFTER ACTIVATION :  2 186
  k75.isOilPresureOK();     // PERF AFTER ACTIVATION :  1 699
  k75.updateRPM();          // PERF AFTER ACTIVATION :  1 680
  k75.updateSpeed();        // PERF AFTER ACTIVATION :  1 658
  k75.updateVentialtion();  // PERF AFTER ACTIVATION :  1 022
  k75.updateEngagedGear();  // PERF AFTER ACTIVATION :    555
  k75.updateFuelLevel();    // PERF AFTER ACTIVATION :    552
  k75.isGlobalWarning();    // PERF AFTER ACTIVATION :    468
  
  if(k75.stopwatch->currentSecond() > lastDispalyedSecond) {
    Serial.println(k75.toString());
    lastDispalyedSecond = k75.stopwatch->currentSecond();
    k75.faker->toggle();
    //Serial.println( k75.faker->toggle() ? "HIGH" : "LOW");
  }
}
