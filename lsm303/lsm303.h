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
int intCS = 4;//D5


#define MAXTAB 33
LSM303 sensor;

 
int valueX;
int valueY;
int valueZ;
int tabX[MAXTAB];
int tabY[MAXTAB];
int tabZ[MAXTAB];
int tabYMax;
int tabZMax;
int tabXMax;
int tabZMin;
int tabYMin;
int tabXMin;
uint16_t cpuVinCycle=0;

uint16_t cpuVin;
uint16_t cpuTemp;




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
//     sensor.shutdown(); 
}


void ww(){
    delaySleepT2(10);
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

void setupFIFO(){
   sensor.writeReg(LSM303::CTRL0, 0x60);//FIFO EMPTY in1
    sensor.writeReg(LSM303::FIFO_CTRL, 0x3C);//stream
    sensor.writeReg(LSM303::CTRL4, 0x01);//thr int2
//     sensor.writeReg(LSM303::CTRL3, 0x01);//FIFO EMPTY in1
     sensor.writeReg(LSM303::INT_CTRL_M, 0x00);//FIFO EMPTY in1
   
//    sensor.writeReg(sensor.CTRL4, 0x06);//data ready
  // sensor.writeRegister(ADXL345_REG_INT_ENABLE, 0x82);  //ENABLE int DATA READY & WATERMARK
 //  sensor.writeRegister(ADXL345_REG_INT_MAP, 0x80);  
 //  sensor.writeRegister(ADXL345_REG_FIFO_CTL, 0x57);  
}
void SetupLSM303()
{
  do{
    delaySleepT2(200);
//   DBGLEDON();
  power_twi_enable(); 
   power_adc_enable();
     pinMode(intPin1,INPUT_PULLUP);//INT1
     pinMode(intPin2,INPUT_PULLUP);//INT2
    pinMode(intCS,OUTPUT);
    //INT2
digitalWrite(intCS,HIGH);
// Wire.begin();
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

//    digitalWrite(intCS,LOW);
//    DBGLEDOFF();
  }while(true);  
 ww();  sensor.writeReg(LSM303::CTRL0, 0xE0);//Fmemory reset
 ww();  sensor.writeReg(LSM303::CTRL0, 0x60);//Fmemory reset
 ww();   sensor.writeReg(LSM303::IG_CFG1, 0x3F);//threshold
  ww();  sensor.writeReg(LSM303::IG_CFG2, 0x00);//threshold
 ww(); sensor.enableDefault();
 //sensor.writeRegister(ADXL345_REG_POWER_CTL, 0x08);  
 // setupFIFO();
 ww(); setupInertial();
//  setupFIFO();
//  setupClick();
ww();sensor.testDevice();
// ww();
  if ( sensor.testDevice()) {
  //   DBGLEDOFF();
   // break; 
  } else{
//    DBGLEDON();
  }   
 startPulse();
}


void computeMeasure(){
  //         DBGLEDON();
    for(int i=0;i<15;i++){
    }  
      //    DBGLEDOFF();
    SendDataAll();
//      DBGPINLOW();
}


void MeasureLSM303()
{
 // DBGPINHIGH();
  //DBGPINLOW();
  //   power_twi_enable();
 //   DBGPINHIGH(); 
// DBGPINLOW();
 DBGLEDON();

//    delaySleepT2(100);
 // sensor.readReg(LSM303::IG_SRC1);
 //   while (digitalRead(intPin2)==LOW)  {  // dataready PIN int2
  if (digitalRead(intPin2)==LOW)  {  // dataready PIN int2
  
   digitalWrite(intCS,HIGH);
//    sensor.readAcc(); 
   digitalWrite(intCS,LOW);

  }
// DBGLEDOFF();

//    SendDataAll();
    //computeMeasure();
 //   SendDataAll();
   //   DBGPINLOW();
  // DBGLEDON();
   uint8_t src =0;
 //   uint8_t src = sensor.readRegister( ADXL345_REG_INT_SOURCE);
  if (src!=0) {
    //      DBGLEDON();
  }
//  DBGLEDOFF();
  
//  for(int ii=0;ii<4;ii++){
  int ii=0;
 // while (digitalRead(intPin2)==HIGH)  {  // dataready PIN int2
 //   sensor.readXYZ(&tabX[ii],&tabY[ii],&tabZ[ii]);
 {
    if (tabX[ii]>tabXMax) tabXMax=tabX[ii];
    if (tabY[ii]>tabYMax) tabYMax=tabY[ii];
    if (tabZ[ii]>tabZMax) tabZMax=tabZ[ii];
    if (tabX[ii]<tabXMin) tabXMin=tabX[ii];
    if (tabY[ii]<tabYMin) tabYMin=tabY[ii];
    if (tabZ[ii]<tabZMin) tabZMin=tabZ[ii];
  }
 // valueX=sensor.getX();
 // valueY=sensor.getY();
 // valueZ=sensor.getZ();
//          DBGLEDOFF();
//     power_twi_enable();
  
}    

void PrepareLSM303()
{
 // SetupADC();
 //   power_adc_enable(); // ADC converter
 //   ACSR = 48;                        // disable A/D comparator
 //   ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
//    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 //   DIDR1 = 0x00;       
  if ( sensor.testDevice()) {
      DBGLEDOFF();
  }
   if (digitalRead(intPin1)==HIGH){  // dataready PIN int2
  DBGLEDON();
}
 
  byte ffx=sensor.readReg(LSM303::FIFO_SRC);
  byte ff=ffx & 0x1F;
    
  for (int8_t i=ff +1; i>=0; i--)
  {
digitalWrite(intCS,HIGH);
    sensor.readAcc();
digitalWrite(intCS,LOW);
  }  

   sensor.readMag(); 

  if (digitalRead(intPin1)==HIGH){  // dataready PIN int2
  DBGLEDOFF();
}
  DBGLEDOFF();
   
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
  sensor.readReg(LSM303::IG_SRC1);
  sensor.readReg(LSM303::IG_SRC2);
  sensor.readReg(LSM303::STATUS_M);
  sensor.readReg(LSM303::FIFO_SRC);

   
  digitalWrite(intCS,LOW);

/*  cpuVinCycle++;
  if (cpuVinCycle % 3) { 
  if ( sensor.testDevice()) {
     DBGLEDOFF(); 
  } else{
 //   DBGLEDON();
  }   
  }
  DBGLEDOFF();
 */ 
 //   DIDR0 = ~(0x10 ); //ADC4D,
 //  startPulse();
 //     startMeassure();

//     pointer=1;
//  DBGLEDON();
  //       power_twi_enable(); 
// sensor.readXYZ(&valueX,&valueY,&valueZ);
//  valueX=sensor.getX();
//  valueY=sensor.getY();
//  valueZ=sensor.getZ();
  valueX=tabXMax;
  valueY=tabYMax;
  valueZ=tabZMax;
//  sensor.init();
 //  DBGLEDOFF();



 //   uint8_t src = sensor.readRegister( ADXL345_REG_INT_SOURCE);
// DBGLEDOFF();
 //   uint8_t ec = sensor.readRegister( ADXL345_REG_DEVID);
  //  if (ec==0xe5)
   //          DBGLEDON();
// setupTap();
}


void DataLSM303(IMFrame &frame)
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
       	DBGINFO("temp: ");
    for(byte i=0;i<15;i++){
    }

        // data->w[2]=hh;
    data->w[3]=(uint16_t)tabXMax;
    data->w[4]=(uint16_t)tabYMax;
    data->w[5]=(uint16_t)tabZMax;
    data->w[6]=(uint16_t)tabXMin;
    data->w[7]=(uint16_t)tabYMin;
    data->w[8]=(uint16_t)tabZMin;
  tabXMax=-30000;
  tabYMax=-30000;
  tabZMax=-30000;
  tabXMin=30000;
  tabYMin=30000;
  tabZMin=30000;

 //   data->w[7]=cpuVinCycle;
 //   data->w[3]=maxValueCurr;
  //  data->w[2]=maxValueCurr-maxValueLast;
      
  //  }
   // int x= 15;
    //data->w[3]=x;
//    data->w[0]=dataIndex;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
   data->w[1]=cpuTemp;
      
 //  scale.power_down();
 // power_adc_disable();  
}



#endif
//
// END OF FILE
//
