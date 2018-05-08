// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: blow fuse monitoring
//
//
// HISTORY:
//

#ifndef imFuseSl00_h
#define imFuseSl00_h


#include "imframe.h"
#include "imdebug.h"
#include "imatmega.h"


#define pinACS A0
#define PC0 0
#define pinVAD A5

uint16_t Measure[85];
uint16_t current;


uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
volatile uint16_t adcReading;
volatile boolean adcDone;
long adcMedium;
long adcLow;
long adcHigh;


#define ADCVHIGH() PORTC|=(B00100000);//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  PORTC&=~(B00100000);//digitalWrite(DBGPIN,LOW)

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

void SetupSensor()
{
  DBGLEDON();
  delay(100);
  SetupADC();

 // disableADCB();
 // TWCR&=~(1<<TWEN);
  //  DIDR0 = ~(0x10 ); //ADC4D,

//  digitalWrite(pinACS,HIGH);
  pinMode(pinACS,INPUT);
  pinMode(pinVAD,OUTPUT);
//  digitalWrite(pinACS,HIGH);
//  digitalWrite(pinVAD,HIGH);
 // pinMode(pinACS,OUTPUT);
 // digitalWrite(pinACS,HIGH);
  current=0;
    //  ADMUX  =  (1<< REFS0) | (0<<REFS1)| (4);    // AVcc and select input port

   DBGLEDOFF();

}

void MeasureSensor()
{ 
     //  ADMUX  =  (1<< REFS0) | (0<<REFS1)| (4);    // AVcc and select input port
//TWCR=0;
 power_adc_enable(); // ADC converter
  SetupADC();
    ACSR=48;
 //  ADCSRA  =_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); // Enable ADC, Set prescaler to 128
  ADCSRB=0;
 ADCSRA = 0b11000101; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
  ADMUX  =_BV(REFS0)|PC0;  // Charge S/H cap from Analog0
 delayMicroseconds(12);

//  ADCVHIGH();
   delaySleepT2(1);
   delaySleepT2(1);
   for (int8_t i=80; i>=0; i--)  //41cycles ~ 40ms
  {
 //  DBGPINHIGH();
   Measure[i]=rawAnalog();
 //   DBGPINLOW();
   setSleepModeT2();
   delayT2();
  // delaySleepT2(1);
   }
//  ADCVLOW();
    setSleepModeT2();
  ShutOffADC();
  
  power_adc_disable();
}


void MeasureVCC(){
    SetupADC();
    cpuVin=internalVcc();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
//    ADMUX=0;
    power_adc_disable();
}

void DataSensor(IMFrame &frame)
{
//   SetupADC();
  long xSum=0;
  byte xLast =0;
  unsigned long xx=0;
  adcLow=90000;
  adcHigh=0;
  for (int8_t i=80; i>=0; i--)
  {
     xSum+=Measure[i];
//     if(x>adcHigh) adcHigh=x;
//     if (x<adcLow) adcLow=x;
   //   xSum+=x;
//      long y=x-adcMedium;
//         xx+=(y*y);
      xLast++;
   }
  adcMedium=xSum/81;
  
  for (int8_t i=80; i>=0; i--)
  {
     long x=Measure[i];
     if(x>adcHigh) adcHigh=x;
     if (x<adcLow) adcLow=x;
   //      xSum+=x;
      long y=x-adcMedium;
         xx+=(y*y);
//      xLast=i;
   }
 //  xx=xx ;
  // adcMedium=(adcMedium *9 +xSum/81)/10;


   IMFrameData *data =frame.Data();


//        DBGINFO(ex);
   current++;
   xLast+=10;
//   ShutOffADC();
   data->w[7]=xx;
   data->w[6]=xSum;
   data->w[5]=adcHigh;
   data->w[4]=adcLow;
   data->w[3]=adcMedium;
  // data->w[4]=trx.dataw3;
  // data->w[3]=trx.Deviation();
   data->w[2]=sqrt32(xx);
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;


  if (cpuVinCycle % 18==2){
      MeasureVCC();
  }
}





#endif
//
// END OF FILE
//
