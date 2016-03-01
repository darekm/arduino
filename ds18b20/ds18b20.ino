
#include <imframe.h>
#define REQUIRESALARMS 0
#include <imatmega.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/wdt.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2







//#.define DBGLVL 2
#include "imdebug.h"
#include "ds18b20.h"





/******************************** Configuration *************************************/

#define MMAC 0x170000  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"
#include "imbufcc1101.h"


// IMCC1101  cc1101;
IMBuffer    buffer;
Transceiver trx;














void PrepareData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
       PrepareDS18B20();
       }
   }  
}  


void SendDataFlood()
{
  return;
   if (trx.Connected())
   {
     static IMFrame frame;
     frame.Reset();
     IMFrameData *data =frame.Data();
     for(byte i = 0; i < 8; i++){
        data->w[0]=100;
        data->w[1]=i;

        trx.SendData(frame);
        trx.Transmit();

     }
   }
}


void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        static IMFrame frame;
        frame.Reset();
        long mm=millis();
        DataDS18B20(frame);
        DBGINFO(" :");
        DBGINFO(millis()-mm);
        

        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
        ERRFLASH();
      } else{
         trx.printCycle();


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

void PrintStatus()
{
  DBGINFO("\r\n");
  DBGINFO(" Status ");
  trx.printStatus();
  DBGINFO("\r\n");

}


void stageloop(byte stage)
{
//   if (stage== STARTBROADCAST){
    DBGINFO("stageloop=");  DBGINFO(millis());
    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();  PrepareData();   break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: SendData();  SendDataFlood();break;
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

   DBGINFO("@@\r\n");

}





void setup()
{
  delay(1000);
  INITDBG();
  ERRLEDINIT();   ERRLEDOFF();
  IMMAC ad=SetupDS18B20();
  wdt_enable(WDTO_8S);

  interrupts ();
  randomSeed(analogRead(0)+internalrandom());
  trx.myMAC=MMAC;
  trx.myMAC+=ad;
//  trx.Init(cc1101);
  trx.Init(buffer);

  //  DBGINFO(" MMAC ");  DBGINFO2(trx.myMAC,HEX);
  //      DBGINFO("  ");
  trx.myDevice=MDEVICE;
  trx.timer.onStage=stageloop;
  pciSetup(9);
  if (ad>0){
    ERRLEDON();
    delay(1000);
    delay(200);
    ERRLEDOFF();
   } else{
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    delay(200);
    ERRLEDON();
    delay(300);
    ERRLEDOFF();
    reboot();

  }     
//  trx.TimerSetup();
//   DBGINFO("classtest Timer");  DBGINFO(IMTimer::ClassTest());
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
