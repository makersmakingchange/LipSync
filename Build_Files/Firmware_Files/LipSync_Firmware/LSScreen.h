/* 
* File: LSScreen.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (20 January 2025)
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
#ifndef _LSSCREEN_H
#define _LSSCREEN_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CONF_SCREEN_WIDTH 128  // OLED display width, in pixels
#define CONF_SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define MAIN_MENU 0
#define EXIT_MENU 1
#define CALIB_MENU 2
#define MODE_MENU 3
#define CURSOR_SP_MENU 4
#define MORE_MENU 5

// Calibration pages
#define CENTER_RESET_PAGE 21
//#define FULL_CALIB_PAGE             22

// Mode pages
#define CONFIRM_MODE_CHANGE 31

// More Menus
#define SOUND_MENU 51
#define SIP_PUFF_MENU 52
#define SIP_THRESH_MENU 521
#define PUFF_THRESH_MENU 522
#define FULL_CALIB_PAGE 53
#define FULL_CALIB_CONFIRM_PAGE 531
#define RESTART_PAGE 54
#define FACTORY_RESET_PAGE 55
#define FACTORY_RESET_CONFIRM2_PAGE 551
#define INFO_PAGE 56

#define SCROLL_DELAY_MILLIS 100 // [ms] This controls the scroll speed of long menu items

#define _MODE_MOUSE_USB 1
#define _MODE_MOUSE_BT 2
#define _MODE_GAMEPAD_USB 3

const int CHAR_PIXEL_HEIGHT_S1 = 8;    // The height of a character on the screen, in pixels, for size 1 text
const int CHAR_PIXEL_WIDTH_S1 = 6;     // The width of a character on the screen, in pixels, for size 1 text

const int CHAR_PIXEL_HEIGHT_S2 = CHAR_PIXEL_HEIGHT_S1 * 2;    // The height of a character on the screen, in pixels, for size 2 text
const int CHAR_PIXEL_WIDTH_S2  = CHAR_PIXEL_WIDTH_S1 * 2;     // The width of a character on the screen, in pixels, for size 2 text

const int TEXT_ROWS = CONF_SCREEN_HEIGHT / CHAR_PIXEL_HEIGHT_S2;

int scrollPixelsPerLoop = 4;

extern bool g_displayConnected;                   // Display connection state
extern bool g_joystickSensorConnected;            // Joystick sensor connection state
extern bool g_mouthpiecePressureSensorConnected;  // Mouthpiece pressure sensor connection state
extern bool g_ambientPressureSensorConnected;     // Ambient pressure sensor connection state

class LSScreen {

public:
  LSScreen();
  void begin();
  void update();
  void clear();
  void show();

  void setupDisplay();
  void splashScreen();
  void splashScreen2();

  void nextMenuItem();
  void selectMenuItem();
  bool isMenuActive();
  void activateMenu();
  void deactivateMenu();
  void centerResetPage();
  void centerResetCompletePage();
  void fullCalibrationPrompt(int stepNum);
  void testPage(unsigned int usbConnectDelay);
  void noUsbPage();
  void errorPageI2C();
  void errorPageCable();
  void warningUSBDebugOn();
  void connectionTimingPage(unsigned long, unsigned long);
  void resetPage();

  bool showCenterResetComplete = false;

private:
  Adafruit_SSD1306 _display = Adafruit_SSD1306(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);

  LSTimer<void> _screenStateTimer;  // Timer
  int _screenStateTimerId;          // The id for the sap state timer

  bool _isActive = false;
  int _currentMenu = 0;
  int _prevMenu = -1;
  int _currentSelection = 0;
  int _selectedLine;

  int _operatingMode;
  int _tempOperatingMode;
  int _communicationMode;
  int _tempCommunicationMode;
  int _cursorSpeedLevel;
  bool _soundOn = true;
  int _soundMode;

  bool _scrollOn = false;
  unsigned long _scrollDelayTimer = millis();
  int _scrollPos = 12;
  const unsigned int _maxCharPerLine = 10;

  int _cursorStart = 0;
  int _countMenuScroll = 0;

  int _testScreenAttempt = 0;

  int _currentMenuLength;
  String *_currentMenuText;
  String _selectedText;

  float _sipPressThresh;
  float _puffPressThresh;

  unsigned long _lastActivityMillis;

  void displayMenu();
  void displayCursor();
  void scrollLongText();
  void drawCentreString(const String &buf, int y);
  void modeMenuHighlight();

  void mainMenu();
  void exitConfirmMenu();
  void calibMenu();
  void modeMenu();
  void confirmModeChange();
  void changeMode();
  void cursorSpeedMenu();
  void bluetoothMenu();
  void moreMenu();
  void fullCalibrationPage();
  void fullCalibrationConfirmPage();
  void centerReset();
  void soundMenu();
  void sipPuffThreshMenu();
  void adjustSipThreshMenu();
  void adjustPuffThreshMenu();
  void restartConfirmPage();
  void factoryResetConfirm1Page();
  void factoryResetConfirm2Page();

  String _mainMenuText[5] = { "Exit Menu", "Center Reset", "Mode", "Cursor Speed", "More" };
  String _exitConfirmText[4] = { "Exit", "settings?", "Confirm", "... Back" };
  String _calibMenuText[4] = { "Center Reset", "... Back", " ", " " };
  String _modeMenuText[4] = { "MOUSE USB", "MOUSE BT", "GAMEPAD ", "... Back" };
  String _modeConfirmText[4] = { "Change", "mode?", "Confirm", "... Back" };
  String _cursorSpMenuText[4] = { "Speed: ", "Increase", "Decrease", "... Back" };
  String _moreMenuText[6] = {
    "Sound",
    "Sip & Puff",
    "Full Calibration",
    "Restart LipSync",
    "Factory Reset",
    "... Back",
  };
  String _soundMenuText[4] = { "Sound:", "<>", "Turn <>", "... Back" };
  String _sipPuffThreshMenuText[4] = { "Sip Threshold", "Puff Threshold", "... Back" };
  String _adjustSipThreshMenuText[4] = { "Sip: ", "Increase", "Decrease", "... Back" };
  String _adjustPuffThreshMenuText[4] = { "Puff: ", "Increase", "Decrease", "... Back" };
  String _restartConfirmText[4] = { "Restart", "LipSync?", "Confirm", "... Back" };
  String _factoryResetConfirm1Text[4] = { "Reset to", "defaults?", "Confirm", "... Back" };
  String _factoryResetConfirm2Text[4] = { "Are you", "sure?", "Confirm", "... Back" };
  String _fullCalibrationConfirmText[4] = { "Are you", "sure?", "Confirm", "... Back" };

  // Number of selectable options in each menu
  const int _mainMenuLen = 5;
  const int _exitConfirmLen = 2;
  const int _calibMenuLen = 2;
  const int _modeMenuLen = 4;
  const int _cursorSpMenuLen = 3;
  const int _moreMenuLen = 6;
  const int _soundMenuLen = 2;
  const int _sipPuffThreshMenuLen = 3;
  const int _adjustSipThreshMenuLen = 3;
  const int _adjustPuffThreshMenuLen = 3;
  const int _restartConfirmLen = 2;
  const int _factoryResetConfirm1Len = 2;
  const int _factoryResetConfirm2Len = 2;
  const int _fullCalibrationConfirmLen = 2;
};


//*********************************//
// Function   : LSScreen
//
// Description: Class for display object
//
// Arguments :  void
//
// Return     : void
//*********************************//
LSScreen::LSScreen() {
}

//*********************************//
// Function   : begin
//
// Description: Activates and clears display
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::begin() {

  if (_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    g_displayConnected = true;
  } else {
    Serial.println(F("ERROR: Display SSD1306 allocation failed"));

    g_displayConnected = false;
    //TODO Handle screen not initialized error
  }

  setupDisplay();  // Clear screen
  _display.setTextWrap(false);
  _display.display();

  _operatingMode = getOperatingMode(false, false);  // TODO JDMc 2025-Jan-24 These should be moved to update function so they are updated if changed through serial API
  _communicationMode = getCommunicationMode(false, false);
  _soundMode = getSoundMode(false, false);

  _lastActivityMillis = millis();
}

//*********************************//
// Function   : clear
//
// Description: Clears the display.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::clear() {
  //_display.clearDisplay();
  setupDisplay();
}


//*********************************//
// Function   : update
//
// Description: Refreshes the display and scrolls text if necessary
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::update() {
  _screenStateTimer.run();

  // Loop for screen functions
  if (_scrollOn) {
    scrollLongText();
  }

  if (((millis() - _lastActivityMillis) > CONF_MENU_TIMEOUT) && _isActive) {
    deactivateMenu();
  }
}

//*********************************//
// Function   : activateMenu
//
// Description: Changes display mode to active and loads main menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::activateMenu() {
  _lastActivityMillis = millis();
  _isActive = true;
  _operatingMode = getOperatingMode(false, false);
  mainMenu();
}

//*********************************//
// Function   : deactivateMenu
//
// Description: Changes display mode to inactive and clears display.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::deactivateMenu() {
  _isActive = false;
  clear();
  _display.display();
}

//*********************************//
// Function   : isMenuActive
//
// Description: Returns a boolean representing state of menu
//
// Arguments :  void
//
// Return     : bool : _isActive : true = menu activated
//*********************************//
bool LSScreen::isMenuActive() {

  return _isActive;
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

  _display.setTextSize(2);
  drawCentreString("LipSync", 12);

  _display.setTextSize(1);
  drawCentreString(lipsyncVersionStr, 32);
  drawCentreString("Makers Making Change", 54);

  _display.display();
}

//*********************************//
// Function   : splashScreen2
//
// Description: Displays a screen indicating it is ready to use (center reset complete) and indicating the mode.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::splashScreen2() {
  setupDisplay();

  _display.setTextSize(2);
  drawCentreString("Ready to", 0);
  drawCentreString("use", 16);
  _display.setTextSize(1);
  drawCentreString("Mode:", 40);
  _display.setTextSize(2);


  _display.setTextSize(2);

  switch (_operatingMode) {
    case CONF_OPERATING_MODE_MOUSE:
      switch (_communicationMode) {
        case CONF_COM_MODE_USB:
          drawCentreString("USB Mouse", 48);
          break;
        case CONF_COM_MODE_BLE:
          drawCentreString("BT Mouse", 48);
          break;
      }
      break;
    case CONF_OPERATING_MODE_GAMEPAD:
      _display.setCursor(1, 48);
      _display.print("USB");
      _display.setTextSize(1);
      _display.print(" ");
      _display.setTextSize(2);
      _display.print("Gamepad");  // text size changed for space so it would all fit on one line
      //_display.print("USB"); _display.setTextSize(1); _display.print(" "); _display.setTextSize(2); _display.print("Gamepad"); // text size changed for space so it would all fit on one line
      //drawCentreString("Gamepad", 48);
      break;
    default:
      _display.println("Error");
  }

  _display.display();

  if (USB_DEBUG){
    delay(2000);
    warningUSBDebugOn();
  } else {
    _screenStateTimerId = _screenStateTimer.setTimeout(CONF_SPLASH_SCREEN_DURATION, clearSplashScreen);
  }

  _lastActivityMillis = millis();
}

//****************************************//
// Functions called from inputs
//****************************************//

//*********************************//
// Function   : nextMenuItem
//
// Description: Cycles to the next item based on current selection and menu.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::nextMenuItem() {
  _lastActivityMillis = millis();

  if (_scrollOn) {
    _display.setCursor(0, _selectedLine * 16);
    _display.print("                                   ");
    _display.setCursor(12, _selectedLine * 16);
    _display.print(_selectedText);
  }

  _currentSelection++;
  if (_currentSelection >= _currentMenuLength) {
    _currentSelection = 0;
    _countMenuScroll = 0;
    displayMenu();
  } else if (_currentSelection + _cursorStart > TEXT_ROWS - 1) {
    _countMenuScroll++;
    displayMenu();
  }

  displayCursor();
}



//*********************************//
// Function   : selectMenuItem
//
// Description: Activates the appropriate action when the current selection is selected.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::selectMenuItem() {
  _lastActivityMillis = millis();

  _countMenuScroll = 0;
  switch (_currentMenu) {
    case MAIN_MENU:
      _currentMenu = _currentSelection + 1;
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
      if (_currentSelection == 1) {
        _currentSelection = 0;
        mainMenu();
      } else {
        setupDisplay();
        _display.println("Exiting");
        _display.display();
        delay(500);  // TODO: remove delay

        deactivateMenu();
      }
      break;
    case CALIB_MENU:
      if (_currentSelection == 0) {
        _currentMenu = CENTER_RESET_PAGE;
        centerResetPage();
      } else if (_currentSelection == 1) {
        _currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case MODE_MENU:
      if (_currentSelection < (_modeMenuLen - 1)) {
        // Confirm mode change
        switch (_currentSelection + 1) {
          case _MODE_MOUSE_USB:
            _tempOperatingMode = CONF_OPERATING_MODE_MOUSE;
            _tempCommunicationMode = CONF_COM_MODE_USB;
            break;
          case _MODE_MOUSE_BT:
            _tempOperatingMode = CONF_OPERATING_MODE_MOUSE;
            _tempCommunicationMode = CONF_COM_MODE_BLE;
            break;
          case _MODE_GAMEPAD_USB:
            _tempOperatingMode = CONF_OPERATING_MODE_GAMEPAD;
            _tempCommunicationMode = CONF_COM_MODE_USB;
            break;
        }
        //_tempOperatingMode = _currentSelection;
        if ((_tempOperatingMode != _operatingMode) || (_tempCommunicationMode != _communicationMode)) {
          confirmModeChange();
        }
      } else if (_currentSelection == (_modeMenuLen - 1)) {
        mainMenu();
      }
      break;
    case CONFIRM_MODE_CHANGE:
      if (_currentSelection == 1) {
        _currentSelection = 0;
        modeMenu();
      } else {
        changeMode();
      }
      break;
    case CURSOR_SP_MENU:
      switch (_currentSelection) {
        case 0:  // Increase
          increaseCursorSpeed(true, false);
          _cursorSpeedLevel = getCursorSpeed(true, false);
          _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel) + " ";
          _display.setCursor(0, 0);
          _display.print(_cursorSpMenuText[0]);
          _display.display();
          break;
        case 1:  // Decrease
          decreaseCursorSpeed(true, false);
          _cursorSpeedLevel = getCursorSpeed(true, false);
          _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel) + " ";
          _display.setCursor(0, 0);
          _display.print(_cursorSpMenuText[0]);
          _display.display();
          break;
        case 2:  // Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case MORE_MENU:
      if (_currentSelection == 0) {
        _currentMenu = SOUND_MENU;
        soundMenu();
      } else if (_currentSelection == 1) {
        _currentMenu = SIP_PUFF_MENU;
        sipPuffThreshMenu();
      } else if (_currentSelection == 2) {
        _currentMenu = FULL_CALIB_CONFIRM_PAGE;
        fullCalibrationConfirmPage();
      } else if (_currentSelection == 3) {
        _currentMenu = RESTART_PAGE;
        restartConfirmPage();
      } else if (_currentSelection == 4) {
        _currentMenu = FACTORY_RESET_PAGE;
        factoryResetConfirm1Page();
      } else if (_currentSelection == 5) {
        _currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case SOUND_MENU:
      switch (_currentSelection) {
        case 0:
          // do function for turning sound on/off
          if (_soundMode == CONF_SOUND_MODE_OFF) {
            buzzerSoundOn();
            _soundMode = CONF_SOUND_MODE_BASIC;
            setSoundMode(false, false, _soundMode);  // TODO: change menu to add Advanced Sound to menu
          } else {
            buzzerSoundOff();
            _soundMode = CONF_SOUND_MODE_OFF;
            setSoundMode(false, false, _soundMode);
          }
          soundMenu();
          break;
        case 1:
          _currentMenu = MAIN_MENU;
          mainMenu();
      }
      break;
    case SIP_PUFF_MENU:
      switch (_currentSelection) {
        case 0:  // Sip
          adjustSipThreshMenu();
          break;
        case 1:  // Puff
          adjustPuffThreshMenu();
          break;
        case 2:
          _currentMenu = MORE_MENU;
          moreMenu();
          break;
      }
      break;
    case SIP_THRESH_MENU:
      switch (_currentSelection) {
        case 0:  // Increase
          _sipPressThresh = getSipPressureThreshold(false, false);
          _sipPressThresh++;  // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
          setSipPressureThreshold(false, false, _sipPressThresh);
          _sipPressThresh = getSipPressureThreshold(false, false);          
          _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";
          _display.setCursor(0, 0);
          _display.print(_adjustSipThreshMenuText[0]);
          _display.display();
          break;
        case 1:  // Decrease
          _sipPressThresh = getSipPressureThreshold(false, false);
          _sipPressThresh--;  // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
          setSipPressureThreshold(false, false, _sipPressThresh);
          _sipPressThresh = getSipPressureThreshold(false, false);
          _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";
          _display.setCursor(0, 0);
          _display.print(_adjustSipThreshMenuText[0]);
          _display.display();
          break;
        case 2:  // Back
          _currentMenu = SIP_PUFF_MENU;
          sipPuffThreshMenu();
          break;
      }
      break;
    case PUFF_THRESH_MENU:
      switch (_currentSelection) {
        case 0:  // Increase
          _puffPressThresh = getPuffPressureThreshold(false, false);
          _puffPressThresh++;  // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?
          setPuffPressureThreshold(false, false, _puffPressThresh);
          _puffPressThresh = getPuffPressureThreshold(false, false);
          _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";
          _display.setCursor(0, 0);
          _display.print(_adjustPuffThreshMenuText[0]);
          _display.display();
          break;
        case 1:  // Decrease
          _puffPressThresh = getPuffPressureThreshold(false, false);
          _puffPressThresh--;  // ** TODO: CHANGE THIS, What values are we expecting? By how much to increase?       
          setPuffPressureThreshold(false, false, _puffPressThresh);
          _puffPressThresh = getPuffPressureThreshold(false, false);          
          _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";
          _display.setCursor(0, 0);
          _display.print(_adjustPuffThreshMenuText[0]);
          _display.display();
          break;
        case 2:  // Back
          _currentMenu = SIP_PUFF_MENU;
          sipPuffThreshMenu();
          break;
      }
      break;
    case FULL_CALIB_CONFIRM_PAGE:
      switch (_currentSelection) {
        case 0:  // Perform full calibration
          _currentMenu = FULL_CALIB_PAGE;
          fullCalibrationPage();
          break;
        case 1:  // Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case RESTART_PAGE:
      switch (_currentSelection) {
        case 0:  // Perform factory reset
          softwareReset();
          break;
        case 1:  // Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case FACTORY_RESET_PAGE:
      switch (_currentSelection) {
        case 0:  // Perform factory reset
          _currentMenu = FACTORY_RESET_CONFIRM2_PAGE;
          factoryResetConfirm2Page();
          break;
        case 1:  // Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case FACTORY_RESET_CONFIRM2_PAGE:
      switch (_currentSelection) {
        case 0:  // Perform factory reset
          setupDisplay();
          _display.println("Resetting");
          _display.display();
          factoryReset(false, false);
          break;
        case 1:  // Back
          _currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
  }
}


//****************************************//
// Menu displaying functions
//****************************************//


//*********************************//
// Function   : setupDisplay
//
// Description: Clears the display, sets text size and color
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::setupDisplay() {
  _display.clearDisplay();
  _scrollOn = false;

  _display.setTextSize(2);                              // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // Draw white text on solid black background

  _display.setCursor(0, 0);
}


//*********************************//
// Function   : displayMenu
//
// Description: Prints each item in current menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::displayMenu() {
  setupDisplay();

  for (int i = 0; i < TEXT_ROWS; i++) {
    if (i >= _cursorStart) {
      _display.print(" "), _display.println(_currentMenuText[i + _countMenuScroll]);
    } else {
      _display.println(_currentMenuText[i]);
    }
  }

  if (_currentMenu == MODE_MENU) {
    modeMenuHighlight();
  }

  _display.display();

  //_currentSelection = 0;
  displayCursor();
}


//*********************************//
// Function   : displayCursor
//
// Description: TODO description
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::displayCursor() {
  int cursorPos;
  if (_currentSelection + _cursorStart > TEXT_ROWS - 1) {
    cursorPos = TEXT_ROWS - 1;
  } else {
    cursorPos = _currentSelection;
  }

  _display.setTextSize(2);                              // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // Draw white text on solid black background

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

  if (_selectedText.length() > (_maxCharPerLine - 1)) {
    //Serial.println("Long text");
    _scrollOn = true;
    _scrollPos = 12;
    delay(200);  // TODO: remove delay
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


//*********************************//
// Function   : scrollLongText
//
// Description: Format and Display Long Menu Items by scrolling
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::scrollLongText() {
  int minPos = -CHAR_PIXEL_WIDTH_S2 * _selectedText.length();

  _display.setTextSize(2);                              // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // Draw white text on solid black background
  _display.setTextWrap(false);

  if (millis() - _scrollDelayTimer >= SCROLL_DELAY_MILLIS) {
    _scrollDelayTimer = millis();

    // Clear previous text by writing over it with blank text
    _display.setCursor(0, _selectedLine * CHAR_PIXEL_HEIGHT_S2);
    _display.print("                                   ");

    // Display text in new position to simulate scrolling
    _display.setCursor(_scrollPos, _selectedLine * CHAR_PIXEL_HEIGHT_S2);
    _display.print(_selectedText);

    _display.setCursor(0, _selectedLine * CHAR_PIXEL_HEIGHT_S2);
    _display.print(">");
    _display.display();

    _scrollPos = _scrollPos - scrollPixelsPerLoop;
    
    if (_scrollPos < minPos) {
      _scrollPos = _display.width();
    }
  }
}

//*********************************//
// Function   : drawCentreString
//
// Description: Format and Display Main Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::drawCentreString(const String &buf, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  int x = 64;
  _display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);  // Calculate width of new string
  _display.setCursor(x - w / 2, y);
  _display.print(buf);
}

//********** MENUS **********//

//*********************************//
// Function   : mainMenu
//
// Description: Format and Display Main Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::mainMenu(void) {
  _currentMenu = MAIN_MENU;

  //if new menu selection
  //if (_prevMenu != _currentMenu) {
  _currentMenuLength = _mainMenuLen;
  _currentMenuText = _mainMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();
  //}
}

//*********************************//
// Function   : exitConfirmMenu
//
// Description: Format and Exit Confirm Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::exitConfirmMenu() {
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
    _currentMenuLength = _calibMenuLen;
    _currentMenuText = _calibMenuText;
    _cursorStart = 0;
    _currentSelection = 0;

    displayMenu();
  }
}


//*********************************//
// Function   : modeMenu
//
// Description: Format and display Operating Mode Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::modeMenu(void) {
  _currentMenu = MODE_MENU;

  _currentMenuLength = _modeMenuLen;
  _currentMenuText = _modeMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();

  modeMenuHighlight();
}

void LSScreen::modeMenuHighlight() {

  _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' coloured text

  switch (_operatingMode) {
    case CONF_OPERATING_MODE_MOUSE:
      switch (_communicationMode) {
        case CONF_COM_MODE_USB:
          _display.setCursor(12, 0);
          _display.print(_modeMenuText[_MODE_MOUSE_USB - 1]);
          break;
        case CONF_COM_MODE_BLE:
          _display.setCursor(12, 16);
          //display.print(" MOUSE BLUETOOTH ");
          _display.print(_modeMenuText[_MODE_MOUSE_BT - 1]);
          break;
      }
      break;
    case CONF_OPERATING_MODE_GAMEPAD:
      _display.setCursor(12, 16 * 2);
      //display.print(" GAMEPAD ");
      _display.print(_modeMenuText[_MODE_GAMEPAD_USB - 1]);
      break;
  }

  _display.display();
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // Reset text colour to white on black
}

//*********************************//
// Function   : confirmModeChange
//
// Description: Format and display Change Mode Confirm Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::confirmModeChange() {
  _currentMenu = CONFIRM_MODE_CHANGE;
  _currentMenuText = _modeConfirmText;
  _currentMenuLength = 2;
  _cursorStart = 2;
  _currentSelection = 0;
  displayMenu();
}


//*********************************//
// Function   : changeMode
//
// Description: Format and display Change Mode Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::changeMode() {
  setupDisplay();
  _display.println("Changing");
  _display.println("mode.");
  _display.println("Release");
  _display.println("joystick.");
  _display.display();
  delay(2000);

  if (_communicationMode != _tempCommunicationMode) {
    _communicationMode = _tempCommunicationMode;
    setCommunicationMode(false, false, _tempCommunicationMode);  // Sets new communication mode, saves in memory
  }

  if (_operatingMode != _tempOperatingMode) {
    _operatingMode = _tempOperatingMode;
    setOperatingMode(false, false, _tempOperatingMode);  // Sets new operating mode, saves in memory, and conducts software reset
  }

  softwareReset();  // TODO: is there a way to avoid software reset if just changing com mode?

  _currentMenu = MAIN_MENU;
  mainMenu();
}


//*********************************//
// Function   : cursorSpeedMenu
//
// Description: Format and display Cursor Speed Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::cursorSpeedMenu(void) {
  _currentMenu = CURSOR_SP_MENU;
  _cursorSpeedLevel = getCursorSpeed(true, false);

  _cursorSpMenuText[0] = "Speed: " + String(_cursorSpeedLevel);

  _currentMenuLength = _cursorSpMenuLen;
  _currentMenuText = _cursorSpMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : moreMenu
//
// Description: Format and display More Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::moreMenu() {
  _currentMenu = MORE_MENU;

  _currentMenuLength = _moreMenuLen;
  _currentMenuText = _moreMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();
}

// ----- CALIBRATION PAGES ----- //

//*********************************//
// Function   : centerResetPage
//
// Description: Format and display Center Reset Start Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::centerResetPage(void) {
  setupDisplay();

  _display.println("Center");
  _display.println("reset, do");
  _display.println("not move");
  _display.println("joystick");

  _display.display();

  // Perform cursor center
  showCenterResetComplete = true;
  setJoystickInitialization(true, false);
}

//*********************************//
// Function   : centerResetCompletePage
//
// Description: Format and display Center Reset Complete Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::centerResetCompletePage(void) {
  showCenterResetComplete = false;

  _display.clearDisplay();
  _display.setCursor(0, 0);
  _display.println("Center");
  _display.println("reset");
  _display.println("complete");

  _display.display();

  delay(2000);

  if (_isActive) {
    mainMenu();
  } else {
    deactivateMenu();
  }
}

//*********************************//
// Function   : fullCalibrationPage
//
// Description: Format and display Full Calibration Start Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::fullCalibrationPage(void) {
  setupDisplay();

  _display.println("Follow");
  _display.println("on screen");
  _display.println("prompts");

  _display.display();

  setJoystickCalibration(false, false);
}

//*********************************//
// Function   : fullCalibrationPrompt
//
// Description: Format and display a  page for each step of the full calibration.
//
// Arguments :  stepNum : int : The number of the calibration step
//
// Return     : void
//*********************************//
void LSScreen::fullCalibrationPrompt(int stepNum) {
  setupDisplay();

  switch (stepNum) {
    case 1:  // Corner 1
      _display.println("Hold");
      _display.println("joystick");
      _display.println("top left");
      break;
    case 2:  // Corner 2
      _display.println("Hold");
      _display.println("joystick");
      _display.println("top right");
      break;
    case 3:  // Corner 3
      _display.println("Hold");
      _display.println("joystick");
      _display.println("bottom");
      _display.println("right");
      break;
    case 4:  // Corner 4
      _display.println("Hold");
      _display.println("joystick");
      _display.println("bottom");
      _display.println("left");
      break;
    case 5:  // Center
      _display.println("Release,");
      _display.println("do not");
      _display.println("move");
      _display.println("joystick");
      break;
    case 6:  // Complete
      _display.println("Joystick");
      _display.println("calibrated");
      _display.display();

      delay(1500);
      if (_isActive) {
        mainMenu();
      } else {
        deactivateMenu();
      }
      break;
    case CONF_JOY_CALIB_ERROR:  // One or more values were too low to be replaced, flag error message
      _display.println("Movement");
      _display.println("too small.");
      _display.println("Returned");
      _display.println("to default.");
  }

  _display.display();
}

// ----- MORE SETTINGS MENUS ----- //

//*********************************//
// Function   : soundMenu
//
// Description: Format and display Sound Menu
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::soundMenu(void) {
  _currentMenu = SOUND_MENU;

  if (_soundMode != CONF_SOUND_MODE_OFF) {
    _soundMenuText[1] = "ON";
    _soundMenuText[2] = "Turn off";
  } else {
    _soundMenuText[1] = "OFF";
    _soundMenuText[2] = "Turn on";
  }

  _currentMenuLength = _soundMenuLen;
  _currentMenuText = _soundMenuText;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : sipPuffThreshMenu
//
// Description: Format and display Sip and Puff Menu Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::sipPuffThreshMenu(void) {
  _currentMenu = SIP_PUFF_MENU;

  _currentMenuLength = _sipPuffThreshMenuLen;
  _currentMenuText = _sipPuffThreshMenuText;
  _cursorStart = 0;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : adjustSipThreshMenu
//
// Description: Format and display Sip Threshold Adjustment Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::adjustSipThreshMenu(void) {
  _currentMenu = SIP_THRESH_MENU;
  _sipPressThresh = getSipPressureThreshold(false, false);

  _adjustSipThreshMenuText[0] = "Sip: " + String(_sipPressThresh) + " ";

  _currentMenuLength = _adjustSipThreshMenuLen;
  _currentMenuText = _adjustSipThreshMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : adjustPuffThreshMenu
//
// Description: Format and display Puff Threshold Adjustment Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::adjustPuffThreshMenu(void) {
  _currentMenu = PUFF_THRESH_MENU;
  _puffPressThresh = getPuffPressureThreshold(false, false);

  _adjustPuffThreshMenuText[0] = "Puff: " + String(_puffPressThresh) + " ";

  _currentMenuLength = _adjustPuffThreshMenuLen;
  _currentMenuText = _adjustPuffThreshMenuText;
  _cursorStart = 1;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : fullCalibrationConfirmPage
//
// Description: Format and display Full Calibration Confirmation Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::fullCalibrationConfirmPage(void) {
  setupDisplay();
  _display.println("Incorrect");
  _display.println("full calib.");
  _display.println("may cause");
  _display.println("drift.");
  _display.display();
  delay(3000);


  _currentMenu = FULL_CALIB_CONFIRM_PAGE;
  _currentMenuLength = _fullCalibrationConfirmLen;
  _currentMenuText = _fullCalibrationConfirmText;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : restartConfirmPage
//
// Description: Format and display Restart Confirmation Page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::restartConfirmPage(void) {
  _currentMenu = RESTART_PAGE;
  _currentMenuLength = _restartConfirmLen;
  _currentMenuText = _restartConfirmText;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}


//*********************************//
// Function   : factoryResetConfirm1Page
//
// Description: Format and display Factory Reset Confirmation Page 1
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::factoryResetConfirm1Page(void) {
  _currentMenu = FACTORY_RESET_PAGE;
  _currentMenuLength = _factoryResetConfirm1Len;
  _currentMenuText = _factoryResetConfirm1Text;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : factoryResetConfirm2Page
//
// Description: Format and display Factory Reset Confirmation Page 2
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::factoryResetConfirm2Page(void) {
  setupDisplay();
  _display.println("This will");
  _display.println("erase all");
  _display.println("custom");
  _display.println("settings");
  _display.display();
  delay(2000);

  _currentMenu = FACTORY_RESET_CONFIRM2_PAGE;
  _currentMenuLength = _factoryResetConfirm2Len;
  _currentMenuText = _factoryResetConfirm2Text;
  _cursorStart = 2;
  _currentSelection = 0;

  displayMenu();
}

//*********************************//
// Function   : testPage
//
// Description: Format and display a test page that shows the operating mode and number of times this function has been called
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::testPage(unsigned int usbConnectDelay) {
  setupDisplay();
  _testScreenAttempt++;

  switch (_operatingMode) {
    case CONF_OPERATING_MODE_MOUSE:
      switch (_communicationMode) {
        case CONF_COM_MODE_USB:
          _display.println("USB Mouse");
          break;
        case CONF_COM_MODE_BLE:
          _display.println("BT Mouse");
          break;
      }
      break;
    case CONF_OPERATING_MODE_GAMEPAD:
      _display.println("Gamepad");
      break;
    default:
      _display.println("Error");
  }
  _display.print("Attempt:");
  _display.println(_testScreenAttempt);

  _display.println(usbConnectDelay);
  _display.display();
}

//*********************************//
// Function   : noUsbPage
//
// Description: Format and display an error page when the USB has not successfully connected.
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::noUsbPage(void) {
  setupDisplay();

  _display.println("No USB");
  _display.println("Use menu");
  _display.println("to change");
  _display.println("modes");
  _display.display();
}

//*********************************//
// Function   : errorPageI2C
//
// Description: Format and display an error page
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::errorPageI2C() {
  setupDisplay();

  _display.println("ERROR: I2C");

  if (!g_joystickSensorConnected) {
    _display.println("JOYSTICK");
  } else {
    _display.println("");
  }

  if (!g_mouthpiecePressureSensorConnected) {
    _display.println("PRESSURE");
  } else {
    _display.println("");
  }

  if (!g_ambientPressureSensorConnected) {
    _display.println("AMBIENT");
  } else {
    _display.println("");
  }

  _display.display();
}

//*********************************//
// Function   : errorPageCable
//
// Description: Format and display an error page related to Interface Cable
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::errorPageCable() {
  // Joystick sensor, ambient pressure sensor and mouthpiece pressure sensor not detected
  // Likely a cable issue

  setupDisplay();

  _display.println("ERROR: ");
  _display.println("No joystick");
  _display.println("detected.");
  _display.println("Try cable.");

  _display.display();
}

//*********************************//
// Function   : warningUSBDebugOn
//
// Description: Format and display an error page when USB_DEBUG is set to 1 (on)
//
// Arguments :  void
//
// Return     : void
//*********************************//
void LSScreen::warningUSBDebugOn(void) {
  setupDisplay();

  _display.println("Warning:");
  _display.println("USB_DEBUG=1");
  _display.println("Set to 0");
  _display.println("for user.");
  _display.display();

  _screenStateTimerId = _screenStateTimer.setTimeout(CONF_SPLASH_SCREEN_DURATION, clearSplashScreen);
}

//*********************************//
// Function   : resetPage
//
// Description: Format and display an page showing the device is going to be reset
//
// Arguments :  void
//
// Return     : void
//*********************************//

void LSScreen::resetPage() {
  setupDisplay();

  _display.println("");
  _display.println("RESETTING...");
  _display.println("");
  //_display.println("");
  //_display.println("");

  _display.display();
}

//*********************************//
// Function   : connectionTimingPage
//
// Description: Format and display an page showing the timestamps related to connecting to USB
//
// Arguments :  void
//
// Return     : void
//*********************************//

void LSScreen::connectionTimingPage(unsigned long before, unsigned long after) {
  setupDisplay();
  _display.setTextSize(1);
  _display.println("Before beginComOpMode");
  _display.setTextSize(2);
  _display.println(before);

  _display.setTextSize(1);
  _display.println("After beginComOpMode");
  _display.setTextSize(2);
  _display.println(after);

  _display.display();
}

#endif
