# HIH8000Command_I2C
Utilize the "command mode" to modify the internal settings of a Honeywell HIH8000 Series sensor connected to an Arduino board using I2C protocol (not SPI):
https://sensing.honeywell.com/sensors/humidity-sensors/HIH8000-series

To acquire humidity and temperature data, use the HIH8000 library instead:
https://github.com/kiatAWDSA/HIH8000_I2C

NOTE: This library will only work for the variants of the Honeywell HIH8000 Series sensors that utilize the I2C protocol.

Useful documents (the third document is especially useful if you want to modify the library):
- Datasheet: https://sensing.honeywell.com/hih8000-datasheet-009075-7-en.pdf
- Installation instructions: https://sensing.honeywell.com/humidicon-sip-4-pin-versions-install-50085524-en-rev-b-final-30may13.pdf
- Using the Command Mode: https://sensing.honeywell.com/command-mode-humidicon-tn-009062-3-en-final-07jun12.pdf
- Understanding the alarms on the sensor: https://sensing.honeywell.com/alarms-on-humidicon-tn-009060-3-en-final-07jun12.pdf

# Instructions for use with Arduino boards
For an example, see the examples folder.

Setting up:
1. Connect the sensor to the appropriate Arduino pins using the recommended configuration (pg 10 of the datasheet)
2. Include Wire.h and HIH8000_I2C.h at the top of your Arduino code
3. Initialize a HIH8000Command_I2C object with the address of your sensor (default address is 0x27)
4. In the setup section of your Arduino code, call Wire.begin()
5. Immediately after that, call the begin() function for your HIH8000CommandI2C object

Reading/modifying sensor internal settings:
Below is a summary of the available functions.

begin()     : Starts command mode. Returns true if successful; false if not.
void end()  : Exits command mode.

These functions will return the limits for the low/high alarms in relative humidity (%) values. If something went wrong, they return -1.
- readAlarmHighOn()
- readAlarmHighOff()
- readAlarmLowOn()
- readAlarmLowOff()

These functions change various settings of the sensor. They return true if successful; false if not.
- changeAlarmHighOn(float humidityValue)    : humidityValue must be at least 0 and at most 100.
- changeAlarmHighOff(float humidityValue)   : humidityValue must be at least 0 and at most 100.
- changeAlarmLowOn(float humidityValue)     : humidityValue must be at least 0 and at most 100.
- changeAlarmLowOff(float humidityValue)    : humidityValue must be at least 0 and at most 100.
- changeAddress(uint8_t newAddress)         : address values must be at least 0 and at most 127.
- changeAlarmLowPolarity(bool lowPolarity)
- changeAlarmLowOutput(bool openDrain)
- changeAlarmHighPolarity(bool lowPolarity)
- changeAlarmHighOutput(bool openDrain)
- changeStartupTimeWindow(bool setTo3ms)