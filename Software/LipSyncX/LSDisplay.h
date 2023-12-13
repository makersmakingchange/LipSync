/* 
* File: LSDisplay.h
* Firmware: LipSync X
* Developed by: MakersMakingChange
* Version: Beta (01 December 2023)
  License: GPL v3.0 or later

  Copyright (C) 2023 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/

//Header definition
#ifndef _LSDISPLAY_H
#define _LSDISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


#define MAIN_MENU   0
#define EXIT_MENU   1
#define CALIB_MENU  2
#define MODE_MENU   3
#define CURSOR_SP_MENU   4
#define MORE_MENU    5

//Calibration pages
#define CENTER_RESET_PAGE 11
#define FULL_CALIB_PAGE   12

//More Menus
#define SOUND_MENU        51
#define SIP_PUFF_MENU     52

#define MODE_MOUSE_USB  0
#define MODE_MOUSE_BT   1
#define MODE_GAMEPAD    2

#define SCROLL_DELAY_MILLIS   100

const String mainMenuText[5] = {"Exit Menu", "Calibrate", "Mode", "Cursor speed", "More"};
const String exitConfirmText[4] = {"Exit", "settings?", "Confirm", "... Back"};
const String calibMenuText[3] = {"Center reset", "Full Calibration", "... Back"};
const String modeMenuText[4] = {"MOUSE USB", "MOUSE BT", "GAMEPAD ", "... Back"};
const String modeConfirmText[4] = {"Change", "mode?", "Confirm", "... Back"};
const String cursorSpMenuText[4] = {"  ", "Increase", "Decrease", "... Back"};
const String moreMenuText[4] = {"Sound", "Sip & Puff", "... Back", "         "};
const String soundMenuText[4] = {"Sound:", "<>", "Turn <>", "... Back"};
const String sipPuffThreshMenuText[4] = {"Sip Threshold", "Puff Threshold", "... Back"};

// Number of selectable options in each menu
const int mainMenuLen = 5;
const int exitConfirmLen = 2;
const int calibMenuLen = 3;
const int modeMenuLen = 4;
const int cursorSpMenuLen = 3;
const int moreMenuLen = 3;
const int soundMenuLen = 2;
const int sipPuffThreshMenuLen = 3;

const int TEXT_ROWS = 4; //Probably a better way to do this with calculations based on text size and screen size

class LSDisplay {
private:
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  bool is_active = false;
  LSTimer <void> displayStateTimer;                      //Timer used to measure time for each sip and puff action. 
  int displayStateTimerId;                                //The id for the sap state timer
  int currentMenu = 0;
  int prevMenu = -1;
  int currentSelection = 0;
  int selectedLine; 

  int mode = MODE_MOUSE_USB;
  int tempMode = MODE_MOUSE_USB;
  int cursorSpeedLevel = 5;
  bool bluetoothOn = false;
  bool soundOn = true;
  
  bool scrollOn = false;
  long scrollDelayTimer = millis();
  int scrollPos = 12;
  
  bool buttonSelPressed = false;
  bool buttonNextPressed = false;
  bool buttonSelPrevPressed = false;
  bool buttonNextPrevPressed = false;
  
  int cursorStart = 0;
  int countMenuScroll = 0;
  
  int currentMenuLength;
  String *currentMenuText;
  String selectedText;


public:
  LSDisplay();
  void begin();
  void clear();
  void show();


};



LSDisplay::LSDisplay() {
}

LSDisplay::begin() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
}

LSDisplay::clear() {
  
}





#endif
