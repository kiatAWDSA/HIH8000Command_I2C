/********************************************************************** 
  Utilize the "command mode" to modify the internal settings of a Honeywell HIH8000
  Series sensor connected to an Arduino board using I2C protocol (not SPI):
  https://sensing.honeywell.com/sensors/humidity-sensors/HIH8000-series
  
  To acquire humidity and temperature data, use the HIH8000 library instead:
  https://github.com/kiatAWDSA/HIH8000_I2C
  
  
  Copyright 2017 Soon Kiat Lau
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
***********************************************************************/

#ifndef HIH8000Command_I2C_H
#define HIH8000Command_I2C_H

#include "Arduino.h"
#include "Wire.h"

class HIH8000Command_I2C  {
  public:
    // Functions affecting the object instance
    HIH8000Command_I2C(uint8_t address);
    HIH8000Command_I2C();
    ~HIH8000Command_I2C();
    void setAddress(uint8_t newAddress);
    
    // Functions affecting the command mode
    bool begin();
    void end();
    
    // Functions for reading some of the registers in the sensor (ignoring the non-mutable ones)
    float readAlarmHighOn();
    float readAlarmHighOff();
    float readAlarmLowOn();
    float readAlarmLowOff();
	uint16_t readAddress();
    // Possible TODO: add functions to read from other parts of the customer config register.
    
    // Functions for changing the registers in the sensor
    bool changeAlarmHighOn(float humidityValue);
    bool changeAlarmHighOff(float humidityValue);
    bool changeAlarmLowOn(float humidityValue);
    bool changeAlarmLowOff(float humidityValue);
    bool changeAddress(uint8_t newAddress);
    bool changeAlarmLowPolarity(bool lowPolarity);
    bool changeAlarmLowOutput(bool openDrain);
    bool changeAlarmHighPolarity(bool lowPolarity);
    bool changeAlarmHighOutput(bool openDrain);
    bool changeStartupTimeWindow(bool setTo3ms);
  
  private:
    // Command bytes affecting the command mode itself
    static const uint8_t COMMAND_START_                          = 0xA0;
    static const uint8_t COMMAND_EXIT_                           = 0x80;
  
    // Command bytes for reading
    static const uint8_t COMMAND_READ_ALARM_HIGH_ON_             = 0x18;
    static const uint8_t COMMAND_READ_ALARM_HIGH_OFF_            = 0x19;
    static const uint8_t COMMAND_READ_ALARM_LOW_ON_              = 0x1A;
    static const uint8_t COMMAND_READ_ALARM_LOW_OFF_             = 0x1B;
    static const uint8_t COMMAND_READ_CUSTCONFIG_                = 0x1C;
    
    // Command bytes for writing (Add 0x40 to the read command bytes)
    static const uint8_t COMMAND_WRITE_ALARM_HIGH_ON_            = 0x58;
    static const uint8_t COMMAND_WRITE_ALARM_HIGH_OFF_           = 0x59;
    static const uint8_t COMMAND_WRITE_ALARM_LOW_ON_             = 0x5A;
    static const uint8_t COMMAND_WRITE_ALARM_LOW_OFF_            = 0x5B;
    static const uint8_t COMMAND_WRITE_CUSTCONFIG_               = 0x5C;
    
    // Data bytes for completing the 4-byte package when sending single command bytes
    static const uint16_t DATA_DUMMY_                            = 0x0000; // 0000 0000 0000 0000
    
    // For removing the address information from the customer config register bytes
    static const uint16_t DATA_ADDRESS_CLEAR_                    = 0xFF80; // 1111 1111 1000 0000
    
    // Data bytes for alarm settings (2 bytes)
    static const uint16_t DATA_ALARM_LOW_POLARITY_ACTIVELOW_     = 0x0040; // 0000 0000 0100 0000
    static const uint16_t DATA_ALARM_LOW_POLARITY_ACTIVEHIGH_    = 0xFFBF; // 1111 1111 1011 1111
    static const uint16_t DATA_ALARM_LOW_OUTPUT_OPENDRAIN_       = 0x0080; // 0000 0000 1000 0000
    static const uint16_t DATA_ALARM_LOW_OUTPUT_PUSHPULL_        = 0xFF7F; // 1111 1111 0111 1111
    static const uint16_t DATA_ALARM_HIGH_POLARITY_ACTIVELOW_    = 0x0100; // 0000 0001 0000 0000
    static const uint16_t DATA_ALARM_HIGH_POLARITY_ACTIVEHIGH_   = 0xFEFF; // 1111 1110 1111 1111
    static const uint16_t DATA_ALARM_HIGH_OUTPUT_OPENDRAIN_      = 0x0200; // 0000 0010 0000 0000
    static const uint16_t DATA_ALARM_HIGH_OUTPUT_PUSHPULL_       = 0xFDFF; // 1111 1101 1111 1111
    
    // Data bytes for startup time
    static const uint16_t DATA_STARTUP_3MS_                      = 0x1000; // 0001 0000 0000 0000
    static const uint16_t DATA_STARTUP_10MS_                     = 0xEFFF; // 1110 1111 1111 1111
  
    // This are used when fetching data. The sensor sends 3 bytes if we're reading a register; 1 byte if we're writing to a register.
    // The first byte is always the status+diagnostic+response byte.
    static const uint8_t RESPONSEBYTECOUNT_READ_    = 3;
    static const uint8_t RESPONSEBYTECOUNT_WRITE_   = 1;
    
    // Internal variables
    bool addressSet_        = false;
    bool inCommandMode_     = false;
    bool sensorBusy_        = false;
    uint8_t address_;
    
    // Internal functions
    bool readCustConfig(uint16_t& dataBytes);
    bool readRegister(uint8_t command, uint16_t& dataBytes);
    bool writeRegister(uint8_t command, uint16_t oldDataBytes, uint16_t newDataBytes, bool bitwiseOr);
    float bitsToHumidity(uint16_t rawBits);
    uint16_t humidityToBits(float humidity);
};

#endif  // HIH8000Command_I2C_H