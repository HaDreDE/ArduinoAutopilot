#include "Arduino.h"
#include "LOS.h"
#include "CoordinateProvider.h"
#include "DataLogger.h"

LOSControl::LOSControl(float lookahead)
{
	look_ahead = lookahead;
}

void LOSControl::setStart(float startLat, float startLon, float startH, CoordinateProvider *provider) {
	//set startposition as current waypoint 
	provider -> convert(startLat, startLon, startH, &currentX, &currentY);
	
	//read first waypoint from SD card and set it as next waypoint
	provider -> readFirst(&nextX, &nextY);
}

float LOSControl::update(float newLat, float newLon, float newH, CoordinateProvider *provider,  DataLogger logger){
	//Acceptance radius = 2 times ship length (cf Moreira)
	float Rk = 2 * 1;
	
	
	
	float newX = 0;
	float newY = 0;
	
	provider->convert(newLat, newLon, newH, &newX, &newY);
	
	
	String message = "";
	message.concat(currentX);
	message.concat("\t");
	message.concat(currentY);
	message.concat("\t");
	message.concat(nextX);
	message.concat("\t");
	message.concat(nextY);
	message.concat("\t");
	message.concat(newX);
	message.concat("\t");
	message.concat(newY);
	message.concat("\t");
	
	
	
	
	float dx = newX - nextX;
	float dy = newY - nextY;
	float dist = sqrt(dx*dx+dy*dy);
	
	/*update waypoint if position is within circle of acceptance */
	if(dist <= Rk) {
		provider->readNext(&currentX, &currentY, &nextX, &nextY);
	}
	
	/*
	* calculate new heading
	*/
	// horizontal path tangential angle
	double gamma_p = atan2(nextY-currentY, nextX-currentX);
	//across track error
	float ye = -(newX - currentX)*sin(gamma_p)+(newY-currentY)*cos(gamma_p);
	
	message.concat(ye);
	message.concat("\n");
	
	//requested heading
	float hd1 = (gamma_p + atan(-ye/look_ahead)) * 180 / PI;
	
	
	if(hd1 >= 360) {hd1 -= 360.0;}
	if(hd1 < 0) {hd1 += 360.0;}
	
	
	logger.publish(message);
	
	return hd1;
}


float LOSControl::update(float newLat, float newLon, float newH, CoordinateProvider* provider)
{
	/*Update current waypoint*/
	//Acceptance radius = 2 times ship length (cf Moreira)
	float Rk = 2 * 1;
	
	float newX = 0;
	float newY = 0;
	
	provider->convert(newLat, newLon, newH, &newX, &newY);
	
	
	float dx = newX - currentX;
	float dy = newY - currentY;
	float dist = sqrt(dx*dx+dy*dy);
	
	
	if(dist <= Rk) {
		provider->readNext(&currentX, &currentY, &nextX, &nextY);
	}
	
	
	/*
	* calculate new heading
	*/
	// horizontal path tangential angle
	double gamma_p = atan2(nextY-currentY, nextX-currentX);
	//across track error
	float ye = -(newX - currentX)*sin(gamma_p)+(newY-currentY)*cos(gamma_p);
	
	//requested heading
	float hd = (gamma_p + atan(-ye/look_ahead)) * 180 / PI;

	
	if(hd >= 360) {hd -= 360.0;}
	if(hd < 0) {hd += 360.0;}
	
	return hd;
}


