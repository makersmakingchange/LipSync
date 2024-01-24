/* 
* File: LSTestS.ino
* Firmware: LipSync
* Developed by: Neil Squire Society / Makers Making Change Program
* Version: 4.0pr (23 January 2024) 
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

  switch(inputTest){
    case(CONF_TEST_MODE_LED):
      testLED();
      break;

  }
  Serial.println("Test Complete");

}





void testLED() {

  //setLedColor(int ledNumber, int ledColorNumber, int ledBrightness) {
  
  Serial.println("LED TEST ACTIVATED");
  Serial.println("TEST LED: All LED OFF");
  led.clearLedAll();
  delay(1000);
  
  Serial.println("TEST LED: Left LED");
  led.clearLedAll();
  led.setLedColor(CONF_LED_LEFT, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  //performLedAction(ledStateStruct);
  delay(1000);

  Serial.println("TEST LED: Middle LED");
  led.clearLedAll();
  led.setLedColor(CONF_LED_MIDDLE, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  //performLedAction(ledStateStruct);
  delay(1000);


  Serial.println("TEST LED: Right LED");
  led.setLedColor(CONF_LED_MIDDLE, LED_CLR_RED, CONF_LED_BRIGHTNESS);
  led.clearLedAll();
  //performLedAction(ledStateStruct);
  delay(1000);

  Serial.println("TEST LED: ALL LED OFF"));
  led.clearLedAll();
  //setLedDefault();
  
}