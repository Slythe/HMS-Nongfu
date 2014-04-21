#include <Arduino.h>

const int pinDistanceSensor = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(pinDistanceSensor, INPUT);
}

void loop()
{

	int i = analogRead(pinDistanceSensor);
	delay(50);
	i = analogRead(pinDistanceSensor);
	Serial.println(i);
	delay(500);

}