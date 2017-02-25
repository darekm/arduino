
/*
 Copyright (C) 201 Dariusz Mazur, <darekm@madar.com.pl>

 */


#include "imframe.h"
#include "imatmega.h"
#include "imdebug.h"
//#include <SPI.h>

//#include <avr/wdt.h>
//#include <avr/power.h>
#include "Wire.h"

#include "HMC5883L.h"
#include "GY271.h"



unsigned long  last_sent=0;

int pinLED = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
unsigned long  interval=2000;
unsigned long  last_sent2=0;

//IMFrame frame;


void PrepareData(){
   digitalWrite(pinLED,HIGH);
   PrepareGY271();
   digitalWrite(pinLED,LOW);
}

void SendData()
{
 static  IMFrame frame;

     frame.Reset();
     DataGY271(frame);
      DBGINFO("<");
     IMFrameData *data =frame.Data();
//      DBGINFO((long)data);
//      DBGINFO(" ");
 
      for (int i=0;i<9;i++){
     DBGINFO(data->w[i]);
//      DBGINFO(Measure[i]);
      DBGINFO(" ");
     }
    DBGINFO("  ::  ");
     for (int i=0;i<20;i++){
//      DBGINFO(Measure[i]);
//      DBGINFO(" ");
     }
    DBGINFO("\r\n");
}





void setup(void)
{
  resetPin();
  INITDBG();
  DBGINFO("***start**");
  DBGINFO(freeRam());
  power_timer0_enable();

  SetupADC();
  interrupts();
//  disableADCB();

   SetupGY271();

}

unsigned long now=0;
void loop() {
  
//  network.update();                          // Check the network regularly
//LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 
  
//  unsigned long 
  now = millis();              // If it's time to send a message, send it!
//    delay(100);
  if ( (now - last_sent) >= interval  )
  {
    DBGINFO("A");
    DBGINFO(now);
    DBGINFO("B");
    DBGINFO((long)&now);
    DBGINFO("A");
    last_sent = now;
    PrepareData();
    SendData();


  }
//   LowPower.idle(SLEEP_1S, ADC_ON, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
//                SPI_ON, USART0_ON, TWI_OFF);

}


