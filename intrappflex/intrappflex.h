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
#include <imsht.h>
#include <imcharger.h>


#define PinAkku A2
#define EnableI2C A3
#define EnableSHT A0
#define EnableExtPower 6
#define EnableAkku A1
#define EnableRS485 5


IMSht2x		imSht2x;
IMCharger	imCharger;




void SetupIntrappFlex()
{
	pinMode(EnableI2C, OUTPUT);
	pinMode(EnableSHT, OUTPUT);
	//pinMode(EnableRS485, OUTPUT);
	digitalWrite(EnableSHT, HIGH);
	digitalWrite(EnableI2C, HIGH);
	//digitalWrite(EnableRS485, LOW);
       imCharger.Init();

}




void DataIntrappFlex(IMFrame &frame)
{
   IMFrameData *data =frame.Data();

	uint16_t temperature = imSht2x.GetTemperatureBin();
	uint16_t humidity = imSht2x.GetHumidityBin();
	float Temperature = imSht2x.GetTemperature();
	float Humidity = imSht2x.GetHumidity();
      	DBGINFO("temp: ");
	DBGINFO(Temperature);
	DBGINFO(" hum: ");
	DBGINFO(Humidity);
       data->w[0]=temperature;
       data->w[1]=humidity;


}





#endif
//
// END OF FILE
//
