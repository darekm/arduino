
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define MMAC 0x480001  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x48     //Type of device
#define MCHANNEL 2

/************************* Module specyfic functions **********************/

#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

int COUNTER=0;

#include "qswitch.h"

void PrepareData()
{
  IMTimer::doneMeasure();
      if (trx.CycleData())
      {
      //  PrepareQtouch();
      }
}
void SendDataFlood()
{
     static IMFrame frame;
     frame.Reset();
     COUNTER++;
     IMFrameData *data =frame.Data();
        data->w[0]=2900;
        data->w[2]=COUNTER;
        data->w[4]=COUNTER;
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();

}


void SendData()
{
      if (trx.CycleData()) {
      //  trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
       IMFrameData *data =frame.Data();
  //      DataQtouch(frame);
        data->w[0]=2950;
        data->w[1]=7;
        data->w[2]=COUNTER;
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
      }
}

byte OrderData(uint16_t a){
//  DBGLEDON();
  return 1;
}  

void ReceiveData()
{
    //    DBGLEDON();

      while (trx.GetData())
      {
        if (trx.Parse())
        {
          DBGINFO(" rxGET ");
        }
      }
      //      DBGLEDOFF();

}


void MeasureData()
{
  IMTimer::doneMeasure();
 // LoopQtouch();
  DBGLEDON();
 SendDataFlood();
  DBGLEDOFF();
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
    case MEASUREDATA: MeasureData();
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
 //  disableADCB();
   byte ad=1;
  SetupSwitch();

  trx.myMAC=MMAC;
  trx.startMAC=0;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
  trx.funOrder=&OrderData;
  
  trx.NoSleep=true;

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
