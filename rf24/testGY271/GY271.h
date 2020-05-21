// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: acs720 logger  for current
//
//
// HISTORY:
//

#ifndef imGY271_h
#define imGY271_h


#include "imframe.h"
#include "imdebug.h"

#include "HMC5883L.h"


#define pinACS A4
#define pinVAD A5



uint16_t cpuVin;
uint16_t cpuVinCycle=0;

HMC5883L_Simple Compass;



 
   #define ADCVHIGH() PORTC|=(B00100000);//digitalWrite(DBGPIN,HIGH)
   #define ADCVLOW()  PORTC&=~(B00100000);//digitalWrite(DBGPIN,LOW)


/*

Below are the connections for a typical Arduino.

    GY-273 Compass Module -> Arduino
    VCC -> VCC (See Note Below)
    GND -> GND
    SCL -> A5/SCL, (Use Pin 21 on the Arduino Mega)
    SDA -> A4/SDA, (Use Pin 20 on the Arduino Mega)
    DRDY -> Not Connected (in this example)
*/
void SetupGY271()
{
 power_twi_enable();

//SetupADC();
 //ShutOffADC();
 power_adc_enable();
  ACSR = 48;                        // disable A/D comparator
//  ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)

  pinMode(A4, INPUT_PULLUP);
    pinMode(A4, OUTPUT);
     pinMode(A4, INPUT);
    DIDR0 = ~(0x10 ); //ADC4D,
  Wire.begin();
//  Compass.SetSamplingMode(COMPASS_SINGLE);
//  Compass.SetScale(COMPASS_SCALE_130);
}




void PrepareGY271()
{
// power_adc_enable();
  Compass.Trigger();
}
void DataGY271(IMFrame &frame)
{
 //  power_adc_enable();
  if (cpuVinCycle % 4==0){

  //  SetupADC();
  //  cpuVin=internalVcc();
  //  ShutOffADC();
  }
  // power_adc_enable();
  //  DIDR0 = ~(0x10 ); //ADC4D,

 // pinMode(A4, INPUT);
   cpuVinCycle++;
  HMC5883L_Simple::MagnetometerSample sample;
   IMFrameData *data =frame.Data();
  sample=Compass.ReadAxes();
 //  float heading = Compass.GetHeadingDegrees();
//   bool ex=sensors.getAddress(deviceAddress, 0);
 //  unsigned long hh=scale.read();
  //       DBGINFO(hh);
       data->w[2]=sample.X;
    data->w[3]=sample.Y;
	data->w[4]=sample.Z;
    data->w[5]=cpuVinCycle;
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;

  //  power_adc_disable();
}





#endif
//
// END OF FILE
//
