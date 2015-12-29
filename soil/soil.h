// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: HC SR501 PIR  logger  for imwave
//
//
// HISTORY:
//

#ifndef imSOIL_h
#define imSOIL_h

#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"
#include <me_atmega.h>

// Setup a oneWire instance to communicate with ANY OneWire devices




void SetupSOIL()
{
  pinMode(SOIL_BUS,INPUT);
  DBGINFO("bus:");
  DBGINFO(SOIL_BUS);

}




bool DataSOIL(IMFrame &frame)
{
   IMFrameData *data =frame.Data();
   uint16_t soil=analogRead(SOIL_BUS);
   uint16_t Vin=internalVcc();
   uint16_t Tin=internalTemp();
    DBGINFO("|");
    DBGINFO(soil);
    DBGINFO("|");
    DBGINFO(Vin);
    DBGINFO("|");
    DBGINFO(Tin);
   data->w[0]=soil;
   data->w[1]=Vin;
   data->w[2]=Tin;

    return true;


}





#endif
//
// END OF FILE
//
