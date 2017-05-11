// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: MAX30100heartrate logger  for imwave
//
//
// HISTORY:
//

#ifndef imHeartRate_h
#define imHeartRate_h

//#include "Arduino.h"
#include "imframe.h"
#include "imdebug.h"


#include <heartRateLib.h>


int intPin = 2;//D2
int thermoCS = A4;
int thermoCLK = A5;
int vccPin = A0;

#define ValueSize 8
#define StepSize 16
MAX30100 sensor;
//uint8_t maxValueIndex[5]; //wait for 5 measurements
//uint8_t maxValuePointer;
uint16_t maxValueLast;
uint16_t maxValueCurr;
uint16_t Value[ValueSize]; 
uint16_t last=0; //last average
float average;
float averageStep;
float stepDown;
float Step[StepSize];
int stepIndex;
int wait=0;
byte sendStore=0;
byte sendPop=0;
byte was;
int pointer=0; //what do next
int meassureMode=0; //looking for falling in avarge
uint16_t dataIndex; //index or every mesurment came in

 

uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;



//void saveMaxValue(){
//  maxValuePointer[maxValueIndex]=dataIndex;
//  maxValuePointer++;//
//}

void ifBigger(){ //check if avarge is rising or falling
 // pointer=1;
 if(wait > 0){
   last=average+1;
 }
 else{
  if(meassureMode==0){
    if(average > last){
      //Serial.println("Start finding max value");
      meassureMode=1;//looking for loss
    }
  }
  else{
    if(average < last){
      DBGLEDON();
      meassureMode=0;
      maxValueLast=maxValueCurr;
      maxValueCurr=dataIndex;
      sendStore++;
//      Value[sendStore %ValueSize]=maxValueCurr-maxValueLast;
      Value[1]=maxValueCurr-maxValueLast;
      wait=32;
      DBGLEDOFF();
   //   dataIndex=0;
//      saveMaxValue(); //index of that measurement
    }
  }
  last=average;
 }
 wait--;
}


void computeStep(){
  if (stepDown>averageStep)
       averageStep=averageStep*0.9+stepDown*0.1;
  else
     averageStep=averageStep*0.7+stepDown*0.3;
}
        
void ifCross(){
  
}  
        
void computeMeasure(){
    for(int i=0;i<15;i++){
      dataIndex++;
      stepIndex++;
      stepIndex%=16;
      average= average*0.9+sensor.dataContainer[i]*0.1;
      stepDown=average -Step[stepIndex];
      Step[stepIndex]=averageStep;
      computeStep();
      
     // average= average*0.99+sensor.dataContainer[i]*0.01;
      ifCross();
    }
}



void interruptMax(){//interrupt function
DBGPINHIGH();
  
//  pointer=2;
  IMTimer::doneMeasure();
//  DBGPINLOW();
}

void startMeassure(){
  dataIndex=0;
  //stopIndex=0;
 // maxValuePointer=0;
  average=sensor.dataContainer[0];
  last=average+1;
}
void startPulse(){ //run settings for heart rate
    attachInterrupt(0,interruptMax,FALLING);
    sensor.IRsettings();
    //sensor.clearInt();
    sensor.clearFIFO();
}
void stopPulse(){
     detachInterrupt(0);    
     sensor.shutdown(); 
}
void whatNext(){ //modes of working
  if(pointer == 0){ //start
     startPulse();
 }
  if(pointer == 2){ //after interrupt
    pointer=1;
    sensor.clearInt();
    sensor.readFullFIFO();
    computeMeasure();
  }
  if(pointer == 3){
    stopPulse();
//     for(int i=1;i<5;i++){
 //      //heart rate(by index of sample);
 //     Serial.println(maxValueIndex[i]-maxValueIndex[i-1]); 
 //    }
  
  }
   
}

void SendDataAll()
{
        DBGLEDON();
        static IMFrame frame;
        frame.Reset();
        IMFrameData *data =frame.Data();
    for(byte i=0;i<10;i++){
      data->w[i]=sensor.dataContainer[i];
    }
    int8_t xx,x2;
    xx=sensor.dataContainer[2]-sensor.dataContainer[1];
    x2=sensor.dataContainer[3]-sensor.dataContainer[2];
    data->w[11]=((uint8_t)x2 <<8) | xx;
           DBGLEDOFF();
         trx.SendData(frame);
}

void SetupMAX30100()
{
  DBGLEDON();
   power_twi_enable(); 
   power_adc_enable();
 Wire.begin();
  pinMode(intPin,INPUT_PULLUP);
  //getPulse();
//  sensor.shutdown();
    startPulse();
      startMeassure();
      DBGLEDOFF();
 
}

void MeasureMAX30100()
{
     power_twi_enable(); 
 //DBGLEDON();
 sensor.clearInt();
    sensor.readFullFIFO();
//    SendDataAll();
    computeMeasure();
    was=1;
 //   DBGLEDOFF();
    
}    

void PrepareMAX30100()
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
         power_twi_enable(); 
 //DBGLEDON();
 if (was ==0) {
    sensor.clearInt();
    sensor.readFullFIFO();
    computeMeasure();
 }   
// DBGLEDOFF();
 
 was=0;
}


void DataMAX30100(IMFrame &frame)
{   
  if (cpuVinCycle % 8==0){
    
 //   SetupADC();
 //   cpuVin=internalVcc();
//    cpuTemp=internalTemp();
//    cpuTemp=internalTemp();
 
//    ShutOffADC();
  }
 // pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
       	DBGINFO("temp: ");
        // data->w[2]=hh;
 //   data->w[3]=(hh >>16);
    data->w[7]=cpuVinCycle;
 //   data->w[3]=maxValueCurr;
  //  data->w[2]=maxValueCurr-maxValueLast;
    byte i=3;
    while ((sendPop<sendStore) && (i<7)){
      sendPop++;
      data->w[i]=Value[sendPop %ValueSize];
      i++;
    //  data->w[2]++;
      
    }
   // int x= 15;
    //data->w[3]=x;
    data->w[0]=dataIndex;
 //  Vin=internalVcc();
 //  data->w[0]=cpuVin;
 //  data->w[1]=cpuTemp;
    data->w[2]=maxValueCurr;
    data->w[1]=maxValueLast;
      
      
 //  scale.power_down();
 // power_adc_disable();  
}





#endif
//
// END OF FILE
//
