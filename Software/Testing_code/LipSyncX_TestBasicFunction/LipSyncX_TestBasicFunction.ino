/* 
* File: LipSyncX_DataCapture.ino
* Firmware: LipSync X
* Developed by: MakersMakingChange
* Version: 2023-Nov-20
* Copyright Neil Squire Society 2023. 
* License: GPL v3

  Copyright (C) 2023 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*
* Summary: This code writes current state to serial port.
*/

#include <Tlv493d.h>  //Infinion TLV493 magnetic sensor
#include <Adafruit_LPS35HW.h>  // Sip and puff pressure sensor
#include <Adafruit_LPS2X.h> // Ambient pressure sensor


#define CONF_SWITCH1_PIN 0  // 3.5mm jack SW1
#define CONF_SWITCH2_PIN 1  // 3.5mm jack SW2
#define CONF_SWITCH3_PIN 2  // 3.5mm jack SW3

#define CONF_BUTTON_NEXT_PIN 10  // S1 MO Tactile next button
#define CONF_BUTTON_SEL_PIN 3    // S2 A3 Tactile select button

#define CONF_LED_MOUSE_PIN 9    // MI
#define CONF_LED_GAMEPAD_PIN 7  // RX

#define CONF_BUZZER_PIN 6     // Buzzer 



Tlv493d Tlv493dSensor = Tlv493d();  // TLV493D Magenetic sensor object;
Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW(); // Sip and puff pressure sensor object
Adafruit_LPS22 lps22; // Ambient pressure sensor
#define LPS22_I2CADDR 0x5C // Modified LPS22 address

float magnetic_x, magnetic_y, magnetic_z;

void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(1); }  // Wait until serial port is opened

  Serial.println("Serial connection established");

  // Initiate Magnet Sensor
Tlv493dSensor.begin();
 Tlv493dSensor.setAccessMode(Tlv493dSensor.MASTERCONTROLLEDMODE); 
  // if (!Tlv493dSensor.begin()){
  //   Serial.println(F("Couldn't find TLV493D chip"));
  // }
  // else {
  //   Tlv493dSensor.setAccessMode(Tlv493dSensor.MASTERCONTROLLEDMODE);
  // }


  
  if (!lps35hw.begin_I2C()) {
    Serial.println(F("Couldn't find LPS35HW chip"));
  }


    // Try to initialize!
  if (!lps22.begin_I2C(LPS22_I2CADDR)) {
    Serial.println("Failed to find LPS22 chip");
    while (1) { delay(10); } // Hang on serial
  }
  Serial.println("LPS22 Found!");

  lps22.setDataRate(LPS22_RATE_25_HZ);
    switch (lps22.getDataRate()) {
    case LPS22_RATE_ONE_SHOT: Serial.println("One Shot / Power Down"); break;
    case LPS22_RATE_1_HZ: Serial.println("1 Hz"); break;
    case LPS22_RATE_10_HZ: Serial.println("10 Hz"); break;
    case LPS22_RATE_25_HZ: Serial.println("25 Hz"); break;
    case LPS22_RATE_50_HZ: Serial.println("50 Hz"); break;
    case LPS22_RATE_75_HZ: Serial.println("75 Hz"); break;
  }





  pinMode(CONF_SWITCH1_PIN, INPUT_PULLUP);
  pinMode(CONF_SWITCH2_PIN, INPUT_PULLUP);
  pinMode(CONF_SWITCH3_PIN, INPUT_PULLUP);

  pinMode(CONF_BUTTON_NEXT_PIN, INPUT_PULLUP);
  pinMode(CONF_BUTTON_SEL_PIN, INPUT_PULLUP);

  pinMode(CONF_LED_MOUSE_PIN, OUTPUT);
  pinMode(CONF_LED_GAMEPAD_PIN, OUTPUT);

  digitalWrite(CONF_LED_MOUSE_PIN, HIGH);
  delay(1000);
  digitalWrite(CONF_LED_MOUSE_PIN, LOW);
  delay(1000);
  digitalWrite(CONF_LED_GAMEPAD_PIN, HIGH);
  delay(1000);
  digitalWrite(CONF_LED_GAMEPAD_PIN, LOW);



}  // end setup

void loop() {

  bool inputStateSW1 = !digitalRead(CONF_SWITCH1_PIN);
  bool inputStateSW2 = !digitalRead(CONF_SWITCH2_PIN);
  bool inputStateSW3 = !digitalRead(CONF_SWITCH3_PIN);

  bool inputStateBN = !digitalRead(CONF_BUTTON_NEXT_PIN);
  bool inputStateBS = !digitalRead(CONF_BUTTON_SEL_PIN);

  int Tlv493dStatus = Tlv493dSensor.updateData();
  float magnetic_x = Tlv493dSensor.getX();
  float magnetic_y = Tlv493dSensor.getY();
  float magnetic_z = Tlv493dSensor.getZ();

  float tube_temperature = lps35hw.readTemperature(); // C
  float tube_pressure = lps35hw.readPressure(); // [hPa]

  sensors_event_t lps22_temp;
  sensors_event_t lps22_pressure;
  lps22.getEvent(&lps22_temp, &lps22_pressure);

  float ambient_temperature = lps22_temp.temperature;
  float ambient_pressure = lps22_pressure.pressure;



  Serial.print(Tlv493dStatus);
  Serial.print("\t");
  Serial.print(magnetic_x);
  Serial.print("\t");
  Serial.print(magnetic_y);
  Serial.print("\t");
  Serial.print(magnetic_z);
  Serial.print("\t");
  Serial.print(inputStateSW1);
  Serial.print("\t");
  Serial.print(inputStateSW2);
  Serial.print("\t");
  Serial.print(inputStateSW3);
  Serial.print("\t");
  Serial.print(inputStateBN);
  Serial.print("\t");
  Serial.print(inputStateBS);
  Serial.print("\t");
  Serial.print(tube_temperature);
  Serial.print("\t");
  Serial.print(tube_pressure);
  Serial.print("\t");
  Serial.print(ambient_temperature);
  Serial.print("\t");
  Serial.println(ambient_pressure);
  
  delay(50);



}  //end loop
