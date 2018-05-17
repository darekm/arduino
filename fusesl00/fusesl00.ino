
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define MMAC 0x440007  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x44     //Type of device
#define MCHANNEL 3

/************************* Module specyfic functions **********************/

#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;
//t_Time SwitchTime;

#include "fusesl00.h"

void PrepareData()
{
   MeasureSensor();
      if (trx.CycleData())
      {
       // DBGLEDON();
  //      PrepareQtouch();
      //  DBGLEDOFF();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
  DBGLEDON();
         static IMFrame frame;
        frame.Reset();
        DataSensor(frame);
        DBGINFO("SendData ");
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
  DBGLEDOFF();
         }
}

byte OrderData(uint16_t a){
 // DBGLEDON();
  return 1;
}  

void ReceiveData()
{ 
      while (trx.GetData())
      {
        if (trx.Parse())
        {
          DBGINFO(" rxGET ");
        }
      }
}


void StepData(void){
//  if ((millisTNow()-SwitchTime)>50 ){
 //   SwitchTime=millisTNow();
//    DBGLEDON();
//  LoopSensor();
 //   DBGLEDOFF();
 //   digitalWrite(DBGCLOCK,HIGH);
//    digitalWrite(DBGCLOCK,LOW);
//    digitalWrite(5,SWtoggle);
//  } 

} 

void HourData()
{
          static IMFrame frame;
        frame.Reset();
        MeasureVCC();
        DataSensor(frame);
        frame.Data()->w[1]=99;
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
}

void MeasureData()
{return;
        static IMFrame frame;
        frame.Reset();
        DataSensor(frame);
        DBGINFO("SendData ");
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
}

void stageloop(byte stage)
{
  switch (stage)
  {
    case STARTBROADCAST: trx.Knock();    break;
    case STOPBROADCAST:       trx.StopListenBroadcast();PrepareData();    break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case MEASUREDATA: MeasureData();
    case CRONHOUR: MeasureData();
    case IMTimer::IDDLESTAGE : {

       DBGINFO("***IDDLE DATA");
       ReceiveData();break;
     }
    case IMTimer::PERIOD : 
  //      ERRFLASH();
  //     PrintStatus();
    break;
    default:
    break;
  }
}





void setup()
{
  resetPin();
//  setMaxStepTimer(91);
  #ifdef DBGCLOCK

   pinMode(DBGCLOCK,OUTPUT);
//  digitalWrite(DBGCLOCK ,HIGH);
  #endif
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  DBGPINHIGH();
  DBGPINLOW();
//  pinMode(DBGLED,OUTPUT);
  INITDBG();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  wdt_enable(WDTO_8S);
  interrupts();
  delay(100);
// ShutOffADC();
 
//  power_timer0_enable();

 // SetupSensor();
  disableADCB();

  trx.myMAC=MMAC;
  trx.startMAC=0;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.myDevice=MDEVICE;
  trx.funOrder=&OrderData;
  trx.Init(buffer);
//  trx.setTimerFunction(&StepData);
  
  power_timer0_disable();
  setupTimer2();
}

void loop()
{
  wdt_reset();
//  SwitchTime=millisTNow();
  byte xstage;
  do{
     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);
}
