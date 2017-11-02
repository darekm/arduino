
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
//#include <avr/wdt.h>
#include <SPI.h>
// Data wire is plugged into pin 2 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x200001  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device
#define MCHANNEL 3






/************************* Module specyfic functions **********************/


#include "imtrans.h"
#include "imbuffer.h"
#include "imbufrfm69.h"
//#include "imbuftest.h"


IMBuffer    buf3;
Transceiver trx;












int COUNTER=0;
uint16_t cpuVin;
uint16_t cpuTemp;
uint16_t cpuVinCycle=0;


void DataVCC(IMFrame &frame)
{   
  if (cpuVinCycle % 8==0){ 
    SetupADC();
    cpuVin=internalVcc();
    cpuTemp=internalTemp();
    cpuTemp=internalTemp();
    ShutOffADC();
    power_adc_disable();
  }
   cpuVinCycle++;
  
   IMFrameData *data =frame.Data();
//   bool ex=sensors.getAddress(deviceAddress, 0);
   data->w[6]=trx.Connected();
 //  Vin=internalVcc();
   data->w[1]=cpuTemp;
   data->w[0]=cpuVin;
   data->w[10]=0xA33B;
}


void DataFlash()
{
    power_spi_disable();
    power_timer0_disable();
  //  power_timer2_enable();
  ShutOffADC();
  DBGPINLOW(); 
     // ERRFLASH();
 //     delaySleepT2(1000);
      DBGPINHIGH();
     power_spi_enable(); 
//     power_timer0_enable();
}    
void SendDataFlood()
{
     static IMFrame frame;
     frame.Reset();
     COUNTER++;
     IMFrameData *data =frame.Data();
     frame.Header.Function=32;
     for(byte i = 0; i < 4; i++){
        data->w[0]=100;
        data->w[1]=i;
        data->w[2]=COUNTER;

        trx.SendData(frame);
//        trx.Transmit();
       DBGINFO("\r\n");
}
     
     
}


void SendData()
{
      if (trx.CycleData()) {
        DBGLEDON();
        trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
        DataVCC(frame);
        
        trx.SendData(frame);
        trx.Transmit();
        DBGLEDOFF();
      }  
}



void ReceiveData()
{
    while(trx.GetData()){ 
  //    if(trx.GetFrame(rxFrame))
      if (trx.Parse())
      {
    //    if (!trx.ParseFrame(rxFrame))
        {
      //    DBGINFO(" rxGET ");
        }
      }
      DBGINFO("\r\n");
    }

}



void stageloop(byte stage)
{
//   if (stage== STARTBROADCAST){
 //   DBGINFO("stageloop=");  DBGINFO(millisT2());
 //   DBGINFO(":");  DBGINFO(stage);
//  }
  switch (stage)
  {
    case STARTBROADCAST:  DBGPINHIGH();trx.Knock();   break;
//    case STOPBROADCAST: digitalWrite(5,HIGH); trx.StopListenBroadcast();      break;
//    case STOPBROADCAST:  trx.Knock();      break;
    case STOPBROADCAST:    DBGPINLOW();SendData(); trx.StopListenBroadcast(); break;
    case STARTDATA: SendData(); break;
    case STOPDATA: SendData();  trx.StopListen();      break;
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
 
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  #ifdef DBGCLOCK
  pinMode(DBGCLOCK,OUTPUT);
  digitalWrite(DBGCLOCK,HIGH);
  #endif  
  wdt_disable();
  INITDBG();
  setupTimer2();
 // digitalWrite(DBGPIN,HIGH);
  DBGINFO("SETUP");
 // digitalWrite(DBGPIN,LOW);
//  DBGINFO(freeRam());
//  DBGINFO(buf3._IM);
  
  DBGINFO("_");
 // setupTimer2();
 // digitalWrite(DBGPIN,HIGH);
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);

 
   wdt_enable(WDTO_8S);
  disableADCB();
  power_timer0_enable();
  DBGPINLOW();
   interrupts ();
//  randomSeed(analogRead(0)+internalrandom());

  trx.myMAC=MMAC;
  trx.myChannel=MCHANNEL;
      DBGINFO2(trx.myMAC,HEX);
//  trx.NoRadio=true;
  trx.Init(buf3);
  trx.myDevice=MDEVICE;
    DBGLEDON();
    delaySleepT2(400);
    DBGLEDOFF();
 
 // trx.timer.onStage=stageloop;
//  trx.DisableWatchdog();
#if DBGLVL >=1  
  long start2=millis();
  long start2T=millisT2();
  
//  pciSetup(9);
  DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
    ERRLEDON(); iiiiii
 //   delay(1000);
    delay(2000); 
    ERRLEDOFF();
    DBGINFO(start2T-start2);
    DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
    delay(200);
    DBGINFO(start2T-start2);
  DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
#endif
     //  trx.TimerSetup();
    //   DBGINFO("classtest Timer");
 // DBGINFO(IMTimer::ClassTest());
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  DBGINFO("ASSR_") ; DBGINFO(ASSR);
  DBGINFO("CLKPR_") ;DBGINFO(CLKPR);
  DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
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
 /*
  for (uint8_t ii=0;ii<8;ii++)
  {
  for (uint8_t i=0;i<16;i++)
  {
    trx.timer.Watchdog(100);
  } 
   DBGINFO("\r\n");
   }
*/
  power_timer0_disable();
  setupTimer2();
 //   TCCR2B=0x2;

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
//  DBGINFO("\r\n");
  DBGINFO("\r\nLOOP");
  DBGINFO(incTimer2());
  DBGINFO(millisT2());
  wdt_reset();  
  byte xstage;
  do{

     xstage=trx.timer.WaitStage();
     stageloop(xstage);
  }while( xstage!=IMTimer::PERIOD);


}
