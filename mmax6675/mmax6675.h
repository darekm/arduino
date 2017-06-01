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
uint16_t cpuTemp;
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
  digitalWrite(thermoCS,HIGH);
  digitalWrite(vccPin, HIGH);
  digitalWrite(thermoCLK, HIGH);
  delaySleepT2(1);
 
 //   DIDR0 = ~(0x10 ); //ADC4D,
//  pinMode(thermoDO, INPUT); 
    thermo.prepare();
}
void DataMAX6675(IMFrame &frame)
{   
  if (cpuVinCycle % 8==0){
    
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
 
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
   data->w[1]=cpuTemp;
     digitalWrite(vccPin, LOW);
     digitalWrite(thermoCS,LOW);
     digitalWrite(thermoCLK,LOW);
     
 //  scale.power_down();
  power_adc_disable();  
}





#endif
//
// END OF FILE
//
