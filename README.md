# GamePadCoProcessor
The Energia code here enables a second TI LaunchPad to function as a WS28xx LED CoProcessor.  Since the USB HID and the WS2811s are very timing dependent, they do not coexist well, so a second LaunchPad is currently required to operate them.

By default, the LED strip will attach to Pin3, and an extra switch can be added to Pin2 to enable default control when not connected to a robot.
It requires usage of the driver here: https://github.com/ILAMtitan/WS2811Driver
