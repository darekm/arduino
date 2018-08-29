
#include <imframe.h>
#include <imatmega.h>
#include <EEPROM.h>
#include <SPI.h>
#include "imdebug.h"

/******************************** Configuration *************************************/
#define MMAC 0x480018  // My MAC
#define ServerMAC 0xA0000  // Server  MAC
#define MDEVICE 0x48     //Type of device
#define MCHANNEL 2

/************************* Module specyfic functions **********************/

#include "imtrans.h"
#include "imtimer.h"
#include "imbufrfm69.h"

Transceiver trx;
IMBuffer    buffer;

int COUNTER=0;
byte SWtoggle;
t_Time SwitchTime;

#include "qswitch.h"

void PrepareData()
{
//    digitalWrite(LEDB3,HIGH);
  IMTimer::doneMeasure();
  PrepareSwitch();
      if (trx.CycleData())
      {
      //  PrepareQtouch();
      }
//    digitalWrite(LEDB3,LOW);

}
void SendDataFlood()
{
     static IMFrame frame;
     frame.Reset();
     COUNTER++;
     IMFrameData *data =frame.Data();
        data->w[0]=2900;
        data->w[2]=COUNTER;
        data->w[4]=COUNTER;
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();

}


void SendData()
{
      if (trx.CycleData()) {
      //  trx.Wakeup();
        static IMFrame frame;
        frame.Reset();
       IMFrameData *data =frame.Data();
  //      DataQtouch(frame);
        data->w[0]=2950;
        data->w[1]=7;
        data->w[2]=COUNTER;
        trx.Wakeup();
        trx.SendData(frame);
        trx.Transmit();
      }
      digitalWrite(LEDB3,LOW);
}
void StepData(void){
  if ((millisTNow()-SwitchTime)>50 ){
    SwitchTime=millisTNow();
//    IMTimer::doneMeasure();
     idx3++;
     idx1+=6;
     idx2+=5;
      SWtoggle = ~SWtoggle;
 //   digitalWrite(DBGCLOCK,HIGH);
//    digitalWrite(DBGCLOCK,LOW);
    digitalWrite(5,SWtoggle);
  } 

} 

byte OrderData(uint16_t a){
  
  switch(a){
    /*
    case 10:
      digitalWrite(2,HIGH);
    break;
    case 11:
      digitalWrite(2,LOW);
    break;
    case 20:
      digitalWrite(5,HIGH);
    break;
    case 21:
      digitalWrite(5,LOW);
    break;
    case 30:
//      analogWrite(4,0);
        digitalWrite(A4,HIGH);
    break;
    case 31:
//      analogWrite(4,255);
        digitalWrite(A4,LOW);
    break;
    */
    case 1:
//      analogWrite(5,0);
        digitalWrite(A5,HIGH);
    break;
    case 11:
//      analogWrite(5,255);
        digitalWrite(A5,LOW);
    break;
    default:
      DBGLEDON();
    
  }
  //DBGLEDON();
  /*if (a==1){
       DBGLEDON();
  }
  if(a==2){
    digitalWrite(2,HIGH);
  }
  if(a==3){
    digitalWrite(2,LOW);
  }
  */
    //digitalWrite(LEDB3,HIGH);
//  if (a==4)
 //   digitalWrite(LEDB3,HIGH);
  return 1;
}  

void ReceiveData()
{

      while (trx.GetData())
      {
        if (trx.Parse())
        {
          //DBGLEDON();
          DBGINFO(" rxGET ");
        }
      }
}


void MeasureData()
{

  //  IMTimer::doneMeasure();
 // LoopQtouch();
 // if (trx.Connected()){
//  DBGLEDON();
//    digitalWrite(LEDB3,HIGH);
// SendDataFlood();
  PrepareSwitch();
//      digitalWrite(LEDB3,LOW);

 // DBGLEDOFF();
 // }
}

void stageloop(byte stage)
{
  switch (stage)
  {
    case STARTBROADCAST: trx.Knock();      break;
    case STOPBROADCAST:    trx.StopListenBroadcast();PrepareData();    break;
    case STARTDATA: SendData();  /*SendDataFlood();*/break;
    case STOPDATA:   trx.StopListen();      break;
    case LISTENDATA : ReceiveData();break;
    case LISTENBROADCAST : ReceiveData();break;
    case MEASUREDATA: MeasureData();
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
}





void setup()
{
  resetPin();
//  setMaxStepTimer(91);
  #ifdef DBGCLOCK

  pinMode(DBGCLOCK,OUTPUT);
  digitalWrite(DBGCLOCK ,HIGH); 
  #endif
  pinMode(5, OUTPUT);
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  DBGPINHIGH();
  DBGPINLOW();
  INITDBG();
  setupTimer2();
  power_timer0_enable();
  SetupADC();
  wdt_enable(WDTO_8S);
  interrupts();
 // delay(300);
   disableADCB();
  SetupSwitch();

  trx.myMAC=MMAC;
  trx.startMAC=0;
  trx.serverMAC=ServerMAC;
  trx.myChannel=MCHANNEL;
  trx.myDevice=MDEVICE;
  trx.funOrder=&OrderData;
//  trx.setTimerFunction(&StepData);
  trx.Init(buffer);
  trx.NoSleep=true;

  power_timer0_disable();
  setupTimer2();
}

void loop()
{
  wdt_reset();
  byte xstage;
  do{
     xstage=trx.timer.WaitStage();
 //     digitalWrite(LEDB3,HIGH);
     stageloop(xstage);
 //     digitalWrite(LEDB3,LOW);
  }while( xstage!=IMTimer::PERIOD);
}
