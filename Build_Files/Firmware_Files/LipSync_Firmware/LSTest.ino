/* 
* File: LSTestS.ino
* Firmware: LipSync
* Developed by: Neil Squire Society / Makers Making Change Program
* Version: v4.0.rc1 (26 January 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>.
*/


// To add tests:
// 1. Create the CONF_TEST_MODE_{name} in LSConfig.h
// 2. Increment the CONF_TEST_MODE_MAX number
// 3. Add the switch case to the activateTest function
// 4. Implement the test below.

//***ACTIVATE TEST FUNCTION***//
// Function   : activateTest
//
// Description: This function activates different tests
//
// Parameters : inputTest : int : The id of the test to activate
//
// Return     : void
//****************************************//
void activateTest(int inputTest) {

  switch (inputTest) {
    case (CONF_TEST_MODE_LED):
      testLED();
      break;
  }
  Serial.println("Test Complete");
}




//***ACTIVATE TEST FUNCTION***//
// Function   : activateTest
//
// Description: This function activates different tests
//
// Parameters : inputTest : int : The id of the test to activate
//
// Return     : void
//****************************************//
void testLED() {

  //setLedColor(int ledNumber, int ledColorNumber, int ledBrightness)

  Serial.println("LED TEST ACTIVATED");
  Serial.println("TEST_MODE_LED: All LED OFF");
  led.clearLedAll();
  delay(1000);

  Serial.println("TEST_MODE_LED: Left LED ON");
  led.clearLedAll();
  led.setLedColor(CONF_LED_LEFT, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: Middle LED ON");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MIDDLE, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  delay(1000);


  Serial.println("TEST_MODE_LED: Right LED ON");
  led.clearLedAll();
  led.setLedColor(CONF_LED_RIGHT, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: ALL LED OFF");
  led.clearLedAll();


  Serial.println("TEST_MODE_LED: Micro LED Blue");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MICRO, LED_CLR_BLUE, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: Micro LED Purple");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MICRO, LED_CLR_PURPLE, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: Micro LED Red");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MICRO, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: Micro LED Orange");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MICRO, LED_CLR_ORANGE, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: Micro LED Yellow");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MICRO, LED_CLR_YELLOW, CONF_LED_BRIGHTNESS);
  delay(1000);

  Serial.println("TEST_MODE_LED: ALL LED OFF");
  led.clearLedAll();
  setLedDefault(); // set LEDs to default
}