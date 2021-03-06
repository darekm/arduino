/*
 Copyright (C) 2015 Dariusz Mazur, <darekm@emadar.com>
 
 */

/**
 * Simplest possible example of using RF24Network 
 *
 * SENSOR NODE - Soil moisure
 * Every 2 seconds, send a payload to the receiver node.
 */

//#include <RF24Network.h>
//#include <RF24.h>
//#include <SPI.h>
//#include "printf.h"

//#include "LowPower.h"

#include <avr/power.h>
#include <me_frame.h>
#include <me_atmega.h>

Frame frame;

const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t other_node = 00;       // Address of the other node in Octal format
int moistPin = A0;
int moistOut = 3;


const unsigned long interval = 2; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor



#define _ME_Serial_






int moistValue(){
  return analogRead(moistPin);    

}

void printOk(bool ok){
  if (ok)
    Serial.println("ok.");
  else {
    String s("failed.");
    s.setCharAt(1,'A');
    s.setCharAt(2,0);
    Serial.println(s);
  }  
  //     delay(100);

}  

void printPayload(MemsValue_t & p ){
  Serial.print(" action:");
  Serial.print(p.action);
  Serial.print(" time:");
  Serial.print(p.time);
  Serial.print(frame.ToString(p));
  // delay(200);

}

void sendFrame(MemsValue_t p){
  Serial.write(0xFA);
  Serial.write(0xFB);
  
  Serial.println(frame.ToFrame(p)); 
//  Serial.println(":::");
//  frame.write(p);
//  Serial.println("--");
//  Serial.write((const unsigned char *)&p,sizeof(MemsValue_t));
//  Serial.println("==");
}
void setup(void)
{
#ifdef _ME_Serial_
  Serial.begin(57600);
  Serial.println("Test tty send frame");
  Serial.println(sizeof(MemsValue_t));
#endif
  frame.setStream(Serial);
  //  network.begin(/*channel*/ 90, /*node address*/ this_node);

  pinMode(ledPin, OUTPUT);
  pinMode(moistOut,INPUT); 
  Serial.flush(); 

}

void loop() {
  delay(1000);
  //LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 

  unsigned long now = millis();              // If it's time to send a message, send it!
//  Serial.print("A");
//  Serial.print(now);
//  Serial.println("A");
  digitalWrite(ledPin,HIGH);
  //    delay(100);
//  if ( now - last_sent >= interval  )
//  {
    last_sent = now;

    MemsValue_t payload = { 
    20 ,154, packets_sent++         };
    payload.value[0]=moistValue();
    payload.value[1]=internalVcc();
    payload.value[2]=internalTemp();
    payload.value[3]=last_sent;
    payload.ms=0x10203040L;
    bool ok;
    ok=false;
//    printPayload(payload);
    sendFrame(payload);
//    frame.write(payload);
 //   printOk(ok);

    //    RF24NetworkHeader header(/*to node*/ other_node);
    //    bool ok = network.write(header,&payload,sizeof(payload));

  
  Serial.flush();
  delay(200);
  digitalWrite(ledPin,LOW);
}




