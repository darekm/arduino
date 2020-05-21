
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
//#include <avr/wdt.h>
#include <SPI.h>





//#.define DBGLVL 2
#include "imdebug.h"



/******************************** Configuration *************************************/

#define MMAC 0x130020  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 3     //Type of device
#define MCHANNEL 1





/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imbuffer.h"
#include "imbufrfm69.h"
//#include "imbuftest.h"


Transceiver trx;
IMBuffer    buf3;



#define pinLED  9
int COUNTER=0;


void SendDataFlood()
{
     static IMFrame frame;
     frame.Reset();
     COUNTER++;
     IMFrameData *data =frame.Data();
     for(byte i = 0; i < 4; i++){
        data->w[0]=100;
        data->w[2]=COUNTER;
        data->w[3]=i;
        data->w[4]=COUNTER;

        trx.SendData(frame);
//        trx.Transmit();
       }
    
}


void SendData()
{
      if (trx.CycleData())
      {
    //    DBGLEDON();
        static IMFrame frame;
        frame.Reset();
         COUNTER++;
         IMFrameData *data =frame.Data();
        data->w[0]=100;
        data->w[2]=COUNTER;
        data->w[3]=1;
       // data->w[4]=trx.myMacLo;
        data->w[6]=trx.Connected();
        
         trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
        DBGLEDOFF();         
     }
}



void ReceiveData()
{
      DBGLEDON();
        while (trx.GetData())
      {
        if (trx.Parse())
        {
      //    DBGINFO(" rxGET ");
        }
      }
     DBGLEDOFF();
         
}


void stageloop(byte stage)
{
  switch (stage)
  {
    case STARTBROADCAST: DBGPINHIGH();trx.Knock();DBGPINLOW();DBGPINHIGH(); DBGPINLOW(); break;
//    case STOPBROADCAST:  trx.StopListenBroadcast();      break;
    case STOPBROADCAST: DBGPINHIGH();DBGPINLOW(); trx.StopListenBroadcast();DBGPINHIGH();DBGPINLOW();      break;
    case STARTDATA:DBGPINHIGH(); SendData();/*trx.ListenData(); */ break;
    case STOPDATA:  DBGPINLOW(); trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
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
  DBGPINHIGH();
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
   wdt_enable(WDTO_8S);
   disableADCB();
   power_timer0_enable();
   interrupts ();
 //  delay(1000);
//  randomSeed(analogRead(0)+internalrandom());
  DBGLEDOFF();
  trx.myMAC=MMAC;
  trx.startMAC=0;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.myDevice=MDEVICE;
      DBGINFO2(trx.myMAC,HEX);
//  trx.NoRadio=true;
  trx.Init(buf3);
  trx.myDevice=MDEVICE;
  #if DBGLED>=1
    DBGLEDON();
    delaySleepT2(500);
    DBGLEDOFF();
  #endif
  power_timer0_disable();
//  power_all_disable(); 
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  DBGINFO("ASSR_") ; DBGINFO(ASSR);
  DBGINFO("CLKPR_") ;DBGINFO(CLKPR);
 
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
