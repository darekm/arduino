// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: SHT31 logger  for imwave
//
//
// HISTORY:
//

#ifndef imSHT31_h
#define imSHT31_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"

#include <Wire.h>
#include <SHT31lib.h>

// Setup a oneWire instance to communicate with ANY OneWire devices



// Set oneWire reference to Dallas Temperature sensor.
SHT31 sensor;

#define SHT31_ADDR    0x45  //  breakboard =0x44

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=1;

uint16_t SetupSHT31()
{
   ShutOffADC(); 
  power_usart0_disable();
//  pinMode(ONE_WIRE_BUS,INPUT_PULLUP);
power_twi_enable();//
power_adc_disable();
power_timer1_disable();
ACSR=0;
   ADCSRA = 0;                        // disable A/D comparator
   DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
  sensor.begin(SHT31_ADDR);
  
  
  return  sensor.readStatus();
 
}

void PrepareSHT31()
{
  power_twi_enable();
 // power_adc_enable();
  sensor.start();
}  


void DataSHT31(IMFrame &frame)
{   
  if (cpuVinCycle % 8==0){ 
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
  //  cpuTemp=internalTemp();
  //  ShutOffADC();
  ACSR=0;
   ADCSRA = 0;                        // disable A/D comparator

    power_adc_disable();
  }
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
   data->w[9]=sensor.readTempHum();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   
       	DBGINFO("temp: ");
        DBGINFO(" ");
        
       data->w[2]=sensor.Temp;
       data->w[3]=sensor.Humidity;
   data->w[6]=trx.Connected();
 //  Vin=internalVcc();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33A;
power_twi_disable();//
}





#endif
//
// END OF FILE
//
