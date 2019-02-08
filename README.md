# SmartTime
Arduino Display with Bluetooth controls

Inspired by the LAMetric display, I have build my own LED display with bluetooth control using a 32x8 LED Matrix and a cheap arduino nano board with bluetooth (Bluething).
It also works with other arduino board + additional bluetooth module (e.g. HC05).

For manual operations, the clock can be controlled with a rotary wheel. You can choose between the following functions:
 -clock
 -timer/countdown
 -stopwatch
 -alarm 
 -display custom text (scolling)
 -options 

To keep track of the time/date I use a simple DS3231 RTC module . 
An active buzzer is used to do the alarm. 
I added a rotary encoder to toggle through the menu and select entries. 
With the HC-06 bluetooth module it is possible to connect with my phone and send commands (currently using a serial console). 

find more details and pictures on http://www.dietzm.de
