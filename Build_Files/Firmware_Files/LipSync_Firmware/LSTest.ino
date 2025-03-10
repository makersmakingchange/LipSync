/* 
* File: LSTestS.ino
* Firmware: LipSync
* Developed by: Neil Squire Society / Makers Making Change Program
* Version: v4.1rc (10 March 2025)
  License: GPL v3.0 or later

  Copyright (C) 2024 - 2025 Neil Squire Society
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

    case (CONF_TEST_MODE_BUZZER):
      testBuzzer();
      break;
    
    case (CONF_TEST_MODE_WATCHDOG):
      testWatchdog();
      break;

  } // end switch inputTest
  Serial.println("Test Complete");
}


//***TEST LED FUNCTION***//
// Function   : testLED
//
// Description: This function tests the LED patterns
//
// Parameters : void
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

  Serial.println("TEST_MODE_LED: Left LED Brightness");
  led.clearLedAll();

  int brightnessDirection = 1;

  for (int brightness = 0; brightness > -1; brightness = brightness + brightnessDirection) {
    led.setLedColor(CONF_LED_LEFT, LED_CLR_RED, brightness);
    if (brightness == 255) {
      brightnessDirection = -1;  // Switch direction at peak
    }
    delay(10);
  }
  delay(1000);


  Serial.println("TEST_MODE_LED: Error 3");
  led.clearLedAll();
  
  int blinks = 3;
  ledError(blinks);

  // imitate operation loop
  while (ledStateTimer.isEnabled(ledTimerId[CONF_TIMER_LED_ERROR]) == true) { 
    ledStateTimer.run();  // Timer for lights
    delay(10);
  }

  delay(1000);
  Serial.println("TEST_MODE_LED: LED DEFAULT");
  setLedDefault();  // set LEDs to default
 
  Serial.println("TEST_MODE_LED: TEST COMPLETE");
}

//***TEST BUZZER FUNCTION***//
// Function   : testBuzzer
//
// Description: This function tests the buzzer sounds
//
// Parameters : void
//
// Return     : void
//****************************************//
void testBuzzer() {

  Serial.println("BUZZER TEST ACTIVATED");

  Serial.println("TEST_MODE_BUZZER: Playing startup sound.");
  buzzer.playStartupSound();
  delay(1000);

  Serial.println("TEST_MODE_BUZZER: Playing ready sounds.");
  buzzer.playReadySound();
  delay(1000);

  Serial.println("TEST_MODE_BUZZER: Playing error sound.");
  buzzer.playErrorSound();
  delay(1000);

  Serial.println("TEST_MODE_BUZZER: Playing corner calibration sound.");
  buzzer.calibCornerTone();
  delay(1000);

  Serial.println("TEST_MODE_BUZZER: Playing center calibration sound.");
  buzzer.calibCenterTone();
  delay(1000);

  Serial.println("TEST_MODE_BUZZER: Playing shutdown sound.");
  buzzer.playShutdownSound();
  delay(1000);

  // TODO Test Sound levels
  // TODO test enable/disable sound

  Serial.println("SOUND TEST COMPLETE");
}  // end testBuzzer()

//***TEST WATCHDOG FUNCTION***//
// Function   : testWatchdog
//
// Description: This function tests the watchdog
//
// Parameters : void
//
// Return     : void
//****************************************//
void testWatchdog() {

  Serial.println("WATCHDOG TEST ACTIVATED");

  Serial.println("TEST_MODE_WATCHDOG: Delaying longer than watchdog reset...");
  Serial.print("TEST_MODE_WATCHDOG: CONF_WATCHDOG_TIMEOUT_SEC:");
  Serial.println(CONF_WATCHDOG_TIMEOUT_SEC);

  unsigned long watchdogTestStart = millis();
  unsigned long currentTime = millis();

  unsigned long watchdogTestTimeout = CONF_WATCHDOG_TIMEOUT_SEC*1000+5000;

  while (currentTime - watchdogTestStart < watchdogTestTimeout) {
    delay(1000);
    Serial.println((currentTime-watchdogTestStart)/1000);
    currentTime = millis();
  }
  Serial.println("TEST_MODE_WATCHDOG:TEST FAILED"); // Should never reach this if watchdog is enabled
  
  Serial.println("WATCHDOG TEST COMPLETE");
}  // end testBuzzer()
