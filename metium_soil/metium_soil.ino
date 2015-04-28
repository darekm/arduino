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
#include <RF24.h>
#include <SPI.h>
//#include "printf.h"
#include "LowPower.h"
#include "me_frame.h"
#include "me_soil.h"

RF24 radio(9,10);                    // nRF24L01(+) radio attached using Getting Started board 

//RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t other_node = 00;       // Address of the other node in Octal format
int moistPin = A0;
int moistOut = 3;


const unsigned long interval = 2000; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor







// http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long internalVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

long internalTemp() {
//  https://code.google.com/p/tinkerit/wiki/SecretThermometer
  long result;
  // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = (result - 125) * 1075;
  return result;
}

int moistValue(){
   return analogRead(moistPin);    

}

void setup(void)
{
  Serial.begin(57600);
  Serial.println("Metium/ moisore sensor");
//  printf_begin();
  SPI.begin();
  radio.begin();
//  network.begin(/*channel*/ 90, /*node address*/ this_node);
 radio.setAutoAck(true);
 #ifndef   _ME_Mega_
  radio.printDetails();
 #endif
  pinMode(ledPin, OUTPUT);
  pinMode(moistOut,INPUT);  

}

void loop() {
  
//  network.update();                          // Check the network regularly

  
  unsigned long now = millis();              // If it's time to send a message, send it!
  if ( now - last_sent >= interval  )
  {
    last_sent = now;

    Serial.print("Sending...");
    me_payload_t payload = { last_sent, packets_sent++ ,454};
    payload.value[0]=moistValue();
    payload.value[1]=internalVcc();
    payload.value[2]=internalTemp();
    bool ok;
//    RF24NetworkHeader header(/*to node*/ other_node);
//    bool ok = network.write(header,&payload,sizeof(payload));
    Serial.print(" ms:");
    Serial.print(payload.ms);
    Serial.print(" sent:");
    Serial.print(payload.counter);
    for(int i=0;i++;i<3){
      Serial.print("val ");
      Serial.print(i);
      Serial.print(" =");
      Serial.print(payload.value[i]);
    }
    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");
  }
   LowPower.idle(SLEEP_1S, ADC_ON, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_ON, USART0_ON, TWI_OFF);

}


