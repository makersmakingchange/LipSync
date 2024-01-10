/* 
* File: LSScreen.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: Beta (09 January 2024)
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

//Mode pages
#define CONFIRM_MODE_CHANGE 31

//More Menus
#define SOUND_MENU        51
#define SIP_PUFF_MENU     52
#define SIP_THRESH_MENU   521
#define PUFF_THRESH_MENU  522

#define MODE_MOUSE_USB  0
#define MODE_MOUSE_BT   1
#define MODE_GAMEPAD    2

#define SCROLL_DELAY_MILLIS   100

//Adafruit_SSD1306 _display(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TEXT_ROWS = 4; 

class LSScreen {
private:
  Adafruit_SSD1306 _display = Adafruit_SSD1306(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);
  bool is_active = false;
  LSTimer <void> screenStateTimer;                      //Timer 
  int screenStateTimerId;                                //The id for the sap state timer
  int _currentMenu = 0;
  int _prevMenu = -1;
  int _currentSelection = 0;
  int _selectedLine; 

  int _mode = MODE_MOUSE_USB;
  int _tempMode = MODE_MOUSE_USB;
  int _cursorSpeedLevel;
  bool _soundOn = true;
  
  bool _scrollOn = false;
  long _scrollDelayTimer = millis();
  int _scrollPos = 12;
  
  int _cursorStart = 0;
  int _countMenuScroll = 0;
  
  int _currentMenuLength;
  String *_currentMenuText;
  String _selectedText;

  float _sipPressThresh;
  float _puffPressThresh;

  //void setupDisplay();
  //void displayMenu();
  //void displayCursor();

  void mainMenu();
  void exitConfirmMenu();
  void calibMenu();
  void modeMenu();
  void confirmModeChange();
  void changeMode();
  void cursorSpeedMenu();
  void bluetoothMenu();
  void moreMenu();
  void centerResetPage();
  void fullCalibrationPage();
  void centerReset();
  void soundMenu();
  void sipPuffThreshMenu();
  void adjustSipThreshMenu();
  void adjustPuffThreshMenu();

  String _mainMenuText[5] = {"Exit Menu", "Calibrate", "Mode", "Cursor speed", "More"};
  String _exitConfirmText[4] = {"Exit", "settings?", "Confirm", "... Back"};
  String _calibMenuText[4] = {"Center reset", "Full Calibration", "... Back", " "};
  String _modeMenuText[4] = {"MOUSE USB", "MOUSE BT", "GAMEPAD ", "... Back"};
  String _modeConfirmText[4] = {"Change", "mode?", "Confirm", "... Back"};
  String _cursorSpMenuText[4] = {"Speed: ", "Increase", "Decrease", "... Back"};
  String _moreMenuText[4] = {"Sound", "Sip & Puff", "... Back", "         "};
  String _soundMenuText[4] = {"Sound:", "<>", "Turn <>", "... Back"};
  String _sipPuffThreshMenuText[4] = {"Sip Threshold", "Puff Threshold", "... Back"};
  String _adjustSipThreshMenuText[4] = {"Sip: ", "Increase", "Decrease", "... Back"};
  String _adjustPuffThreshMenuText[4] = {"Puff: ", "Increase", "Decrease", "... Back"};

  // Number of selectable options in each menu
  const int mainMenuLen = 5;
  const int exitConfirmLen = 2;
  const int calibMenuLen = 3;
  const int modeMenuLen = 4;
  const int cursorSpMenuLen = 3;
  const int moreMenuLen = 3;
  const int soundMenuLen = 2;
  const int sipPuffThreshMenuLen = 3;
  const int adjustSipThreshMenuLen = 3;
  const int adjustPuffThreshMenuLen = 3;


public:
  LSScreen();
  void begin();
  void update();
  void clear();
  void show();
  //void startupScreen();
  //void nextSelection();
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
  //Loop for screen functions 
  if (_scrollOn){
    scrollLongText();
  }
}

void LSScreen::activateMenu() {
  is_active = true;
  mainMenu();
}

void LSScreen::deactivateMenu() {
  is_active = false;
  clear();
  _display.display();
  
}

bool LSScreen::isActive() {

return is_active;
 
}

//*********************************//
// Function   : splashScreen
// 
// Description: Displays a screen with the device name and version. To be used when the device starts up.
//              
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSScreen::splashScreen() {
  setupDisplay();
  
  _display.println("LipSync");
  _display.println("v4.0.1");

  _display.setTextSize(1);
  _display.println("Makers Making Change");
  _display.display();
  
  _display.setTextSize(2);
  
}

//------------------------------------------//
// Functions called from inputs
//------------------------------------------//

void LSScreen::nextMenuItem() {
  if (_scrollOn){
    _display.setCursor(0, _selectedLine *16);
    _display.print("                                   ");
    _display.setCursor(12, _selectedLine *16);
    _display.print(_selectedText);
  }

  _currentSelection++;
  if (_currentSelection >= _currentMenuLength) {   
    _currentSelection = 0;
    _countMenuScroll = 0;
    displayMenu();
  } else if (_currentSelection + _cursorStart > TEXT_ROWS-1){
    _countMenuScroll++;
    displayMenu();
  } 

  displayCursor();
}

void LSScreen::selectMenuItem() {
  _countMenuScroll = 0;
  switch (_currentMenu) {
    case MAIN_MENU:
      _currentMenu = _currentSelection+1;
      switch (_currentMenu) {
        case MAIN_MENU:
          mainMenu();
          break;
        case EXIT_MENU:
          exitConfirmMenu();
          break;
        case CALIB_MENU:
          calibMenu();
          break;
        case MODE_MENU:
          modeMenu();
          break;
        case CURSOR_SP_MENU:
          cursorSpeedMenu();
          break;
        case MORE_MENU:
          moreMenu();
          break;
      }
      break;
    case EXIT_MENU:
        if (_currentSelection == 1){
          _currentSelection = 0;
          mainMenu();
        } else {
          setupDisplay();
          _display.println("Exiting");
          _display.display();
          delay(2000);                  // TODO: remove delay

          deactivateMenu();
        }
        break;
    case CALIB_MENU:
      if (_currentSelection == 0){
        _currentMenu = CENTER_RESET_PAGE;
        centerResetPage();
      } else if (_currentSelection == 1){
        _currentMenu = FULL_CALIB_PAGE;
        fullCalibrationPage();
      } else if (_currentSelection == 2){
        _currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case MODE_MENU:
      if (_currentSelection < (modeMenuLen - 1)){
        // Confirm mode change
        _tempMode = _currentSelection;
        if (_tempMode != _mode){
          confirmModeChange();
        }
      } else if (_currentSelection == (modeMenuLen-1)){
        mainMenu();
      }
      break;
    case CONFIRM_MODE_CHANGE:
      if (_currentSelection == 1){
        _currentSelection = 0;
        modeMenu();
      } else {
        changeMode();
      }
      break;
    case CURSOR_SP_MENU:
      switch (_currentSelection){
        case 0:       //Increase
          increaseJoystickSpeed(true,false);
          _cursorSpeedLevel = getJoystickSpeed(true,false);  
          _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel) + " ";
          _display.setCursor(0,0);
          _display.print(_cursorSpMenuText[0]);
          _display.display();
          break;
        case 1:       //Decrease
          decreaseJoystickSpeed(true,false);
          _cursorSpeedLevel = getJoystickSpeed(true,false);  
          _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel) + " ";
          _display.setCursor(0,0);
          _display.print(_cursorSpMenuText[0]);
          _display.display();
          break;
        case 2:       //Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case MORE_MENU:
      if (_currentSelection == 0){
        _currentMenu = SOUND_MENU;
        soundMenu();
      } else if (_currentSelection == 1){
        _currentMenu = SIP_PUFF_MENU;
        sipPuffThreshMenu();
      } else if (_currentSelection == 2){
        _currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case SOUND_MENU:
       switch (_currentSelection){
        case 0:
          _soundOn = !_soundOn;
          //do function for turning sound on/off
          if (_soundOn){
            buzzerSoundOn();
          } else {
            buzzerSoundOff();
          }
          soundMenu();
          break;
        case 1:
          _currentMenu = MAIN_MENU;
          mainMenu(); 
        }
        break;
      case SIP_PUFF_MENU:
       switch (_currentSelection){
        case 0:     // Sip
          adjustSipThreshMenu();
          break;
        case 1:     // Puff
          adjustPuffThreshMenu();
          break;
        case 2:
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
        }
        break;
      case SIP_THRESH_MENU:
        switch (_currentSelection){
          case 0:       //Increase
            _sipPressThresh = getSipPressureThreshold(false,false);
            _sipPressThresh++;                                                                                    // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
            setSipPressureThreshold(false, false, _sipPressThresh);
            _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";
            _display.setCursor(0,0);
            _display.print(_adjustSipThreshMenuText[0]);
            _display.display();
            break;
          case 1:       //Decrease
            _sipPressThresh = getSipPressureThreshold(false,false);
            _sipPressThresh--;                                                                                    // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
            setSipPressureThreshold(false, false, _sipPressThresh);
            _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";
            _display.setCursor(0,0);
            _display.print(_adjustSipThreshMenuText[0]);
            _display.display();
            break;
          case 2:       //Back
            _currentMenu = MAIN_MENU;
            mainMenu();
            break;
        }
        break;
      case PUFF_THRESH_MENU:
        switch (_currentSelection){
          case 0:       //Increase
            _puffPressThresh = getPuffPressureThreshold(false,false);
            _puffPressThresh++;                                                                                    // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
            setPuffPressureThreshold(false, false, _puffPressThresh);
            _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";
            _display.setCursor(0,0);
            _display.print(_adjustPuffThreshMenuText[0]);
            _display.display();
            break;
          case 1:       //Decrease
            _puffPressThresh = getPuffPressureThreshold(false,false);
            _puffPressThresh--;                                                                                    // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
            setPuffPressureThreshold(false, false, _puffPressThresh);
            _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";
            _display.setCursor(0,0);
            _display.print(_adjustPuffThreshMenuText[0]);
            _display.display();
            break;
          case 2:       //Back
            _currentMenu = MAIN_MENU;
            mainMenu();
            break;
        }
        break;
  }
}


//------------------------------------------//
// Menu displaying functions
//------------------------------------------//

void LSScreen::setupDisplay() {
  _display.clearDisplay();

  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  _display.setCursor(0, 0);
}

void LSScreen::displayMenu() {

  setupDisplay();

  for (int i = 0; i < TEXT_ROWS; i++) {
    if (i >= _cursorStart){
      _display.print(" "), _display.println(_currentMenuText[i+_countMenuScroll]);
    } else {
      _display.println(_currentMenuText[i]);
    }
  }

  _display.display();

  //_currentSelection = 0;
  displayCursor();
}

void LSScreen::displayCursor() {
  int cursorPos;
  if (_currentSelection + _cursorStart > TEXT_ROWS-1){
    cursorPos = TEXT_ROWS-1;
  } else {
    cursorPos = _currentSelection;
  }

  // These settings are likely already implemented and these lines can likely be removed, this is mostly here just to make sure 
  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  // Show cursor on text line of selection index, erase previous cursor
  _display.setCursor(0, 16 * _cursorStart);  
  for (int i = 0; i < _currentMenuLength; i++) {    
    if (i == cursorPos) {
      _display.println(">");
    } else {
      _display.println(" ");
    }
  }

  _display.display();

  _selectedLine = _cursorStart + _currentSelection;
  _selectedText = _currentMenuText[_selectedLine];
  
  if (_selectedText.length() > 9){
    Serial.println("Long text");
    _scrollOn = true;
    _scrollPos = 12;
    delay(200);                           // TODO: remove delay
    scrollLongText();
  } else {
    _scrollOn = false;
  }
}

/*
void LSScreen::nextSelection() {
  if (_scrollOn){
    _display.setCursor(0, _selectedLine *16);
    _display.print("                                   ");
    _display.setCursor(12, _selectedLine *16);
    _display.print(_selectedText);
  }

  _currentSelection++;
  if (_currentSelection >= _currentMenuLength) {   
    _currentSelection = 0;
    _countMenuScroll = 0;
    displayMenu();
  } else if (_currentSelection + _cursorStart > TEXT_ROWS-1){
    _countMenuScroll++;
    displayMenu();
  } 

  displayCursor();

}
*/

void LSScreen::scrollLongText() {
  Serial.println("scroll function");
  int minPos = -12 * _selectedText.length();
  
  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  _display.setTextWrap(false);
  
  if (millis() - _scrollDelayTimer >= SCROLL_DELAY_MILLIS){
    Serial.println("Timer good");
    _scrollDelayTimer = millis();
    
    //Clear previous text by writing over it with blank text
    _display.setCursor(0, _selectedLine *16);
    _display.print("                                   ");

    //Display text in new position to simulate scrolling
    _display.setCursor(_scrollPos, _selectedLine *16);
    _display.print(_selectedText);

    _display.setCursor(0, _selectedLine *16);
    _display.print(">");
    _display.display();
    //displayCursor();
    _scrollPos = _scrollPos-4;
    if (_scrollPos < minPos) _scrollPos = _display.width();
  }
  
}

//********** MENUS **********//

void LSScreen::mainMenu(void) {
  _currentMenu = MAIN_MENU;
  
  //if new menu selection
  //if (_prevMenu != _currentMenu) {
    _currentMenuLength = mainMenuLen;
    _currentMenuText = _mainMenuText;
    _cursorStart = 0;
    _currentSelection = 0;

    displayMenu();
  //}
}

void LSScreen::exitConfirmMenu(){
  _prevMenu = _currentMenu;
  _currentMenu = EXIT_MENU;
  _currentMenuText = _exitConfirmText;
  _currentMenuLength = 2;
  _cursorStart = 2;
  _currentSelection = 0;
  displayMenu();

}

void LSScreen::calibMenu(void) {
  _currentMenu = CALIB_MENU;
  if (_prevMenu != _currentMenu) {
    _currentMenuLength = calibMenuLen;
    _currentMenuText = _calibMenuText;
    _cursorStart = 0;
    _currentSelection = 0;
  
    displayMenu();
  }

}

void LSScreen::modeMenu(void) {
  _currentMenu = MODE_MENU;

  _currentMenuLength = modeMenuLen;
  _currentMenuText = _modeMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();

  _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' coloured text
  if (_mode == MODE_MOUSE_USB){
      _display.setCursor(12, 0);
      _display.print(_modeMenuText[MODE_MOUSE_USB]);
  } else if (_mode == MODE_MOUSE_BT){
      _display.setCursor(12, 16);
      //display.print(" MOUSE BLUETOOTH ");
      _display.print(_modeMenuText[MODE_MOUSE_BT]);
  } else if (_mode == MODE_GAMEPAD){
      _display.setCursor(12, 16*2);
      //display.print(" GAMEPAD ");
      _display.print(_modeMenuText[MODE_GAMEPAD]);
  }

  _display.display();
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Reset text colour to white on black
  
}
//
void LSScreen::confirmModeChange() {
  _currentMenu = CONFIRM_MODE_CHANGE;
  _currentMenuText = _modeConfirmText;
  _currentMenuLength = 2;
  _cursorStart = 2;
  _currentSelection = 0;
  displayMenu();

}

void LSScreen::changeMode(){
  _mode = _tempMode;

  /*                  // ************************************************************** TODO: CHANGE THIS <>
  switch (_mode){
    case MODE_MOUSE_USB:
      digitalWrite(PIN_LED_GAMEPAD, LOW);
      digitalWrite(PIN_LED_MOUSE, HIGH);
      break;
    case MODE_MOUSE_BT:
      digitalWrite(PIN_LED_GAMEPAD, LOW);
      digitalWrite(PIN_LED_MOUSE, HIGH);
      break;
    case MODE_GAMEPAD:
      digitalWrite(PIN_LED_MOUSE, LOW);
      digitalWrite(PIN_LED_GAMEPAD, HIGH);
      break;
  }
  */                  // ************************************************************** TODO: CHANGE THIS ^

  setupDisplay();
  _display.println("Resetting");
  _display.println("device");
  _display.display();

  delay(2000);
  
  //conduct mode change
  //save mode into flash
  //user feedback showing current mode    // ************************************************************** TODO: CHANGE THIS <>
  
  //software reset

  //remove this for final code, because device will reset
  //readButtons();                            // ************************************************************** TODO: CHANGE THIS <>
  _currentMenu = MAIN_MENU;
  mainMenu();

  //software reset
}

void LSScreen::cursorSpeedMenu(void) { 
  _currentMenu = CURSOR_SP_MENU;
  _cursorSpeedLevel = getJoystickSpeed(true,false); 
  
  _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel);
  
  _currentMenuLength = cursorSpMenuLen;
  _currentMenuText = _cursorSpMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();

}

void LSScreen::moreMenu(){
  _currentMenu = MORE_MENU;

  _currentMenuLength = moreMenuLen;
  _currentMenuText = _moreMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();

}

// ----- CALIBRATION PAGES ----- //
void LSScreen::centerResetPage(void){
  setupDisplay();

  _display.println("Center");
  _display.println("reset, do");
  _display.println("not move");
  _display.println("joystick");

  _display.display();

  //Perform cursor center
  setJoystickInitialization(true,false);

  delay(100);

  _display.clearDisplay();
  _display.setCursor(0,0);
  _display.println("Center");
  _display.println("reset");
  _display.println("complete");

  _display.display();

  delay(2000);

  mainMenu();
  
}

void LSScreen::fullCalibrationPage(void){

  _display.println("Full");
  _display.println("Calibration");

  _display.display();

  //TO DO
  //Add prompts and add function for calibration // ************************************************************** TODO: CHANGE THIS <>

  delay(1000);

  mainMenu();
}

// ----- MORE SETTINGS MENUS ----- //

void LSScreen::soundMenu(){
  _currentMenu = SOUND_MENU;
  
  if (_soundOn) {
    _soundMenuText[1] = "ON";
    _soundMenuText[2] = "Turn off";
  } else {
    _soundMenuText[1] = "OFF";
    _soundMenuText[2] = "Turn on";
  }

  _currentMenuLength = soundMenuLen;
  _currentMenuText = _soundMenuText;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}

void LSScreen::sipPuffThreshMenu(){
  _currentMenu = SIP_PUFF_MENU;

  _currentMenuLength = sipPuffThreshMenuLen;
  _currentMenuText = _sipPuffThreshMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  //Add sip/puff  thresh adjustment function // ************************************************************** TODO: CHANGE THIS <>

  displayMenu();
}

void LSScreen::adjustSipThreshMenu(void) { 
  _currentMenu = SIP_THRESH_MENU;
  _sipPressThresh = getSipPressureThreshold(false,false);
  
  _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";
  
  _currentMenuLength = adjustSipThreshMenuLen;
  _currentMenuText = _adjustSipThreshMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();

}

void LSScreen::adjustPuffThreshMenu(void) { 
  _currentMenu = PUFF_THRESH_MENU;
  _sipPressThresh = getPuffPressureThreshold(false,false);
  
  _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";
  
  _currentMenuLength = adjustPuffThreshMenuLen;
  _currentMenuText = _adjustPuffThreshMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();

}

#endif
