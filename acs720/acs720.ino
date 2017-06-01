
#include <imframe.h>
#include <imatmega.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <EEPROM.h>
// Data wire is plugged into pin 0 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x220038 // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 22     //Type of device






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imbuffer.h"
#include "imbufrfm69.h"





Transceiver trx;
IMBuffer    buf3;


#include "acs720.h"






#define pinLED 9

void PrepareData(){
      if (trx.CycleData())
      {
  
 // digitalWrite(pinLED,HIGH);
  MeasureACS720();
//  digitalWrite(pinLED,LOW);
      }
}




void SendData()
{
      if (trx.CycleData())
      {
        trx.Wakeup();
//        SetupADC();
        static IMFrame frame;
        frame.Reset();
         IMFrameData *data =frame.Data();
        DataACS720(frame);
   //     trx.setMeasure(TimeMeasure,TimeMeasure+1);

   //     data->w[8]=0xA;
        data->w[10]=0xA;
        
      DBGINFO("data:");
      DBGINFO(data->w[0]);
      DBGINFO(":");
      DBGINFO(data->w[1]);
      DBGINFO("\r\n");

//        DBGINFO("SendData ");
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
  switch (stage)
  {
    case STARTBROADCAST:  trx.Knock();  break;
    case STOPBROADCAST:   PrepareData();      break;
    case STARTDATA: SendData(); break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA :DBGPINHIGH(); ReceiveData();DBGPINLOW();break;
    case LISTENBROADCAST : DBGPINHIGH();ReceiveData();DBGPINLOW();break;
//    case MEASUREDATA : MeasureACS();break;
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
    resetPin();
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);

  INITDBG();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
 //  setupTimer2();
  //  wdt_enable(WDTO_8S);
//   power_timer0_disable();
//  disableADCB();
//  digitalWrite(4,LOW);
   interrupts ();
   delay(500);
   disableADCB();
    wdt_enable(WDTO_8S);
 // trx.startMAC=MMAC;
  trx.myMAC=MMAC;
         SetupACS720();
//  trx.NoRadio=true;
  trx.Init(buf3);
  trx.myDevice=MDEVICE;
 
//  SetupACS720();
  power_timer0_disable();

 setupTimer2();
}

void loop()
{
  wdt_reset();
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
