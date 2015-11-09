// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: SHT driver  for imwave
//
//
// HISTORY:
//

#ifndef imFlex_h
#define imFlex_h

#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


IMSht2x		imSht2x;




void SetupIntrappFlex()
{
	pinMode(EnableI2C, OUTPUT);
	pinMode(EnableSHT, OUTPUT);
	//pinMode(EnableRS485, OUTPUT);
	digitalWrite(EnableSHT, HIGH);
	digitalWrite(EnableI2C, HIGH);
	//digitalWrite(EnableRS485, LOW);
}




void DataIntrappFlex(IMFrame &frame)
{
   IMFrameData *data;

	uint16_t temperature = imSht2x.GetTemperatureBin();
	uint16_t humidity = imSht2x.GetHumidityBin();
      	DBGINFO("temp: ");
	DBGINFO(temperature);
	DBGINFO(" hum: ");
	DBGINFO(humidity);
       data->w[1]=temperature;
       data->w[2]=humidity;


}





#endif
//
// END OF FILE
//
