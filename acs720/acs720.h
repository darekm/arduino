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



void SetupACS720()
{
  SetupADC();
  pinMode(pinACS,INPUT);
  pinMode(pinVAD,OUTPUT);
  digitalWrite(pinVAD,HIGH);
  current=0;

}

void MeasureACS720()
{ 
   delaySleepT2(1);
   for (int8_t i=19; i>=0; i--)
  {
   DBGPINHIGH();
   Measure[i]=analogRead(pinACS);
   DBGPINLOW();
   delaySleepT2(1);
   }

}  


void DataACS720(IMFrame &frame)
{
 // long x=0;
//   SetupADC();
   IMFrameData *data =frame.Data();


//        DBGINFO(ex);
  //     data->w[2]=hh;
          for (int8_t i=19; i>=0; i--)
  {
     data->w[i+2]=Measure[i];
   }
   uint16_t Vin=internalVcc();
   data->w[0]=Vin;
   current++;
//   ShutOffADC();



}





#endif
//
// END OF FILE
//
