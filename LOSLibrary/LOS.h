/*
  LOSControl.h - Class for performing LOS navigation.
  Created by Henning Drewer, Mai 13, 2022.
  Released into the public domain.
*/
#ifndef LOSControl_h
#define LOSControl_h

#include "Arduino.h"
#include "CoordinateProvider.h"
#include "DataLogger.h"

class LOSControl
{
  public:
    LOSControl(float lookahead);
	float update(float newLat, float newLon, float newH, CoordinateProvider* provider);
	float update(float newLat, float newLon, float newH, CoordinateProvider* provider,  DataLogger logger);
	void setStart(float startX, float startY, float startH, CoordinateProvider* provider);
	
	
  private:
	float look_ahead;
	float currentX;
	float currentY;
	float nextX;
	float nextY;
};

#endif