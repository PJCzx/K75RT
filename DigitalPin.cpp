#include "DigitalPin.h"

#include "PCF8574.h"

using namespace std;


DigitalPin::DigitalPin(int io_type, int pin): pin(pin) { };
DigitalPin::DigitalPin(int io_type, PCF8574* pcf8574, int pin): pcf8574(pcf8574), pin(pin) { };
        
void DigitalPin::setup(int setupValue) {
    Serial.println(ioType);
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
