// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: SHT31 logger  for imwave
//
//
// HISTORY:
//

#ifndef imSHT31_h
#define imSHT31_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

#include <Wire.h>
#include <SHT31lib.h>

// Setup a oneWire instance to communicate with ANY OneWire devices



// Set oneWire reference to Dallas Temperature sensor.
SHT31 sensor;



uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;

IMMAC SetupSHT31()
{
  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  sensor.begin();
  
  
  if ( sensor.readStatus()=0xFFFF) return 0;
  return 1;
}

void PrepareSHT31()
{
   sensor.start();
}  


void DataSHT31(IMFrame &frame)
{   
  if (cpuVinCycle % 8==0){ 
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
  }
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
   data->w[9]=sensor.readTempHum();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   
       	DBGINFO("temp: ");
        DBGINFO(" ");
        
       data->w[2]=sensor.Temp;
       data->w[3]=sensor.Humidity;
   data->w[6]=trx.Connected();
 //  Vin=internalVcc();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33A;
}





#endif
//
// END OF FILE
//
