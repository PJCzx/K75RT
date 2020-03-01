#include "Helper.h"
#include "PCF8574.h"
#include "DigitalPin.h"

using namespace std;

DigitalPin::DigitalPin(int ioType, int pin): ioType(ioType), pin(pin) { };
DigitalPin::DigitalPin(int ioType, PCF8574* pcf8574, int pin): ioType(ioType), pcf8574(pcf8574), pin(pin) { };
        
void DigitalPin::setup(int setupValue) {
    if (pcf8574 == NULL) pinMode(pin, setupValue != -1 ? setupValue : ioType);
    else pcf8574->pinMode(pin, setupValue != -1 ? setupValue : ioType);
}

bool DigitalPin::state() {
  if (pcf8574 == NULL) return digitalRead(pin);
  else return pcf8574->digitalRead(pin);
}

void DigitalPin::set(bool value) {
  if(pcf8574) {
    pcf8574->digitalWrite(pin, value);
  } else {
    digitalWrite(pin, value);
  }
} 

void DigitalPin::high() {
  set(HIGH); 
}

void DigitalPin::low() {
  set(LOW);
}


AnalogicPin::AnalogicPin(int ioType, int pin): ioType(ioType), pin(pin) { };

void AnalogicPin::setup(int setupValue) {
    pinMode(pin, setupValue != -1 ? setupValue : ioType);
}

float AnalogicPin::value() {
  return mapf(analogRead(pin), 0, 1023, 0.0, 1.0);
}

void AnalogicPin::set(float value) {
  analogWrite(pin, mapf(value, 0.0, 1.0, 0, 255));
}
