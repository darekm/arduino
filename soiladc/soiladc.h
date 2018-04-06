#include <SoftwareSerial.h>

// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: qtouch sensor  for imwave
//
//
// HISTORY:
//

#ifndef imQSoiladc_h
#define imQSoiladc_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;


const uint16_t ledFadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255}; // this is an exponential series to model the perception of the LED brightness by the human eye

int idx1,idx2,idx3,mm;
int idMax,idMin;
int shift1,shift2;
byte ledLimit;
byte idxbool=0,IDXBOOL=0,idxmm,fromm;
bool ib1,ib2,ib3;
#define TPIN1 A0
#define TPIN2 2

#define LEDB1 6
#define LEDB2 1



//imTouch touch;



int martinread(byte ADCChannel, int samples=200)
{
 ADCSRA = 0b11000101; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
 	long _value = 0;
	for(int _counter = 0; _counter < samples; _counter ++)
	{
		// set the analog pin as an input pin with a pullup resistor
		// this will start charging the capacitive element attached to that pin
		pinMode(ADCChannel, INPUT_PULLUP);
		
		// connect the ADC input and the internal sample and hold capacitor to ground to discharge it
		ADMUX |=   0b11111;
		// start the conversion
		ADCSRA |= (1 << ADSC);

		// ADSC is cleared when the conversion finishes
		while((ADCSRA & (1 << ADSC)));

		pinMode(ADCChannel, INPUT);
		_value += analogRead(ADCChannel);
	}
	return _value / samples;
}

int  senseadctwice(void) {
  /*
      Capacitive sensing using charge sharing between 
      the S/H capacitor and an external sensing pad
      
      ATMega 328P
      Float/ref  Analog0  = PC0
      Sense an  Analog1 = PC1
  */  
#define PC0 1
#define PC1 0
  int dat1,dat2;
  
  // Precharge Low
  PORTC = _BV(PC0);    // S/H Charge Vdd (Analog0), Cext (Analog1) gnd
  DDRC = _BV(PC1)|_BV(PC0);
   ADMUX  =_BV(REFS0)|0x0f;
  delayMicroseconds(2);
  ADMUX  =_BV(REFS0)|PC0;  // Charge S/H cap from Analog0
  
  delayMicroseconds(8);
  DDRC  &=~(_BV(PC1));  // float input
              // additional delay due to ADC logic

  ADMUX  =_BV(REFS0)|PC1; // Read Cext from Analog1
  ADCSRA  |=_BV(ADSC); // Start conversion
  
  while (!(ADCSRA&_BV(ADIF)));    
  ADCSRA  |=_BV(ADIF); // Clear ADIF

  dat1=ADC;

  // Precharge High
  ADMUX  =_BV(REFS0)|PC0;  // Charge S/H cap from Analog0
  
  PORTC = _BV(PC1);    // S/H Charge gnd (Analog0), Cext (Analog1) Vdd
  DDRC = _BV(PC1)|_BV(PC0);
  delayMicroseconds(8);

  DDRC  &=~(_BV(PC1));
  PORTC =0;      // pull up off
            // additional delay due to ADC logic

  ADMUX  =_BV(REFS0)|PC1; // Read Cext from Analog1
  ADCSRA  |=_BV(ADSC); // Start conversion

  
  while (!(ADCSRA&_BV(ADIF)));
  ADCSRA  |=_BV(ADIF); // Clear ADIF

  dat2=ADC;

  return (dat2-dat1)*10;
}

int sense(byte ADCChannel, int samples=20)
{
  ACSR=48;
  ADMUX  =_BV(REFS0)|0x0f;
//  ADCSRA  =_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0); // Enable ADC, Set prescaler to 128
  ADCSRB=0;
 ADCSRA = 0b11000101; // enable ADC (bit7), initialize ADC (bit6), no autotrigger (bit5), don't clear int-flag  (bit4), no interrupt (bit3), clock div by 16@16Mhz=1MHz (bit210) ADC should run at 50kHz to 200kHz, 1MHz gives decreased resolution
 	long _value = 0;
	for(int _counter = 0; _counter < samples; _counter ++)
	{
            _value+=senseadctwice();

         }
	return _value / samples;
}




void SetupQtouch()
{
  pinMode(LEDB2, OUTPUT);
// pinMode(LEDB1, OUTPUT);
 //  analogWrite(LEDB1, 100);
//  analogWrite(LEDB1, 100);
//  digitalWrite(7, 100);
 // if (funtest()>0){
//  digitalWrite(LEDB2, HIGH);
//  delay(300);
 //  digitalWrite(LEDB2, LOW);
  DBGLEDON();
  delay(100);
 // }
  //  ref1=ADCTouchRead(A1,30);
 //   ref2=ADCTouchRead(A0,30);

//  touch.setup();
 SetupADC();
 DIDR0 = 0x00;
 //DBGLEDON();
 IDXBOOL=0;
  shift1=0;
  ib1=false;
  ib2=false;
  ib3=false;
  for (int i=0;i<4;i++){
  delay(200);
   // shift1+=touch.check(TPIN1);
//   shift1+=martinread(TPIN1);
  shift1+=sense(TPIN1);
  }  
   shift1/=4;
   shift1-=10;
   mm=11;
   idxmm=0;
   cpuTemp=2;
   ledLimit=0;
   DBGLEDOFF();
}


void computeShift(){
    shift1=(shift1*11+idx1)/12;
 //   shift2=shift2*0.95+idx2*0.05;
 //   shift3=shift3*0.95+idx3*0.05;
    idxmm=0; 
}

void lightSwitch(){
//   digitalWrite(LEDB1, ib1);
 //  digitalWrite(LEDB2,ib2);
 //  digitalWrite(LEDB3,ib3);
}

void offSwitch(){
   digitalWrite(LEDB1,LOW);
//   digitalWrite(LEDB2,LOW);
//   digitalWrite(LEDB3,LOW);
}

void LoopQtouch() {
  ledLimit+=1;
 power_adc_enable(); // ADC converter
//     touch.setup();
  // idx1=touch.check(TPIN1)-shift1;
//     idx1=touch.read(TPIN1,shift1);
//     idx1/=2;
 // idx1=martinread(TPIN1)-shift1;
 
 idx3=idx2;
 idx2=idx1;
 idx1=sense(TPIN1)-shift1;
 if (idMax<idx1)idMax=idx1;
 if (idMin>idx1)idMin=idx1;
 //int idx=sqrt32(idx1);
 int idx=idx1/10;
//   idx2=touch.check(TPIN2);
//   idx3=touch.check(TPIN3);
   
  // calculate the index to the LED fading table
 if (idx<0) idx=0;
 //if(idx>31) idx= 31; // limit the index!!!
//  if(idx2>31) idx2= 31; // limit the index!!!
  //if(idx3>31) idx3= 31; // limit the index!!!
  
    ib1=(idx1-shift1)>(ib1?5:mm);
 //  ib2=(idx2-shift2)>(ib2?5:mm);
 //  ib3=(idx3-shift3)>(ib3?7:mm);
  
  // fade the LED
  if ((idx)>ledLimit){
//    DBGLEDON();
  }else{
 //   DBGLEDOFF();
  }
  //  analogWrite(LEDB1, 60);
   // analogWrite(9, ledFadeTable[idx]);
  //analogWrite(7, ledFadeTable[idx]);
 //  analogWrite(LEDB1, ledFadeTable[idx1]);
   bool change=false;
   idxbool=0;
   if (ib1) idxbool+=1;
  // if (ib2) idxbool+=2;
  // if (ib3) idxbool+=4;
//   ShutOffADC();
    setSleepModeT2();

    power_adc_disable();
    ShutOffADC();
   if (idxbool!=IDXBOOL){
//      IMTimer::doneMeasure();
      idxmm=0;
      fromm=2;
//      lightSwitch();
   } else{
     idxmm++;
     if (idxmm>32){
        if (ib1) idx1-=mm; 
  //      if (ib2) idx2-=mm;
  //      if (ib3) idx3-=mm;
          
//        computeShift();
     }  
   } 
}


void PrepareQtouch()
{
  fromm--;
 // IMTimer::doneMeasure();
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

void DataQtouch(IMFrame &frame)
{   
  if (cpuVinCycle % 18==2){
      MeasureVCC();
  }
  
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
 //      data->w[2]=hh;
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[2]=idxbool;
   data->w[3]=idx1;
   data->w[4]=idMin;
   data->w[5]=idMax;
   data->w[6]=(uint16_t)shift1;
 //  data->w[7]=shift2;
 //  data->w[8]=shift3;
   data->w[10]=0xB11B;
   IDXBOOL=idxbool;
   idMax=-30000;
   idMin=30000;
}





#endif
//
// END OF FILE
//
