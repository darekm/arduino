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


int intPin = 2;//D2
int CS = 2;//D2
int thermoCLK = A5;
int vccPin = A0;


//ADXL345 sensor;
Adafruit_ADXL345_Unified sensor;
//uint8_t maxValueIndex[5]; //wait for 5 measurements
//uint8_t maxValuePointer;

 
int valueX;
int valueY;
int valueZ;
uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;




        



void interruptMax(){//interrupt function
//DBGPINHIGH();
  IMTimer::doneMeasure();
//  DBGPINLOW();
}

void startPulse(){ //run settings for heart rate
    attachInterrupt(0,interruptMax,FALLING);
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

void SetupADXL345()
{
//  DBGLEDON();
   power_twi_enable(); 
   power_adc_enable();
// Wire.begin();
 // sensor.init();
  pinMode(CS,OUTPUT);
   digitalWrite(CS,HIGH);
  if (! sensor.begin()) 
     DBGLEDON(); 
 
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
  DBGPINHIGH();
  //DBGPINLOW();
     power_twi_enable();
 //   DBGPINHIGH(); 
// sensor.clearInt();
// DBGPINLOW();
// DBGPINHIGH();
  //  sensor.readFullFIFO();
//    sensor.clearInt();
  //  sensor.clearFIFO();

//    DBGPINHIGH();
  //  DBGPINLOW();
  //   DBGPINHIGH();

//    SendDataAll();
    //computeMeasure();
    SendDataAll();
   //   DBGPINLOW();
    
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
         power_twi_enable(); 
 DBGLEDON();
// sensor.readXYZ(&valueX,&valueY,&valueZ);
  valueX=sensor.getX();
  valueY=sensor.getY();
  valueZ=sensor.getZ();
 DBGLEDOFF();
 
}


void DataADXL345(IMFrame &frame)
{  
  if (cpuVinCycle % 8==0){
    
    SetupADC();
    cpuVin=internalVcc();
//    cpuTemp=internalTemp();
//    cpuTemp=internalTemp();
 
//    ShutOffADC();
  }
 // pinMode(A4, INPUT);
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
       	DBGINFO("temp: ");
    for(byte i=0;i<15;i++){
    }

        // data->w[2]=hh;
    data->w[3]=valueX;
    data->w[4]=valueY;
    data->w[5]=valueZ;
 //   data->w[7]=cpuVinCycle;
 //   data->w[3]=maxValueCurr;
  //  data->w[2]=maxValueCurr-maxValueLast;
      
  //  }
   // int x= 15;
    //data->w[3]=x;
//    data->w[0]=dataIndex;
 //  Vin=internalVcc();
   data->w[0]=cpuVin;
 //  data->w[1]=cpuTemp;
      
 //  scale.power_down();
 // power_adc_disable();  
}





#endif
//
// END OF FILE
//
