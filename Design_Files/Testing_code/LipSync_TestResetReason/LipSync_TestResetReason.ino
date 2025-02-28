/* 
* File: LipSync_TestResetReason.h
* Developed by: MakersMakingChange
* Version: 2025-Feb-25
  License: GPL v3.0 or later

  Copyright (C) 2025 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/

/* This test is intended to test the soft reset and watchdog reset and determine the reason for reset when the device is restarted.
* INSTRUCTIONS:
 1. Flash the firmware to the NRF52840 board
 2. Open a serial terminal
 3. Enter 0 in serial terminal to trigger soft reset, 1 to trigger delay to trigger watchdog

*/

#include "Adafruit_TinyUSB.h"  //  Required for Serial
#include <nrf_power.h> // Defines the POWER_RESETREAS masks
#include <nrf_wdt.h>  //  Defines the watchdog constants


#define PRINT_MASK_BITS 0 // Set to 1 to serial print mask bits

//volatile uint32_t *const RESETREAS = (uint32_t *)0x40000400;  //  Pointer to reset reason register on NR52
//const uint32_t RESETPIN_MASK = 0x1;
const uint32_t WATCHDOG_RESET_MASK = 0x2;                     //POWER_RESETREAS_DOG_Msk; //  0x2; // The bit in the RESETREAS register that corresponds to a watchdog reset.
uint32_t watchdogSet = 0xFFFFFFFF & WATCHDOG_RESET_MASK;
const uint32_t RESET_REASON_CLEAR = 0xFFFFFFFF;

#define CONF_WATCHDOG_TIMEOUT_SEC 10 // How long until the watchdog is triggered in seconds


uint32_t g_lastRebootReasonSoftDevice = 7;
uint32_t g_lastRebootReason = 32;

void setup() {
  
  // Get the reset reason from the power register
  // Somewhere mentioned that the NRF_POWER->RESETREAS method  should only be used when softDevice is NOT enabled, otherwise it will cause a reset
  //g_lastRebootReasonSoftDevice = NRF_POWER->RESETREAS;  // Access reset reason register directly. 

  // Alternative method for getting RESETREASON when a soft device is enabled
  // bool preSerialSoftDeviceEnabled = false;

  // #ifdef SOFTDEVICE_PRESENT
  //   preSerialSoftDeviceEnabled = true;
  // #endif

  uint32_t getErrorCode = 0;
  //getErrorCode = sd_power_reset_reason_get(&g_lastRebootReasonSoftDevice);  // Get the reset reason // 2025-Feb-25 -> This throws an error as Soft Device not enabled

  //#if NRE_POWER_HAS_RESETREAS
  g_lastRebootReason = nrf_power_resetreas_get(NRF_POWER);
  //#else
  //g_lastRebootReason = nrf_reset_resetreas_get(NRF_RESET);
  //#endif
  
  //g_lastRebootReason = NRF_POWER->RESETREAS;  // Access reset reason register directly. // (May cause a reset) // Returns 0000000000000000


  Serial.begin(115200);

  while(!Serial) {
    delay(1);
  }
  Serial.println("==============RESTARTED=============");


 
  // Serial print the different masks for different bits corresponds to different reset reasons
  if(PRINT_MASK_BITS) {
    Serial.println("=====BIT MASKS=====");
    
    Serial.print("NRF_POWER_RESETREAS_RESETPIN_MASK: \t");
    printBits(NRF_POWER_RESETREAS_RESETPIN_MASK);

    Serial.print("NRF_POWER_RESETREAS_DOG_MASK: \t \t");
    printBits(NRF_POWER_RESETREAS_DOG_MASK);

    Serial.print("NRF_POWER_RESETREAS_SREQ_MASK: \t \t");
    printBits(NRF_POWER_RESETREAS_SREQ_MASK);

    Serial.print("NRF_POWER_RESETREAS_LOCKUP_MASK: \t");
    printBits(NRF_POWER_RESETREAS_LOCKUP_MASK);

    Serial.print("NRF_POWER_RESETREAS_OFF_MASK: \t \t");
    printBits(NRF_POWER_RESETREAS_OFF_MASK);

    Serial.print("RESET_REASON_CLEAR: \t \t \t");
    printBits(RESET_REASON_CLEAR);  // Serial print the mask used to clear the RESETREAS
    
    Serial.print("watchdogSet: \t \t \t \t");  // Serial print the mask used to try to manually set the watchdog bit in the RESETREAS register
    printBits(watchdogSet);

    Serial.println("==================");
  }


  //Serial.print("preSerialSoftDeviceEnabled: ");
  //Serial.println(preSerialSoftDeviceEnabled);


  // Output error code 
  Serial.print("getErrorCode: ");
  Serial.println(getErrorCode);

  //Serial.print("g_lastRebootReasonSoftDevice: \t \t");
  //printBits(g_lastRebootReasonSoftDevice);

  
  Serial.print("g_lastRebootReason: \t \t \t");
  printBits(g_lastRebootReason);

 
  // Determine if the watchdog reset bit is on, and if so, print a message to serial.
  Serial.print("Reset reason: ");
  if ((g_lastRebootReason & NRF_POWER_RESETREAS_RESETPIN_MASK) == NRF_POWER_RESETREAS_RESETPIN_MASK) { 
    Serial.println("Reset Pin Reset");
  } else if ((g_lastRebootReason & NRF_POWER_RESETREAS_DOG_MASK) == NRF_POWER_RESETREAS_DOG_MASK) {
    Serial.println("Watchdog Reset");
  } else if ((g_lastRebootReason & NRF_POWER_RESETREAS_SREQ_MASK) == NRF_POWER_RESETREAS_SREQ_MASK) {
    Serial.println("Software Reset");
  } else {
    Serial.println("Other reset");
  }

  // Clear the RESETREAS register
  //NRF_POWER->RESETREAS = RESET_REASON_CLEAR;  //  Directly write to register
  //int clearErrorCode = 0;
  //clearErrorCode = sd_power_reset_reason_clr(g_lastRebootReasonSoftDevice); // Alternative method to reset reason when using softDevice

  //Serial.print("clearErrorCode: ");
  //Serial.println(clearErrorCode);

  //sd_power_reset_reason_get(&g_lastRebootReasonSoftDevice);
  nrf_power_resetreas_clear(NRF_POWER, g_lastRebootReason);
  Serial.print("Clear1: \t \t \t \t");
  //printBits(NRF_POWER->RESETREAS); // Directly access to register
  printBits(g_lastRebootReason);  // Output 
 
  
  //Serial.print("Clear2: \t \t");
  //printBits(NRF_POWER->RESETREAS);

  // Initiatialize the watchdog timer. This starts a countdown clock that will reset the device unless 
  // watchdogLoop() is called to reset the timer.
  initWatchdog();  

  Serial.println("SERIAL INPUT OPTIONS: 0 - Soft reset; 1 - Delay to trigger watchdog, 2 - manually set watchdog reset bit");
}

void loop() {

  if (Serial.available() > 0 ) {
    char recieved_char = Serial.read();  // Read the character from the serial connection
    switch(recieved_char){
      case '0':  // Perform a soft reset
        Serial.println("Soft Reset Triggered");
        yield();
        delay(1000);
        NVIC_SystemReset();
        break;
      case '1': // Stop the code long enough to trigger the watchdog
        Serial.println("Delay Triggered");
        delay(CONF_WATCHDOG_TIMEOUT_SEC*1000+5000); 
        break;
      case '2': // Attempt to manually write the watchdog bit in the RESETREAS register
        Serial.println("Attempt to manually set watchdog bit");
        NRF_POWER->RESETREAS = watchdogSet;
        Serial.print("New resetreas \t \t");
        printBits(NRF_POWER->RESETREAS);
        break;

    }
  }
  delay(100);

  watchdogLoop();  // Pet / feed the watchdog to restart the timer.

}


//***PRINT BITS FUNCTION***//
// Function   : printBits
//
// Description: This function initializes the hardware watchdog
//
// Parameters : value : uint32_t : Serial print the input value bit by bit
//
// Return     : void
//****************************************//
void printBits(uint32_t value) {
  for (int i = 31; i >= 0; i--) {    // Loop from MSB (bit 31) to LSB (bit 0)
    Serial.print((value >> i) & 1);  // Shift right and mask with 1
  }
  Serial.println();  // Move to a new line after printing all bits
}

//***INITIALIZE WATCHDOG FUNCTION***//
// Function   : initWatchdog
//
// Description: This function initializes the hardware watchdog
//
// Parameters : void
//
// Return     : void
//****************************************//
void initWatchdog() {
  // Configure watchdog timer with a defined timeout
  NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) |  // Keep running during halt
                    (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   // Keep running in sleep mode
  NRF_WDT->CRV = CONF_WATCHDOG_TIMEOUT_SEC * 32768;                   // Convert timeout in seconds to watchdog cycles (1 cycle = 1/32.768kHz)
  NRF_WDT->RREN |= WDT_RREN_RR0_Msk;                                  // Enable reload register 0
  NRF_WDT->TASKS_START = 1;                                           // Start watchdog
}

//***WATCHDOG LOOP FUNCTION***//
// Function   : watchdogLoop
//
// Description: This function resets the watchdog timer to prevent reset
//
// Parameters : void
//
// Return     : void
//****************************************//
void watchdogLoop(void) {
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;  // Feed (reset) the watchdog timer
}