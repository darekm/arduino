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
  scale.begin(A4, A5,128);
}

void PrepareHX711()
{
    scale.power_up();
     pinMode(A4, INPUT_PULLUP);
    pinMode(A4, OUTPUT);
     pinMode(A4, INPUT);
    DIDR0 = ~(0x10 ); //ADC4D,
}
void DataHX711(IMFrame &frame)
{   
  if (cpuVinCycle % 4==0){
    
  //  SetupADC();
  //  cpuVin=internalVcc();
   // ShutOffADC();
  }
 // pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   unsigned long hh=scale.read();
       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
    data->w[3]=(hh >>16);
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
   scale.power_down();
}





#endif
//
// END OF FILE
//
