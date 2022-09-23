/*
  Control.h - Class for writing PWM values to Servo and ESC.
  Created by Henning Drewer, Mai 13, 2022.
  Released into the public domain.
*/
#ifndef Control_h
#define Control_h

#include "Arduino.h"
#include "Adafruit_TiCoServo.h"

#define numOfChannels 6
#define throttleIndex_left 0
#define throttleIndex_right 1
#define steeringIndex_left 2
#define steeringIndex_right 3
#define diffOffset 7
#define armPulse 1380
#define throttleMin 1420
#define throttleMax 1672
#define RCNeutral 1553
#define steeringMin 1427
#define steeringMax 1679
#define diffFactor 2.5


enum direction { LEFT, NEUTRAL, RIGHT };


class Control
{
  public:
    Control();
	Adafruit_TiCoServo servos[numOfChannels];
	boolean init(byte pins[]);
	void writePWMValue(int index, unsigned long value);
	void writeSteeringValue(int value, double diff);
	void writeThrottleValue(int value);
	void arm();
	
  private:
	direction dir;
	double actualDiffDrive;
  
};

#endif