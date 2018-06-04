// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: blow fuse monitoring
//
//
// HISTORY:
//

#ifndef imFuseGate_h
#define imFuseGate_h


#include "imframe.h"
#include "imdebug.h"
#include "imatmega.h"


#define pinA1 A0
#define pinA2 A1
#define pinA3 A2
#define pinVAD A5
#define FPC1 0
#define FPC2 1
#define FPC3 2
#define macCount 15

uint16_t current;


uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
uint16_t idx1,idx2,idx3;
IMMAC macTable[macCount];
long  macCycle[macCount];

#define ADCVHIGH() PORTD|=(B00100000);//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  PORTD&=~(B00100000);//digitalWrite(DBGPIN,LOW)


byte findTable(IMMAC mac){
 byte ii=0;
// long ic=mac
 for(byte i=0;i<macCount;i++){
    if (macTable[i]==mac){
      return i;
    }
    if (macCycle[i]==0){
      ii=i;
    }
 }
 macTable[ii]=mac;
 return ii;
}

void LightOn(){
   digitalWrite(FPC1,HIGH);
};

void SetupSensor()
{
  DBGLEDON();
  delay(100);
  SetupADC();


  pinMode(pinA1,INPUT);
  pinMode(pinA2,INPUT);
  pinMode(pinA3,INPUT);
  current=0;

   DBGLEDOFF();
   ShutDownADC();
}

void MeasureSensor()
{

}


void MeasureVCC(){
    SetupADC();
    cpuVin=internalVcc();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
}

bool ParseSensor(IMFrame &frame){
  IMFrameData *data =frame.Data();
  byte ii=findTable(data->w[6]);
  macCycle[ii]=trx.timer.Cycle();
  if (data->w[2]!=data->w[3]){
    LightOn();
  }

}
void DataSensor(IMFrame &frame)
{
  long xSum1=0;
  long xSum2=0;
  long xSum3=0;
  byte xLast =0;
   
   // adcMedium=(adcMedium *9 +xSum/81)/10;


   IMFrameData *data =frame.Data();


//        DBGINFO(ex);
   current++;

  
   data->w[5]=0xACAC;
   data->w[4]=idx3;
   data->w[3]=idx2;
   data->w[2]=idx1;
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;


  if (cpuVinCycle % 18==2){
      MeasureVCC();
  }
  cpuVinCycle++;
}





#endif
//
// END OF FILE
//
