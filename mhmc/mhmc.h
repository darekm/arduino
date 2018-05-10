// 
//    FILE:
// VERSION: 0.2.00
// PURPOSE: HMC5883L logger  for imwave
//
//
// HISTORY:
//

// lub:https://github.com/sleemanj/HMC5883L_Simple

#ifndef imMHMC_h
#define imMHMC_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <HMC5883L.h>



HMC5883L_Simple Compass;

uint16_t cpuVin;
uint16_t cpuVinCycle=0;
long averageX=0;
long averageY=0;
long averageZ=0;
#define stepAVG  8L
/*

Below are the connections for a typical Arduino.

    GY-273 Compass Module -> Arduino
    VCC -> VCC (See Note Below)
    GND -> GND
    SCL -> A5/SCL, (Use Pin 21 on the Arduino Mega)
    SDA -> A4/SDA, (Use Pin 20 on the Arduino Mega)
    DRDY -> Not Connected (in this example)
*/
void SetupMHMC()
{
  SetupADC();
 power_twi_enable();

//SetupADC();
 //ShutOffADC();
 power_adc_enable();
  ACSR = 48;                        // disable A/D comparator
//  power_twi_disable();
//  power_adc_disable();
//  ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
 //   DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 
//  pinMode(A4, INPUT_PULLUP);
//    pinMode(A4, OUTPUT);
 //    pinMode(A4, INPUT);
 //   DIDR0 = ~(0x10 ); //ADC4D,
  Wire.begin();
  Compass.SetSamplingMode(COMPASS_SINGLE);
   ShutOffADC();
//  Shutoff
//  Compass.SetScale(COMPASS_SCALE_130);
} 

void PrepareMHMC()
{
 power_adc_enable();
 power_twi_enable();
  Compass.Trigger();    
}
void DataMHMC(IMFrame &frame)
{   
 //  power_adc_enable();
  if (cpuVinCycle % 8==0){
    
  //  SetupADC();
    cpuVin=internalVcc();
    cpuVin=internalVcc();
  //  ShutOffADC();
  ACSR = 48;                        // disable A/D comparator
     power_adc_enable();
  }
  // power_adc_enable();
  //  DIDR0 = ~(0x10 ); //ADC4D,

 // pinMode(A4, INPUT);
   cpuVinCycle++;
  HMC5883L_Simple::MagnetometerSample sample;
   IMFrameData *data =frame.Data();
  sample=Compass.ReadAxes();
 //  float heading = Compass.GetHeadingDegrees();
  
   averageX = (averageX*stepAVG + sample.X );
   averageX = averageX / (stepAVG+1) ;
   averageY=((averageY*stepAVG)+ sample.Y )/(stepAVG+1);
   averageZ=((averageZ*stepAVG)+ sample.Z )/(stepAVG+1);
       data->w[2]=sample.X;
    data->w[3]=sample.Y;
	data->w[4]=sample.Z;
	data->w[5]=averageX & 0xFFFF;
	data->w[6]=averageY & 0xFFFF;
	data->w[7]=averageZ & 0xFFFF;
    data->w[1]=cpuVinCycle;
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
   
    power_adc_disable();
  power_twi_disable();
}





#endif
//
// END OF FILE
//
