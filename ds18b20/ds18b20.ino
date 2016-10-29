
#include <imframe.h>
#include <imatmega.h>
#include <OneWire.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define REQUIRESALARMS 0
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define MMAC 0x170000  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device

/************************* Module specyfic functions **********************/

#include <DallasTemperature.h>
#include "ds18b20.h"
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;


void PrepareData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
         PrepareDS18B20();
      }
   }  
}  

void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        static IMFrame frame;
        frame.Reset();
//        long mm=millis();
        DataDS18B20(frame);
//        DBGINFO(" :");        DBGINFO(millis()-mm);
        DBGINFO("SendData ");
        trx.SendData(frame);
      } else{
         trx.printCycle();
      }
   } else {
     trx.ListenBroadcast();
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
    case STARTBROADCAST:  trx.ListenBroadcast();  PrepareData();   break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: trx.Wakeup();SendData();  /*SendDataFlood();*/break;
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
  pinMode(4,INPUT_PULLUP);
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  pinMode(DBGPIN ,OUTPUT);
  digitalWrite(DBGPIN ,HIGH);
  digitalWrite(DBGPIN ,LOW);
  wdt_disable();

  INITDBG();
  DBGINFO(F("*****start"));
  ERRLEDINIT();   ERRLEDOFF();
  IMMAC ad=SetupDS18B20();
//  wdt_enable(WDTO_8S);

  interrupts ();
  randomSeed(analogRead(0)+internalrandom());
  trx.myMAC=MMAC;
  trx.myMAC+=ad;
  trx.Init(buffer);

    DBGINFO(" MMAC ");  DBGINFO2(trx.myMAC,HEX);
//       DBGINFO();
  trx.myDevice=MDEVICE;
//  trx.timer.onStage=stageloop;
//    pciSetup(9);
  if (ad>0){
    ERRLEDON();
    delay(1000);
    delay(200);
    ERRLEDOFF();
    DBGINFO(F("TEMPERARUEEE\r\n"));
   } else{
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    delay(200);
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    reboot();

  }     
//  DBGINFO(F("Free RAM bytes: "));DBGINFO(freeRam());
  
//  trx.TimerSetup();
//   DBGINFO("classtest Timer");  DBGINFO(IMTimer::ClassTest());
}

void loop()
{
//  wdt_reset();
  PrintStatus();  
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
