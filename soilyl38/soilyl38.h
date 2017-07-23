// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: soil logger  for imwave
//
//
// HISTORY:
//

#ifndef imSOILYL38_h
#define imSOILYL38_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#define SOIL_BUS A5
#define SOIL_VCC 2

uint16_t cpuVin;
uint16_t cpuVinCycle=0;

void SetupSOIL()
{


//SetupADC();
 //ShutOffADC();
 power_adc_enable();
//  ACSR = 48;                        // disable A/D comparator
//  ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 
      pinMode(SOIL_VCC, OUTPUT);
pinMode(SOIL_BUS, INPUT_PULLUP);
    pinMode(SOIL_BUS, OUTPUT);
     pinMode(SOIL_BUS, INPUT);
    DIDR0 = ~(0x30 ); //ADC4D,

}

void PrepareSOIL()
{
 power_adc_enable();
  digitalWrite(SOIL_VCC,HIGH);
}

void DataSOIL(IMFrame &frame)
{   
 //  power_adc_enable();
  if (cpuVinCycle % 4==0){
     SetupADC();
     cpuVin=internalVcc();
     ShutOffADC();
  }
   power_adc_enable();
  //  DIDR0 = ~(0x10 ); //ADC4D,

 // pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
   uint16_t hh=   analogRead(SOIL_BUS);

       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
    data->w[4]=cpuVinCycle;
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
    digitalWrite(SOIL_VCC,LOW);
  power_adc_disable();
}





#endif
//
// END OF FILE
//
