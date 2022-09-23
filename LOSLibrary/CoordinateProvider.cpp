#include "Arduino.h"
#include "CoordinateProvider.h"
#include "SD.h"
#include "DataLogger.h"


CoordinateProvider::CoordinateProvider(byte pin) {
	init = 0;
	endOfFile = 0;
	stop = 0;
	pos = 0;
	SD.begin(pin);
}

void CoordinateProvider::convert(float lat, float lon, float h, float* x, float* y, boolean useInitH = false) {
	
	double clat = cos(DEGREES_TO_RADIAN * lat);
	double slat = sin(DEGREES_TO_RADIAN * lat);
	double clon = cos(DEGREES_TO_RADIAN * lon);
	double slon = sin(DEGREES_TO_RADIAN * lon);
	double Rea = 6378137.0;
	double e2 = 0.0066943801;
	
	if(useInitH) {
		h = startH;
	}
	
	double Ne = Rea / sqrt(1.0 - e2 * slat * slat);

    
	//ECEF coordinates
	float ex = (Ne+h)*clat*clon;
	float ey = (Ne+h)*clat*slon;
	float ez = (Ne*(1.0-e2)+h)*slat;
	
	if(!init) {
		ecefX = ex;
		ecefY = ey;
		ecefZ = ez;
	}
	
	*x = -sin(startLat)*cos(startLon)*(ex-ecefX) + -sin(startLat)*sin(startLon)*(ey-ecefY) + cos(startLat)*(ez-ecefZ);
	*y = -sin(startLon)*(ex-ecefX) + cos(startLon)*(ey-ecefY);
	
}


void CoordinateProvider::readFirst(float *x, float *y) {
	readCoordinate(x, y);
}

void CoordinateProvider::readNext(float *lastX, float *lastY, float *nextX, float *nextY) {
	//former next coordinate becomes current waypoint
	*lastX = *nextX;
	*lastY = *nextY;
	
	
	if(endOfFile && stop) {
		stop = 2;
	}
	else if(endOfFile && !stop) {
		*nextX = startX;
		*nextY = startY;
		stop = 1;
	}	
	else {
		readCoordinate(nextX, nextY);
	}
	
}


void CoordinateProvider::readNext(float *lastX, float *lastY, float *nextX, float *nextY, DataLogger logger) {
	//former next coordinate becomes current waypoint
	*lastX = *nextX;
	*lastY = *nextY;
	
	String msg = "";
	msg.concat("Last waypoint at: ");
	msg.concat(*lastX);
	msg.concat("\t");
	msg.concat(*lastY);
	msg.concat("\t Position: ");
	msg.concat(pos);
	msg.concat("\n");
	
	if(endOfFile && stop) {
		stop = 2;
	}
	else if(endOfFile && !stop) {
		*nextX = startX;
		*nextY = startY;
		stop = 1;
	}	
	else {
		readCoordinate(nextX, nextY);
	}
	msg.concat("New waypoint at: ");
	msg.concat(*nextX);
	msg.concat("\t");
	msg.concat(*nextY);
	msg.concat("\t Position: ");
	msg.concat(pos);
	msg.concat("\n");
	
	logger.publish(msg);
}

void CoordinateProvider::readCoordinate(float* x, float* y) {
	File file = SD.open("waypoint.txt");
	
		
	file.seek(pos);
	
	String readString = "";
	String latString;
	String lonString;
	
	
	
	while(file.available()) {
		char c = file.read();
		
		//skip markup characters
		if(c < 33) {
			continue;
		}
		
		if(c == 59) {
			int sep = readString.indexOf(',');
			latString = readString.substring(0, sep);
			lonString = readString.substring(sep+1);
			
			if(readString.length() > 0) {break;}
			else {continue;}
		} else {
			readString += c;
		}
	}
	
	if(!file.available()) {
		endOfFile = 1;
	}
	
	pos = file.position();
	file.close();
	
	//define next waypoint
	float longitude = lonString.toFloat();
	float latitude = latString.toFloat();
	
	convert(latitude, longitude, 0, x, y, true);
	
}



void CoordinateProvider::setStart(float initLat, float initLon, float initH) {
	startLat = initLat * DEGREES_TO_RADIAN;
	startLon = initLon * DEGREES_TO_RADIAN;
	startH = initH;
	convert(initLat, initLon, startH, &startX, &startY);
	
	init = 1;
}
