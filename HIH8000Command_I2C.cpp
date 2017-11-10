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

#include "HIH8000Command_I2C.h"

// Instantiate with device address.
HIH8000Command_I2C::HIH8000Command_I2C(uint8_t address) {
  setAddress(address);
}

// If user prefers to instantiate class first and set the address later.
HIH8000Command_I2C::HIH8000Command_I2C() {
}

// If in command mode, exit it as precaution.
HIH8000Command_I2C::~HIH8000Command_I2C() {
  if (inCommandMode_) {
    end();
  }
}

// Allow user to change address of the sensor that the class will communicate with only if not in command mode.
// Address MUST be 7-bits (max value is 0x7F i.e. 127)
void HIH8000Command_I2C::setAddress(uint8_t newAddress) {
  if (!inCommandMode_) {
    if (newAddress <= 0x7F) {
      address_ = newAddress;
      addressSet_ = true;
    }
  }
}

// Begin the command mode. Note that this has to be done within 3 or 10 ms upon sensor power-up, depending on the
// startup time setting of the sensor.
// Returns true if in command mode; false if not.
bool HIH8000Command_I2C::begin() {
  sensorBusy_ = true;
  uint8_t statusByte;
  
  Wire.beginTransmission(address_);
  Wire.write(COMMAND_START_);
  Wire.write(DATA_DUMMY_);
  Wire.write(DATA_DUMMY_);
  Wire.endTransmission();
  
  Wire.requestFrom(address_, RESPONSEBYTECOUNT_WRITE_);
  statusByte = Wire.read();
  
  // Check if sensor is in Command mode
  if (statusByte & 0x80) {
    inCommandMode_ = true;
    return true;
  } else {
    return false;
  }
}

// Ends the command mode. Note that the sensor should be restarted (disconnect and reconnect power) if
// a new address was given to the sensor, otherwise it will continue using the old address.
void HIH8000Command_I2C::end() {
  Wire.beginTransmission(address_);
  Wire.write(COMMAND_EXIT_);
  Wire.write(DATA_DUMMY_);
  Wire.write(DATA_DUMMY_);
  Wire.endTransmission();
  
  inCommandMode_ = false;
}

// Get the upper limit of the zone that triggers the high alarm. Returns -1 if failed to get a reading.
float HIH8000Command_I2C::readAlarmHighOn() {
  uint16_t dataBytes;
  
  if (readRegister(COMMAND_READ_ALARM_HIGH_ON_, dataBytes)) {
    return bitsToHumidity(dataBytes);
  } else {
    return -1;
  }
}

// Get the lower limit of the zone that triggers the high alarm. Returns -1 if failed to get a reading.
float HIH8000Command_I2C::readAlarmHighOff() {
  uint16_t dataBytes;
  
  if (readRegister(COMMAND_READ_ALARM_HIGH_OFF_, dataBytes)) {
    return bitsToHumidity(dataBytes);
  } else {
    return -1;
  }
}

// Get the lower limit of the zone that triggers the low alarm. Returns -1 if failed to get a reading.
float HIH8000Command_I2C::readAlarmLowOn() {
  uint16_t dataBytes;
  
  if (readRegister(COMMAND_READ_ALARM_LOW_ON_, dataBytes)) {
    return bitsToHumidity(dataBytes);
  } else {
    return -1;
  }
}

// Get the upper limit of the zone that triggers the low alarm. Returns -1 if failed to get a reading.
float HIH8000Command_I2C::readAlarmLowOff() {
  uint16_t dataBytes;
  
  if (readRegister(COMMAND_READ_ALARM_LOW_OFF_, dataBytes)) {
    return bitsToHumidity(dataBytes);
  } else {
    return -1;
  }
}

// Get the sensor address stored in the customer config register (might not be the actual address if the sensor hasn't been restarted)
uint16_t HIH8000Command_I2C::readAddress() {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    //return (dataBytes & 0x7F);
	return dataBytes;
  } else {
    return 100; // TODO: document this?
  }
}

// Change the upper limit of the zone that triggers the high alarm.
bool HIH8000Command_I2C::changeAlarmHighOn(float humidity) {
  uint16_t dataBytes = humidityToBits(humidity); 
  return writeRegister(COMMAND_WRITE_ALARM_HIGH_ON_, dataBytes, dataBytes, true);
}

// Change the lower limit of the zone that triggers the high alarm.
bool HIH8000Command_I2C::changeAlarmHighOff(float humidity) {
  uint16_t dataBytes = humidityToBits(humidity); 
  return writeRegister(COMMAND_WRITE_ALARM_HIGH_OFF_, dataBytes, dataBytes, true);
}

// Change the lower limit of the zone that triggers the low alarm.
bool HIH8000Command_I2C::changeAlarmLowOn(float humidity) {
  uint16_t dataBytes = humidityToBits(humidity); 
  return writeRegister(COMMAND_WRITE_ALARM_LOW_ON_, dataBytes, dataBytes, true);
}

// Change the upper limit of the zone that triggers the low alarm.
bool HIH8000Command_I2C::changeAlarmLowOff(float humidity) {
  uint16_t dataBytes = humidityToBits(humidity); 
  return writeRegister(COMMAND_WRITE_ALARM_LOW_OFF_, dataBytes, dataBytes, true);
}

// Change the internal I2C address of the sensor itself. The device must be restarted to apply the change.
// Address MUST be 7-bits (max value is 0x7F i.e. 127)
bool HIH8000Command_I2C::changeAddress(uint8_t newAddress) {
  if (newAddress <= 0x7F) {
    uint16_t dataBytes;
    
    if (readCustConfig(dataBytes)) {
      dataBytes = dataBytes & DATA_ADDRESS_CLEAR_;
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, newAddress, true);
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// Change the alarm-low polarity. True for HIGH when alarm is active, false for LOW when alarm is active.
bool HIH8000Command_I2C::changeAlarmLowPolarity(bool lowPolarity) {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    if (lowPolarity) {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_LOW_POLARITY_ACTIVELOW_, true);
    } else {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_LOW_POLARITY_ACTIVEHIGH_, false);
    }
  } else {
    return false;
  }
}

// Change the alarm-low output type. True for open drain, false for full push-pull.
bool HIH8000Command_I2C::changeAlarmLowOutput(bool openDrain) {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    if (openDrain) {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_LOW_OUTPUT_OPENDRAIN_, true);
    } else {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_LOW_OUTPUT_PUSHPULL_, false);
    }
  } else {
    return false;
  }
}


// Change the alarm-high polarity. True for HIGH when alarm is active, false for LOW when alarm is active.
bool HIH8000Command_I2C::changeAlarmHighPolarity(bool lowPolarity) {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    if (lowPolarity) {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_HIGH_POLARITY_ACTIVELOW_, true);
    } else {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_HIGH_POLARITY_ACTIVEHIGH_, false);
    }
  } else {
    return false;
  }
}

// Change the alarm-high output type. True for open drain, false for full push-pull.
bool HIH8000Command_I2C::changeAlarmHighOutput(bool openDrain) {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    if (openDrain) {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_HIGH_OUTPUT_OPENDRAIN_, true);
    } else {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_ALARM_HIGH_OUTPUT_PUSHPULL_, false);
    }
  } else {
    return false;
  }
}

// Change the time window after startup for receiving the command to enter command mode.
// True for 3 ms, false for 10 ms.
bool HIH8000Command_I2C::changeStartupTimeWindow(bool setTo3ms) {
  uint16_t dataBytes;
  
  if (readCustConfig(dataBytes)) {
    if (setTo3ms) {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_STARTUP_3MS_, true);
    } else {
      return writeRegister(COMMAND_WRITE_CUSTCONFIG_, dataBytes, DATA_STARTUP_10MS_, false);
    }
  } else {
    return false;
  }
}

// This register contains sensor I2C address, alarm polarity and output, and time window on startup for entering the command mode
bool HIH8000Command_I2C::readCustConfig(uint16_t& dataBytes) {
  if (readRegister(COMMAND_READ_CUSTCONFIG_, dataBytes)) {
    return true;
  } else {
    return false;
  }
}

// Internal function for reading from sensor registers and checking if it was successful.
bool HIH8000Command_I2C::readRegister(uint8_t command, uint16_t& dataBytes) {
  if (inCommandMode_) {
    sensorBusy_ = true;
    uint8_t statusByte;
    
    Wire.beginTransmission(address_);
    Wire.write(command);
    Wire.write(DATA_DUMMY_); // Implicitly converted to one byte
    Wire.write(DATA_DUMMY_); // Implicitly converted to one byte
    Wire.endTransmission();
    
    while (sensorBusy_) {
      Wire.requestFrom(address_, RESPONSEBYTECOUNT_READ_);
      statusByte = Wire.read();
      
      // Check if sensor is no longer busy
      if (statusByte & 0x03) {
        sensorBusy_ = false;
        // Read the data bytes
        dataBytes = Wire.read();
        dataBytes <<= 8;
        uint8_t readBuffer = Wire.read();
        dataBytes = dataBytes | readBuffer;
      }
    }
    
    // Extract response bit for "positive acknowledge"
    if (statusByte & 0x01) {
      return true;
    } else {
      return false;
    }
  }
}

// Internal function for writing to sensor registers and checking if it was successful.
bool HIH8000Command_I2C::writeRegister(uint8_t command, uint16_t oldDataBytes, uint16_t newDataBytes, bool bitwiseOr) {
  if (inCommandMode_) {
    sensorBusy_ = true;
    uint8_t statusByte;
    uint16_t combinedDataByte;
    
    if (bitwiseOr) {
      combinedDataByte = oldDataBytes | newDataBytes;
    } else {
      combinedDataByte = oldDataBytes & newDataBytes;
    }
    
    uint8_t dataByte1 = combinedDataByte >> 8;
    uint8_t dataByte2 = combinedDataByte & 0xFF;
    
    Wire.beginTransmission(address_);
    Wire.write(command);
    Wire.write(dataByte1);
    Wire.write(dataByte2);
    Wire.endTransmission();
    
    while (sensorBusy_) {
      Wire.requestFrom(address_, RESPONSEBYTECOUNT_WRITE_);
      statusByte = Wire.read();
      
      // Check if sensor is still busy
      if (statusByte & 0x03) {
        sensorBusy_ = false;
      }
    }
    
    // Extract response bit for "positive acknowledge"
    if (statusByte & 0x01) {
      return true;
    } else {
      return false;
    }
  }
}

// See pg6 of datasheet: https://sensing.honeywell.com/hih8000-datasheet-009075-7-en.pdf
float HIH8000Command_I2C::bitsToHumidity(uint16_t rawBits) {
  return rawBits/16382.0 * 100;
}

// See pg6 of datasheet: https://sensing.honeywell.com/hih8000-datasheet-009075-7-en.pdf
uint16_t HIH8000Command_I2C::humidityToBits(float humidity) {
  return (uint16_t) humidity/100 * 16382;
}