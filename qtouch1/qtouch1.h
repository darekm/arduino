// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: qtouch sensor  for imwave
//
//
// HISTORY:
//

#ifndef imQtouch_h
#define imQtouch_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;


const uint16_t ledFadeTable[32] = {0, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 9, 10, 12, 15, 17, 21, 25, 30, 36, 43, 51, 61, 73, 87, 104, 125, 149, 178, 213, 255}; // this is an exponential series to model the perception of the LED brightness by the human eye

//int ref2,ref1;


#define TPIN1 1
#define TPIN2 2
#define TPIN3 3
#define LEDB1 6
#define LEDB2 1
#define LEDB3 7


imTouch touch;






void SetupQtouch()
{
  pinMode(LEDB2, OUTPUT);
 pinMode(LEDB3, OUTPUT);
// analogWrite(9, 100);
  analogWrite(LEDB1, 100);
//  digitalWrite(7, 100);
  digitalWrite(LEDB2, HIGH);
  delay(300);
   digitalWrite(LEDB2, LOW);
 
  //  ref1=ADCTouchRead(A1,30);
 //   ref2=ADCTouchRead(A0,30);

  touch.setup();

}

void LoopQtouch() {
  int idx1=touch.read(TPIN1,40);
  int idx2=touch.read(TPIN2,0);
  int idx3=touch.read(TPIN3,200);
  // calculate the index to the LED fading table
  if(idx1>31) idx1= 31; // limit the index!!!
  if(idx2>31) idx2= 31; // limit the index!!!
  if(idx3>31) idx3= 31; // limit the index!!!

  // fade the LED
//  analogWrite(9, ledFadeTable[idx3]);
  //analogWrite(7, ledFadeTable[idx]);
   analogWrite(LEDB1, ledFadeTable[idx1]);
   digitalWrite(LEDB2,idx2>20);
   digitalWrite(LEDB3,idx3>10);
}


void PrepareQtouch()
{
 // IMTimer::doneMeasure();
//   sensors.requestTemperatures();
}  


void DataQtouch(IMFrame &frame)
{   
 /* if (cpuVinCycle % 8==0){
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
  }
  */
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
 //      data->w[2]=hh;
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33A;
}





#endif
//
// END OF FILE
//
