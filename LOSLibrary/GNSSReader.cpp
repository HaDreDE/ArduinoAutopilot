#include "Arduino.h"
#include "GNSSReader.h"
#include "DataLogger.h"





String readString;


GNSSReader::GNSSReader()
{
  
}

void GNSSReader::init() {
	Serial3.begin(38400);
	gga = 0;
	vtg = 0;
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String GNSSReader::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean GNSSReader::update(float* lat, float* lon, float* h, double* course, double* sog)
{
	gga = 0;
	vtg = 0;
	timer = millis();
	while(!(vtg && gga)) {
		
		if(millis() - timer > TIMEOUT) {
			Serial.println("Timeout");
			return 0;
		}
		while (Serial3 && Serial3.available()) {
		char c = Serial3.read();
	
		//36 is dollar sign in ASCII and indicates a new line
		if (c == 36) {
			//Serial.print(readString);
			if (readString.startsWith("$GNGGA"))
				{
					String latValue = getValue(readString, ',', 2);
					String lonValue = getValue(readString, ',', 4);
					
					if(latValue.length() > 0 && lonValue.length() > 0) {
						//update position
						*lat = getCoordinate(latValue);
						*lon = getCoordinate(lonValue);
						*h = getValue(readString,',',9).toFloat() + getValue(readString,',', 11).toFloat();
						//quality = getQuality(readString);
						gga=1;
					}				
					
				} 
			else if (readString.startsWith("$GNVTG"))
				{
					if(getCourse(readString, course) && getVelocity(readString, sog)) {
						vtg =  1; 
					}
					
				}
			readString = c;
		} else {
		readString += c;
		}
		}

  }
  
  return 1;
}

boolean GNSSReader::update(float* lat, float* lon, float* h, double* course, double* sog, DataLogger logger)
{
	gga = 0;
	vtg = 0;
	timer = millis();
	while(!(vtg && gga)) {
		
		if(millis() - timer > TIMEOUT) {
			logger.publish("Timeout\n");
			return 0;
		}
		while (Serial3 && Serial3.available()) {
		char c = Serial3.read();
	
		//36 is dollar sign in ASCII and indicates a new line
		if (c == 36) {
			//Serial.print(readString);
			if (readString.startsWith("$GNGGA"))
				{
					String latValue = getValue(readString, ',', 2);
					String lonValue = getValue(readString, ',', 4);
					
					if(latValue.length() > 0 && lonValue.length() > 0) {
						//update position
						*lat = getCoordinate(latValue);
						*lon = getCoordinate(lonValue);
						*h = getValue(readString,',',9).toFloat() + getValue(readString,',', 11).toFloat();
						//quality = getQuality(readString);
						gga=1;
					}				
					
				} 
			else if (readString.startsWith("$GNVTG"))
				{
					if(getCourse(readString, course) && getVelocity(readString, sog)) {
						vtg =  1; 
					}
					
				}
			readString = c;
		} else {
		readString += c;
		}
		}

  }
  
  return 1;
}


float GNSSReader::getCoordinate(String value) {
  
  int _degrees = value.toInt()/100;
  float minutes = value.toFloat()-_degrees*100;
  

  //Convert to DD.MMMMMMMMM
  float result = _degrees + minutes / 60.0;
	
  return result;
}



boolean GNSSReader::getCourse(String nmea, double* course) {
	String courseString = getValue(nmea, ',', 1);
	*course = courseString.toFloat();
	
	if(*course >= 360.0) {
		*course -= 360.0;
	}
	return courseString.length() > 0;
}

boolean GNSSReader::getVelocity(String nmea, double* sog) {
	String velocityString = getValue(nmea, ',', 7);
	*sog = velocityString.toFloat();
	
	return velocityString.length() > 0;
}



