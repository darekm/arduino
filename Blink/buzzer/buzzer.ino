
//http://sprae.jogger.pl/2009/02/26/skynet-czyli-jak-opanowac-arduino-cz-3/
int spkPin = 7; // przydzielenie etykiety dla pinu glosnika

void setup()
{
  pinMode(spkPin, OUTPUT); // ustawienie pinu glosnika jako wyjscie
}

void loop()
{
  digitalWrite(spkPin, LOW); // pierwsza czesc okresu fali
  delayMicroseconds(20);
  digitalWrite(spkPin, HIGH); // druga czesc okresu fali
  delayMicroseconds(120);
}


