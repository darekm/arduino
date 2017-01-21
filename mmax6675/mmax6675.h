// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: max6675 logger  for imwave
//
//
// HISTORY:
//

#ifndef imMAX6675_h
#define imMAX6675_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <max6675.h>


int thermoDO = 2;
int thermoCS = A4;
int thermoCLK = A5;
int vccPin = A0;

 

MAX6675 thermo (thermoCLK, thermoCS, thermoDO);
uint16_t cpuVin;
uint16_t cpuVinCycle=0;

void SetupMAX6675()
{
   pinMode(vccPin, INPUT); 
  pinMode(vccPin, OUTPUT); 
   digitalWrite(vccPin, LOW);
   thermo.restart();
 //scale.begin(A4, A5,128);
}

void PrepareMAX6675()
{
 // SetupADC();
 //   power_adc_enable(); // ADC converter
 //   ACSR = 48;                        // disable A/D comparator
 //   ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
//    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 //   DIDR1 = 0x00;       
  digitalWrite(vccPin, HIGH);
  digitalWrite(thermoCS,HIGH);
 //   DIDR0 = ~(0x10 ); //ADC4D,
//  pinMode(thermoDO, INPUT); 
    thermo.prepare();
}
void DataMAX6675(IMFrame &frame)
{   
  if (cpuVinCycle % 4==0){
    
    SetupADC();
    cpuVin=internalVcc();
    ShutOffADC();
  }
 // pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   int16_t hh=thermo.read();
       	DBGINFO("temp: ");
         DBGINFO(hh);
       data->w[2]=hh;
 //   data->w[3]=(hh >>16);
    data->w[4]=cpuVinCycle;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
     digitalWrite(vccPin, LOW);
     digitalWrite(thermoCS,LOW);
 //  scale.power_down();
}





#endif
//
// END OF FILE
//
