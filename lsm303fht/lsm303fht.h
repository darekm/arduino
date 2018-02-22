// 
//    FILE:
// VERSION: 0.1.00
// PURPOSE: LSM303D acc with FHT transformate logger  for imwave
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


LSM303 sensor;

 
byte cpuVinCycle=0;

uint16_t cpuVin;
uint16_t cpuTemp;
byte dataCount=0;


void interruptMax(){//interrupt function
  IMTimer::doneMeasure();
}

void startPulse(){ //run settings for heart rate
    attachInterrupt(0,interruptMax,RISING);  //D2 -> INT1
}


void ww(){
//    delaySleepT2(5);
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
  IMFrameData *data =frame.Data();

  for (byte i=0;i<20;i++){
        frame.Body[i]=fht_lin_out8[i];
  }
  for (byte i=0;i<9;i++){

//   data->w[i]=fht_log_out[i];
  }    

    // data->w[0]=cpuVin;
  //  data->w[1]=cpuTemp;
}
  



void setupInertialPCB(){
//    sensor.writeReg(LSM303::IG_CFG1, 0x20);//threshold
 ww();   sensor.writeReg(LSM303::IG_THS1, 0x03);//threshold
//    sensor.writeReg(LSM303::CTRL4, 0x01);//threshold
 //   sensor.writeReg(LSM303::CTRL3, 0x02);//threshold
     sensor.writeReg(LSM303::CTRL1, 0x47);//3=12.5Hz  4=25Hz

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
    delaySleepT2(100);
   DBGLEDOFF();
  power_twi_enable(); 
   power_adc_enable();
     pinMode(intPin1,INPUT_PULLUP);//INT1
     pinMode(intPin2,INPUT_PULLUP);//INT2
  ww();sensor.testDevice();
  ww(); sensor.init(sensor.device_D,LSM303::sa0_high);
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
  }  ;
  }while(true);
  
     DBGLEDON();
 
 ww();  sensor.writeReg(LSM303::CTRL0, 0xE0);//Fmemory reset
 ww();  sensor.writeReg(LSM303::CTRL0, 0x60);//Fmemory reset
 ww();   sensor.writeReg(LSM303::IG_CFG1, 0x3F);//threshold
  ww();  sensor.writeReg(LSM303::IG_CFG2, 0x00);//threshold
 ww(); sensor.enableDefault();
  ww(); setupInertialPCB();
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
    fht_mag_lin(); // take the output of the fht
  //  fht_mag_octave(); // take the output of the fht
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
  //  fht_input[dataCount]=sensor.a.x;
    fht_input[dataCount]=160;
    if (dataCout>48) 
      fht_input[dataCount]=1000;
    
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
