/********************************************************************** 
  Utilize the "command mode" to modify the internal settings of a Honeywell HIH8000
  Series sensor connected to an Arduino board using I2C protocol (not SPI):
  https://sensing.honeywell.com/sensors/humidity-sensors/HIH8000-series
  
  To acquire humidity and temperature data, use the HIH8000 library instead:
  https://github.com/kiatAWDSA/HIH8000_I2C

  
  *************************************************************************************
  *                               INSTRUCTIONS                                        *
  *************************************************************************************
  1. Connect your sensor according to the recommended layout in pg10 of the datasheet
     linked above. Make sure the SDA and SCL pins of the sensor are connected to the
     appropriate pins on your Arduino board (see above link to the Wire documentation).
  2. Open your serial monitor and send any of the following commands:
        aXXX      : Change the I2C address of the sensor to XXX (Must be <= 127).
        uXXX.XX   : Change the upper limit of the zone that triggers the high alarm
                    (Alarm_High_On) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        iXXX.XX   : Change the lower limit of the zone that triggers the high alarm
                    (Alarm_High_Off) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        kXXX.XX   : Change the lower limit of the zone that triggers the low alarm
                    (Alarm_Low_On) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.
        lXXX.XX   : Change the upper limit of the zone that triggers the low alarm
                    (Alarm_Low_Off) to XXX.XX. 0 <= XXX.XX <= 100. Max 2 decimal places.

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

bool newCommand = false;
byte readData = 0;
char serialCommand[2];
char serialData[7];

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);
  hihSensor.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (newCommand)
  {
    char trimmedData[readData];

    for (byte i = 0; i < readData; i++) {
      trimmedData[i] = serialData[i];
    }
    
    switch (serialCommand[0])
    {
      case 'a':
        if (hihSensor.changeAddress((uint8_t)atoi(trimmedData)))
        {
          Serial.println("Sensor I2C address changed to " + String(trimmedData));
          Serial.println("Remember to restart (power off then on) the sensor for the changes to take place.");
        }
        else
        {
          Serial.println("Something went wrong..");
        }

        serialCommand[0] = 'i';
        break;

      case 'u':
        if (hihSensor.changeAlarmHighOn(atof(trimmedData)))
        {
          Serial.println("The upper limit of the zone that triggers the high alarm (Alarm_High_On) has been changed to " + String(trimmedData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }

        serialCommand[0] = 'i';
        break;
        
      case 'i':
        if (hihSensor.changeAlarmHighOff(atof(trimmedData)))
        {
          Serial.println("The lower limit of the zone that triggers the high alarm (Alarm_High_Off) has been changed to " + String(trimmedData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }

        serialCommand[0] = 'i';
        break;

      case 'k':
        if (hihSensor.changeAlarmLowOn(atof(trimmedData)))
        {
          Serial.println("The lower limit of the zone that triggers the low alarm (Alarm_Low_On) has been changed to " + String(trimmedData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }

        serialCommand[0] = 'i';
        break;

      case 'l':
        if (hihSensor.changeAlarmLowOff(atof(trimmedData)))
        {
          Serial.println("The upper limit of the zone that triggers the low alarm (Alarm_Low_Off) has been changed to " + String(trimmedData));
        }
        else
        {
          Serial.println("Something went wrong..");
        }

        serialCommand[0] = 'i';
        break;
      
      default:
        Serial.println("Invalid command");

        serialCommand[0] = 'i';
        break;
    }
  }
}

void serialEvent() {
  serialCommand[0] = Serial.read();
  readData = 0;
  
  while (Serial.available() && readData < 6)
  {
    serialData[readData] = Serial.read();
    readData++;
  }
  
  newCommand = true;
}
