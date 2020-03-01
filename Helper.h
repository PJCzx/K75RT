#ifndef __helper
#define __helper

#include "Arduino.h"

using namespace std;

float mapf(double val, double in_min, double in_max, double out_min, double out_max);

class Stopwatch {
  public:
    unsigned long int currentMillis = 0;
    unsigned long int previousMillis = 0;
    void run();
};

#endif
