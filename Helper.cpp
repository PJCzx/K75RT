#include "Arduino.h"
#include "Helper.h"

using namespace std;

float mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Stopwatch::run() {
  ///If code run over 50 days : Manage time overflow
  if(currentMillis < previousMillis) {
    previousMillis = currentMillis;
  }
  
  //Backup before update
  previousMillis = currentMillis;
  
  //update
  currentMillis = millis();
}

unsigned long Stopwatch::timeSpentFromLastRun() {
  return millis() - previousMillis;
}

unsigned long int Stopwatch::currentSecond() {
  return currentMillis/1000;
}