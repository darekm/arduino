// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: MAX30100heartrate logger  for imwave
//
//
// HISTORY:
//

#ifndef imAdxl345_h
#define imAdxl345_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <ADXL345lib.h>


int intPin1 = 2;//D2  WATERMARK
int intPin2 = 5;//D5 DATA READY - DISABLE dbgpin
int CS = 5;//D5


#define MAXTAB 33
//ADXL345 sensor;
Adafruit_ADXL345_Unified sensor;
//uint8_t maxValueIndex[5]; //wait for 5 measurements
//uint8_t maxValuePointer;

 
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
    attachInterrupt(0,interruptMax,RISING);  //D2 -> INT1
    //sensor.clearInt();
}
void stopPulse(){
     detachInterrupt(0);    
//     sensor.shutdown(); 
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


void setupFIFO(){
   sensor.writeRegister(ADXL345_REG_INT_ENABLE, 0x82);  //ENABLE int DATA READY & WATERMARK
   sensor.writeRegister(ADXL345_REG_INT_MAP, 0x80);  
   sensor.writeRegister(ADXL345_REG_FIFO_CTL, 0x57);  
}
void setupTap(){
   sensor.writeRegister(ADXL345_REG_THRESH_TAP, 0x01);  
   sensor.writeRegister(ADXL345_REG_DUR, 0x01);  
   sensor.writeRegister(ADXL345_REG_TAP_AXES, 0x07);  
   sensor.writeRegister(ADXL345_REG_THRESH_ACT, 0x21);  
   sensor.writeRegister(ADXL345_REG_THRESH_INACT, 0x03);  
   sensor.writeRegister(ADXL345_REG_ACT_INACT_CTL, 0x77);  
 //  sensor.writeRegister(ADXL345_REG_FIFO_CTL, 0x41);  
   sensor.writeRegister(ADXL345_REG_INT_ENABLE, 0x70);  
 //  sensor.writeRegister(ADXL345_REG_INT_ENABLE, 0x68);  
  
}
void SetupADXL345()
{
//  DBGLEDON();
   power_twi_enable(); 
   power_adc_enable();
     pinMode(intPin1,INPUT_PULLUP);//INT1
     pinMode(intPin2,INPUT_PULLUP);//INT2

// Wire.begin();
 // sensor.init();
//  pinMode(CS,OUTPUT);
//   digitalWrite(CS,HIGH);
  if (! sensor.begin()) 
     DBGLEDON(); 
    ;
 sensor.writeRegister(ADXL345_REG_POWER_CTL, 0x08);  
  setupFIFO();
 startPulse();
}


void computeMeasure(){
  //         DBGLEDON();
    for(int i=0;i<15;i++){
  // DBGPINLOW();
           
//   DBGPINHIGH();
        
    }
    
    
      //    DBGLEDOFF();
     
    SendDataAll();
//      DBGPINLOW();
}


void MeasureADXL345()
{
 // DBGPINHIGH();
  //DBGPINLOW();
     power_twi_enable();
 //   DBGPINHIGH(); 
// DBGPINLOW();
// DBGPINHIGH();

//    DBGPINHIGH();
  //  DBGPINLOW();
  //   DBGPINHIGH();

//    SendDataAll();
    //computeMeasure();
 //   SendDataAll();
   //   DBGPINLOW();
   DBGLEDON();
    uint8_t src = sensor.readRegister( ADXL345_REG_INT_SOURCE);
  if (src!=0) {
          DBGLEDON();
  }
//  DBGLEDOFF();
  
//  for(int ii=0;ii<4;ii++){
  int ii=0;
  while (digitalRead(intPin2)==HIGH)  {  // dataready PIN int2
    sensor.readXYZ(&tabX[ii],&tabY[ii],&tabZ[ii]);
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
          DBGLEDOFF();
//     power_twi_enable();
  
}    

void PrepareADXL345()
{
 // SetupADC();
 //   power_adc_enable(); // ADC converter
 //   ACSR = 48;                        // disable A/D comparator
 //   ADCSRA = (1<<ADEN)+7;                     // ADPS2, ADPS1 and ADPS0 prescaler
//    DIDR0 = 0x00;                           // disable all A/D inputs (ADC0-ADC5)
 //   DIDR1 = 0x00;       
  
 //   DIDR0 = ~(0x10 ); //ADC4D,
 //  startPulse();
 //     startMeassure();

//     pointer=1;
  DBGLEDON();
  //       power_twi_enable(); 
// sensor.readXYZ(&valueX,&valueY,&valueZ);
//  valueX=sensor.getX();
//  valueY=sensor.getY();
//  valueZ=sensor.getZ();
  valueX=tabXMax;
  valueY=tabYMax;
  valueZ=tabZMax;
   DBGLEDOFF();

 //   uint8_t src = sensor.readRegister( ADXL345_REG_INT_SOURCE);
// DBGLEDOFF();
 //   uint8_t ec = sensor.readRegister( ADXL345_REG_DEVID);
  //  if (ec==0xe5)
   //          DBGLEDON();
// setupTap();
}


void DataADXL345(IMFrame &frame)
{  
  if (cpuVinCycle % 8==0){
    
    SetupADC();
    cpuVin=internalVcc();
   cpuTemp=internalTemp();
    cpuTemp=internalTemp();
 //    cpuTemp=internalTemp();
//    cpuTemp=internalTemp();
 
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
