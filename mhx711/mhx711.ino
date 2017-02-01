
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include <HX711.h>


#include "imdebug.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
#define MMAC 0x190000  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 19     //Type of device

/************************* Module specyfic functions **********************/


#include "mhx711.h"
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;


void PrepareData()
{
       if (trx.CycleData())
      {
//  DBGPINHIGH();
  PrepareHX711();
//  DBGPINLOW();
      }
   
}  

void SendData()
{
 //  if (trx.Connected())
  // {
      if (trx.CycleData()) {
        DBGPINHIGH();
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataHX711(frame);
        DBGPINLOW();
        DBGINFO("SendData ");
        trx.SendData(frame);
         trx.Transmit();
       }
  //    trx.ListenData();
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
   resetPin();
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
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
  SetupHX711();
//  wdt_enable(WDTO_8S);
   disableADCB();

  trx.myMAC=MMAC;
 
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
  power_timer0_disable();
  setupTimer2();
}

void loop()
{
//  wdt_reset();
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
    // DBGPINLOW();
     stageloop(xstage);
   // DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}