
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>


#include "imdebug.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino
#define MMAC 0x270003  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 0x27     //Type of device
#define MCHANNEL 1


/************************* Module specyfic functions **********************/

#include "soilyl38.h"
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;


void PrepareData()
{
       if (trx.CycleData())
      {
        DBGLEDON();
//  DBGPINHIGH();
  PrepareSOIL();
    DBGLEDOFF();
//  DBGPINLOW();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
        DBGPINHIGH();
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataSOIL(frame);
        DBGPINLOW();
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



void stageloop(byte stage)
{
//   if (stage== STARTBROADCAST){
//    DBGINFO("stageloop=");  DBGINFO(millis());
//    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST: trx.Knock();      break;
    case STOPBROADCAST:  trx.StopListenBroadcast(); PrepareData();    break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
   // case CRONHOUR : delaySleepT2(10000);break;
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
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  INITDBG();
  wdt_disable();
  INITDBG();
  DBGINFO(F("*****start"));
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  interrupts();
  delay(100);
   wdt_enable(WDTO_8S);
   disableADCB();
 SetupSOIL();

  trx.myMAC=MMAC;
  trx.myChannel=MCHANNEL;
 
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
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
