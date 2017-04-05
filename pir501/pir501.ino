
#include <imframe.h>

// Data wire is plugged into pin 2 on the Arduino
#define PIR_BUS A1







//#.define DBGLVL 2
#include "imdebug.h"
#include "pir501.h"





/******************************** Configuration *************************************/

#define MMAC 0x102070  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 8     //Type of device

#define RadioDelay 2900  //Time between calls - Let hardware serial write out async
#define BroadcastDelay 200
#define BroadcastDuration 600
#define BroadcastCallibrate 300



#define DataDelay 1500
#define DataDuration 600
#define CycleDuration 3000

#define TimerDataCycle 1
#define TimerKnockCycle TimerDataCycle*30
#define TimerHelloCycle 30  
/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"


IMCC1101  cc1101;
Transceiver trx;

int freeRam ()
{
      extern int __heap_start, *__brkval;
      int v;
      return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}






void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}





void OnRead(byte state)
{
  DBGINFO(state);
  if (state==2)
  {
    return ;
  }
  trx.timer.doneListen();
}





void SendData()
{
   if (trx.Connected())
   {
      DBGINFO("CYCLE<");     
      if ((trx.timer.Cycle()%TimerDataCycle)==0){
        DBGINFO(millis());     
        DBGINFO(">");     
        static IMFrame frame;
        frame.Reset();
        if (DataPIR501(frame)){

          DBGINFO("SendData ");
          trx.SendData(frame);
          trx.Transmit();
        } 
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



void stageloop(byte stage)
{
//  DBGINFO("stageloop=");  DBGINFO(millis());
//  DBGINFO(":");  DBGINFO(stage);
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();      break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: SendData();break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case IMTimer::IDDLESTAGE : {
     DBGINFO("***IDDLE DATA");
     DBGINFO(millis());

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
  SetupPIR501();

  interrupts ();
  trx.Init(cc1101);
  trx.myMAC=MMAC;
  trx.myDevice=MDEVICE;
  trx.onEvent=OnRead;
  trx.timer.onStage=stageloop;
  pciSetup(9);

}

void loop()
{
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
