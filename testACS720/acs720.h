// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: acs720 logger  for current
//
//
// HISTORY:
//

#ifndef imACS720_h
#define imACS720_h


#include "imframe.h"
#include "imdebug.h"



#define pinACS A4
#define pinVAD A5

uint16_t Measure[32];
uint16_t current;


uint16_t cpuVin;
uint16_t cpuVinCycle=0;

volatile int adcReading;
volatile boolean adcDone;
int adcMedium;


// ADC complete ISR
ISR (ADC_vect)
  {
     uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
//  ADCSRA |= (1 << ADIF);     // Clear ADIF
  adcReading = (high<<8) | low;
  adcDone = true;
}  // end of ADC_vect
 
 
   #define ADCVHIGH() PORTC|=(B00100000);//digitalWrite(DBGPIN,HIGH)
   #define ADCVLOW()  PORTC&=~(B00100000);//digitalWrite(DBGPIN,LOW)
 

void SetupACS720()
{
  SetupADC();
  pinMode(pinACS,INPUT);
  pinMode(pinVAD,OUTPUT);
  digitalWrite(pinVAD,HIGH);
  current=0;
    ADMUX  =  (1<< REFS0) | (0<<REFS1)| (4);    // AVcc and select input port
//  adcReading = analogRead(pinACS);

}

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

void MeasureACS720()
{ 
  //  ADMUX  =  (1<< REFS0) | (4);    // AVcc and select input port
//  adcReading = analogRead(pinACS);
//  digitalWrite(pinVAD,HIGH);
  ADCVHIGH();
  delaySleepT2(1);
   for (int8_t i=19; i>=0; i--)
  {
//   DBGPINHIGH();
   Measure[i]=rawAnalog();
//   Measure[i]=analogRead(pinACS);
//   DBGPINLOW();
   setSleepModeT2();
   delaySleepT2(1);
   }
   ADCVLOW();
//   digitalWrite(pinVAD,LOW);   
}  


void DataACS720(IMFrame &frame)
{
  int x=0;
  long xx=0;
   for (int8_t i=19; i>=0; i--)
  {
         x+=Measure[i];
      int y=Measure[i]-adcMedium;    
         xx+=y*y;
   }
   xx=xx ;
   adcMedium=x/20;
   
   
   
//   SetupADC();

   IMFrameData *data =frame.Data();


//        DBGINFO(ex);
  //     data->w[2]=hh;
          for (int8_t i=1; i>=0; i--)
  {
//     data->w[i+2]=Measure[i];
     data->w[i+2]=i;
   }
 //  uint16_t Vin=internalVcc();
   data->w[3]=adcMedium;
   data->w[2]=xx;
   current++;
//   ShutOffADC();



}





#endif
//
// END OF FILE
//
