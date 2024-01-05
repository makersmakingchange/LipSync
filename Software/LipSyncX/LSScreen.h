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

//Mode pages
#define CONFIRM_MODE_CHANGE 31

//More Menus
#define SOUND_MENU        51
#define SIP_PUFF_MENU     52

#define MODE_MOUSE_USB  0
#define MODE_MOUSE_BT   1
#define MODE_GAMEPAD    2

#define SCROLL_DELAY_MILLIS   100

//Adafruit_SSD1306 _display(CONF_SCREEN_WIDTH, CONF_SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  int cursorSpeedLevel;
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

  String mainMenuText[5] = {"Exit Menu", "Calibrate", "Mode", "Cursor speed", "More"};
  String exitConfirmText[4] = {"Exit", "settings?", "Confirm", "... Back"};
  String calibMenuText[4] = {"Center reset", "Full Calibration", "... Back", " "};
  String modeMenuText[4] = {"MOUSE USB", "MOUSE BT", "GAMEPAD ", "... Back"};
  String modeConfirmText[4] = {"Change", "mode?", "Confirm", "... Back"};
  String cursorSpMenuText[4] = {"  ", "Increase", "Decrease", "... Back"};
  String moreMenuText[4] = {"Sound", "Sip & Puff", "... Back", "         "};
  String soundMenuText[4] = {"Sound:", "<>", "Turn <>", "... Back"};
  String sipPuffThreshMenuText[4] = {"Sip Threshold", "Puff Threshold", "... Back"};


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
  if (scrollOn){
    scrollLongText();
  }
}

void LSScreen::activateMenu() {
  is_active = true;
  //change inputs to menu inputs (Next and Select)
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

void LSScreen::selectMenuItem() {
  countMenuScroll = 0;
  switch (currentMenu) {
    case MAIN_MENU:
      currentMenu = currentSelection+1;
      switch (currentMenu) {
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
        if (currentSelection == 1){
          currentSelection = 0;
          mainMenu();
        } else {
          //some function to exit
          setupDisplay();
          _display.println("Exiting");
          _display.display();
          delay(2000);
      
          //mainMenu(); // ************************************************************** CHANGE THIS <>
          deactivateMenu();
        }
        break;
    case CALIB_MENU:
      if (currentSelection == 0){
        currentMenu = CENTER_RESET_PAGE;
        centerResetPage();
      } else if (currentSelection == 1){
        currentMenu = FULL_CALIB_PAGE;
        fullCalibrationPage();
      } else if (currentSelection == 2){
        currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case MODE_MENU:
      if (currentSelection < (modeMenuLen - 1)){
        // Confirm mode change
        tempMode = currentSelection;
        if (tempMode != mode){
          confirmModeChange();
        }
      } else if (currentSelection == (modeMenuLen-1)){
        mainMenu();
      }
      break;
    case CONFIRM_MODE_CHANGE:
      if (currentSelection == 1){
        currentSelection = 0;
        modeMenu();
      } else {
        changeMode();
      }
      break;
    case CURSOR_SP_MENU:
      switch (currentSelection){
        case 0:       //Increase
          increaseJoystickSpeed(true,false);                      // ********************************** HAS NOT BEEN TESTED <>
          cursorSpeedLevel = getJoystickSpeed(true,false);  
          cursorSpMenuText[0] = "Speed: " + String(cursorSpeedLevel) + " ";
          _display.setCursor(0,0);
          _display.print(cursorSpMenuText[0]);
          _display.display();
          break;
        case 1:       //Decrease
          decreaseJoystickSpeed(true,false);                      // ********************************** HAS NOT BEEN TESTED <>
          cursorSpeedLevel = getJoystickSpeed(true,false);  
          cursorSpMenuText[0] = "Speed: " + String(cursorSpeedLevel) + " ";
          _display.setCursor(0,0);
          _display.print(cursorSpMenuText[0]);
          _display.display();
          break;
        case 2:       //Back
          currentMenu = MAIN_MENU;
          mainMenu();
          break;
      }
      break;
    case MORE_MENU:
      if (currentSelection == 0){
        currentMenu = SOUND_MENU;
        soundMenu();
      } else if (currentSelection == 1){
        currentMenu = SIP_PUFF_MENU;
        sipPuffThreshMenu();
      } else if (currentSelection == 2){
        currentMenu = MAIN_MENU;
        mainMenu();
      }
      break;
    case SOUND_MENU:
       switch (currentSelection){
        case 0:
          soundOn = !soundOn;
          //do function for turning sound on/off
          soundMenu();
          break;
        case 1:
          currentMenu = MAIN_MENU;
          mainMenu();
          break;
        }
      case SIP_PUFF_MENU:
       switch (currentSelection){
        case 0:
          //add action
          break;
        case 1:
          //add action
          break;
        case 2:
          currentMenu = MAIN_MENU;
          mainMenu();
          break;
        }
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
    Serial.println("Long text");
    scrollOn = true;
    scrollPos = 12;
    delay(200);                           // may need to remove this
    scrollLongText();
  } else {
    scrollOn = false;
  }
}

/*
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
*/

void LSScreen::scrollLongText() {
  Serial.println("scroll function");
  int minPos = -12 * selectedText.length();
  
  _display.setTextSize(2);                                   // 2x scale text
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  _display.setTextWrap(false);
  
  if (millis() - scrollDelayTimer >= SCROLL_DELAY_MILLIS){
    Serial.println("Timer good");
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

//********** MENUS **********//

void LSScreen::mainMenu(void) {
  currentMenu = MAIN_MENU;
  
  //if new menu selection
  //if (prevMenu != currentMenu) {
    currentMenuLength = mainMenuLen;
    currentMenuText = mainMenuText;
    cursorStart = 0;
    currentSelection = 0;

    displayMenu();
  //}
}

void LSScreen::exitConfirmMenu(){
  prevMenu = currentMenu;
  currentMenu = EXIT_MENU;
  currentMenuText = exitConfirmText;
  currentMenuLength = 2;
  cursorStart = 2;
  currentSelection = 0;
  displayMenu();

/*
  while (!buttonSelPressed){
    //readButtons(); // ************************************************************** CHANGE THIS <>
    nextMenuItem();
  }

  while (buttonSelPressed){
    //readButtons(); // wait until Sel button is released // ************************************************************** CHANGE THIS <>
  }
  
  if (currentSelection == 1){
    currentSelection = 0;
    mainMenu();
  } else {
    //some function to exit
    setupDisplay();
    _display.println("Exiting");
    _display.display();
    delay(2000);

    //mainMenu(); // ************************************************************** CHANGE THIS <>
    deactivateMenu();
  }
  */
}

void LSScreen::calibMenu(void) {
  currentMenu = CALIB_MENU;
  if (prevMenu != currentMenu) {
    currentMenuLength = calibMenuLen;
    currentMenuText = calibMenuText;
    cursorStart = 0;
    currentSelection = 0;
  
    displayMenu();
  }

}

void LSScreen::modeMenu(void) {
  currentMenu = MODE_MENU;

  currentMenuLength = modeMenuLen;
  currentMenuText = modeMenuText;
  cursorStart = 0;
  currentSelection = 0;

  displayMenu();

  _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' coloured text
  if (mode == MODE_MOUSE_USB){
      _display.setCursor(12, 0);
      _display.print(modeMenuText[MODE_MOUSE_USB]);
  } else if (mode == MODE_MOUSE_BT){
      _display.setCursor(12, 16);
      //display.print(" MOUSE BLUETOOTH ");
      _display.print(modeMenuText[MODE_MOUSE_BT]);
  } else if (mode == MODE_GAMEPAD){
      _display.setCursor(12, 16*2);
      //display.print(" GAMEPAD ");
      _display.print(modeMenuText[MODE_GAMEPAD]);
  }

  _display.display();
  _display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Reset text colour to white on black
  
}
//
void LSScreen::confirmModeChange() {
  currentMenu = CONFIRM_MODE_CHANGE;
  currentMenuText = modeConfirmText;
  currentMenuLength = 2;
  cursorStart = 2;
  currentSelection = 0;
  displayMenu();
/*
  while (!buttonSelPressed){
    //readButtons();      // ************************************************************** CHANGE THIS <>
    nextMenuItem();
  }
  
  if (currentSelection == 1){
    currentSelection = 0;
    buttonSelPressed=false;
    modeMenu();
  } else {
    changeMode();
  }
  */
}

void LSScreen::changeMode(){
  mode = tempMode;

  /*                  // ************************************************************** CHANGE THIS <>
  switch (mode){
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
  */                  // ************************************************************** CHANGE THIS ^

  setupDisplay();
  _display.println("Resetting");
  _display.println("device");
  _display.display();

  delay(2000);
  
  //conduct mode change
  //save mode into flash
  //user feedback showing current mode    // ************************************************************** CHANGE THIS <>
  
  //software reset

  //remove this for final code, because device will reset
  //readButtons();                            // ************************************************************** CHANGE THIS <>
  currentMenu = MAIN_MENU;
  mainMenu();

  //software reset
}

void LSScreen::cursorSpeedMenu(void) { 
  currentMenu = CURSOR_SP_MENU;
  cursorSpeedLevel = getJoystickSpeed(true,false); 
  
  cursorSpMenuText[0] = "Speed: " + String(cursorSpeedLevel);
  
  currentMenuLength = cursorSpMenuLen;
  currentMenuText = cursorSpMenuText;
  cursorStart = 1;
  currentSelection = 0;

  displayMenu();

}

void LSScreen::moreMenu(){
  currentMenu = MORE_MENU;

  currentMenuLength = moreMenuLen;
  currentMenuText = moreMenuText;
  cursorStart = 0;
  currentSelection = 0;

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
  //Add prompts and add function for calibration // ************************************************************** CHANGE THIS <>

  delay(1000);

  mainMenu();
}

// ----- MORE SETTINGS MENUS ----- //

void LSScreen::soundMenu(){
  currentMenu = SOUND_MENU;
  
  if (soundOn) {
    soundMenuText[1] = "ON";
    soundMenuText[2] = "Turn off";
  } else {
    soundMenuText[1] = "OFF";
    soundMenuText[2] = "Turn on";
  }

  currentMenuLength = soundMenuLen;
  currentMenuText = soundMenuText;
  cursorStart = 2;
  currentSelection = 0;

  displayMenu();
}

void LSScreen::sipPuffThreshMenu(){
  currentMenu = SIP_PUFF_MENU;

  currentMenuLength = sipPuffThreshMenuLen;
  currentMenuText = sipPuffThreshMenuText;
  cursorStart = 0;
  currentSelection = 0;

  //Add sip/puff  thresh adjustment function // ************************************************************** CHANGE THIS <>

  displayMenu();
}


#endif
