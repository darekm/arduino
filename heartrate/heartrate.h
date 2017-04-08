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


MAX30100 sensor;
//uint8_t maxValueIndex[5]; //wait for 5 measurements
//uint8_t maxValuePointer;
uint16_t maxValueLast;
uint16_t maxValueCurr;
uint16_t last=0; //last avarage
uint16_t average;
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
  if(meassureMode==0){
    if(average>last){
      //Serial.println("Start finding max value");
      meassureMode=1;//looking for loss
    }
  }
  else{
    if(average < last){
      meassureMode=0;
      maxValueLast=maxValueCurr;
      maxValueCurr=dataIndex;
      dataIndex=0;
//      saveMaxValue(); //index of that measurement
    }
  }
  last=average;
}

void computeMeasure(){
    for(int i=0;i<15;i++){
      dataIndex++;
      average= average*0.99+sensor.dataContainer[i]*0.01;
      ifBigger();
    }
}



void interruptMax(){//interrupt function
DBGPINHIGH();
  
//  pointer=2;
  IMTimer::doneMeasure();
  DBGPINLOW();
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
 DBGLEDON();
 sensor.clearInt();
    sensor.readFullFIFO();
    computeMeasure();
    DBGLEDOFF();
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
     pointer=1;
         power_twi_enable(); 
 DBGLEDON();
 sensor.clearInt();
    sensor.readFullFIFO();
    computeMeasure();
    DBGLEDOFF();

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
    data->w[6]=cpuVinCycle;
    data->w[3]=maxValueCurr;
    data->w[2]=maxValueCurr-maxValueLast;
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
