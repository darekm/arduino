
#define LOG_OUT 0 // use the log output function
#define LIN_OUT8 1 // use the log output function
#define LIN_OUT 1 // use the lin output function
#define OCTAVE 0 // use the log output function
#define FHT_N 64 // set to 256 point fht
#define SCALE 256
#include <imframe.h>
#include <imatmega.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "LSM303D.h"
#include <FHT.h>


#include "imdebug.h"
#include "imeprom.h"

/******************************** Configuration *************************************/

// Data wire is plugged into pin 2 on the Arduino

#define MMAC 0x2A0005  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x2A     //Type of device
#define MCHANNEL 3


/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

#include "lsm303fht.h"

void PrepareData()
{
  if (trx.CycleData())    {
     PrepareLSM303();
  }
}  

void SendData()
{
  return;
  if (trx.CycleData()) {
      CheckModeLSM(trx.myMode);
      EnableLSM();
     //MeasureLSM303();
  /*       trx.Wakeup();
         static IMFrame frame;
         frame.Reset();
         DataLSM303(frame);
         trx.SendData(frame);
         trx.Transmit();
 */    } 
}



void ReceiveData()
{
// DBGLEDON();
 while (trx.GetData())
      {
        if (trx.Parse())
        {
          DBGINFO(" rxGET ");
        }
      }
     DBGINFO("\r\n");
  //  DBGLEDOFF();  
}



void stageloop(byte stage)
{
  switch (stage)
  {
    case STARTBROADCAST:  trx.Knock();     break;
    case STOPBROADCAST: trx.StopListenBroadcast(); PrepareData();     break;
    case STARTDATA: SendData();  break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case MEASUREDATA: MeasureLSM303();break;
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
  INITDBG();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  interrupts();
  delay(10);
  wdt_enable(WDTO_8S);
   disableADCB();
  SetupLSM303();
  trx.startMAC=0;
  trx.myMAC=MMAC;
  trx.myChannel=MCHANNEL;
  trx.serverMAC=ServerMAC;
//  trx.NoRadio=true;
  
 
  trx.Init(buffer);
  trx.myDevice=MDEVICE;
 // trx.NoKnock=true;
 // trx.NoSleep=true;
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
