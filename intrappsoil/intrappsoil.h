// 
//    FILE:
// VERSION: 0.9.00
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
#define SOIL_BUS A5



IMSht2x		imSht2x;
IMCharger	imCharger;

uint16_t temperature;
uint16_t humidity;

#define  VCC_PIN PinAkku

void SetupSOIL()
{
  pinMode(SOIL_BUS,INPUT);
  DBGINFO("bus:");
  DBGINFO(SOIL_BUS);
}


void SetupIntrappFlex()
{
        pinMode(SOIL_BUS,INPUT);
        
	pinMode(EnableI2C, OUTPUT);
	pinMode(EnableSHT, OUTPUT);
	pinMode(EnableExtPower, OUTPUT);
	digitalWrite(EnableExtPower, HIGH);
	//pinMode(EnableRS485, OUTPUT);
	digitalWrite(EnableSHT, HIGH);
	digitalWrite(EnableI2C, HIGH);
	//digitalWrite(EnableRS485, LOW);
	pinMode(EnableAkku, OUTPUT);
	digitalWrite(EnableAkku, LOW);
       imCharger.Init();
       pinMode(VCC_PIN,INPUT);

}

/*
//https://tkkrlab.nl/wiki/Arduino_KY-013_Temperature_sensor_module
int sensorPin = A5; // select the input pin for the potentiometer

double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
   //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
   return Temp;
}

void setup() {
 Serial.begin(9600);
}

void loop() {
 int readVal=analogRead(sensorPin);
 double temp =  Thermistor(readVal);

 Serial.println(temp);  // display tempature
 //Serial.println(readVal);  // display tempature

 delay(500);
}
*/

void PrepareIntrappFlex(){
	temperature = imSht2x.GetTemperatureBin();
	humidity = imSht2x.GetHumidityBin();
	digitalWrite(EnableExtPower, LOW);

}

bool DataSOIL()
{
//   IMFrameData *data =frame.Data();
   uint16_t soil=analogRead(SOIL_BUS);
   uint16_t Vin=internalVcc();
   uint16_t Tin=internalTemp();
    DBGINFO("|");
    DBGINFO(soil);
    DBGINFO("|");
    DBGINFO(Vin);
    DBGINFO("|");
    DBGINFO(Tin);
//   data->w[0]=soil;
//   data->w[1]=Vin;
//   data->w[2]=Tin;

    return true;


}


void DataIntrappFlex(IMFrame &frame)
{
   IMFrameData *data =frame.Data();
        uint16_t soil=analogRead(SOIL_BUS);
	digitalWrite(EnableExtPower, HIGH);

        uint16_t vcc=analogRead(VCC_PIN);
        uint16_t Vin=internalVcc();
        uint16_t CHR=(imCharger.IsChrg() <<4);

          CHR|=imCharger.GetVinStat();
        
//	float Temperature = imSht2x.GetTemperature();
//	float Humidity = imSht2x.GetHumidity();
//      	DBGINFO("temp: ");
//	DBGINFO(imSht2x.GetTemperature());
//	DBGINFO(" hum: ");
//	DBGINFO(Humidity);
	DBGINFO(" vin: ");
	DBGINFO(Vin);
	DBGINFO(" CHR: ");
	DBGINFO(CHR);
	DBGINFO(" vcc: ");
	DBGINFO(vcc);
        DBGINFO("|");
	DBGINFO(" soil: ");
        DBGINFO(soil);

	DBGINFO(" \r\n");
         
       data->w[2]=temperature;
       data->w[3]=humidity;
       data->w[4]=soil;
       data->w[5]=Vin;
       data->w[1]=CHR;
       data->w[0]=vcc;


}





#endif
//
// END OF FILE
//
