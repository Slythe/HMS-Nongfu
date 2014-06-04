#include <Arduino.h>
#include "Servo.h"


Servo rudder; //Servo acting as rudder controller
Servo navigationMotor; //Servo that moves the ultrasound emitter/receiver


int rudderPosition; //current rudder angle
int navigationMotorBearing; //where is the ultrasound pointing

bool clockwise; //which way is nav motor spinning

// values to indicate how far right and left the servo should sweep and by how big a step
const int intMaxRightDegrees = 135;
const int intMaxLeftDegrees = 45;
const int intResolution = 3;


//HC-SR04 variables
int echoPin = 11; // Echo Pin
int trigPin = 12; // Trigger Pin

int maximumRange = 400; // Maximum range needed
int minimumRange = 2; // Minimum range needed

String debugLine;


void setup()
{

  //Communication for debugging
  Serial.begin(9600);

  //For Sonar device
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //attach the servo
  rudder.attach(3);
  navigationMotor.attach(10);

  //align the rudder straight for launch
  rudder.write(90);
  navigationMotor.write(90);

  //record positions
  rudderPosition = 90;
  navigationMotorBearing = 90;
  
  //start by looking to the right
  clockwise = true;

  //wait one second for the servos to set
  delay(1000);

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


void moveNavigationMotor(){


	if (clockwise){

		if (navigationMotorBearing < intMaxRightDegrees){
			navigationMotorBearing = navigationMotorBearing + intResolution;
		}

		if (navigationMotorBearing >= intMaxRightDegrees){
			clockwise = false;
		}
		
	}
	else{

		if (navigationMotorBearing > intMaxLeftDegrees){
			navigationMotorBearing = navigationMotorBearing - intResolution;
		}

		if (navigationMotorBearing <= intMaxLeftDegrees){
			clockwise = true;
		}
	}
	
	//adjust bearing by 1 degree in appropriate direction
	navigationMotor.write(navigationMotorBearing);

	//wait for it to get there
	delayMicroseconds(100);

}


long takeDistanceReading(){


	//Serial.println("Taking Distance Reading");

	long culumativeDistance = 0;
	long distance;
	long duration;


	for (int i = 0; i <= 2; i++){

		/* The following trigPin/echoPin cycle is used to determine the
		distance of the nearest object by bouncing soundwaves off of it. */
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);

		digitalWrite(trigPin, HIGH);
		delayMicroseconds(10);

		digitalWrite(trigPin, LOW);
		duration = pulseIn(echoPin, HIGH);

		//Calculate the distance (in cm) based on the speed of sound.
		distance = duration / 58.2;

		//Set some values if they are out of range
		if (distance >= maximumRange) {
			distance = 400;
		}
		else if (distance <= minimumRange) {
			distance = 2;
		}

		culumativeDistance = culumativeDistance + distance;

		//wait before taking the next reading
		delay(50);
		
	}

	
	return culumativeDistance / 3;

		
}


void loop()
{


	moveNavigationMotor();

	long obstacles = takeDistanceReading();
	
	debugLine = "";
	debugLine += "Bearing: ";
	debugLine += navigationMotorBearing;
	debugLine += " Distance: ";
	debugLine += obstacles;

	Serial.println(debugLine);

	delay(250);



	//take a reading of the sensors
	//DistanceSensorValues dVals = ReadCollisionSensorValues();

	//set the course appropriately
	//SetCourse(dVals);


	//rudder.write(180);
	//navigationMotor.write(180);

	//delay(3000);

	//rudder.write(0);
	//navigationMotor.write(0);

	//delay(3000);


	}