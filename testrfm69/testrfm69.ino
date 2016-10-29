
#include <imframe.h>
#include <imatmega.h>
//#include <avr/wdt.h>
#include <SPI.h>
// Data wire is plugged into pin 2 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x130020  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 3     //Type of device






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imbuffer.h"
#include "imbufrfm69.h"
//#include "imbuftest.h"


Transceiver trx;
IMBuffer    buf3;












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
       DBGINFO("\r\n");
}
     
      ERRFLASH();
     
}


void SendData()
{
   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        static IMFrame frame;
        frame.Reset();
         COUNTER++;
         IMFrameData *data =frame.Data();
//        long mm=millis();
//        DataDS18B20(frame);
//        DBGINFO(" :");
//        DBGINFO(millis()-mm);
        data->w[0]=100;
        data->w[2]=COUNTER;
        data->w[3]=1;
        data->w[4]=COUNTER;
        

//        DBGINFO("SendData ");
        trx.SendData(frame);
//        trx.Transmit();
//        ERRFLASH();
      } else{
         trx.printCycle();


      }

   } else {
     trx.ListenBroadcast();
   }


}



void ReceiveData()
{
//ERRLEDON();
//  DBGINFO(" Receive ");
      while (trx.GetData())
      {
        if (trx.Parse())
        {
          DBGINFO(" rxGET ");
        }
      }
      DBGINFO("\r\n");

//ERRLEDOFF();

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
//    DBGINFO("stageloop=");  DBGINFO(millisT2());
//    DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  trx.ListenBroadcast();   break;
//    case STOPBROADCAST:  trx.StopListenBroadcast();      break;
    case STOPBROADCAST:  trx.Knock();      break;
    case STARTDATA: trx.Wakeup();SendData();/*trx.ListenData(); */ break;
    case STOPDATA:   trx.StopListen();      break;
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

   DBGINFO("@@\r\n");

}





void setup()
{
  noInterrupts(); 
  pinMode(DBGPIN ,OUTPUT);
  digitalWrite(DBGPIN ,HIGH);
  digitalWrite(DBGPIN ,LOW);
  wdt_disable();
  INITDBG();
  digitalWrite(DBGPIN ,HIGH);
  DBGINFO("SETUP");
  digitalWrite(DBGPIN,LOW);
//  DBGINFO(freeRam());
//  DBGINFO(buf3._IM);
  
  DBGINFO("_");
//  setupTimer2();
  digitalWrite(4,HIGH);
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  ERRLEDINIT();
  ERRLEDOFF();
  //  wdt_enable(WDTO_8S);
//  disableADCB();
  digitalWrite(4,LOW);
   interrupts ();
//  randomSeed(analogRead(0)+internalrandom());

  trx.myMAC=MMAC;
      DBGINFO2(trx.myMAC,HEX);
  trx.Init(buf3);
  trx.myDevice=MDEVICE;
 // trx.timer.onStage=stageloop;
//  trx.DisableWatchdog();
  
  long start2=millis();
  long start2T=millisT2();
//  pciSetup(9);
    ERRLEDON();
 //   delay(1000);
    delay(200);
    ERRLEDOFF();
     //  trx.TimerSetup();
    //   DBGINFO("classtest Timer");
 // DBGINFO(IMTimer::ClassTest());
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  DBGINFO("ASSR_") ; DBGINFO(ASSR);
  DBGINFO("CLKPR_") ;DBGINFO(CLKPR);
//  CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
//  CLKPR = 0x00;    // 1/256 prescaler = 60KHz for a 16MHz crystal

/*  delay(1000);
  start2 -=millis();
  start2T -=millisT2();
  Serial.flush();
  DBGINFO("TIMER") ; DBGINFO(start2);DBGINFO("  ");DBGINFO(start2T);
  start2=millis();
  start2T=millisT2();
  delay(1200);
  DBGINFO("\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") ;  
  start2 -=millis();
  start2T -=millisT2();
  DBGINFO("\r\nTIMERB") ; DBGINFO(start2);DBGINFO("  ");DBGINFO(start2T);
  Serial.flush();
  start2=millis();
  start2T=millisT2();
  delaySleepT2(1200);
DBGINFO("\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") ;  
  start2 -=millis();
  start2T -=millisT2();
  DBGINFO("\r\nTIMERST") ; DBGINFO(start2);DBGINFO("  ");DBGINFO(start2T);
  Serial.flush();
  start2=millis();
  start2T=millisT2();
  delaySleep(1200);
  start2 -=millis();
  start2T -=millisT2();
  DBGINFO("\r\nTIMERS") ; DBGINFO(start2);DBGINFO("  ");DBGINFO(start2T);
  */
  /*
  Serial.flush();
  for (uint8_t i=0;i<32;i++)\
  {
    Serial.println(i);
  } 
 */ 
  for (uint8_t ii=0;ii<8;ii++)
  {
  for (uint8_t i=0;i<16;i++)
  {
    trx.timer.Watchdog(100);
  } 
   DBGINFO("\r\n");
   }

}

void loop()
{
/*  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  DBGINFO("ASSR_") ; DBGINFO(ASSR);
  DBGINFO("CLKPR_") ;DBGINFO(CLKPR);
  */
//  wdt_reset();
//  PrintStatus();
//  delay(300);
  DBGINFO("\r\n");
  DBGINFO("LOOP");
    
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
