/*
  DataLogger.h - Class for logging data to an SD card.
  Created by Henning Drewer, June 07, 2022.
  Released into the public domain.
*/
#ifndef DataLogger_h
#define DataLogger_h 

#include "Arduino.h"
#include "SD.h"

#define numOfDigits 6


class DataLogger
{
  public:
    DataLogger(byte pin);
	void publish(String message);
	void printTime(String message);

  private:
	String filename;
	
};

#endif