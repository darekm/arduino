/*************************************************** 
  This is a library for the touch library

  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include "imtouchlib.h"
#define CHARGE_DELAY  5 // time it takes for the capacitor to get charged/discharged in microseconds
#define TRANSFER_DELAY  5 // time it takes for the capacitors to exchange charge
#define TOUCH_VALUE_BASELINE -65 // this is the value my setup measures when the probe is not touched. For your setup this might be different. In order for the LED to fade correctly, you will have to adjust this value
#define TOUCH_VALUE_SCALE 4 // this is also used for the LED fading. The value should be chosen such that the value measured when the probe is fully touched minus TOUCH_VALUE_BASELINE is scaled to 31, e.g. untouched_val= 333, touched_val= 488, difference= 155, divide by 5 to get 31.

// ADC constants
#define ADMUX_MASK  0b00001111 // mask the mux bits in the ADMUX register
#define MUX_GND 0b00001111 // mux value for connecting the ADC unit internally to GND
#define MUX_REF_VCC 0b01000000 // value to set the ADC reference to Vcc

imTouch::imTouch() {
}


void imTouch::setup() {
  // prepare the ADC unit for one-shot measurements
  // see the atmega328 datasheet for explanations of the registers and values
  ADMUX = 0b01000000; // Vcc as voltage reference (bits76), right adjustment (bit5), use ADC0 as input (bits3210)
  ADCSRA = 0b11000100; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
  ADCSRB = 0b00000000; // autotrigger source free running (bits210) doesn't apply
  while(ADCSRA & (1<<ADSC)){  } // wait for first conversion to complete
}

uint16_t imTouch::probe(uint8_t pin, uint8_t partner, bool dir) {
  uint8_t mask= _BV(pin) | _BV(partner);

  DDRC|= mask; // config pins as push-pull output
  if(dir)
    PORTC= (PORTC & ~_BV(pin)) | _BV(partner); // set partner high to charge the  s&h cap and pin low to discharge touch probe cap
  else
    PORTC= (PORTC & ~_BV(partner)) | _BV(pin); // set pin high to charge the touch probe and pin low to discharge s&h cap cap

  // charge/discharge s&h cap by connecting it to partner
  ADMUX = MUX_REF_VCC | partner; // select partner as input to the ADC unit

  delayMicroseconds(CHARGE_DELAY); // wait for the touch probe and the s&h cap to be fully charged/dsicharged

  DDRC&= ~mask; // config pins as input
  PORTC&= ~mask; // disable the internal pullup to make the ports high Z

  // connect touch probe cap to s&h cap to transfer the charge
  ADMUX= MUX_REF_VCC | pin; // select pin as ADC input

  delayMicroseconds(TRANSFER_DELAY); // wait for charge to be transfered

  // measure
  ADCSRA|= (1<<ADSC); // start measurement
  while(ADCSRA & (1<<ADSC)){  } // wait for conversion to complete
  return ADC; // return conversion result
}


int imTouch::read(uint8_t pin, int ashift){
  unsigned int adc1, adc2; // store the avarage of the charge resp. discharge measurement
  adc1= 0; // clear the averaging variables for the next run
  adc2= 0;
  for (int i=0; i<4; i++) {
    // first measurement: charge touch probe, discharge ADC s&h cap, connect the two, measure the volage
    adc1+= probe(pin, TPINSS, false); // accumulate the results for the averaging

    // second measurement:discharge touch probe, charge ADC s&h cap, connect the two, measure the volage
   adc2+= probe(pin, TPINSS, true); // accumulate the results for the averaging
  }
  // 4 measurements are taken and averaged to improve noise immunity
 // adc1>>=2; // divide the accumulated measurements by 16
//  adc2>>=2;
    int16_t add= (adc1-adc2); // the value of adc1 (probe charged) gets higher when the probe ist touched, the value of adc2 (s&h charged) gets lower when the probe ist touched, so, it has to be be subtracted to amplify the detection accuracy
    int16_t idx= (add-TOUCH_VALUE_BASELINE-ashift); // offset probe_val by value of untouched probe
    if(idx<0) idx= 0; // limit the index!!!
    idx/= TOUCH_VALUE_SCALE; // scale the index
    return idx;
}




/*********************************************************************/
