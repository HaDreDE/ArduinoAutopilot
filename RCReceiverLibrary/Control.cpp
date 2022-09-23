#include "Arduino.h"
#include "Control.h"
#include "Adafruit_TiCoServo.h"



Control::Control() {
	dir = NEUTRAL;
	actualDiffDrive = 0;
}

boolean Control::init(byte outPins[]) {

	for(int i = 0; i < numOfChannels; i++) {
		servos[i].attach(outPins[i]);
	}
	
}

void Control::writePWMValue(int index, unsigned long value) {
	servos[index].write(value);
}

void Control::writeSteeringValue(int value, double diff) {
	int cvalue = constrain(value, steeringMin, steeringMax);
	servos[steeringIndex_left].write(cvalue);
	servos[steeringIndex_right].write(cvalue);
	
	/*
  	*  Support left direction
	*/
	if(value <= steeringMin) {
		dir = LEFT;
		/*
		* Support right direction
		*/
	} else if (value >= steeringMax) {
		dir = RIGHT;
	} else {
		dir = NEUTRAL;
	}
	actualDiffDrive = diff * diffFactor;
}

void Control::writeThrottleValue(int value) {
	int cvalue = constrain(value, throttleMin, throttleMax);
	
	if(dir == NEUTRAL) {
		servos[throttleIndex_left].write(value);
		servos[throttleIndex_right].write(value + diffOffset);
	} else if(dir == LEFT) {
		if(value + diffOffset + actualDiffDrive <= throttleMax) {
			servos[throttleIndex_left].write(value);
			servos[throttleIndex_right].write(value + diffOffset + actualDiffDrive);
		} else {
			int rValue = throttleMax;
			int lValue = value - ((value + diffOffset + actualDiffDrive) - throttleMax);
			lValue = constrain(lValue, throttleMin, throttleMax);
			servos[throttleIndex_left].write(lValue);
			servos[throttleIndex_right].write(rValue);
		}
	} else if(dir == RIGHT) {
		if(value +  actualDiffDrive <= throttleMax) {
			servos[throttleIndex_left].write(value + actualDiffDrive);
			servos[throttleIndex_right].write(value + diffOffset);
		} else {
			int lValue = throttleMax;
			int rValue = value - ((value  + actualDiffDrive) - throttleMax) + diffOffset;
			rValue = constrain(lValue, throttleMin, throttleMax);
			servos[throttleIndex_left].write(lValue);
			servos[throttleIndex_right].write(rValue);
		}
	}
	
}



void Control::arm() {
	servos[throttleIndex_left].write(armPulse);
	servos[throttleIndex_right].write(armPulse);
}





