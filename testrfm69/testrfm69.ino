
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
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
       DBGINFO("\r\n");
}
     
      ERRFLASH();
     
}


void SendData()
{
//   if (trx.Connected())
//   {
      if (trx.CycleData())
      {
       digitalWrite(pinLED,HIGH);
    trx.Wakeup();
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
        data->w[6]=trx.Connected();
        

//        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
             digitalWrite(pinLED,LOW);

//        ERRFLASH();
  //    } else{
   //      trx.printCycle();


     }
  //    trx.ListenData();

 //  } else {
  //   trx.ListenBroadcast();
 //  }


}



void ReceiveData()
{
//  static IMFrame rxFrame;
//ERRLEDON();
//  DBGINFO(" Receive ");
//DBGPINHIGH();
//DBGPINLOW();
      while (trx.GetData())
      {
        if (trx.Parse())
        {
      //    DBGINFO(" rxGET ");
        }
      }
      
      DBGINFO("\r\n");

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
    case STARTBROADCAST: DBGPINHIGH();trx.ListenBroadcast();DBGPINLOW();DBGPINHIGH(); trx.ListenBroadcast();  DBGPINLOW(); break;
//    case STOPBROADCAST:  trx.StopListenBroadcast();      break;
    case STOPBROADCAST: DBGPINHIGH();DBGPINLOW(); trx.Knock();DBGPINHIGH();DBGPINLOW();      break;
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

   DBGINFO("@@\r\n");

}





void setup()
{
   resetPin();
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
//    pinMode(DBGPIN,INPUT);
//  pinMode(DBGCLOCK,INPUT);
//  pinMode(DBGPIN,OUTPUT);
//  pinMode(DBGCLOCK,OUTPUT);
  
  DBGPINHIGH();
  DBGPINLOW();
  wdt_disable();
  INITDBG();
  DBGPINHIGH();
  DBGINFO("SETUP");
  DBGPINLOW();

//  DBGINFO(freeRam());
//  DBGINFO(buf3._IM);
  
  DBGINFO("_");
  setupTimer2();
  DBGPINHIGH();
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  ERRLEDINIT();
  ERRLEDOFF();
  //  wdt_enable(WDTO_8S);
   disableADCB();
   power_timer0_enable();
   interrupts ();
   delay(1000);
//  randomSeed(analogRead(0)+internalrandom());

  trx.myMAC=MMAC;
      DBGINFO2(trx.myMAC,HEX);
//  trx.NoRadio=true;
  trx.Init(buf3);
  trx.myDevice=MDEVICE;
  power_timer0_disable();
   
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
  */
 
 
 setupTimer2();
}

void loop()
{
  wdt_reset();
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
