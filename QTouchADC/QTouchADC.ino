unsigned int adc1, adc2; // store the avarage of the charge resp. discharge measurement
int ref2,ref1;
int probe_val; // store the resulting touch measurement

// ADC constants
#define ADMUX_MASK  0b00001111 // mask the mux bits in the ADMUX register
#define MUX_GND 0b00001111 // mux value for connecting the ADC unit internally to GND
#define MUX_REF_VCC 0b01000000 // value to set the ADC reference to Vcc

const uint16_t ledFadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255}; // this is an exponential series to model the perception of the LED brightness by the human eye

#define TPIN1 1
#define TPIN2 0 // this is currently only used as a supply of Vcc to charge the s&h cap

#define CHARGE_DELAY  5 // time it takes for the capacitor to get charged/discharged in microseconds
#define TRANSFER_DELAY  5 // time it takes for the capacitors to exchange charge
#define TOUCH_VALUE_BASELINE -65 // this is the value my setup measures when the probe is not touched. For your setup this might be different. In order for the LED to fade correctly, you will have to adjust this value
#define TOUCH_VALUE_SCALE 3 // this is also used for the LED fading. The value should be chosen such that the value measured when the probe is fully touched minus TOUCH_VALUE_BASELINE is scaled to 31, e.g. untouched_val= 333, touched_val= 488, difference= 155, divide by 5 to get 31.

void touch_setup() {
  // prepare the ADC unit for one-shot measurements
  // see the atmega328 datasheet for explanations of the registers and values
  ADMUX = 0b01000000; // Vcc as voltage reference (bits76), right adjustment (bit5), use ADC0 as input (bits3210)
  ADCSRA = 0b11000100; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
  ADCSRB = 0b00000000; // autotrigger source free running (bits210) doesn't apply
  while(ADCSRA & (1<<ADSC)){  } // wait for first conversion to complete  
}

uint16_t touch_probe(uint8_t pin, uint8_t partner, bool dir) {
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

int ADCTouchRead(byte ADCChannel, int samples)
{
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	uint8_t digitalPin = pgm_read_byte(analog_pin_to_digital_pin + ADCChannel);
	if (digitalPin == NOT_A_PIN) return;
#endif
	long _value = 0;
	for(int _counter = 0; _counter < samples; _counter ++)
	{
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		// set the digital pin corresponding to the analog pin as an input pin with a pullup resistor
		// this will start charging the capacitive element attached to that pin
		pinMode(digitalPin, INPUT_PULLUP);
		
		// connect the ADC input and the internal sample and hold capacitor to ground to discharge it
		ADMUX = (ADMUX & 0xF0) | 0b1101;
#else
		// set the analog pin as an input pin with a pullup resistor
		// this will start charging the capacitive element attached to that pin
		pinMode(ADCChannel, INPUT_PULLUP);
		
		// connect the ADC input and the internal sample and hold capacitor to ground to discharge it
		ADMUX |=   0b11111;
#endif
		// start the conversion
		ADCSRA |= (1 << ADSC);

		// ADSC is cleared when the conversion finishes
		while((ADCSRA & (1 << ADSC)));

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
		pinMode(digitalPin, INPUT);
#else
		pinMode(ADCChannel, INPUT);
#endif
		_value += analogRead(ADCChannel);
	}
	return _value / samples;
}


 
void setup() {
  analogWrite(9, 100);
  delay(300);
  //  ref1=ADCTouchRead(A1,30);
 //   ref2=ADCTouchRead(A0,30);

  touch_setup();
}

void loop() {
  unsigned long time;
  time= micros();
  
  // 4 measurements are taken and averaged to improve noise immunity
  for (int i=0; i<4; i++) {
    // first measurement: charge touch probe, discharge ADC s&h cap, connect the two, measure the volage
    adc1+= touch_probe(TPIN1, TPIN2, false); // accumulate the results for the averaging

    // second measurement:discharge touch probe, charge ADC s&h cap, connect the two, measure the volage
   adc2+= touch_probe(TPIN1, TPIN2, true); // accumulate the results for the averaging
//    adc1+=ADCTouchRead(A1,3);
 //   adc2+=ADCTouchRead(A0,3);
  //  adc1-=ref1;
 //   adc2-=ref2;
  }
 // adc1>>=2; // divide the accumulated measurements by 16
//  adc2>>=2;

  time= micros() - time;
 
  // resulting raw touch value
  probe_val= adc1-adc2; // the value of adc1 (probe charged) gets higher when the probe ist touched, the value of adc2 (s&h charged) gets lower when the probe ist touched, so, it has to be be subtracted to amplify the detection accuracy
  
  // calculate the index to the LED fading table
  int16_t idx= (probe_val-TOUCH_VALUE_BASELINE); // offset probe_val by value of untouched probe
  if(idx<0) idx= 0; // limit the index!!!
  idx/= TOUCH_VALUE_SCALE; // scale the index
  if(idx>31) idx= 31; // limit the index!!!

  // print some info to the serial
  
  // fade the LED
  analogWrite(9, ledFadeTable[idx]);
  
  adc1= 0; // clear the averaging variables for the next run
  adc2= 0;
  delay(10); // take 100 measurements per second
}
 
