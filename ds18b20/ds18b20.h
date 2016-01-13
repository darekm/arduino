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

#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Setup a oneWire instance to communicate with ANY OneWire devices

OneWire oneWire(ONE_WIRE_BUS);

// Set oneWire reference to Dallas Temperature sensor.
DallasTemperature sensors(&oneWire);


DeviceAddress  dsAddress;

void SetupDS18B20()
{
  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
  sensors.begin();
  DBGINFO("devices:");
  DBGINFO(sensors.getDeviceCount());
  sensors.setWaitForConversion(false);
//  sensors.getAddress(&dsAddress,1);
  sensors.getAddress(dsAddress, 0);
  
   for(byte i = 0; i < 8; i++)
    {
      DBGINFO2( dsAddress[i], HEX );
    }  

}

void PrepareDS18B20()
{
   sensors.requestTemperatures();
  
}  


void DataDS18B20(IMFrame &frame)
{
   IMFrameData *data =frame.Data();
   DeviceAddress deviceAddress;
//   bool ex=sensors.getAddress(deviceAddress, 0);
  int16_t hh=sensors.getTempHex((uint8_t*)dsAddress);
 
//  float Temp=sensors.getTempCByIndex(0);
      	DBGINFO("temp: ");
       DBGINFO(sensors.getTempCByIndex(0));
        DBGINFO(" ");
        DBGINFO(hh);
//        DBGINFO(ex);
       data->w[0]=hh;


}





#endif
//
// END OF FILE
//
