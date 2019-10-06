#ifndef _BMW_K75_
#define _BMW_K75_

#include <AUnit.h>
using namespace aunit;

class BMWK75 {
public:
  BMWK75 (int val){}

    bool getVal(){
      return val = !val;    
    }
private:
    bool val = true;   
};



BMWK75 tested (1);

test(first_attemp__) {
  assertEqual(tested.getVal(), false);
}

test(second_attempt__) {
  assertEqual(tested.getVal(), true);
}
#endif
