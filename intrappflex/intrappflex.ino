#include <Wire.h>
#include <avr/power.h>
#include <imframe.h>
#include <imsht.h>
#include <imcharger.h>

#define PinAkku A2
#define EnableI2C A3
#define EnableSHT A0
#define EnableExtPower 6
#define EnableAkku A1
#define EnableRS485 5




//#.define DBGLVL 2
#include "imdebug.h"
#include "intrappflex.h"





/******************************** Configuration *************************************/

#define MMAC 0x102050  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 6     //Type of device

#define RadioDelay 2900  //Time between calls - Let hardware serial write out async
#define BroadcastDelay 200
#define BroadcastDuration 600
#define BroadcastCallibrate 300


#define DataDelay 1000
#define DataDuration 300
#define CycleDuration 3000


/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imtimer.h"

IMCharger	imCharger;

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
      static IMFrame frame;
      frame.Reset();
      DataIntrappFlex(frame);

      DBGINFO("SendData ");
      trx.SendData(frame);
      trx.Transmit();

   } else {
     trx.ListenBroadcast();
   }

}



void ReceiveData()
{
  static IMFrame rxFrame;
  if (trx.GetData()  )
  {
//    trx.printReceive();
      if (trx.GetFrame(rxFrame))
      {
        DBGINFO(" RSSI: ");           DBGINFO(trx.Rssi());            DBGINFO("dBm  ");
        if (!trx.ParseFrame(rxFrame))
        {
          if (rxFrame.NeedACK())
             trx.SendACK(rxFrame);
          DBGINFO(" rxGET ");
        }
      }
      else
      {
        DBGERR("!VALID");
      }
      DBGINFO("\r\n");
  }

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

    default:
    break;
  }
}





void setup()
{
  INITDBG();
  ERRLEDINIT(); ERRLEDOFF();
  SetupIntrappFlex();

  imCharger.Init();
  interrupts ();
  trx.Init(cc1101);
  trx.myMAC=MMAC;
  trx.myDevice=MDEVICE;
  trx.onEvent=OnRead;
  trx.timer.onStage=stageloop;
  pciSetup(9);
//   DBGINFO("classtest Timer");  DBGINFO(IMTimer::ClassTest());
    trx.timer.Setup(trx.timer.PERIOD,CycleDuration);
    trx.timer.Setup(STARTDATA,DataDelay);
    trx.timer.Setup(STOPDATA,DataDelay+DataDuration);
    trx.timer.Setup(STARTBROADCAST,BroadcastDelay);
    trx.timer.Setup(STOPBROADCAST,BroadcastDelay+BroadcastDuration);
}

void loop()
{

  ERRFLASH();
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage==trx.timer.PERIOD);


}
