// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: ds18B20 logger  for imwave
//
//
// HISTORY:
//

#ifndef imDS18B20_h
#define imDS18B20_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

#include <OneWire.h>
#include <DallasTemperature.h>

// Setup a oneWire instance to communicate with ANY OneWire devices

OneWire oneWire(ONE_WIRE_BUS);

// Set oneWire reference to Dallas Temperature sensor.
DallasTemperature sensors(&oneWire);


DeviceAddress  dsAddress;
uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;

IMMAC SetupDS18B20()
{
  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  sensors.begin();
  DBGINFO("devices:");
  DBGINFO(sensors.getDeviceCount());
  DBGINFO("=");
  sensors.setWaitForConversion(false);
//  sensors.getAddress(&dsAddress,1);
  sensors.getAddress(dsAddress, 0);
  for(byte i = 0; i < 8; i++)
    {
      DBGINFO2( dsAddress[i], HEX );
      DBGINFO(" ");
    }  
  IMMAC xx=(dsAddress[6]);
  
 // xx=0xDA34;
  xx=(xx << 8);
  xx+= dsAddress[7];
  return xx;
}

void PrepareDS18B20()
{
   sensors.requestTemperatures();
}  


void DataDS18B20(IMFrame &frame)
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
//   bool ex=sensors.getAddress(deviceAddress, 0);
   int16_t hh=sensors.getTempHex((uint8_t*)dsAddress);
       	DBGINFO("temp: ");
        DBGINFO(sensors.getTempCByIndex(0));
        DBGINFO(" ");
        DBGINFO(hh);
       data->w[2]=hh;
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
