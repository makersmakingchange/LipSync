/* 
* File: LipSyncX_TestSoftwareReset.ino
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
* Summary: This code does a software reset when the Next button is pushed.
*/

#define CONF_BUTTON_NEXT_PIN 10  // S1 MO Tactile next button
#define CONF_BUTTON_SEL_PIN 3    // S2 A3 Tactile select button

#define CONF_LED_MOUSE_PIN 9    // MI
#define CONF_LED_GAMEPAD_PIN 7  // RX

#define CONF_BUZZER_PIN 6     // Buzzer 

#define CONF_LED_MICRO_RED LED_RED        // Xiao NRF52840 User LED Red
#define CONF_LED_MICRO_GREEN LED_GREEN    // Xiao NRF52840 User LED Green
#define CONF_LED_MICRO_BLUE LED_BLUE      // Xiao NRF52840 User LED Blue


void setup() {
  Serial.begin(115200);

  while (!Serial) { delay(1); }  // Wait until serial port is opened

  Serial.println("Serial connection established");

  pinMode(CONF_BUTTON_NEXT_PIN, INPUT_PULLUP);
  pinMode(CONF_BUTTON_SEL_PIN, INPUT_PULLUP);

  pinMode(CONF_LED_MOUSE_PIN, OUTPUT);
  pinMode(CONF_LED_GAMEPAD_PIN, OUTPUT);

    // Turn microcontroller LED green
  digitalWrite(CONF_LED_MICRO_RED,HIGH);
  digitalWrite(CONF_LED_MICRO_GREEN,LOW);
  digitalWrite(CONF_LED_MICRO_BLUE,HIGH);

}  // end setup

void loop() {
  bool inputStateBN = !digitalRead(CONF_BUTTON_NEXT_PIN);
  bool inputStateBS = !digitalRead(CONF_BUTTON_SEL_PIN);


  if (inputStateBS) {
    Serial.println("Initiating software reset");
  softwareReset();
  }
 



}  //end loop

//***INITIATE SOFTWARE RESET***//
// Function   : softwareReset
//
// Description: This function initiates a software reset.
//
// Parameters :  none
//
// Return     : none
//******************************************//
void softwareReset() {
      // Turn microcontroller LED red
  digitalWrite(CONF_LED_MICRO_RED,LOW);
  digitalWrite(CONF_LED_MICRO_GREEN,HIGH);
  digitalWrite(CONF_LED_MICRO_BLUE,HIGH);

  NVIC_SystemReset();
  delay(10);
}
