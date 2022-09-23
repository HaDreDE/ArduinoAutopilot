This project was developed as a master thesis work at HafenCityUniversity Hamburg by Henning Drewer

It contains the various components of an autopilot. A line of sight algorithm is used as the guidance algorithm. 
To tune the control, the look ahead distance can be adjusted, and the differential steering factor in the Control.h file can be changed.
The navigation is done by reading the GNSS data sets GGA and VTG, which are provided by a Trimble R8 antenna via a RS232 interface.

In addition to calculating the control values, the user himself can also take control of the control system by means of an RC remote control.