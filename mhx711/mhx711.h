// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: hx711 logger  for imwave
//
//
// HISTORY:
//

#ifndef imMHX711_h
#define imMHX711_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <HX711.h>



HX711 scale;

uint16_t cpuVin;
uint16_t cpuVinCycle=0;

void SetupHX711()
{
  scale.begin(2, 3);
}


void DataHX711(IMFrame &frame)
{   
  if (cpuVinCycle % 4==0){ 
    SetupADC();
    cpuVin=internalVcc();
    ShutOffADC();
  }
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   int16_t hh=scale.read();
       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
}





#endif
//
// END OF FILE
//
