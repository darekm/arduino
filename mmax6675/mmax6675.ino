
#include <imframe.h>
#include <imatmega.h>
#include <SPI.h>
#include <EEPROM.h>
#include "mmax6675.h"


#include "imdebug.h"
#include "imeprom.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino

#define MMAC 0x210000  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 21     //Type of device

/************************* Module specyfic functions **********************/


#include "max6675.h"
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;


void PrepareData()
{
 //  if (trx.Connected())
 //  {
      if (trx.CycleData())
      {
  DBGPINHIGH();
  PrepareMAX6675();
  DBGPINLOW();
      }
 //  }  
}  

void SendData()
{
 //  if (trx.Connected())
 //  {
      if (trx.CycleData()) {
        DBGPINHIGH();
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataMAX6675(frame);
        DBGPINLOW();
        DBGINFO("SendData ");
        trx.SendData(frame);
         trx.Transmit();
  //    } else {
  //      trx.printCycle();
      }
 //     trx.ListenData();
 //  } else {
     //trx.ListenBroadcast();
 //  }
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



void stageloop(byte stage)
{
//   if (stage== STARTBROADCAST){
//    DBGINFO("stageloop=");  DBGINFO(millis());
//    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();  PrepareData();  break;
    case STOPBROADCAST:  trx.Knock();      break;
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
 // pinMode(3,OUTPUT);
 // digitalWrite(3,LOW);
  pinMode(DBGCLOCK,OUTPUT);
  digitalWrite(DBGCLOCK ,HIGH);
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  pinMode(DBGPIN ,OUTPUT);
  DBGPINHIGH();
  DBGPINLOW();
  wdt_disable();
  INITDBG();
  DBGINFO(F("*****start"));
  ERRLEDINIT();   ERRLEDOFF();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  interrupts();
  delay(1000);
  SetupMAX6675();
//  wdt_enable(WDTO_8S);
   disableADCB();

  trx.myMAC=MMAC;
 
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
  power_timer0_disable();
//  trx.timer.onStage=stageloop;
//    pciSetup(9);

#if DBGLED>=1
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
#endif
setupTimer2();
}

void loop()
{
//  wdt_reset();
  byte xstage;
  do{
DBGPINLOW();
     xstage=trx.timer.WaitStage();
    // DBGPINLOW();
     stageloop(xstage);
   // DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}
