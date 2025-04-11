/* 
* File: LipSync_TestI2C.ino
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: 2025-Jan-13
* Copyright Neil Squire Society 2025. 
* License: GPL v3

  Copyright (C) 2025 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*
* Summary: This code checks to see that the I2C sensors for the LipSync are connected and communicating.
*/


#define USB_DEBUG true

#include <Wire.h>

//I2C Devices
#define I2CADDR_DISPLAY 0x3D   // Display address (61)
#define I2CADDR_LPS22 0x5C     // Modified LPS22 address (92)
#define I2CADDR_LPS35HW  0x5D  // LPS35HW address (93)
#define I2CADDR_TLV493D 0x5E   // 3D Hall Effect Sensor (94)


//***CHECK I2C FUNCTION***//
// Function   : checkI2C
//
// Description: This function checks to ensure the necessary i2C devices are connected.
//
// Parameters : void
//
// Return     : void
//****************************************//
void checkI2C()
{
  if (USB_DEBUG) { Serial.println("USBDEBUG: Checking I2C Devices"); }
  
  Wire.begin();

  Wire.beginTransmission(I2CADDR_DISPLAY);
    byte error_display;
    error_display = Wire.endTransmission();

    if (error_display == 0)
    {
      Serial.println("Display: Found");
    }
    else
    {
      Serial.println("Display: Not found");
    }

  // Scan for Ambient Pressure Sensor
    Wire.beginTransmission(I2CADDR_LPS22);
    byte error_LPS22;
    error_LPS22 = Wire.endTransmission();

    if (error_LPS22 == 0)
    {
      Serial.println("Ambient Pressure Sensor: Found");
    }
    else
    {
      Serial.println("Ambient Pressure Sensor: Not found");
    }

  // Scan for Sip and Puff Sensor
    Wire.beginTransmission(I2CADDR_LPS35HW);
    byte error_LPS35HW;
    error_LPS35HW = Wire.endTransmission();

    if (error_LPS35HW == 0)
    {
      Serial.println("Sip and Puff Sensor: Found");
    }
    else
    {
      Serial.println("Sip and Puff Sensor: Not found");
    }

  // Scan for Joystick Sensor
    Wire.beginTransmission(I2CADDR_TLV493D);
    byte error_TLV493D;
    error_TLV493D = Wire.endTransmission();

    if (error_TLV493D == 0)
    {
      Serial.println("Joystick Sensor: Found");
    }
    else
    {
      Serial.println("Joystick Sensor: Not found");
    }
  Wire.end();

}


void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(1); }  // Wait until serial port is opened

  Serial.println("Serial connection established");

  checkI2C();


}  // end setup

void loop() {

  delay(5000);
  checkI2C();

 



}  //end loop
