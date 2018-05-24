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


#define pinA1 A0
#define pinA2 A1
#define pinA3 A2
#define pinVAD A5
#define FPC1 0
#define FPC2 1
#define FPC3 2
#define fpCount 20

uint16_t Measure1[25];
uint16_t Measure2[25];
uint16_t Measure3[25];
uint16_t current;


uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
volatile uint16_t adcReading;
volatile boolean adcDone;
uint16_t idx1,idx2,idx3;

#define ADCVHIGH() PORTD|=(B00100000);//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  PORTD&=~(B00100000);//digitalWrite(DBGPIN,LOW)

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



int rawAnalog( byte aPC )
{
  adcReading=3;
//  https://forum.arduino.cc/index.php?topic=45949.0
 // Generate an interrupt when the conversion is finished
 ADCSRA |= _BV( ADIE );
  ADMUX  =_BV(REFS0)|aPC;  // Charge S/H cap from Analog0

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

  pinMode(pinA1,INPUT);
  current=0;
    //  ADMUX  =  (1<< REFS0) | (0<<REFS1)| (4);    // AVcc and select input port

   DBGLEDOFF();
   ShutDownADC();
}

void MeasureSensor()
{ 
  //  ADMUX  =  (1<< REFS0) | (0<<REFS1)| (4);    // AVcc and select input port
 power_adc_enable(); // ADC converter
  SetupADC();
    ACSR=48;
 //  ADCSRA  =_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); // Enable ADC, Set prescaler to 128
  ADCSRB=0;
 ADCSRA = 0b11000101; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
//  ADMUX  =_BV(REFS0)|FPC0;  // Charge S/H cap from Analog0
 DIDR0 = 0x00;
 delayMicroseconds(12);
  
 //  delaySleepT2(1);
//   delaySleepT2(1);
  ADCVHIGH();
   for (int8_t i=fpCount; i>=0; i--)  //41cycles ~ 40ms
  {
 //  DBGPINHIGH();
   Measure1[i]=rawAnalog(FPC1);
   Measure2[i]=rawAnalog(FPC2);
   Measure3[i]=rawAnalog(FPC3);
 //   DBGPINLOW();
   setSleepModeT2();
   delayT2();
  // delaySleepT2(1);
   }
  ADCVLOW();
    setSleepModeT2();
    ShutDownADC();
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
  long xSum1=0;
  long xSum2=0;
  long xSum3=0;
  byte xLast =0;
  for (int8_t i=fpCount; i>=0; i--)
  {
     xSum1+=Measure1[i];
     xSum2+=Measure2[i];
     xSum3+=Measure3[i];
//     if(x>adcHigh) adcHigh=x;
//     if (x<adcLow) adcLow=x;
   //   xSum+=x;
//      long y=x-adcMedium;
//         xx+=(y*y);
      xLast++;
   }
  idx1=xSum1/fpCount;
  idx2=xSum2/fpCount;
  idx3=xSum3/fpCount;
  
   // adcMedium=(adcMedium *9 +xSum/81)/10;


   IMFrameData *data =frame.Data();


//        DBGINFO(ex);
   current++;
   xLast+=10;
//   ShutOffADC();
   
  
   data->w[4]=idx3;
   data->w[3]=idx2;
   data->w[2]=idx1;
  // data->w[4]=trx.dataw3;
  // data->w[3]=trx.Deviation();
  // data->w[2]=sqrt32(xx);
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;


  if (cpuVinCycle % 18==2){
      MeasureVCC();
  }
  cpuVinCycle++;
}





#endif
//
// END OF FILE
//
