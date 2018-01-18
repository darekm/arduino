/*************************************************** 
  This is a library for the QTouch

  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef imTouchlib_h
#define imTouchlib_h

#include "Arduino.h"
#include <avr/power.h>


class imTouch {
 public:
  imTouch();


  static const int TPINSS = 0; //define TPINSS 0 // this is currently only used as a supply of Vcc to charge the s&h cap

   void setup();
  int read(uint8_t pin, int aShift);
  int check(uint8_t pin);
  uint16_t probe(uint8_t pin, uint8_t partner, bool dir) ;
  int compute(int aValue, int aShift) ;

};

#endif
