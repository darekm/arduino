
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define REQUIRESALARMS 0
// Data wire is plugged into pin 2 on the Arduino
#define MMAC 0x260005  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x26     //Type of device
#define MCHANNEL 1

/************************* Module specyfic functions **********************/

#include <Wire.h>
#include <SHT31lib.h>
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#include "sht31.h"

void PrepareData()
{
      if (trx.CycleData())
      {
        DBGLEDON();
        PrepareSHT31();
        DBGLEDOFF();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataSHT31(frame);
        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
      }
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
  //      DBGLEDOFF();
       DBGINFO("\r\n");
}

void PrintStatus()
{
  DBGINFO("\r\n");
  DBGINFO(" Status ");
//  trx.printStatus();
  DBGINFO("\r\n");
}


void stageloop(byte stage)
{
  switch (stage)
  {
    case STARTBROADCAST: trx.Knock();      break;
    case STOPBROADCAST:   trx.StopListenBroadcast(); PrepareData();    break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
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
  #ifdef DBGCLOCK
    pinMode(DBGCLOCK,OUTPUT);
    digitalWrite(DBGCLOCK ,HIGH);
  #endif
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  DBGPINHIGH();
  DBGPINLOW();
  INITDBG();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  wdt_enable(WDTO_8S);
  interrupts();
  delay(300);
   disableADCB();
  uint16_t ad=SetupSHT31();

  trx.myMAC=MMAC;
  trx.startMAC=0;
 // trx.myMAC+=ad;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
//  trx.timer.onStage=stageloop;
//    pciSetup(9);
#if DBGPIN>111
  if (ad==0){
      for(int i=0;i<100000;i++){
        DBGPINHIGH();
        delaySleepT2(10);
        DBGPINLOW();
        delaySleepT2(1);
         
      }
      reboot();
  }
 
#endif  
#if DBGLED>=1
  if (ad!=0xFFFF){
    DBGLEDON();
    delaySleepT2(300);
    DBGLEDOFF();
    DBGINFO(F("TEMPERARUEEE\r\n"));
   } else{
    DBGLEDON();
    delay(200);
    DBGLEDOFF();
    delay(200);
    DBGLEDON();
    delay(200);
    DBGLEDOFF();
    delay(200);
    DBGLEDON();
    delay(200);
//    DBGLEDOFF();
    reboot();

  }
#endif
  power_timer0_disable();
  setupTimer2();
}

void loop()
{
  wdt_reset();
  byte xstage;
  do{
     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);
}
