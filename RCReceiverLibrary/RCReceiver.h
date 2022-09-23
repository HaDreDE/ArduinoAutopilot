/*
  RCReceiver.h - Class for reading RC receive PWM values.
  Created by Henning Drewer, Mai 13, 2022.
  Released into the public domain.
*/
#ifndef RCReceiver_h
#define RCReceiver_h

#include "Arduino.h"
#include "Control.h"
#include "DataLogger.h"
#define numOfChannels 6
#define THRESHOLD 1500


class RCReceiver
{
  public:
    RCReceiver(byte pins[]);
	void writeRCValues(byte pins[], Control *control);
	void writeRCValues(byte pins[], Control *control, DataLogger logger);
	boolean isAutoMode(byte pin);
	
  private:
	
};

#endif