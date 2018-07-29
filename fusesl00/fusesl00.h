// 
//    FILE:
// VERSION: 0.2.00
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
#define pinA4 A3
#define pinA5 A4
#define pinA6 A5
#define pinVAD A5
#define FPC1 0
#define FPC2 1
#define FPC3 2
#define FPC4 3
#define FPC5 4
#define FPC6 5
#define fpCount 40
#define fpDiv 20
#define maxCount 45

int16_t Measure1[maxCount];
int16_t Measure2[maxCount];
int16_t Measure3[maxCount];
int16_t Measure4[maxCount];
uint16_t current;


uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
volatile uint16_t adcReading;
volatile boolean adcDone;
uint16_t idx1,idx2,idx3,idx4;
uint16_t thresholdFuse;
byte maskFuse;
byte currentFuse;
byte stepFuse;
#define ADCVHIGH() PORTD|=(B00100000);//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  PORTD&=~(B00100000);//digitalWrite(DBGPIN,LOW)
//#define ADCVHIGH()  DBGLEDON()
//#define ADCVHIGH()  DBGLEDOFF()


ISR (ADC_vect)
  {
     uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both
//  ADCSRA |= (1 << ADIF);     // Clear ADIF
  adcReading = (high<<8) | low;
  adcDone = true;
}  


uint16_t rawAnalogOne( byte aPC )
{
  ADMUX  =_BV(REFS0)|aPC;  // Charge S/H cap from Analog0
 delayMicroseconds(5);
 ADCSRA |= _BV( ADIE );
 
 sleep_enable();

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

uint16_t rawAnalog(byte aPC){
  adcReading=3;
 // Enable Noise Reduction Sleep Mode
  set_sleep_mode( SLEEP_MODE_ADC );
  return rawAnalogOne(aPC);
}

uint16_t rawAnalogDuo(byte aPC1,byte aPC2){
  adcReading=3;
 // Enable Noise Reduction Sleep Mode
  set_sleep_mode( SLEEP_MODE_ADC );
  int x1= rawAnalogOne(aPC1);
  int x2= rawAnalogOne(aPC2);
  x1-=x2;
 // x1*=x1;
  return abs(x1);
}



void InitSensor(){
  maskFuse=7;
  thresholdFuse=50;
}

void Compute(){
  long xSum1=0;
  long xSum2=0;
  long xSum3=0;
  long xSum4=0;
  byte xLast =0;
  for (int8_t i=fpCount; i>=0; i--)
  {
     xSum1+=Measure1[i];
     xSum2+=Measure2[i];
     xSum3+=Measure3[i];
     xSum4+=Measure4[i];
//     if(x>adcHigh) adcHigh=x;
//     if (x<adcLow) adcLow=x;
   //   xSum+=x;
//      long y=x-adcMedium;
//         xx+=(y*y);
      xLast++;
   }
  idx1=xSum1/fpDiv;
  idx2=xSum2/fpDiv;
  idx3=xSum3/fpDiv;
  idx4=xSum4/fpCount;
  if (xSum1>200000L) idx1=0x6666; 
  if (xSum2>200000L) idx2=0x6666; 
  if (xSum3>200000L) idx3=0x6666; 
  if (xSum4>200000L) idx4=0x6666; 
  uint16_t cFuse=0;
  if (idx1>thresholdFuse) cFuse|=0x1;
  if (idx2>thresholdFuse) cFuse|=0x2;
  if (idx3>thresholdFuse) cFuse|=0x4;
  if (cFuse>0){
      if (currentFuse!=cFuse){
         currentFuse=cFuse;
         stepFuse=4;
      }
     ADCVHIGH();
     DBGLEDON();

  } else{
      if (stepFuse>0){  
     
        ADCVHIGH();
        DBGLEDON();

        stepFuse--;
        return;   
      }
      currentFuse=cFuse;
  }  
}  

void SetupSensor()
{
  DBGLEDON();
     ADCVHIGH();
  delay(100);
  DBGLEDOFF();
     ADCVLOW();
  delay(100);
  DBGLEDON();
     ADCVHIGH();
  delay(100);
  DBGLEDOFF();
     ADCVLOW();
  delay(100);
    DBGLEDON();

     ADCVHIGH();
  delay(100);
     ADCVLOW();
  SetupADC();


  pinMode(pinA1,INPUT);
  pinMode(pinA2,INPUT);
  pinMode(pinA3,INPUT);
  pinMode(pinA4,INPUT);
  pinMode(pinA5,INPUT);
  pinMode(pinA6,INPUT);

   DBGLEDOFF();
   ShutDownADC();
   InitSensor();
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
  setSleepModeT2();
  delayT2();
  
 //  delaySleepT2(1);
//   delaySleepT2(1);
  ADCVHIGH();
  DBGLEDON();
   for (int8_t i=fpCount; i>=0; i--)  //41cycles ~ 40ms
  {
 //  DBGPINHIGH();
   Measure4[i]=rawAnalog(FPC1);
   //Measure1[i]=rawAnalogDuo(FPC1,FPC2);
   //Measure2[i]=rawAnalogDuo(FPC3,FPC4);
   //Measure3[i]=rawAnalogDuo(FPC5,FPC6);
   Measure1[i]=rawAnalogDuo(FPC1,FPC4);
   Measure2[i]=rawAnalogDuo(FPC2,FPC5);
   Measure3[i]=rawAnalogDuo(FPC3,FPC6);
   Measure4[i]=rawAnalog(FPC1);
  // Measure3[i]=rawAnalog(FPC3);
 //   DBGPINLOW();
   setSleepModeT2();
   delayT2();
  // delaySleepT2(1);
   }
  ADCVLOW();
  DBGLEDOFF();
    setSleepModeT2();
    ShutDownADC();
   Compute();
}


void MeasureVCC(){
    SetupADC();
    cpuVin=internalVcc();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
}

void DataSensor(IMFrame &frame)
{
   IMFrameData *data =frame.Data();
   data->w[5]=0xADAD;
   data->w[6]=idx1;
   data->w[7]=idx2;
   data->w[8]=idx3;
   data->w[9]=idx4;
   data->w[3]=maskFuse;
   data->w[2]=currentFuse;
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;

   data->w[10]=trx.myMAC >> 16;
   data->w[9]=trx.myMAC;
 //data->w[22]=trx.myMAC;

  if (cpuVinCycle % 18==2){
      MeasureVCC();
  }
  cpuVinCycle++;
}





#endif
//
// END OF FILE
//
