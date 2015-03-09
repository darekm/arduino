#include <SPI.h>

/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.
 
 Most Arduinos have an on-board LED you can control. On the Uno and
 Leonardo, it is attached to digital pin 13. If you're unsure what
 pin the on-board LED is connected to on your Arduino model, check
 the documentation at http://arduino.cc
 
 This example code is in the public domain.
 
 modified 8 May 2014
 by Scott Fitzgerald
 */
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

int buttonState = 0;         // variable for reading the pushbutton status
int interval = 0;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);     

}

// the loop function runs over and over again forever
void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {     
    // turn LED on:    
    interval=100;
  } else {
    interval=4,0000000000000000000000;
  }  
  digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(interval);              // wait for a second
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  delay(1800);              // wait for a second
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(interval);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1800);              // wait for a second
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(interval);              // wait for a second
  
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(4000-interval);              // wait for a second
  
}

