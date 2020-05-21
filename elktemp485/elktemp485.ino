
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include "imdebug.h"
//#include "SoftwareSerial.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino

#define MMAC 0x630001  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x63     //Type of device
#define MCHANNEL 2

/************************* Module specyfic functions **********************/

#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#include "elktemp485.h"

void PrepareData()
{
      if (trx.CycleData())
      {
        DBGLEDON();
        PrepareELKT();
        DBGLEDOFF();
      }
}  

void SendData()
{
      if (trx.CycleData()) {
        static IMFrame frame;
        frame.Reset();
        DataELKT(frame);
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
      }
}

byte OrderData(uint16_t a){
//  DBGLEDON();
  return 1;
}  


#ifdef HAVE_HWSERIAL1
  tttt
#endif
  
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
    case STOPBROADCAST:    trx.StopListenBroadcast();PrepareData();    break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case IMTimer::IDDLESTAGE : {        ReceiveData();break; }
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
 
 //   SetupELKT();
 //  disableADCB();
  SetupELKT();
  trx.myMAC=MMAC;
  //trx.startMAC=0;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.myDevice=MDEVICE;
//  trx.funOrder=&OrderData;
  trx.Init(buffer);
//  trx.timer.onStage=stageloop;


#if DBGLED>=1
  if (true){
    DBGLEDON();
    delaySleepT2(300);
    DBGLEDOFF();
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
    //reboot();
  }
#endif
 // power_timer0_disable();
  setupTimer2();
    power_usart0_enable();

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
