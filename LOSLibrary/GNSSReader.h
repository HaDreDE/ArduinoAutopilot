/*
  GNSSReader.h - Class for reading the values from the GNSS.
  Created by Henning Drewer, June 07, 2022.
  Released into the public domain.
*/
#ifndef GNSSReader_h
#define GNSSReader_h
#define TIMEOUT 5000

#include "Arduino.h"
#include "DataLogger.h"



class GNSSReader
{
  public:
    GNSSReader();
	boolean update(float* lat, float* lon, float* h, double* course, double* sog);
	boolean update(float* lat, float* lon, float* h, double* course, double* sog, DataLogger logger);
	float getCoordinate(String value);
	void init();
	
	
	private: 
	String getValue(String data, char separator, int index);
	boolean getCourse(String nmea, double* course);
	boolean getVelocity(String nmea, double *sog);
	boolean gga;
	boolean vtg;
	unsigned long timer;
	
};

#endif