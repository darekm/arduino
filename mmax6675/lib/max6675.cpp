// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

//#ifdef __AVR
//  #include <avr/pgmspace.h>
//#elif defined(ESP8266)
//  #include <pgmspace.h>
//#endif
//#include <util/delay.h>
//#include <stdlib.h>
#include "max6675.h"

MAX6675::MAX6675(int8_t SCLK, int8_t CS, int8_t MISO) {
  sclk = SCLK;
  cs = CS;
  miso = MISO;

  //define pin modes
  pinMode(cs, OUTPUT);
  pinMode(sclk, OUTPUT); 
  pinMode(miso, INPUT);

  digitalWrite(cs, HIGH);
}

void MAX6675::delay_mx(int8_t x){
	x*=3;
	_mxx+=x;
	
}
uint16_t MAX6675::read(void) {

  uint16_t v;

  digitalWrite(cs, LOW);
  delay_mx(1);

  v = spiread();
  v <<= 8;
  v |= spiread();

  digitalWrite(cs, HIGH);
  return v; 
 // if (v & 0x4) {
    // uh oh, no thermocouple attached!
 //   return NAN; 
    //return -100;
  //}

 // v >>= 3;

 // return v*0.25;
}


byte MAX6675::spiread(void) { 
  int i;
  byte d = 0;

  for (i=7; i>=0; i--)
  {
    digitalWrite(sclk, LOW);
    delay_mx(1);
    if (digitalRead(miso)) {
      //set the bit to 0 no matter what
      d |= (1 << i);
    }

    digitalWrite(sclk, HIGH);
    delay_mx(1);
  }

  return d;
}
