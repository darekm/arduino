
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define REQUIRESALARMS 0
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define MMAC 0x170000  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x17     //Type of device
#define MCHANNEL 1

/************************* Module specyfic functions **********************/

#include <DallasTemperature.h>
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#include "ds18b20.h"

void PrepareData()
{
      if (trx.CycleData())
      {
        DBGLEDON();
        PrepareDS18B20();
        DBGLEDOFF();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataDS18B20(frame);
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
//   if (stage== STARTBROADCAST){
//    DBGINFO("stageloop=");  DBGINFO(millis());
//    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST: trx.Knock();      break;
    case STOPBROADCAST:    trx.StopListenBroadcast();PrepareData();    break;
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
//   DBGINFO("@@\r\n");
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
  DBGINFO(F("*****start"));
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  wdt_enable(WDTO_8S);
  interrupts();
  delay(300);
  IMMAC ad=SetupDS18B20();
   disableADCB();

  trx.myMAC=MMAC;
  trx.startMAC=0;
  trx.myMAC+=ad;
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
  if (ad>0){
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
    DBGLEDOFF();
    reboot();

  }
#endif
  power_timer0_disable();
  setupTimer2();
}

void loop()
{
  wdt_reset();
//  PrintStatus(); 
  byte xstage;
  do{
     xstage=trx.timer.WaitStage();
    // DBGPINLOW();
     stageloop(xstage);
   // DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}
