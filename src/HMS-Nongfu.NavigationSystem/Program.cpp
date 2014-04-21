#include <Arduino.h>
#include <Servo/Servo.h>


const int pinRightIRDS = 0; //Right infrared distance sensor
const int pinLeftIRDS = 1; //Left infrared distance sensor


Servo rudder; //Servo acting as rudder controller


int rudderPosition; //current rudder angle


struct DistanceSensorValues
{
	int LeftReading;
	int RightReading;
};



void setup()
{

  //Communication for debugging
  Serial.begin(9600);

  //set the pin modes
  pinMode(pinRightIRDS, INPUT);
  pinMode(pinLeftIRDS, INPUT);
  
  //attach the servo
  rudder.attach(3);

  //align the rudder straight for launch
  rudder.write(90);

  //wait one second for the rudder to get there
  delay(1000);

}



DistanceSensorValues ReadCollisionSensorValues()
{

	DistanceSensorValues dVals;
	
	//Read twice to get a more stable value

	//Read left side
	dVals.LeftReading = analogRead(pinLeftIRDS);
	delay(5);
	dVals.LeftReading = analogRead(pinLeftIRDS);

	//Read right side
	dVals.RightReading = analogRead(pinRightIRDS);
	delay(5);
	dVals.RightReading = analogRead(pinRightIRDS);

	//TODO: Currently mapped to either a 1 or 0, with better IR sensors a distance can be returned and 
	//higher resolution returned.
	
	return dVals;


}


void adjustRudderPosition(int newRudderPosition)
{

	if (newRudderPosition != rudderPosition)
	{

		//only adjust the rudder if the position has changed
		rudder.write(newRudderPosition);


		int delayDuration = 15; //15ms per degree

		if (newRudderPosition > rudderPosition)
		{
			delayDuration = (newRudderPosition - rudderPosition) * 15;
		}
		else
		{
			delayDuration = (rudderPosition - newRudderPosition) * 15;
		}

		delay(delayDuration);

	};
	

}


void SetCourse(DistanceSensorValues sesnsorValues)
{

	if (sesnsorValues.LeftReading == 0 && sesnsorValues.RightReading == 0)
	{
		//Both sides clear - full steam ahead
		adjustRudderPosition(90);
	}
	else if (sesnsorValues.RightReading == 1 && sesnsorValues.LeftReading == 0)
	{
		//there is something on our right, change rudder to avoid collision
		adjustRudderPosition(135);
	}
	else if (sesnsorValues.RightReading == 0 && sesnsorValues.LeftReading == 1)
	{
		//something of the left, move out the way
		adjustRudderPosition(45);
	}
	else
	{
		//something in dead in front, time to reverse (not yet supported)
		//TODO: reverse
	}

}


void loop()
{

	//take a reading of the sensors
	DistanceSensorValues dVals = ReadCollisionSensorValues();

	//set the course appropriately
	SetCourse(dVals);

	}