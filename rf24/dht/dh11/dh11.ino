
#include <dht.h>
#include "LowPower.h"


//https://github.com/petervojtek/diy/wiki/Arduino-with-Very-Low-Power-Consumption
#define dht_dpin A2 //no ; here. Set equal to channel sensor is on

dht DHT;
const int ledPin =  13;      // the number of the LED pin


// http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long vccVoltage() {
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


//https://github.com/rocketscream/Low-Power
void sleepTenMinutes()
{
  for (int i = 0; i < 75; i++) { 
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  }
}




void setup(){
   pinMode(ledPin, OUTPUT);

  Serial.begin(57600);
  delay(300);//Let system settle
  Serial.println("Humidity and temperature\n\n");
  delay(700);//Wait rest of 1000ms recommended delay before
  //accessing sensor
}//end "setup()"

void loop(){
  //This is the "heart" of the program.
 // DHT.read11(dht_dpin);
    Serial.print("Battery:");
    Serial.print( vccVoltage(), DEC );
    Serial.print("V  temp:");
    Serial.print(internalTemp(),DEC);
    Serial.print("C ");
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
     digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);              // wait for a second
  LowPower.powerDown(SLEEP_250MS,ADC_OFF,BOD_OFF);
  digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW

 LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_OFF);

  //should be once per two seconds, fastest,
  //but seems to work after 0.8 second.
}// end loop()

