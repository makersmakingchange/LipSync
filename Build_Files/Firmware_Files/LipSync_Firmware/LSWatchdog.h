/* 
* File: LSWatchdog.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (21 February 2025)
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
volatile uint32_t *const RESETREAS = (uint32_t *)0x40000400;  //  Pointer to reset reason register on NR52
const uint32_t WATCHDOG_RESET_MASK = 0x2; //POWER_RESETREAS_DOG_Msk; //  0x2;
extern bool g_watchdogReset;


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
  //const uint32_t LAST_REBOOT_REASON = *RESETREAS;
  //uint32_t last_reboot_reason;
  //last_reboot_reason = NRF_POWER->RESETREAS;

  // Clear any enabled reset reasons
  //*RESETREAS |= *RESETREAS;


  uint32_t lastRebootReason;

	/* This doesn't run
  #ifndef SOFTDEVICE_PRESENT
		// Read reset reason
		lastRebootReason = NRF_POWER->RESETREAS; // this should be used only when Softdevice NOT enabled, otherwise the board will reset
		NRF_POWER->RESETREAS = 0xFFFFFFFF;
	#endif
  */
	#ifdef SOFTDEVICE_PRESENT
		sd_power_reset_reason_get(&lastRebootReason);// If reset caused by Watchdog the resetReason is 2, and if caused by power reset, the reason is 1
		sd_power_reset_reason_clr(0xFFFFFFFF);
	#endif

  // Check if last reset reason was the watchdog
  if ((lastRebootReason & WATCHDOG_RESET_MASK) == WATCHDOG_RESET_MASK) {
    g_watchdogReset = true;
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
