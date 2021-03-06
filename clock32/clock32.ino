
#include <imframe.h>
#include <imatmega.h>
#include <avr/power.h> 
//#include <avr/wdt.h>
//#include <SPI.h>
// Data wire is plugged into pin 2 on the Arduino







//#.define DBGLVL 2
#include "imdebug.h"





/******************************** Configuration *************************************/

#define MMAC 0x200001  // My MAC
#define ServerMAC 0xA000  // Server  MAC
#define MDEVICE 7     //Type of device






/************************* Module specyfic functions **********************/


//#include "imtrans.h"
//#include "imbuffer.h"
//#include "imbufrfm69.h"
//#include "imbuftest.h"


//IMBuffer    buf3;
//Transceiver trx;







byte toggle2;
      





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


void SendData()
{
/*   if (trx.Connected())
   {
      if (trx.CycleData())
      {
        static IMFrame frame;
        frame.Reset();
        long mm=millis();
//        DataDS18B20(frame);
        DBGINFO(" :");
        DBGINFO(millis()-mm);
        

        DBGINFO("SendData ");
        trx.SendData(frame);
        trx.Transmit();
      } else{
         trx.printCycle();


      }

   } else {
     trx.ListenBroadcast();
   }

*/
}




void PrintStatus()
{
  DBGINFO("\r\n");
  DBGINFO(" Status ");
//  trx.printStatus();
  DBGINFO("\r\n");

}

#define CRYSTAL32K 

void setupTimer3()
{
//http://electronics.stackexchange.com/questions/26363/how-do-i-create-a-timer-interrupt-with-arduino
  TCCR2B = 0x00;        //Disable Timer2 while we set it up
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag

   /* First disable the timer overflow interrupt while we're configuring */
  TIMSK2 &= ~(1<<TOIE2);
  TIMSK2 &= ~(1<<OCIE2A);
  TIMSK2=0;

  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));

  TCCR2A |= ((1<<WGM21) );
//  TCCR2A=2;

  TCCR2B &= ~(1<<WGM22);

  /* Select clock source: internal I/O clock */
  //  ASSR &= ~((1<<EXCLK));
  /* Disable Compare Match A interrupt enable (only want overflow) */
  #ifdef CRYSTAL32K
//   TCCR2A &= ~(1<<WGM21);
//  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag

 
 // DDRB|=(1<<DDB3);
  DDRD|= (1<<DDD3);
 // TCCR2A |= ((1<<COM2A0) );
 // TCCR2A &= ~(1<<COM2A1);
  TCCR2A |= ((1<<COM2B0) );
  TCCR2A &= ~(1<<COM2B1);  
 
  ASSR |= ((1<<AS2));
  TCCR2B=0x3;
//  TCCR2B &= ~((1<<CS22)  | (1<<CS20)); // Set bits
//  TCCR2B &= ~(1<<CS22);             // Clear bit
//  TCCR2B &= ~(1<<CS20);             // Clear bit
 // TCCR2B |= (1<<FOC2A);             // Clear bit
 // TCCR2B |= (1<<FOC2B);             // Clear bit
  OCR2A= 15;
  #else
   ASSR &= ~((1<<AS2));

                                jkhbkjbh

  /* Now configure the prescaler to CPU clock divided by 128 */
  TCCR2B |= (1<<CS22)  | (1<<CS20); // Set bits
  TCCR2B &= ~(1<<CS21);             // Clear bit
   //  if (F_CPU==8000000L)
    TCCR2B &= ~(1<<CS20);             // Clear bit

//      TCCR2B |= (1<<CS21)  ; // Set bits
  /* We need to calculate a proper value to load the timer counter.
   * The following loads the value 131 into the Timer 2 counter register
   * The math behind this is:
   * (CPU frequency) / (prescaler value) = 125000 Hz = 8us.
   * (desired period) / 8us = 125.
   * MAX(uint8) + 1 - 125 = 131;
   */
  /* Save value globally for later reload in ISR */

  //Setup Timer2 to fire every 1ms
  /* Finally load end enable the timer */
  OCR2A= 124;
  #endif
  // (clock /prescaler*desired_time )-1
  // 16000000 /126*0.001 = 125

//  TCNT2 = counterTCNT2;
//  TIMSK2 |= (1<<TOIE2);
  TIMSK2 |= (1<<OCIE2A);


//  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
//  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
//  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
//  TCCR2B = 0x05;        //Timer2 Control Reg B: Timer Prescaler set to 128
}      


void setup()
{
  for (byte i=0; i<20; i++) {    //make all pins inputs with pullups enabled
        pinMode(i, INPUT_PULLUP);
   }

  pinMode(DBGPIN,OUTPUT);
  pinMode(DBGCLOCK,INPUT);
  pinMode(DBGCLOCK,OUTPUT);
  wdt_disable();
  INITDBG();
 // digitalWrite(DBGPIN,HIGH);
  DBGINFO("SETUP");
  digitalWrite(DBGCLOCK,HIGH);
//  DBGINFO(freeRam());
//  DBGINFO(buf3._IM);
  
  DBGINFO("_");
 // setupTimer2();
  DBGINFO("TCCR2A_") ; DBGINFO(TCCR2A);
  DBGINFO("TCCR2B_") ; DBGINFO(TCCR2B);
  DBGINFO("TIMSK2_") ; DBGINFO(TIMSK2);
  ERRLEDINIT();
  ERRLEDOFF();
  toggle2=true;
  //  wdt_enable(WDTO_8S);
  disableADCB();
  power_timer0_disable();
//      power_spi_disable(); // SPI

  DBGPINHIGH();
   interrupts ();
//  randomSeed(analogRead(0)+internalrandom());

 
 // trx.timer.onStage=stageloop;
//  trx.DisableWatchdog();
     //  trx.TimerSetup();
    //   DBGINFO("classtest Timer");
 // DBGINFO(IMTimer::ClassTest());
  DBGINFO(incTimer2());
    DBGINFO(millisT2());
    DBGINFO("\r\n");
//  CLKPR = 0x80;    // Tell the AtMega we want to change the system clock
//  CLKPR = 0x00;    // 1/256 prescaler = 60KHz for a 16MHz crystal

  setupTimer3();
}


#if defined(__AVR_ATmega328P__)
ISR(TIMER2_COMPA_vect) {
 // incTimer2();
  #ifdef DBGCLOCK
    toggle2 = ~toggle2;
  /*  if (toggle){
        PORTD|=(1<<PORTD6);
    } else{
        PORTD&=~(1<<PORTD6);
    };
*/
//  digitalWrite(DBGCLOCK,toggle2);  
 // digitalWrite(DBGCLOCK,~toggle2);
      digitalWrite(DBGCLOCK,toggle2);
  // digitalWrite(DBGCLOCK,LOW);
 //   digitalWrite(DBGCLOCK,HIGH);
  #endif
}
#endif        
void loop()
{

//  wdt_reset();
//  PrintStatus();
//  delay(300);
//  DBGINFO("\r\n");
  DBGINFO("\r\nLOOP");
  DBGINFO(incTimer2());
  DBGINFO(millisT2());
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable();
  long watchdog =0;
  do{
  //   sei();
     watchdog++; 
     while ((ASSR & (1<<OCR2AUB)) != 0x00) {
       DBGPINLOW();
       DBGPINHIGH();
     };
     goSleep();
  //   cli();
     OCR2A= 15;
    
   }while( watchdog<10);
// sleep_disable(); 
 sei();
  DBGPINLOW();
  DBGPINHIGH();
  digitalWrite(DBGCLOCK,LOW);

}
