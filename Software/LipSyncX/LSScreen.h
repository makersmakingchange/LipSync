/* 
* File: LSScreen.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: Beta (03 January 2024)
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
#ifndef _LSSCREEN_H
#define _LSSCREEN_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CONF_SCREEN_WIDTH 128 // OLED display width, in pixels
#define CONF_SCREEN_HEIGHT 64 // OLED display height, in pixels

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

//Adafruit_SSD1306 _display(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);

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

const int TEXT_ROWS = 4; 

class LSScreen {
private:
  Adafruit_SSD1306 _display = Adafruit_SSD1306(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);
  bool is_active = false;
  LSTimer <void> screenStateTimer;                      //Timer used to measure time for each sip and puff action. 
  int screenStateTimerId;                                //The id for the sap state timer
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

  //void setupDisplay();
  //void displayMenu();
  //void displayCursor();



public:
  LSScreen();
  void begin();
  void update();
  void clear();
  void show();
  //void startupScreen();
  void nextSelection();
  void scrollLongText();

  void setupDisplay();
  void displayMenu();
  void displayCursor();
      
  void splashScreen();
  void nextMenuItem();
  void selectMenuItem();
  bool isActive();
  void activateMenu();
  void deactivateMenu();
};



LSScreen::LSScreen() {
}


void LSScreen::begin() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }


  setupDisplay();
  _display.setTextWrap(false);
  _display.display();

  //startupScreen();

}

void LSScreen::clear() {
  //_display.clearDisplay();
  setupDisplay();
}

void LSScreen::update() {
  
}

void LSScreen::splashScreen() {
  setupDisplay();
  
  _display.println("");
  _display.println("LipSync");
  _display.println("v4.0.1");

  _display.setTextSize(1);
  _display.println("Makers Making Change");
  _display.display();
  
  _display.setTextSize(2);
  
}

void LSScreen::nextMenuItem() {
  
}

void LSScreen::selectMenuItem() {
  
}

void LSScreen::activateMenu() {
  is_active = true;
  
}

void LSScreen::deactivateMenu() {
  is_active = false;
  
}

void LSScreen::setupDisplay() {
  _display.clearDisplay();

  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  _display.setCursor(0, 0);
}

/*
void LSScreen::startupScreen(){
  
  setupDisplay();

  _display.println("LipSync");
  _display.println("v4.0.1");

  _display.setTextSize(1);
  _display.println("Makers Making Change");
  _display.display();
  
  _display.setTextSize(2);
}
*/

void LSScreen::displayMenu() {

  setupDisplay();

  for (int i = 0; i < TEXT_ROWS; i++) {
    if (i >= cursorStart){
      _display.print(" "), _display.println(currentMenuText[i+countMenuScroll]);
    } else {
      _display.println(currentMenuText[i]);
    }
  }

  _display.display();

  //currentSelection = 0;
  displayCursor();
}

void LSScreen::displayCursor() {
  int cursorPos;
  if (currentSelection + cursorStart > TEXT_ROWS-1){
    cursorPos = TEXT_ROWS-1;
  } else {
    cursorPos = currentSelection;
  }

  // These settings are likely already implemented and these lines can likely be removed, this is mostly here just to make sure 
  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  // Show cursor on text line of selection index, erase previous cursor
  _display.setCursor(0, 16 * cursorStart);  
  for (int i = 0; i < currentMenuLength; i++) {    
    if (i == cursorPos) {
      _display.println(">");
    } else {
      _display.println(" ");
    }
  }

  _display.display();

  selectedLine = cursorStart + currentSelection;
  selectedText = currentMenuText[selectedLine];
  
  if (selectedText.length() > 9){
    scrollOn = true;
    scrollPos = 12;
    delay(200);                           // may need to remove this
    scrollLongText();
  } else {
    scrollOn = false;
  }
}

void LSScreen::nextSelection() {
  if (scrollOn){
    _display.setCursor(0, selectedLine *16);
    _display.print("                                   ");
    _display.setCursor(12, selectedLine *16);
    _display.print(selectedText);
  }

  currentSelection++;
  if (currentSelection >= currentMenuLength) {   
    currentSelection = 0;
    countMenuScroll = 0;
    displayMenu();
  } else if (currentSelection + cursorStart > TEXT_ROWS-1){
    countMenuScroll++;
    displayMenu();
  } 

  displayCursor();

}

void LSScreen::scrollLongText() {
  int minPos = -12 * selectedText.length();
  
  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  _display.setTextWrap(false);
  
  if (millis() - scrollDelayTimer >= SCROLL_DELAY_MILLIS){
    scrollDelayTimer = millis();
    
    //Clear previous text by writing over it with blank text
    _display.setCursor(0, selectedLine *16);
    _display.print("                                   ");

    //Display text in new position to simulate scrolling
    _display.setCursor(scrollPos, selectedLine *16);
    _display.print(selectedText);

    _display.setCursor(0, selectedLine *16);
    _display.print(">");
    _display.display();
    //displayCursor();
    scrollPos = scrollPos-4;
    if (scrollPos < minPos) scrollPos = _display.width();
  }
  
}



bool LSScreen::isActive() {

return is_active;
 
}

#endif
