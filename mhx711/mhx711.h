// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: hx711 logger  for imwave
//
//
// HISTORY:
//

#ifndef imMHX711_h
#define imMHX711_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <HX711.h>



HX711 scale;

uint16_t cpuVin;
uint16_t cpuVinCycle=0;

void SetupHX711()
{


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

  scale.begin(A4, A5,128);
}

void PrepareHX711()
{
 power_adc_enable();
 scale.power_up();
   
}
void DataHX711(IMFrame &frame)
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
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   unsigned long hh=scale.read();
       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
    data->w[3]=(hh >>16);
    data->w[4]=cpuVinCycle;
  //  data->w[1]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
   scale.power_down();
    power_adc_disable();
}





#endif
//
// END OF FILE
//
