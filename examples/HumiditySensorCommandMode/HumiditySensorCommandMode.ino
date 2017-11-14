/********************************************************************** 
  Utilize the "command mode" to modify the internal settings of a Honeywell HIH8000
  Series sensor connected to an Arduino board using I2C protocol (not SPI):
  https://sensing.honeywell.com/sensors/humidity-sensors/HIH8000-series
  
  To acquire humidity and temperature data, use the HIH8000 library instead:
  https://github.com/kiatAWDSA/HIH8000_I2C

  
  *************************************************************************************
  *                               INSTRUCTIONS                                        *
  *************************************************************************************
  0. In the first few lines of the code below, replace the argument in the following line:
                HIH8000Command_I2C hihSensor = HIH8000Command_I2C(0x27);
     with the address of your sensor in hexadecimal form. The default address of all 
     HIH8000 sensors is 0x27.
  1. Connect your sensor according to the recommended layout in pg10 of the datasheet
     linked above. Make sure the SDA and SCL pins of the sensor are connected to the
     appropriate pins on your Arduino board (see above link to the Wire documentation).
     DO NOT connect the Vdd pin of the sensor the the Arduino 3.3 V pin yet!!
  2. Connect >220 Ohm resistor between pin 7 and the 3.3V line so it forms a parallel
     connection with the HIH8000 sensor. Do not connect this 3.3 V line to the Arduino
     3.3 V pin yet!!
  3. Connect the Arduino to your computer and upload the program. Open the serial monitor.
  4. With the serial monitor open, connect the 3.3 V pin of your Arduino to the connection
     shared between the sensor Vdd pin and the 220 Ohm resistor to pin 7. The moment you
     connect it, the serial monitor should come up with a message "Entered command mode".
     If you get the message "Sensor connected, but failed to enter command mode", disconnect
     the Arduino from computer and check your connections again.
  5. Send any of the following commands via the serial monitor:
        g         : Get the current I2C address of the sensor.
        aXXX      : Change the I2C address of the sensor to XXX (Must be <= 127).
        uXXX.XX   : Change the upper limit of the zone that triggers the high alarm
                    (Alarm_High_On) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        iXXX.XX   : Change the lower limit of the zone that triggers the high alarm
                    (Alarm_High_Off) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        kXXX.XX   : Change the lower limit of the zone that triggers the low alarm
                    (Alarm_Low_On) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        lXXX.XX   : Change the upper limit of the zone that triggers the low alarm
                    (Alarm_Low_Off) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
  6. Whenever you made a change to the sensor settings and want to see the new changes, you need
     to disconnect the sensor from the power, and reconnect it.

  If you get a message saying something went wrong, either you gave a value that doesn't
  satisfy the constraints stated above, the sensor did not enter the command mode, something
  is wrong with the sensor, or there is a bug with the library!
  *************************************************************************************
  
  
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

#include <Wire.h>
#include "HIH8000Command_I2C.h"

HIH8000Command_I2C hihSensor = HIH8000Command_I2C(0x27);

bool inCommandMode = false;
bool failedCommandMode = false;
bool newCommand = false;
byte readData = 0;
byte startupDetectPin = 7; // Pin that is parallel to the 3.3 V line of the sensor. Detects when the sensor receives power
char serialCommand[2];
char serialData[7];

void setup() {
  // put your setup code here, to run once:
  pinMode(7, INPUT);
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);
  Serial.setTimeout(50);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!inCommandMode && !failedCommandMode)
  {
    if (digitalRead(startupDetectPin)) {
      inCommandMode = hihSensor.begin();

      if (inCommandMode) {
        Serial.println("Entered command mode");
      } else {
        failedCommandMode = true;
        Serial.println("Failed to enter command mode.");
        Serial.println("Either the given sensor address is wrong or the time window for entering command window has passed.");
      }
    }
  }

  
  if (newCommand)
  {    
    switch (serialCommand[0])
    {
      case 'a':
        if (hihSensor.changeAddress((uint8_t)atoi(serialData)))
        {
          Serial.println("Sensor I2C address changed to " + String(serialData));
          Serial.println("Remember to restart (power off then on) the sensor for the changes to take place.");
        }
        else
        {
          Serial.println("Something went wrong..");
        }
        break;

      case 'g':
        uint16_t storedAddress;
        storedAddress = hihSensor.readAddress();
        if (storedAddress <= 127) {
          Serial.println("Address in register (binary form): " + String(storedAddress, BIN));
          Serial.println("Address in register (decimal form): " + String(storedAddress, DEC));
        } else {
          Serial.println("Something went wrong..");
        }
        break;

      case 'u':
        if (hihSensor.changeAlarmHighOn(atof(serialData)))
        {
          Serial.println("The upper limit of the zone that triggers the high alarm (Alarm_High_On) has been changed to " + String(serialData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }
        break;
        
      case 'i':
        if (hihSensor.changeAlarmHighOff(atof(serialData)))
        {
          Serial.println("The lower limit of the zone that triggers the high alarm (Alarm_High_Off) has been changed to " + String(serialData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }
        break;

      case 'k':
        if (hihSensor.changeAlarmLowOn(atof(serialData)))
        {
          Serial.println("The lower limit of the zone that triggers the low alarm (Alarm_Low_On) has been changed to " + String(serialData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }
        break;

      case 'l':
        if (hihSensor.changeAlarmLowOff(atof(serialData)))
        {
          Serial.println("The upper limit of the zone that triggers the low alarm (Alarm_Low_Off) has been changed to " + String(serialData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }
        break;

      case 'c':
        if (inCommandMode) {
          Serial.println("In command mode");
        } else {
          Serial.println("Not in command mode");
        }
        break;
        
      default:
        Serial.println("Invalid command");
        break;
    }
    
    serialCommand[0] = 'q'; // Idle state
    newCommand = false;
  }
}

void serialEvent() {
  // Check if the incoming byte is actually a command, or just extra numbers from a previous command
  if (!isDigit(Serial.peek()) && Serial.peek() != '\n') {
    serialCommand[0] = Serial.read();
    memset(serialData, '\0', sizeof(serialData)); // Empty the array
    Serial.readBytesUntil('\n', serialData, 6);
    newCommand = true;
  } else {
    // Grab the buffer, but don't store it anywhere
    Serial.read();
  }
}
