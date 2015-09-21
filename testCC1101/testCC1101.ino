/*
        radino CC1101 UART-Bridge
        for more information: www.in-circuit.de or www.radino.cc

	Copyright (c) 2014 In-Circuit GmbH
        
        v0.1 - 2014.12.19
	    Basic implementation - functions and comments are subject to change
        
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software
	and associated documentation files (the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
	LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//#include <SoftwareSerial.h>
//#include <avr/wdt.h>
#include "transceiver.h"
#include "imqueue.h"
// #.include "imack.h"
#include "uart.h"



/******************************** Configuration *************************************/

// #define NWID 0x01  //My networks ID
#define MID  0x00  //My ID
#define TID  0x00  //Default target ID
// #define HOPS 0x01  //Max hops - currently unused


#define DINRAILADAPTER 0  //Is a DinRailAdapter (Bridge SoftwareSerial on pins 10,11,12)
#define RADINOSPIDER   1  //Is a radino Spider (Bridge Serial1)

#define RADIO_BUFFSIZE 725  //Buffer for radio
#define RADIO_SENDTHRES 710  //Start writing when this full

#define UART_TIMEOUT 10  //UART is considered empty after this time in ms
#define RESEND_TIMEOUT 75  //Wait this time in ms for an radio ack
#define PACKET_GAP 10 //Delay between receive and outgoing ack

//Used for error signaling (ON when restransmitting, OFF on receive success)
#define ERRLEDON() digitalWrite(13,HIGH)
#define ERRLEDOFF() digitalWrite(13,LOW)
#define ERRLEDINIT() pinMode(13, OUTPUT)

//Write messages on dbgSerial
//0:none
//1:Errors
//2:Information
#define DBGLVL 2

/***************************** Debug messages ***********************/

//Write out errors on dbgSerial
#if DBGLVL>=1
  #define DBGERR(x) dbgSerial.print(x)
  #define DBGERR2(x,y) dbgSerial.print(x,y)
  #define DBGERRWRITE(x) dbgSerial.write(x)
#else
  #define DBGERR(x) do{}while(0)
  #define DBGERR2(x,y) do{}while(0)
  #define DBGERRWRITE(x) do{}while(0)
#endif

//Write out information on dbgSerial
#if DBGLVL>=2
  #define DBGINFO(x) dbgSerial.print(x)
  #define DBGINFO2(x,y) dbgSerial.print(x,y)
  #define DBGWRITE(x) dbgSerial.write(x)
#else
  #define DBGINFO(x) do{}while(0)
  #define DBGINFO2(x,y) do{}while(0)
  #define DBGWRITE(x) do{}while(0)
#endif

/******************************* Module specific settings **********************/
#undef TXEN_PIN


  #define dbgSerial Serial
  #define dbgSerialSpeed 57600


  
  #define bridge Serial1
  #define bridgeSpeed 9600
  #define bridgeBurst 10  //Chars written out per call
  #define bridgeDelay 3000  //Time between calls - Let hardware serial write out async
//  #define bridgeDelay ((bridgeBurst*1000)/(bridgeSpeed/8))  //Time between calls - Let hardware serial write out async
//  #define TXEN_PIN 2  //Which pin to use for RS485 functionality - comment to disable
  




#define INDEX_NWID 0  //Network ID
#define INDEX_SRC  1  //Source ID
#define INDEX_DEST 2  //Target ID
#define INDEX_SEQ  3  //Senders current sequence number
#define INDEX_PSEQ 4  //Last sequence number heard from target
#define INDEX_HOPC 5  //Hopcount
#define INDEX_SIZE 6  //Size of Packet
#define INDEX_CRC  7  //CRC



/****************** Vars *******************************/


uint32_t uart_timeout = 0;

char radioBuf[RADIO_BUFFSIZE] = {0,};
unsigned short radioBufLen = 0;
unsigned short radio_writeout = 0xFFFF;
unsigned long radioOut_delay = 0;

unsigned long resend_to = 0;

Transceiver trx;
    CC1101 cc1101;  //The CC1101 device
    IMQueue queue;
   IMFrame fr1;
   IMFrame fr2;

/************************* Functions **********************/


void printRadio()
{
    Serial.print("UART");
    Serial.print(radioBufLen);
    for (int i=0 ; i<bridgeBurst && (radio_writeout<radioBufLen) ; radio_writeout++, i++ )
    {
      Serial.write(radioBuf[radio_writeout]);
    }
    Serial.println(";");

}
void printReceive()
{
       DBGINFO("Receive(");
      DBGINFO(trx.rSize);
      DBGINFO("): ");
      for (unsigned short i=0;i<trx.rSize ;i++)
      {
        DBGINFO2(((uint8_t*)&trx.RX_buffer)[i],HEX);
        DBGWRITE(' ');
      }
      DBGINFO("-> ");
} 



void testQueue()
{
  fr1.Header.Function=resend_to;
  queue.push(fr1);
  Serial.print(resend_to);
  Serial.print("::");
  Serial.print(queue.temp);
  Serial.print("=");
//  Serial.print(queue.tab[1].Header.Function);
//  Serial.print(queue.tab[(queue.temp-1) & _QueueMask);

  Serial.print("Queue");
  Serial.print(queue.pop(fr2) );
  Serial.print("=");
  Serial.print(fr2.Header.Function );
}  

//Initialize the system
void setup()
{
//  wdt_enable(WDTO_8S);  //Watchdog 8s
  dbgSerial.begin(dbgSerialSpeed);
  Serial.print("start");
  dbgSerial.print("\r\n\r\nHello\r\n\r\n");
  
  bridge.begin(bridgeSpeed);
//  TXEN_INIT();
  
  ERRLEDINIT(); ERRLEDOFF();
  trx.Init(cc1101);
  trx.myID= MID;

}

//Main loop is called over and over again
void loop()
{
         ERRLEDON();
         delay(50);
         ERRLEDOFF();
 
  static IMFrame frame;


  testQueue();
  

   //Check for freezes
//  wdt_reset();  //At least arduino is running
  trx.StartReceive();

  
/************** radio to UART ************************/

  //No chars received since UARTTIMEOUT ms AND have chars in outputbuffer
  //writes bridgeBurst chars every bridgeDelay ms (Let the hardware serial write out async)
  if /*(uart_timeout<millis() && radio_writeout<radioBufLen &&*/( radioOut_delay<millis())
  {
    radioOut_delay = millis()+bridgeDelay;
    printRadio();
    radio_writeout = 0xFFFF;  //signal write complete to radio reception
    radioBufLen = 0;
  }

  delay(500);
  
  
  
  if (trx.GetData()  )
    {
      printReceive();      
      
      
      if( trx.Valid())
      {
          trx.Rssi();
          DBGINFO(" RSSI: ");           DBGINFO(trx.rssi);            DBGINFO("dBm");
          DBGINFO(" CRC: ");            DBGINFO(trx.crc);             DBGINFO(" rr: ");

          if (trx.crcCheck()==0)
          {
            resend_to = millis()+PACKET_GAP;  //short delay on answers
            radioBufLen+=trx.Get((uint8_t*)&(radioBuf[radioBufLen]));
            if (radio_writeout == 0xFFFF) radio_writeout = 0;  //signal the uart handler to start writing out
          } else { //end if CRC ok
            DBGERR("!CRC");
          }
      } else { //end if size valid
        DBGERR("!SIZE");
      }
      DBGINFO("\r\n");

  }
  
 
/************** UART to radio ************************/
/*  
  //read incoming chars from UART
  crc = bridge.available();
  while(uartBufLen<UART_BUFFSIZE && crc)  //still have bufferspace and remaining chars on uart
  {
    cnt = bridge.read();
    crc--;
    //last char within timeout OR fresh block
    if (uart_timeout>millis() || uartBufLen==0)
    {
      uartBuf[uartBufLen++] = cnt;
      uart_timeout = millis()+UART_TIMEOUT;
    }
    if (!crc) crc = bridge.available();
  }
  */

  generatorUart();    Serial.println(" R  ");
   
  
  //Buffer has content AND since timeout no chars || Buffer over filllevel || lastseqnr waiting ack || partner send new seqnr
  if (resend_to<millis() && ((uartBufLen>0 && uart_timeout<millis()) || uartBufLen>=UART_SENDTHRES )  )
  {
    DBGINFO(millis());    DBGINFO(" ");
    DBGINFO(resend_to);    DBGINFO(" ");
    DBGINFO(uart_timeout);    DBGINFO(" ");
    DBGINFO(uartBufLen);    DBGINFO(" ");

    //Still no ack for last sent sequence number -> resend
    if (trx.ack.noack(TID))
    {
      //retried too often
      if (trx.ack.retry())
      {
//        ERRLEDON();
        DBGERR("reached max Retries\r\n");
        trx.StartReceive();
        Serial.flush();
        return;
      } else {
        DBGINFO("Retry (");
      }
    }
    
    //Next data chunk [Last packet was acked AND ( (we have data AND uart timed out) OR uart beyond threshold) ]
    else if ( ((uartBufLen>0 && uart_timeout<millis()) || uartBufLen>UART_SENDTHRES))
    {
//      lastpartnerseqnr = partnerseqnr;  //update ack signal
            //Refill packet buffer with new data
      UartPrepareData(frame);
      DBGINFO("Sending (");
    }
    
    //Last packet was acked and no new data to send -> send only ack
    else     {
      trx.txHeader->Len = 0;
      DBGINFO("SendAck (");
    }
      DBGINFO("return");
    return;
    
    trx.PrepareTransmit(TID);
//    trx.txHeader->pseq = ack.Answer(TID);

    DBGINFO(trx.TX_buffer.len);
    DBGINFO(",");
    DBGINFO(trx.txHeader->Len);
    DBGINFO(",");
    DBGINFO(trx.txHeader->crc);
    
    if (cc1101.StopReceive())
    {
//         ERRLEDON();
//         delay(100);
//         ERRLEDOFF();
         DBGINFO("transmit");
      if (trx.Transmit())
      {
      } else {
      }
      resend_to = millis()+RESEND_TIMEOUT;
    }
    else
    {
      DBGERR("ERROR Stop Receive\r\n");
    }
    DBGINFO(")\r\n");
  }
  */
  Serial.flush();
 delay(500);
 /*
 ERRLEDON();
 delay(100);
 ERRLEDOFF();
 delay(100);
  
 ERRLEDON();
 delay(100);
 ERRLEDOFF();
 */
  
}

