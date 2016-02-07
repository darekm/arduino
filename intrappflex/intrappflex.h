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


#define  VCC_PIN PinAkku

void SetupIntrappFlex()
{
	pinMode(EnableI2C, OUTPUT);
	pinMode(EnableSHT, OUTPUT);
	//pinMode(EnableRS485, OUTPUT);
	digitalWrite(EnableSHT, HIGH);
	digitalWrite(EnableI2C, HIGH);
	//digitalWrite(EnableRS485, LOW);
	pinMode(EnableAkku, OUTPUT);
	digitalWrite(EnableAkku, LOW);
       imCharger.Init();
       pinMode(VCC_PIN,INPUT);

}




void DataIntrappFlex(IMFrame &frame)
{
   IMFrameData *data =frame.Data();
        uint16_t vcc=analogRead(VCC_PIN);
	uint16_t temperature = imSht2x.GetTemperatureBin();
	uint16_t humidity = imSht2x.GetHumidityBin();
        uint16_t Vin=internalVcc();
        uint16_t CHR=(imCharger.IsChrg() <<4);
          CHR|=imCharger.GetVinStat();
        
//	float Temperature = imSht2x.GetTemperature();
//	float Humidity = imSht2x.GetHumidity();
      	DBGINFO("temp: ");
	DBGINFO(imSht2x.GetTemperature());
//	DBGINFO(" hum: ");
//	DBGINFO(Humidity);
	DBGINFO(" vin: ");
	DBGINFO(Vin);
	DBGINFO(" CHR: ");
	DBGINFO(CHR);
	DBGINFO(" vcc: ");
	DBGINFO(vcc);
	DBGINFO(" \r\n");
         
       data->w[3]=temperature;
       data->w[4]=humidity;
       data->w[5]=Vin;
       data->w[2]=CHR;
       data->w[1]=vcc;


}





#endif
//
// END OF FILE
//
