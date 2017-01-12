// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifndef MAX6675_h
#define MAX6675_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MAX6675 {
 public:
  MAX6675(int8_t SCLK, int8_t CS, int8_t MISO);

  uint16_t read(void);
  private:
  int8_t _mxx;
  void delay_mx(int8_t x);
  int8_t sclk, miso, cs;
  uint8_t spiread(void);
};

#endif
