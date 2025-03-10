/* 
* File: LSWatchdog.h
* Firmware: LipSync
* Developed by: MakersMakingChange
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
  If not, see <http://www.gnu.org/licenses/>
*/

// Header definition
#ifndef _LSWATCHDOG_H
#define _LSWATCHDOG_H

#include <nrf_wdt.h>
#include <hal/nrf_power.h>  //  Defines reset reasons for NRF52 (https://github.com/particle-iot/nrf5_sdk/blob/master/modules/nrfx/hal/nrf_power.h)

const uint32_t WATCHDOG_RESET_MASK = 0x2; //POWER_RESETREAS_DOG_Msk; //  0x2;
extern bool g_watchdogReset;
extern uint32_t g_lastRebootReason;

//*********************************//
// Watchdog Functions
//*********************************//

//***CHECK RESET REASON FUNCTION***//
// Function   : initWatchdog
//
// Description: This function checks why the microcontroller was reset / powered.
//
// Parameters : void
//
// Return     : void
//****************************************//
void checkResetReason() {
  // Check the internal register for why the device was reset 
   
  // Determine last reset reason
    g_lastRebootReason = readResetReason(); // from Adafruit_nRF52_Arduino/cores/NRF/wiring.c

  	
  // Check why the device was reset
  if(USB_DEBUG) { Serial.print("Reset reason: "); }
  if ((g_lastRebootReason & NRF_POWER_RESETREAS_RESETPIN_MASK) == NRF_POWER_RESETREAS_RESETPIN_MASK) { 
    if(USB_DEBUG) { Serial.println("Reset Pin Reset"); }
  } else if ((g_lastRebootReason & NRF_POWER_RESETREAS_DOG_MASK) == NRF_POWER_RESETREAS_DOG_MASK) {
    if(USB_DEBUG) { Serial.println("Watchdog Reset"); }
    g_watchdogReset = true;
  } else if ((g_lastRebootReason & NRF_POWER_RESETREAS_SREQ_MASK) == NRF_POWER_RESETREAS_SREQ_MASK) {
    if(USB_DEBUG) { Serial.println("Software Reset"); }
  } else {
    if(USB_DEBUG) { Serial.println("Other reset"); }
  }

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
  if (USB_DEBUG) { Serial.println("USBDEBUG: initWatchdog()"); }
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
  //if (USB_DEBUG) { Serial.println("USBDEBUG: watchDogLoop()"); }

  NRF_WDT->RR[0] = WDT_RR_RR_Reload;  // Feed (reset) the watchdog timer
}


#endif
