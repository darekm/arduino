// 
//    FILE:
// VERSION: 0.2.00
// PURPOSE:  logger  for current transformer
//
//
// HISTORY:
//

#ifndef imYHDC720_h
#define imYHDC720_h


#include "imframe.h"
#include "imdebug.h"
#include "imatmega.h"


//#define pinACS 4
//#define pinVAD A5

uint16_t Measure[85];
uint16_t MeasureSUM[10];
//uint16_t MeasureSQR[10];
uint16_t MeasureCycle;
//uint16_t current;

uint16_t xcount;
uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
uint16_t adcTime;
volatile uint16_t adcReading;
volatile boolean adcDone;
long adcMedium;
long adcLow;
long adcHigh;
uint32_t adcSum;
uint16_t adcValue;

#define pinACS 0
#define ADCVHIGH() do{}while(0) ;//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  do{}while(0) ;//digitalWrite(DBGPIN,LOW)
//#define ADCVHIGH() PORTC|=(B00100000);//digitalWrite(DBGPIN,HIGH)
//#define ADCVLOW()  PORTC&=~(B00100000);//digitalWrite(DBGPIN,LOW)
 
#define setupREF (1<< REFS0) | (1<<REFS1)| (pinACS)    //internal 1.1V vReference

// disassembly
// http://rcarduino.blogspot.com/2012/09/how-to-view-arduino-assembly.html

// ADC complete ISR
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
  waitASSR();
  
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

void SetupACS720()
{
  SetupADC();
//  TWCR=0;
//  DIDR0=0;
  
 // disableADCB();
 // TWCR&=~(1<<TWEN);
  //  DIDR0 = ~(0x10 ); //ADC4D,

//  digitalWrite(pinACS,HIGH);
  pinMode(A0,INPUT);  //pinACS
 //pinMode(A4,INPUT);  //pinACS
 //pinMode(A5,OUTPUT);  //pinACS
//  pinMode(pinVAD,OUTPUT);
//  digitalWrite(pinACS,HIGH);
//  digitalWrite(pinVAD,HIGH);
 // pinMode(pinACS,OUTPUT);
 // digitalWrite(pinACS,HIGH);
 // current=0;
      ADMUX  = setupREF;


}

void MeasureACS720()
{ 
 //TWCR=0;
  SetupADC();//ADMUX after setupADC
       ADMUX  =  setupREF;    // AVcc and select input port
// power_adc_enable();
  ADCVHIGH();
  long xSum=0;
 // MeasureCycle++;
    delaySleepT2(1);
   delaySleepT2(1);
  t_Time xstart = millisTNow();
   xcount=0;
 // syncTimer2(-1);
    for (int8_t i=80; i>=0; i--)  //41cycles ~ 40ms
  {
 //  DBGPINHIGH();
   Measure[i]=rawAnalog();
 //   DBGPINLOW();
   setSleepModeT2();
   xSum+=Measure[i];
 //  syncTimer2(-1);
   delayT2();
   xcount++;
  // delaySleepT2(1);
   }
  adcMedium=xSum/81;
  ADCVLOW();
  adcTime=  millisTNow()-xstart;
 
  setSleepModeT2();
  ShutOffADC();
 // ShutDownADC();
  power_adc_disable();
}

void ComputeACS720()
{
  unsigned long xx=0;
  adcLow=60000;
  adcHigh=0;

  for (int8_t i=80; i>=0; i--)
  {
     long x=Measure[i];
     if (x>adcHigh) adcHigh=x;
     if (x<adcLow) adcLow=x;
   //      xSum+=x;
      long y=x-adcMedium;
         xx+=(y*y);
    //  xLast=i;
   }
   adcValue=sqrt32(xx);
   adcSum+=adcValue;  
}

void DataACS720(IMFrame &frame)
{
//   SetupADC();
//  for (int8_t i=80; i>=0; i--)
//  {
//     xSum+=Measure[i];
//     if(x>adcHigh) adcHigh=x;
//     if (x<adcLow) adcLow=x;
   //   xSum+=x;
//      long y=x-adcMedium;
//         xx+=(y*y);
//      xLast++;
 //  }
  
  // adcMedium=(adcMedium *9 +xSum/81)/10;


   IMFrameData *data =frame.Data();

//   current++;
//   Shu0tOffADC();
   data->w[9]=xcount;
  // data->w[7]=xx;
  // data->w[6]=xSum;
  // data->w[8]=MeasureCycle;
   data->w[8]=adcTime;
   data->w[6]=adcHigh;
  // data->w[4]=adcLow;
   data->w[4]=setupREF;
   data->w[3]=adcMedium;
   //uint16_t xws=sqrt32(xx);
   data->w[2]=adcValue;
//   adcSum+=xws;
   data->w[4]=adcSum ;
   data->w[5]=adcSum >> 16; 
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   MeasureCycle=0;
  if ((cpuVinCycle % 28)==0){
    SetupADC();
    cpuVin=internalVcc();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
     ADMUX  =  setupREF;    // AVcc and select input port

    ShutOffADC();
    power_adc_disable();
  }
}





#endif
//
// END OF FILE
//
