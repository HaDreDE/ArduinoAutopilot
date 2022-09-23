#include "Arduino.h"
#include "RCReceiver.h"
#include "Control.h"
#include "DataLogger.h"

RCReceiver::RCReceiver(byte pins[])
{
	for(int i = 0; i < numOfChannels; i++) {
		pinMode(pins[i], INPUT);
	}
}

void RCReceiver::writeRCValues(byte pins[], Control *control, DataLogger logger) {
	String msg = "";
	unsigned long pwmValues [numOfChannels];
	noInterrupts();
	for(int i = 0; i < numOfChannels; i++) {
		unsigned long value = pulseIn(pins[i], HIGH, 30000);
		msg.concat(value);
		msg.concat("\t");
		pwmValues[i] = value;
	}
	msg.concat("\n");
	logger.publish(msg);
	interrupts();
	//Serial.println(msg);
	
	for(int i = 0; i< numOfChannels; i++) {
		control->writePWMValue(i, pwmValues[i]);
	}
	
}


void RCReceiver::writeRCValues(byte pins[], Control *control) {
	String msg = "";
	unsigned long pwmValues [numOfChannels];
	noInterrupts();
	for(int i = 0; i < numOfChannels; i++) {
		unsigned long value = pulseIn(pins[i], HIGH, 30000);
		pwmValues[i] = value;
	}
	interrupts();
	
	
	for(int i = 0; i< numOfChannels; i++) {
		control->writePWMValue(i, pwmValues[i]);
	}
	
}


boolean RCReceiver::isAutoMode(byte pin) {
	unsigned long value = pulseIn(pin, HIGH);
	
	return value < THRESHOLD && value > 500;
}