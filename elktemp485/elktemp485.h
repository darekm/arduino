// 
//    FILE:
// VERSION: 0.9.00
// PURPOSE: modbus controller logger  for imwave
//
//
// HISTORY:
//

#ifndef imELKT_h
#define imELKT_h

#include "imframe.h"
#include "imdebug.h"

#define wireTX 11  
#define wireRX 10

//#include "SoftwareSerial.h"

// Setup a oneWire instance to communicate with ANY OneWire devices

//SoftwareSerial wire(wireRX,wireTX);


//HardwareSerial wire=Serial;

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;

void SetupELKT()
{
  power_usart0_enable();
 // wire.begin(9600,SERIAL_8N2);//modbus
  wire.begin(38400,SERIAL_8N1);//ascii
}

void PrepareELKT()
{
 // wire.write('ab');
 /* 
 Serial.write(7);
 Serial.flush(); 
 Serial.write(4);
 Serial.flush(); 
 Serial.write(0);
 Serial.flush();
  Serial.write(0);
 Serial.flush(); 
 Serial.write(0);
 Serial.flush(); 
 Serial.write(1);
 Serial.flush(); 
 Serial.write(0x31);
 Serial.flush(); 
 Serial.write(0xAC);
 Serial.flush(); 
 */
  //Serial.write("TEMPTEST05E\r");
 
  Serial.flush();
  //wire.listen();
//   sensors.requestTemperatures();
}  


void DataELKT(IMFrame &frame)
{   
  if (cpuVinCycle % 28==0){ 
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
  }
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();

   data->w[6]=trx.Connected();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33A;
   for (int i=2;i<10;i++){
    if (wire.available()>0) {
      char inb=wire.read();
      data->w[i]=byte(inb);
    }
   }
}





#endif
//
// END OF FILE
//
