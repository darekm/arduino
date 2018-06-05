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
#define LEDB1 1
#define LEDB2 6
// DBGCLOCK 6
#define LEDB3 7

#define macCount 15

uint16_t current;


uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;
uint16_t idx1,idx2,idx3;
IMMAC macTable[macCount];
long  macCycle[macCount];
byte  macFuse[macCount];
int fusesOn=0;
int lightOn=0;
#define ADCVHIGH() PORTD|=(B00100000);//digitalWrite(DBGPIN,HIGH)
#define ADCVLOW()  PORTD&=~(B00100000);//digitalWrite(DBGPIN,LOW)

void resetTable(){
  for (byte i=0;i<macCount;i++){
    macTable[i]=0;
    macFuse[i]=0;
  }
}

byte findTable(IMMAC mac){
 byte ii=0;
// long ic=mac
 for(byte i=0;i<macCount;i++){
    if (macTable[i]==mac){
      return i;
    }
    if (macCycle[i]==0){
      ii=i;
      macFuse[ii]=0;
    }
 }
 macTable[ii]=mac;
 return ii;
}

void LightOn(){
   digitalWrite(LEDB1,HIGH);
   fusesOn++;
};

void SetupSensor()
{
 pinMode(LEDB1, OUTPUT);
  pinMode(LEDB2, OUTPUT);
 pinMode(LEDB3, OUTPUT);
// analogWrite(9, 100);
//  analogWrite(LEDB1, 100);
//  digitalWrite(7, 100);
 // if (funtest()>0){
  digitalWrite(LEDB1, HIGH);
  digitalWrite(LEDB2, HIGH);
  digitalWrite(LEDB3, HIGH);
  delay(300);
   digitalWrite(LEDB2, LOW);
  delay(200);
   digitalWrite(LEDB2, HIGH);
  delay(300);
   digitalWrite(LEDB2, LOW);
  delay(600);
 digitalWrite(LEDB1, LOW);
  digitalWrite(LEDB3, LOW);
 

  DBGLEDON();
  delay(100);
  SetupADC();


  pinMode(pinA1,INPUT);
  pinMode(pinA2,INPUT);
  pinMode(pinA3,INPUT);

   DBGLEDOFF();
   ShutDownADC();
}

void MeasureSensor()
{
 if (fusesOn) {
    lightOn=10;
 } else{
    lightOn--;
 }
 fusesOn=0;
 if (lightOn<1){
    lightOn=0;
    digitalWrite(LEDB1,LOW);
 }
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
  digitalWrite(LEDB3, HIGH);
  IMFrameData *data =frame.Data();
  byte ii=findTable(data->w[6]);
  macCycle[ii]=trx.timer.Cycle();
  if (data->w[2]!=data->w[3]){
    LightOn();

  }
  macFuse[ii]=data->w[2];

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
