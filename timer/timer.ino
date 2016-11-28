
#include <imframe.h>
#include <imatmega.h>
//#include <avr/wdt.h>
#include <SPI.h>
// Data wire is plugged into pin 2 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x200001  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device






/************************* Module specyfic functions **********************/


#include "imtimer.h"
//#include "imbuffer.h"
//#include "imbufrfm69.h"
//#include "imbuftest.h"

IMTimer  timer;













int COUNTER=0;


void DataFlash()
{
    power_spi_disable();
    power_timer0_disable();
  //  power_timer2_enable();
  ShutOffADC();
  DBGPINLOW(); 
     // ERRFLASH();
      delaySleepT2(1000);
      DBGPINHIGH();
     power_spi_enable(); 
     power_timer0_enable();
}    








void PrintStatus()
{
  DBGINFO("\r\n");
  DBGINFO(" Status ");
//  trx.printStatus();
  DBGINFO("\r\n");

}



#define BroadcastDelay 200
#define BroadcastDuration 50


#define DataDelay 600
#define DataDuration 100
#define CCDuration 1024UL





void setup()
{
  for (byte i=0; i<20; i++) {    //make all pins inputs with pullups enabled
        pinMode(i, INPUT_PULLUP);
   }

  pinMode(DBGPIN,OUTPUT);
  pinMode(DBGCLOCK,OUTPUT);
  wdt_disable();
  INITDBG();
 // digitalWrite(DBGPIN,HIGH);
  DBGINFO("SETUP");

  DBGINFO("_");
 // setupTimer2();
  digitalWrite(DBGPIN,HIGH);
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  ERRLEDINIT();
  ERRLEDOFF();
  //  wdt_enable(WDTO_8S);
  disableADCB();
  power_timer0_disable();
  DBGPINLOW();
   interrupts ();
  timer.Setup(STARTBROADCAST,BroadcastDelay);
  timer.Setup(IMTimer::PERIOD,CCDuration);


   timer.Setup(STARTDATA,DataDelay);
    timer.Setup(STOPDATA,DataDelay+DataDuration);
    timer.Setup(STOPBROADCAST,BroadcastDelay+BroadcastDuration);

 // trx.timer.onStage=stageloop;
//  trx.DisableWatchdog();


  DBGINFO("CLKPR_") ;DBGINFO(CLKPR);
  DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
//  CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
//  CLKPR = 0x00;    // 1/256 prescaler = 60KHz for a 16MHz crystal



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
  setupTimer2();
}

void loop()
{

//  wdt_reset();
//  PrintStatus();
//  delay(300);
//  DBGINFO("\r\n");
  DBGINFO("\r\nLOOP");
  DBGINFO(incTimer2());
  DBGINFO(millisT2());
    
  byte xstage;
  do{
     DBGPINLOW();
     xstage=timer.WaitStage();
     DBGPINHIGH();
  }while( xstage!=IMTimer::PERIOD);


}
