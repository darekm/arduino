// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: LSM303D acc and magnetic logger  for imwave
//
//
// HISTORY:
//

#ifndef imLSM303_h
#define imLSM303_h

#include "imframe.h"
#include "imdebug.h"
#include <LSM303D.h>


int intPin1 = 2;//D2  INT2 WATERMARK
int intPin2 = 5;//D5 INT 1 DATA READY - DISABLE dbgpin
//int intCS = 4;//D5


#define MAXTAB 3
LSM303 sensor;

 
int tabX[MAXTAB];
int tabY[MAXTAB];
int tabZ[MAXTAB];
int16_t tabXMax;
int16_t tabYMax;
int16_t tabZMax;
int16_t tabMGX;
int16_t tabMGY;
int16_t tabMGZ;
int tabXMin;
int tabYMin;
int tabZMin;
uint16_t cpuVinCycle=0;

uint16_t cpuVin;
uint16_t cpuTemp;

float dttt;
//float dvx,dvx0,ddx,ddx0;
//float dvy,dvy0;
//float dvz,dvz0;
//byte dataCount=0;
byte stepLSM=0;
byte  maxLSM=0xF0;
bool enabledLSM =false;


void interruptMax(){//interrupt function
  IMTimer::doneMeasure();
}

void startPulse(){ //run settings for heart rate
  //  attachInterrupt(0,interruptMax,FALLING);  //D2 -> INT1
    attachInterrupt(0,interruptMax,RISING);  //D2 -> INT1
    //sensor.clearInt();
}
void stopPulse(){
     detachInterrupt(0);    
}

void ww(){
    delaySleepT2(10);
}


void DisableLSM(){
DBGLEDOFF();
  ww();     sensor.writeReg(LSM303::CTRL1, 0x00);//0 - PWR down
  ww();    sensor.writeReg(LSM303::CTRL7, 0x03);//magnetic power down
enabledLSM=false;
DBGLEDON();
DBGLEDOFF();

}

void EnableLSM(){
//  if (stepLSM>0){
//     stepLSM=0;
//  } else {
  if (!enabledLSM){
 //   stepLSM=0;
   power_twi_enable();
 DBGLEDOFF();
 DBGLEDON();
 enabledLSM=true;
 DBGLEDOFF();
    ww();      sensor.writeReg(LSM303::CTRL1, 0x47);//3=12.5Hz  4=25Hz  ** 7=xyz 1=x 4 =z
      DBGLEDON();
  ww();    sensor.writeReg(LSM303::CTRL7, 0x24);//magnetic power up LOWPOWER  +filter
power_twi_disable();
      DBGLEDOFF();
  }
}

void CheckDisableLSM(){
   stepLSM++;
   if (stepLSM>maxLSM) {
     stepLSM=0;
     DisableLSM();
   }
}

void CheckModeLSM(uint16_t aMode){
 uint8_t xCycle= aMode & 0xFF;
 maxLSM=0xF0;
 if (xCycle>=1){
   maxLSM=1;
 }
} 



void SendDataAll()
{
//        DBGLEDON();
        static IMFrame frame;
        frame.Reset();
        IMFrameData *data =frame.Data();
        uint16_t xl=0;//sensor.dataContainer[0];
        uint16_t xm;
        data->w[1]=xl;
    for(byte i=1;i<15;i++){
//      frame.Body[i+7]=ValueStep[i];
        xm=1;//sensor.dataContainer[i]; 
        int8_t tt=xl-xm;
        frame.Body[i+7]=(uint8_t)tt;
        xl=xm;
    }
               DBGLEDON();
  trx.Wakeup();
          trx.SendData(frame);
               DBGLEDOFF();
}


void setupClick(){
    sensor.writeReg(sensor.CLICK_CFG, 0x3F);//enable single click on XYZ
    sensor.writeReg(sensor.CLICK_THS, 0x01);//threshold
    sensor.writeReg(sensor.TIME_LIMIT, 0x01);//enable single click on XYZ
    sensor.writeReg(LSM303::TIME_LATENCY , 0x0F);//threshold
    sensor.writeReg(LSM303::TIME_WINDOW, 0x1F);//threshold
    sensor.writeReg(LSM303::CTRL4, 0x80);//threshold
  //  sensor.writeReg(sensor.CTRL0, 0x04);//data ready
}

void setupInertial(){
//    sensor.writeReg(LSM303::IG_CFG1, 0x20);//threshold
 ww();   sensor.writeReg(LSM303::IG_THS1, 0x03);//threshold
//    sensor.writeReg(LSM303::CTRL4, 0x01);//threshold
 //   sensor.writeReg(LSM303::CTRL3, 0x02);//threshold
     sensor.writeReg(LSM303::CTRL1, 0x27);

 ww(); sensor.writeReg(LSM303::FIFO_CTRL, 0x46);//stream
ww();    sensor.writeReg(LSM303::CTRL4, 0x02);//thr int2
   //   sensor.writeReg(LSM303::CTRL5, 0x6D);//threshold

}

void setupInertialPCB(){
//    sensor.writeReg(LSM303::IG_CFG1, 0x20);//threshold
 ww();   sensor.writeReg(LSM303::IG_THS1, 0x03);//threshold
//     sensor.writeReg(LSM303::CTRL1, 0x27);//ACC rate

 ww(); sensor.writeReg(LSM303::FIFO_CTRL, 0x4F);//stream +threshold
ww();    sensor.writeReg(LSM303::CTRL2, 0xD0);//alias filter
ww();    sensor.writeReg(LSM303::CTRL3, 0x04);//thr int1
ww();    sensor.writeReg(LSM303::CTRL4, 0x01);//dataready int2 

   ww();      sensor.writeReg(LSM303::CTRL1, 0x47);//3=12.5Hz  4=25Hz  ** 7=xyz 1=x 4 =z
 
}


void setupFIFO(){
   sensor.writeReg(LSM303::CTRL0, 0x60);//FIFO EMPTY in1
    sensor.writeReg(LSM303::FIFO_CTRL, 0x3C);//stream
    sensor.writeReg(LSM303::CTRL4, 0x01);//thr int2
//     sensor.writeReg(LSM303::CTRL3, 0x01);//FIFO EMPTY in1
     sensor.writeReg(LSM303::INT_CTRL_M, 0x00);//FIFO EMPTY in1
   
//    sensor.writeReg(sensor.CTRL4, 0x06);//data ready
}
void SetupLSM303()
{
  stepLSM=0;
  do{
    delaySleepT2(200);
   DBGLEDOFF();
  power_twi_enable(); 
   power_adc_enable();
     pinMode(intPin1,INPUT_PULLUP);//INT1
     pinMode(intPin2,INPUT_PULLUP);//INT2
//    pinMode(intCS,OUTPUT);
    //INT2
//digitalWrite(intCS,HIGH);
  ww();sensor.testDevice();
  ww(); sensor.init(sensor.device_D,LSM303::sa0_high);
//  pinMode(CS,OUTPUT);
//   digitalWrite(CS,HIGH);
  ww();
  if ( sensor.testDevice()) {
  //   DBGLEDOFF();
    break; 
  } else{
    DBGLEDON();
  }   
    ;
  if ( sensor.testDevice()) {
  //   DBGLEDOFF();
    break; 
  } else{
    DBGLEDON();
  }   
    ;

  }while(true);  
  
     DBGLEDON();
 
 ww();  sensor.writeReg(LSM303::CTRL0, 0xE0);//Fmemory reset
 ww();  sensor.writeReg(LSM303::CTRL0, 0x60);//Fmemory reset
 ww();   sensor.writeReg(LSM303::IG_CFG1, 0x3F);//threshold
  ww();  sensor.writeReg(LSM303::IG_CFG2, 0x00);//threshold
 ww(); sensor.enableDefault();
 // setupFIFO();
  ww(); setupInertialPCB();
//  setupFIFO();
//  setupClick();
  ww();sensor.testDevice();
  EnableLSM();
    delaySleepT2(300);
    DBGLEDOFF();
//dttt=0.1;  
 startPulse();
}

void ComputeMax(){
    if (sensor.a.x>tabXMax) tabXMax=sensor.a.x;
    if (sensor.a.y>tabYMax) tabYMax=sensor.a.y;
    if (sensor.a.z>tabZMax) tabZMax=sensor.a.z;
    if (sensor.a.x<tabXMin) tabXMin=sensor.a.x;
    if (sensor.a.y<tabYMin) tabYMin=sensor.a.y;
    if (sensor.a.z<tabZMin) tabZMin=sensor.a.z;
}


void ReadLSM303(){
  power_twi_enable();
  byte ffx=sensor.readReg(LSM303::FIFO_SRC);
  byte ff=ffx & 0x1F;
 // byte ff=4;  
  for (int8_t i=ff ; i>=0; i--)
  {
//DBGLEDON();
    if (sensor.readAcc()==6){
      
       ComputeMax();
   //    ComputeDist();
    }
//      DBGLEDOFF();
  }  

   sensor.readMag(); 
   tabMGX=sensor.m.x;
   tabMGY=sensor.m.y;
   tabMGZ=sensor.m.z;
//      DBGLEDOFF();
   CheckDisableLSM();
   power_twi_disable();
}

void computeMeasure(){
  //         DBGLEDON();
      //    DBGLEDOFF();
//    SendDataAll();
//      DBGPINLOW();
}


void MeasureLSM303()
{
// DBGLEDON();
  ReadLSM303();
//  DBGLEDOFF();
//    delaySleepT2(100);
 // sensor.readReg(LSM303::IG_SRC1);
 //   while (digitalRead(intPin2)==LOW)  {  // dataready PIN int2
/*  if (digitalRead(intPin2)==LOW)  {  // dataready PIN int2
  
   digitalWrite(intCS,HIGH);
//    sensor.readAcc(); 
   digitalWrite(intCS,LOW);

  }
  */
  
//  for(int ii=0;ii<4;ii++){
  int ii=0;
 // while (digitalRead(intPin2)==HIGH)  {  // dataready PIN int2
 //   sensor.readXYZ(&tabX[ii],&tabY[ii],&tabZ[ii]);
//          DBGLEDOFF();
//     power_twi_enable();
  
}    



void PrepareLSM303()
{
   if (digitalRead(intPin2)==HIGH){  // dataready PIN int2
//     DBGLEDON();
   }
//ReadLSM303(); 
//  DBGLEDOFF();
   
//  while (digitalRead(intPin2)==HIGH)  {  // dataready PIN int2
/*
  if (digitalRead(intPin2)==HIGH)  {  // dataready PIN int2
  DBGLEDON();
    sensor.readMag(); 
   DBGLEDOFF();
  } else{
//    sensor.writeReg(LSM303::CTRL0, 0x80);//threshold
  }
  */

}


void DataLSM303(IMFrame &frame)
{  
  if (cpuVinCycle % 18==2){
       SetupADC();
    cpuVin=internalVcc();
   cpuTemp=internalTemp();
    cpuTemp=internalTemp();
     ShutOffADC();
  }
 // pinMode(A4, INPUT);
   cpuVinCycle++;  
   IMFrameData *data =frame.Data();
  
        // data->w[2]=hh;
    data->w[3]=(uint16_t)tabXMax;
    data->w[4]=(uint16_t)tabYMax;
    data->w[5]=(uint16_t)tabZMax;
    data->w[6]=(uint16_t)tabXMin;
    data->w[7]=(uint16_t)tabYMin;
    data->w[8]=(uint16_t)tabZMin;
    data->w[9]=(uint16_t)tabMGX;
    data->w[10]=(uint16_t)tabMGY;
    data->w[11]=(uint16_t)tabMGZ;
//     data->w[11]=round((ddx-ddx0)*1000);
//     ddx0=ddx;
  tabXMax=-30000;
  tabYMax=-30000;
  tabZMax=-30000;
  tabXMin=30000;
  tabYMin=30000;
  tabZMin=30000;

      
   data->w[0]=cpuVin;
   data->w[1]=cpuTemp;
}



#endif
//
// END OF FILE
//
