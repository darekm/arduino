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

#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#define pinACS A2

uint16_t Measure[32];
uint8_t current;




void SetupACS720()
{

  pinMode(pinACS,INPUT);
  current=0;

}

void MeasureACS720()
{
   current++;
   if (current>20)
      return;
   Measure[current]=analogRead(pinACS);


}  


void DataACS720(IMFrame &frame)
{
  long x=0;
//   SetupADC();
   IMFrameData *data =frame.Data();


//  float Temp=sensors.getTempCByIndex(0);
      	DBGINFO("temp: ");
       DBGINFO(sensors.getTempCByIndex(0));
        DBGINFO(" ");
        DBGINFO(hh);
//        DBGINFO(ex);
       data->w[2]=hh;
   uint16_t Vin=internalVcc();
   data->w[0]=Vin;
   current++;
//   ShutOffADC();



}





#endif
//
// END OF FILE
//
