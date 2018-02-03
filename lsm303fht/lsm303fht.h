// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: LSM303D acc and magnetic logger  for imwave
//
//
// HISTORY:
//

#ifndef imLSM303fht_h
#define imLSM303fht_h

#include "imframe.h"
#include "imdebug.h"
#include <LSM303D.h>
#include <FHT.h>


int intPin1 = 2;//D2  INT2 WATERMARK
int intPin2 = 5;//D5 INT 1 DATA READY - DISABLE dbgpin
//int intCS = 4;//D5


#define MAXTAB 3
LSM303 sensor;

 
byte cpuVinCycle=0;

uint16_t cpuVin;
uint16_t cpuTemp;
byte dataCount=0;


void interruptMax(){//interrupt function
  IMTimer::doneMeasure();
}

void startPulse(){ //run settings for heart rate
  //  attachInterrupt(0,interruptMax,FALLING);  //D2 -> INT1
    attachInterrupt(0,interruptMax,RISING);  //D2 -> INT1
}
void stopPulse(){
     detachInterrupt(0);    
}


void ww(){
    delaySleepT2(10);
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
  cpuVinCycle++;  
 // IMFrameData *data =frame.Data();

  for (byte i=0;i<6;i++){
        frame.Body[i]=fht_oct_out[i];
  }    
    // data->w[0]=cpuVin;
  //  data->w[1]=cpuTemp;
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
//    sensor.writeReg(LSM303::CTRL4, 0x01);//threshold
 //   sensor.writeReg(LSM303::CTRL3, 0x02);//threshold
     sensor.writeReg(LSM303::CTRL1, 0x27);

 ww(); sensor.writeReg(LSM303::FIFO_CTRL, 0x47);//stream +threshold
ww();    sensor.writeReg(LSM303::CTRL3, 0x04);//thr int1
ww();    sensor.writeReg(LSM303::CTRL4, 0x01);//dataready int2 
   //   sensor.writeReg(LSM303::CTRL5, 0x6D);//threshold
}


void setupFIFO(){
   sensor.writeReg(LSM303::CTRL0, 0x60);//FIFO EMPTY in1
    sensor.writeReg(LSM303::FIFO_CTRL, 0x3C);//stream
    sensor.writeReg(LSM303::CTRL4, 0x01);//thr int2
//     sensor.writeReg(LSM303::CTRL3, 0x01);//FIFO EMPTY in1
     sensor.writeReg(LSM303::INT_CTRL_M, 0x00);//FIFO EMPTY in1
   
}
void SetupLSM303()
{
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

  }while(true);
  
     DBGLEDON();
 
 ww();  sensor.writeReg(LSM303::CTRL0, 0xE0);//Fmemory reset
 ww();  sensor.writeReg(LSM303::CTRL0, 0x60);//Fmemory reset
 ww();   sensor.writeReg(LSM303::IG_CFG1, 0x3F);//threshold
  ww();  sensor.writeReg(LSM303::IG_CFG2, 0x00);//threshold
 ww(); sensor.enableDefault();
 //sensor.writeRegister(ADXL345_REG_POWER_CTL, 0x08);  
 // setupFIFO();
  ww(); setupInertialPCB();
//  setupFIFO();
//  setupClick();
  ww();sensor.testDevice();
    delaySleepT2(300);
    DBGLEDOFF();
 startPulse();
}


void ComputeFFT(){
  fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
  //  fht_mag_log(); // take the output of the fht
    fht_mag_octave(); // take the output of the fht
}
void SendDataFFT(){
         trx.Wakeup();
         static IMFrame frame;
         frame.Reset();

         DataLSM303(frame);
         trx.SendData(frame);
         trx.Transmit();

}

void AddData(){
    fht_input[dataCount]=sensor.a.x;
    ++dataCount;

    if (dataCount  >=FHT_N )  {
      dataCount=0;
DBGLEDON();
      ComputeFFT();
      SendDataFFT();
      DBGLEDOFF();
     }
}

void ReadLSM303(){
  byte ffx=sensor.readReg(LSM303::FIFO_SRC);
  byte ff=ffx & 0x1F;
 // byte ff=4;  
  for (int8_t i=ff ; i>=0; i--)
  {
//digitalWrite(intCS,HIGH);
    if (sensor.readAcc()==6){
       AddData();
    }
//digitalWrite(intCS,LOW);
  }  

   sensor.readMag(); 
}
void computeMeasure(){
  //         DBGLEDON();
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
  
}



void PrepareLSM303()
{
   if (digitalRead(intPin2)==HIGH){  // dataready PIN int2
//     DBGLEDON();
      ReadLSM303();
   }
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
 //       power_twi_enable(); 

}





#endif
//
// END OF FILE
//
