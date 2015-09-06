/*
 Copyright (C) 2015 Dariusz Mazur <darekm@emadar.com>

 */
 
/**
 * @file me_frame.h
 *
 * Setup necessary to direct stdout to madar library
 */

#ifndef __MEATMEGA_H__
#define __MEATMEGA_H__
#include "Arduino.h"


// http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long internalVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

long internalTemp() {
  //  https://code.google.com/p/tinkerit/wiki/SecretThermometer
  long result;
  // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = (result - 125) * 1075;
  return result;
}


#endif // __MEATMEGA_H__
