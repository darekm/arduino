// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: max6675 logger  for imwave
//
//
// HISTORY:
//

#ifndef imMAX6675_h
#define imMAX6675_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <max6675.h>


int thermoDO = A4;
int thermoCS = 2;
int thermoCLK = A5;
int vccPin = 3;

 

MAX6675 thermo (thermoCLK, thermoCS, thermoDO);
uint16_t cpuVin;
uint16_t cpuVinCycle=0;

void SetupMAX6675()
{
  pinMode(vccPin, OUTPUT); 
 //scale.begin(A4, A5,128);
}

void PrepareMAX6675()
{
  digitalWrite(vccPin, HIGH);
 //   scale.power_up();
    DIDR0 = ~(0x10 ); //ADC4D,
}
void DataMAX6675(IMFrame &frame)
{   
  if (cpuVinCycle % 4==0){
    
    SetupADC();
    cpuVin=internalVcc();
    ShutOffADC();
  }
  pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   int16_t hh=thermo.read();
       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
 //   data->w[3]=(hh >>16);
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
     digitalWrite(vccPin, LOW);
 //  scale.power_down();
}





#endif
//
// END OF FILE
//
