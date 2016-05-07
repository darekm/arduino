#include <Wire.h>
#include <avr/power.h>
#include <imframe.h>
#include <imatmega.h>
#include <imsht.h>
#include <imcharger.h>





//#define DBGLED 0
 //#.define DBGLVL 2
#include "imdebug.h"
#include "intrappflex.h"





/******************************** Configuration *************************************/

#define MMAC 0x160002  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 6     //Type of device




/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"


IMCC1101  cc1101;
Transceiver trx;











/*
void PrepareData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
       PrepareIntrappFlex();
       }
   }
}
*/





void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleDataPrev()){
        PrepareIntrappFlex();
      }
      if (trx.CycleData())
      {
         trx.printTime();
         static IMFrame frame;
         frame.Reset();
         DataIntrappFlex(frame);

         trx.SendData(frame);
         trx.Transmit();
         
//         ERRFLASH();
      } 

   } else {
     trx.ListenBroadcast();
   }

}



void ReceiveData()
{
  static IMFrame rxFrame;
      if (trx.GetFrame(rxFrame))
      {
        if (!trx.ParseFrame(rxFrame))
        {
          DBGINFO(" rxGET ");
        }
      }
      DBGINFO("\r\n");

}


//void ISRTrans() // handle pin change interrupt for D8 to D13 here
ISR(PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 {
  trx.Rupture();
 }

void stageloop(byte stage)
{
//   DBGINFO("stageloop=");  DBGINFO(millis());
//   DBGINFO(":");  DBGINFO(stage);
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();     break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: SendData();break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case IMTimer::IDDLESTAGE : {
     DBGINFO("***IDDLE DATA ");

       ReceiveData();break;
     }
     

    default:
    break;
  }
}





void setup()
{
  INITDBG();
  ERRLEDINIT(); ERRLEDOFF();
  SetupIntrappFlex();
  wdt_enable(WDTO_8S);  
  interrupts ();
  randomSeed(analogRead(0)+internalrandom());
  trx.myMAC=MMAC;
  trx.myDevice=MDEVICE;
  trx.Init(cc1101);
  trx.timer.onStage=stageloop;
  pciSetup(9);
//   DBGINFO("classtest Timer");  DBGINFO(IMTimer::ClassTest());
}

void loop()
{
 //     static IMFrame frame2;
//      frame2.Reset();
  wdt_reset();     
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
