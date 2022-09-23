#include <Control.h>
#include <RCReceiver.h>
#include <LOS.h>
#include <GNSSReader.h>
#include <ArduPID.h>
#include <CoordinateProvider.h>
#include <DataLogger.h>



const byte SDpin = 53;
const byte inPins[] = {2, 3, 18, 19, 20, 21};
const byte outPins[] = {5, 6, 7, 8, 11, 12};
const byte modePin = 10;
int armed = 0;



// Classes used
GNSSReader gnss;
CoordinateProvider provider = CoordinateProvider(SDpin);
RCReceiver rc = RCReceiver(inPins);


Control control;

DataLogger logger = DataLogger(SDpin);
LOSControl los = LOSControl(4.0);
ArduPID steeringControl;
ArduPID throttleControl;

/*
   PID Values for  steering
   steeringInput -> actual measured course angle
   steeringOutput -> PWM pulse lenght for both steering servos, set to neutral as initial
   steeringSetpoint -> desired course angle given by LOS algorithm
*/
double steeringInput = 0;
double steeringOutput = RCNeutral;
double steeringSetpoint = 0;

/*
   PID Values for  throttle
   throttleInput -> actual measured speed over ground
   throttleOutput -> PWM pulse length for both esc, set to middle output as initial
   throttleSetpoint -> desired speed over ground hard coded
*/
double throttleInput = 0;
double throttleOutput = 1600;
double throttleSetpoint = 3.6;

//PID gains
double p_Steering_Small = 10.0;
double p_Steering_Medium = 10.0;
double p_Steering_Large = 20.0;
double i_Steering = 0.0;
double d_Steering = 0.0;

double ku_throttle = 320;
double oscTime = 1.0;
double p_Throttle_Large = ku_throttle / 3.0;
double i_Throttle = ku_throttle * 2/3 / oscTime;
double d_Throttle = ku_throttle * 0.111 * oscTime;
double bias = 90;

//Position
float lat, lon, h;


void setup() {
  Serial.begin(38400);

  while (!Serial) {}

  //start serial connection to GNSS Receiver
  gnss.init();
  while (!gnss.update(&lat, &lon, &h, &steeringInput, &throttleInput)) {}

  //initialize Guidance and Coordinate conversion
  provider.setStart(lat, lon, h);
  los.setStart(lat, lon, h, &provider);

  // init Actuator control
  control.init(outPins);

  // start PID controller for steering and thusting
  steeringControl.begin(&steeringInput, &steeringOutput, &steeringSetpoint, p_Steering_Medium, i_Steering, d_Steering);
  steeringControl.setOutputLimits(steeringMin, steeringMax);
  steeringControl.start();
  
 
  throttleControl.begin(&throttleInput, &throttleOutput, &throttleSetpoint, p_Throttle_Large, i_Throttle, d_Throttle);
  throttleControl.setOutputLimits(throttleMin, throttleMax);
  throttleControl.setBias(bias);
  throttleControl.start();
 
  //starttup only allowed in manual mode for safety purposes
  while (rc.isAutoMode(modePin)) {
    logger.publish("AutoMode on startup forbidden! \n");
    delay(1000);
  }

}

void loop() {

  /*
     Reasons for Manual mode:
     1. selected from user
     2. no GNSS signal
     3. vehicle successfully returned to start position
  */
  long time0 = micros();
  if (rc.isAutoMode(modePin) && provider.stop != 2 && gnss.update(&lat, &lon, &h, &steeringInput, &throttleInput, logger)) {

  long time1 = micros();
    /*
       Update steering setpoint with current position
       Eventually updaet waypoints
    */
    steeringSetpoint = los.update(lat, lon, h, &provider, logger);

  long time2= micros();
    /*
       Remove overflow error
       e.g. setpoint = 359° and actual course = 15° is only a difference of 16° and not 344°
    */
     int diff = steeringSetpoint - steeringInput;
     if(abs(diff) > 180) {
      steeringSetpoint -= ((diff > 0) - (diff < 0)) * 360;
    }
    

    if (abs(steeringInput - steeringSetpoint) > 5.0 ) {
      steeringControl.setCoefficients(p_Steering_Large, i_Steering, d_Steering);
    } else if(abs(steeringInput - steeringSetpoint) > 2.0 ) {
      steeringControl.setCoefficients(p_Steering_Medium, i_Steering, d_Steering);
    } else {
      steeringControl.setCoefficients(p_Steering_Small, i_Steering, d_Steering);
    }

 /*   if (abs(throttleInput - throttleSetpoint) > 0.5 ) {
      throttleControl.setCoefficients(p_Throttle_Large, i_Throttle, d_Throttle);
    } else if(abs(throttleInput - throttleSetpoint) > 0.2 ) {
      throttleControl.setCoefficients(p_Throttle_Medium, i_Throttle, d_Throttle);
    } else {
      throttleControl.setCoefficients(p_Throttle_Small, i_Throttle, d_Throttle);
    }
*/
    /*
     * Arming the ESC if this was not done yet
    */
    if (armed == 0) {
      //Set initial values for PWM
      control.arm();
      armed = 1;
    }
    //compute new values for PWM
    String msg = "";
    msg.concat("last: ");
    msg.concat(steeringOutput);
    msg.concat("\t");
    msg.concat("ist: ");
    msg.concat(steeringInput);
    msg.concat("\t");
    msg.concat("soll: ");
    msg.concat(steeringSetpoint);
    msg.concat("\t");

    steeringControl.compute();

    msg.concat("action: ");
    msg.concat(steeringOutput);
    msg.concat("\n");
    logger.publish(msg);

    msg = "";
    msg.concat("last: ");
    msg.concat(throttleOutput);
    msg.concat("\t");
    msg.concat("ist: ");
    msg.concat(throttleInput);
    msg.concat("\t");
    msg.concat("soll: ");
    msg.concat(throttleSetpoint);
    msg.concat("\t");

    throttleControl.compute();

    msg.concat("action: ");
    msg.concat(throttleOutput);
    msg.concat("\n");
    logger.publish(msg);




    /*
       VAlues are writen to the actuators
    */
    control.writeSteeringValue(steeringOutput, abs(steeringSetpoint - steeringInput));
    control.writeThrottleValue(throttleOutput);

  long time3 = micros();

  String timeMsg = "";
  timeMsg.concat(time1-time0);
  timeMsg.concat("\t");
  timeMsg.concat(time2-time1);
  timeMsg.concat("\t");
  timeMsg.concat(time3-time2);
  timeMsg.concat("\n");
  logger.printTime(timeMsg);
  
  } else {
    /*
       The values are read from RC and transmitted to actuators
    */
    rc.writeRCValues(inPins, &control);
  }

}
