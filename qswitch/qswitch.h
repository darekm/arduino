// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: qtouch sensor  for imwave
//
//
// HISTORY:
//

#ifndef imQswitch_h
#define imQswitch_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;

#define LEDB1 6
#define LEDB2 1
#define LEDB3 7







void SetupSwitch()
{
 pinMode(LEDB2, OUTPUT);
  pinMode(2, OUTPUT); //pin D2
  pinMode(5, OUTPUT); //pin D5
  pinMode(A4,OUTPUT); //SDA
  pinMode(A5,OUTPUT);//SCL
  pinMode(LEDB3, OUTPUT);
  // analogWrite(9, 100);
 // analogWrite(LEDB1, 100);
  digitalWrite(LEDB2, HIGH);
  //digitalWrite(LEDB3, HIGH);
  digitalWrite(5, HIGH);
  DBGLEDON();
  delay(300);
  DBGLEDOFF();
  digitalWrite(LEDB2, LOW);

}

int idx1,idx2,idx3;
void LoopSwitch() {
 //  idx1++;
//   idx2+=11;
   if (idx1>100) idx1=0;
   if (idx2>100) idx2=0;
  // fade the LED
//  analogWrite(9, idx2);
  //analogWrite(7, ledFadeTable[idx]);
 //  analogWrite(LEDB1, idx1);
 //  digitalWrite(LEDB2,idx2>50);
  // digitalWrite(LEDB3,idx1>50);
}


void PrepareSwitch()
{
  LoopSwitch();
}  


void DataSwitch(IMFrame &frame)
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
//   bool ex=sensors.getAddress(deviceAddress, 0);
//   int16_t hh=sensors.getTempHex((uint8_t*)dsAddress);
 //      data->w[2]=hh;
//   data->w[6]=trx.Connected();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[5]=idx1;
   data->w[10]=0xA33A;
}





#endif
//
// END OF FILE
//
