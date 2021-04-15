// 
//    FILE:
// VERSION: 0.9.00
// PURPOSE: modbus controller logger  for imwave
//
//
// HISTORY:
//

#ifndef imELMODBUS_h
#define imELMODBUS_h

#include "imframe.h"
#include "imdebug.h"

#define IDD  0  

#define TXEN 5
#include "modbuslib.h"
  // https://github.com/smarmengol/Modbus-Master-Slave-for-Arduino/blob/master/ModbusRtu.h#L1391


// Setup a oneWire instance to communicate with ANY OneWire devices

//SoftwareSerial wire(wireRX,wireTX);


//HardwareSerial wire=Serial;


Modbus master(IDD,Serial,TXEN);
modbus_t telegram;

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;

uint16_t mData;
uint16_t au16data[32];
uint8_t u8state;

void SetupModbus()
{
  power_usart0_enable();
  pinMode(0,INPUT);
  pinMode(1,OUTPUT);
  pinMode(TXEN,OUTPUT);
 
  //Serial.begin(9600,SERIAL_8N2);//modbus
//  Serial.begin(38400,SERIAL_8N1);//ascii
 
Serial.begin(9600);
master.setTimeOut(20);
master.start();
}


void sendTelegram(){

   // master.poll();
 //digitalWrite( TXEN, HIGH );
  //  digitalWrite( TXEN, LOW );
   telegram.u8id = 1; // slave address
    telegram.u8fct = 4; // function code (this one is registers read)
    telegram.u16RegAdd = 0x400E; // start address in slave
    telegram.u16CoilsNo = 3; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
}

void PrepareModbus()
{
 // wire.write('ab');
 /* 
 Serial.write(7);
 */
  //Serial.write("TEMPTEST05E\r");
 
 // Serial.write("\rTEMP06i\r");
 // Serial.flush();
  //wire.listen();
  sendTelegram();
}  

void DataModbus(IMFrame &frame)
{
  if (cpuVinCycle % 28==0){ 
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
  }
  uint8_t x=master.pollCheck();
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();

   data->w[6]=trx.Connected();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33A;
   mData=au16data[0];
   for (int i=0;i<3;i++){
  //  if (Serial.available()>0) {
  //    char inb=Serial.read();
  //    data->w[i]=byte(inb);
  //  }
      data->w[i+2]=au16data[i];
   }
  if (x==0){
             DBGLEDON();
        DBGLEDOFF();
  }
  if (au16data[1]==91){
             DBGLEDON();
        DBGLEDOFF();
  }
  if (au16data[2]==0xffc6){
             DBGLEDON();
        DBGLEDOFF();
  }
  
  data->w[7]=master.getState();
}





#endif
//
// END OF FILE
//
