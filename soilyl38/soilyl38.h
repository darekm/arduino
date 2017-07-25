 // 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: soil logger  for imwave
//
//
// HISTORY:
//

#ifndef imSOILYL38_h
#define imSOILYL38_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#define SOIL_BUS A5
#define SOIL_VCC 2

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
volatile uint16_t adcReading;
volatile boolean adcDone;
uint16_t soilData;
ISR (ADC_vect)
  {
     uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
//  ADCSRA |= (1 << ADIF);     // Clear ADIF
  adcReading = (high<<8) | low;
  adcDone = true;
}  // end of ADC_vect

int rawAnalog( void )
{
  adcReading=3;
//  https://forum.arduino.cc/index.php?topic=45949.0
 // Generate an interrupt when the conversion is finished
 ADCSRA |= _BV( ADIE );

 // Enable Noise Reduction Sleep Mode
 set_sleep_mode( SLEEP_MODE_ADC );
 sleep_enable();

 // Any interrupt will wake the processor including the millis interrupt so we have to...
 // Loop until the conversion is finished
 do
 {
   // The following line of code is only important on the second pass.  For the first pass it has no effect.
   // Ensure interrupts are enabled before sleeping
   sei();
   // Sleep (MUST be called immediately after sei)
   sleep_cpu();
   // Checking the conversion status has to be done with interrupts disabled to avoid a race condition
   // Disable interrupts so the while below is performed without interruption
   cli();
 }
 // Conversion finished?  If not, loop.
 while( ( (ADCSRA & (1<<ADSC)) != 0 ) );

 // No more sleeping
 sleep_disable();
 // Enable interrupts
 sei();

 // The Arduino core does not expect an interrupt when a conversion completes so turn interrupts off
 ADCSRA &= ~ _BV( ADIE );

 // Return the conversion result
 return( adcReading );
}

void ResetupADC(){
     DIDR0 = ~(0x30 ); //ADC4D,
     ADMUX  =  (1<< REFS0) | (1<<REFS1)| (5);    // AVcc and select input port
}

void SetupSOIL()
{
   SetupADC();
//  ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 
      pinMode(SOIL_VCC, OUTPUT);
      pinMode(SOIL_BUS, INPUT_PULLUP);
    pinMode(SOIL_BUS, OUTPUT);
     pinMode(SOIL_BUS, INPUT);
  //  DIDR0 = ~(0x30 ); //ADC4D,
   //  ADMUX  =  (1<< REFS0) | (1<<REFS1)| (5);    // AVcc and select input port
  ResetupADC();
}

void PrepareSOIL()
{
// power_adc_enable();
  digitalWrite(SOIL_VCC,HIGH);
DBGLEDON();
 SetupADC();
// ADMUX = bit (REFS0) | bit (REFS1);
//  digitalWrite(SOIL_VCC,HIGH);
    delaySleepT2(1);
   delaySleepT2(1);
   delaySleepT2(1);
   delaySleepT2(1);
   delaySleepT2(1);
   delaySleepT2(1);
   soilData=rawAnalog();
  digitalWrite(SOIL_VCC,LOW);
    setSleepModeT2();
    
    if (cpuVinCycle % 4==0){
   //  SetupADC();
     cpuVin=internalVcc();
     ResetupADC();
    // ShutOffADC();
  }
  
  ShutOffADC();
  
  power_adc_disable();
DBGLEDOFF();
}

void DataSOIL(IMFrame &frame)
{   
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
 //  DBGLEDON(); 

    data->w[2]=soilData;
    data->w[4]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
   // digitalWrite(SOIL_VCC,LOW);
   // DBGLEDOFF();
}





#endif
//
// END OF FILE
//
