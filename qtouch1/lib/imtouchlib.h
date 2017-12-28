/*************************************************** 
  This is a library for the QTouch

  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef imTouchlib_h
#define imTouchlib_h

 #include "Arduino.h"


class imTouch {
 public:
  imTouch();


  static const int TPINSS = 0; //define TPINSS 0 // this is currently only used as a supply of Vcc to charge the s&h cap

   void setup();
  int read(uint8_t pin, int ashift);
  uint16_t probe(uint8_t pin, uint8_t partner, bool dir) ;

};

#endif
