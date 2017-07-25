#include <imframe.h>
#include <imatmega.h>
#include <SPI.h>
#include <EEPROM.h>
#include "mmax6675.h"


#include "imdebug.h"
#include "imeprom.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino

#define MMAC 0x210006  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 21     //Type of device
#define MCHANNEL 2


/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#include "max6675.h"

void PrepareData()
{
      if (trx.CycleData())
      {
        DBGLEDON();
        PrepareMAX6675();
        DBGLEDOFF();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
     //   DBGPINHIGH();
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataMAX6675(frame);
    //    DBGPINLOW();
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
  switch (stage)
  {
    case STARTBROADCAST:  trx.Knock();    break;
    case STOPBROADCAST:   PrepareData();     break;
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
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  pinMode(DBGPIN ,OUTPUT);
//  DBGPINHIGH();
 // DBGPINLOW();
  wdt_disable();
  INITDBG();
  DBGINFO(F("*****start"));
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  interrupts();
  delay(100);
  SetupMAX6675();
   disableADCB();
  wdt_enable(WDTO_8S);
  trx.startMAC=0;
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
