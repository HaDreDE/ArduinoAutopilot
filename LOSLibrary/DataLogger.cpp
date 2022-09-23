#include "Arduino.h"
#include "DataLogger.h"
#include "SD.h"

DataLogger::DataLogger(byte pin) {
	filename = "LOGFILE.txt";
	SD.begin(pin);
	int suffix = 1;
	
	while(SD.exists(filename)) {
		filename = "LOGFILE (";
		filename.concat(suffix);
		filename.concat(").txt");
		suffix += 1;
	}
}


void DataLogger::publish(String message) {
	File myFile = SD.open(filename, FILE_WRITE);
	if(myFile) {
		myFile.print(message);
		myFile.close();
	}
	
}

void DataLogger::printTime(String message) {
	File myFile = SD.open("time.txt", FILE_WRITE);
	if(myFile) {
		myFile.print(message);
		myFile.close();
	}
}

