/*
  CoordinateProvider.h - Class for reading the route coordinates from the SD card.
  Created by Henning Drewer, Mai 13, 2022.
  Released into the public domain.
*/
#ifndef CoordinateProvider_h
#define CoordinateProvider_h
#define DEGREES_TO_RADIAN PI/180
#define RADIAN_TO_DEGREES 180/PI

#include "Arduino.h"
#include "DataLogger.h"
#include "SD.h"



class CoordinateProvider
{
  public:
    CoordinateProvider(byte pin);
	void readFirst(float *x, float *y);
	void readNext(float *lastX, float *lastY, float *nextX, float *nextY, DataLogger logger);
	void readNext(float *lastX, float *lastY, float *nextX, float *nextY);
	void setStart(float initLat, float initLon, float initH);
	void convert(float lat, float lon, float h, float* x, float* y, boolean useInitH = false);
	int init;
	int endOfFile;
	int stop;
	float Rea;
	

  private:
	void readCoordinate(float* x, float* y);
	float ecefX;
	float ecefY;
	float ecefZ;
	float startX;
	float startY;
	float startLat;
	float startLon;
	float startH;
	long pos;
	
 
};

#endif