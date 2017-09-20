
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include <HMC5883L.h>
#include <Wire.h>


#include "imdebug.h"

/******************************** Configuration *************************************/


#define MMAC 0x230006  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 23     //Type of device
#define MCHANNEL 1

/************************* Module specyfic functions **********************/


#include "mhmc.h"
#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#define pinLED 9
void PrepareData()
{
       if (trx.CycleData())
      {
        digitalWrite(pinLED,HIGH);
//  DBGPINHIGH();
  PrepareMHMC();
    digitalWrite(pinLED,LOW);
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
        DataMHMC(frame);
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
    case STARTBROADCAST:  trx.Knock();    break;
    case STOPBROADCAST:  PrepareData();     break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
  //  case CRONHOUR : delaySleepT2(1000);break;
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
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  DBGLEDON();
  interrupts();
  delay(1000);
  DBGLEDOFF();
   wdt_enable(WDTO_8S);
//   disableADCB();
// SetupMHMC();

  trx.myMAC=MMAC;
  trx.myChannel=MCHANNEL;
 
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
  SetupMHMC();
 power_timer0_disable();
  setupTimer2();
}

void loop()
{
  wdt_reset();
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
    // DBGPINLOW();
     stageloop(xstage);
   // DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}
