
#include <imframe.h>
#include <imatmega.h>
//#include <avr/wdt.h>
#include <SPI.h>
// Data wire is plugged into pin 2 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x210030  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 21     //Type of device






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imbuffer.h"
#include "imbufrfm69.h"
#include "imatmega.h"
#include "acs720.h"
//#include "imbuftest.h"


Transceiver trx;
IMBuffer    buf3;











void MeasureData(){
  MeasureACS720();
}




void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        trx.Wakeup();
//        SetupADC();
        static IMFrame frame;
        frame.Reset();
         IMFrameData *data =frame.Data();
        DataACS720(frame);
        trx.setMeasure(TimeMeasure,TimeMeasure+1);

//        long mm=millis();
//        DBGINFO(" :");
//        DBGINFO(millis()-mm);
        data->w[8]=0xA;
        data->w[10]=0xA;
        
      DBGINFO("data:");
      DBGINFO(data->w[0]);
      DBGINFO(":");
      DBGINFO(data->w[1]);
      DBGINFO("\r\n");

//        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
//        ERRFLASH();
      } else{
         trx.printCycle();
      }
      trx.ListenData();

   } else {
  //   trx.ListenBroadcast();
   }


}



void ReceiveData()
{
//  static IMFrame rxFrame;
//ERRLEDON();
//  DBGINFO(" Receive ");
      while (trx.GetData())
      {
        if (trx.Parse())
        {
      //    DBGINFO(" rxGET ");
        }
      }
      DBGINFO("RD\r\n");

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
    DBGINFO("stageloop=");  DBGINFO(millisT2());
    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();   break;
//    case STOPBROADCAST:  trx.StopListenBroadcast();      break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: SendData();/*trx.ListenData(); */ break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA :DBGPINHIGH(); ReceiveData();DBGPINLOW();break;
    case LISTENBROADCAST : DBGPINHIGH();ReceiveData();DBGPINLOW();break;
    case MEASUREDATA : MeasureACS();break;
    case IMTimer::IDDLESTAGE : {
     DBGINFO("***IDLE DATA");

       ReceiveData();break;
     }
    case IMTimer::PERIOD : 
  //      ERRFLASH();
 //     PrintStatus();
    break;
    default:
    break;
  }

   DBGINFO("@@\r\n");

}





void setup()
{
  pinMode(4,INPUT_PULLUP);
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  pinMode(DBGCLOCK,OUTPUT);
  pinMode(DBGPIN ,OUTPUT);
  wdt_disable();
  INITDBG();
  digitalWrite(DBGPIN,HIGH);
  DBGINFO("SETUP");
  DBGINFO(internalVcc());
  digitalWrite(DBGPIN,LOW);

  DBGINFO("_");
//  setupTimer2();
  DBGPINHIGH();
  ERRLEDINIT();
  ERRLEDOFF();
 //  wdt_enable(WDTO_8S);
//   power_timer0_disable();
//  disableADCB();
//  digitalWrite(4,LOW);
   interrupts ();
  trx.myMAC=MMAC;
      DBGINFO2(trx.myMAC,HEX);
//  trx.NoRadio=true;
  trx.Init(buf3);
  trx.myDevice=MDEVICE;


 setupTimer2();
}

void loop()
{
//  wdt_reset();
//  PrintStatus();
  DBGINFO("\r\n");
  DBGINFO("LOOP");
    
  byte xstage;
  do{
   DBGPINLOW();
     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}
