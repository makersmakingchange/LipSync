/*******************
  This is an exmaple of the OLED screen menu for the LipSyncX
  Last edited: November 23, 2023
 *******************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PIN_BUTTON_SEL    3
#define PIN_BUTTON_NEXT   10

#define PIN_LED_MOUSE   7
#define PIN_LED_GAMEPAD 9

#define MAIN_MENU   0
#define EXIT_MENU   1
#define CALIB_MENU  2
#define MODE_MENU   3
#define CURSOR_SP_MENU   4
#define ADVANCED_MENU    5
#define BLUETOOTH_MENU   6


//Calibration pages
#define CENTER_RESET_PAGE 11
#define FULL_CALIB_PAGE   12

//Advanced Menus
#define SOUND_MENU        51
#define SIP_PUFF_MENU     52

#define MODE_MOUSE_USB  0
#define MODE_MOUSE_BT   1
#define MODE_GAMEPAD    2

#define SCROLL_DELAY_MILLIS   100

const int TEXT_ROWS = 4; //Probably a better way to do this with calculations based on text size and screen size

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

int currentMenuLength;
String *currentMenuText;
String selectedText;

String mainMenuText[5] = {"Exit Menu", "Calibrate", "Mode", "Cursor speed", "Advanced"};
String exitConfirmText[4] = {"Exit", "settings?", "Confirm", "... Back"};
String calibMenuText[4] = {"Center reset", "Full Calibration", "... Back"};
String modeMenuText[4] = {"MOUSE USB", "MOUSE BT", "GAMEPAD ", "... Back"};
String modeConfirmText[4] = {"Change", "mode?", "Confirm", "... Back"};
String cursorSpMenuText[4] = {"  ", "Increase", "Decrease", "... Back"};
String bluetoothMenuText[4] = {"Bluetooth:", "<>", "Turn <>", "... Back"};
String advancedMenuText[2] = {"Sound", "Sip & Puff"};
String soundMenuText[4] = {"Sound:", "<>", "Turn <>", "... Back"};

// Number of selectable options in each menu
const int mainMenuLen = 5;
const int exitConfirmLen = 2;
const int calibMenuLen = 3;
const int modeMenuLen = 4;
const int cursorSpMenuLen = 3;
const int bluetoothMenuLen = 2;
const int advancedMenuLen = 2;
const int soundMenuLen = 2;

void setup() {
  pinMode(PIN_BUTTON_SEL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);

  pinMode(PIN_LED_MOUSE, OUTPUT);
  pinMode(PIN_LED_GAMEPAD, OUTPUT);
  digitalWrite(PIN_LED_MOUSE, LOW);
  digitalWrite(PIN_LED_GAMEPAD, LOW);

  Serial.begin(115200);

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
  
  delay(500); // Start up delay so screen is ready to write to

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.display();

  //Start on main menu
  currentMenu = MAIN_MENU;
  mainMenu();

}

void loop() {
  // put your main code here, to run repeatedly:

  // read switches
  readButtons();

  buttonActions();

  prevMenu = currentMenu;

  if (scrollOn){
    scrollLongText();
  }

}

void displayMenu() {

  display.clearDisplay();

  display.setTextSize(2);                                   // 2x scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  display.setTextWrap(false);
  display.setCursor(0, 0);

  for (int i = 0; i < TEXT_ROWS; i++) {
    if (i >= cursorStart){
      display.print(" "), display.println(currentMenuText[i]);
    } else {
      display.println(currentMenuText[i]);
    }
  }

  display.display();

  currentSelection = 0;
  displayCursor();
}

void displayCursor(void) {

  display.setTextSize(2);                                   // 2x scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background

  // Show cursor on text line of selection index, erase previous cursor
  display.setCursor(0, 16 * cursorStart);  
  for (int i = 0; i < currentMenuLength; i++) {    
    if (i == currentSelection) {
      display.println(">");
    } else {
      display.println(" ");
    }
  }

  display.display();

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

void nextSelection(void) {
  if (scrollOn){
    display.setCursor(0, selectedLine *16);
    display.print("                                   ");
    display.setCursor(12, selectedLine *16);
    display.print(selectedText);
  }

  currentSelection++;
  if (currentSelection >= currentMenuLength) {   
    currentSelection = 0;
  }

  displayCursor();

}


void scrollLongText(void){
  int minPos = -12 * selectedText.length();
  
  display.setTextSize(2);                                   // 2x scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  display.setTextWrap(false);
  
  if (millis() - scrollDelayTimer >= SCROLL_DELAY_MILLIS){
    scrollDelayTimer = millis();
    
    //Clear previous text by writing over it with blank text
    display.setCursor(0, selectedLine *16);
    display.print("                                   ");

    //Display text in new position to simulate scrolling
    display.setCursor(scrollPos, selectedLine *16);
    display.print(selectedText);

    display.setCursor(0, selectedLine *16);
    display.print(">");
    display.display();
    //displayCursor();
    scrollPos = scrollPos-4;
    if (scrollPos < minPos) scrollPos = display.width();
  }
  
}

void readButtons(void){
  buttonSelPrevPressed = buttonSelPressed;
  buttonNextPrevPressed = buttonNextPressed;

  buttonSelPressed = !digitalRead(PIN_BUTTON_SEL);
  buttonNextPressed = !digitalRead(PIN_BUTTON_NEXT);
}

void buttonActions(void){
  inputNext();
  inputSelect();
}

void inputNext(void){
    // Move this to general loop? 
  if (buttonNextPressed){
    //start timer to determine long press
  } else if (buttonNextPrevPressed){          // when button is released
    nextSelection();
  }
}

void inputSelect(void){
  if (buttonSelPressed){
    //some action, long press timer?
  } else if (buttonSelPrevPressed){
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
          case ADVANCED_MENU:
            advancedMenu();
            break;
          case BLUETOOTH_MENU:
            bluetoothMenu();
            break;
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
      case CURSOR_SP_MENU:
        switch (currentSelection){
          case 0:       //Increase
            cursorSpeedLevel++;
            if (cursorSpeedLevel>10){
              cursorSpeedLevel=10;
            }
            display.setCursor(0,0);
            display.print("Speed: "); display.print(cursorSpeedLevel); display.println("  ");
            display.display();
            break;
          case 1:       //Decrease
            cursorSpeedLevel--;
            if (cursorSpeedLevel<1){
              cursorSpeedLevel=1;
            }
            display.setCursor(0,0);
            display.print("Speed: "); display.print(cursorSpeedLevel); display.println("  ");
            display.display();
            break;
          case 2:       //Back
            currentMenu = MAIN_MENU;
            mainMenu();
            break;
        }
        break;
      case ADVANCED_MENU:
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
      case BLUETOOTH_MENU:
        switch (currentSelection){
          case 0:
            bluetoothOn = !bluetoothOn;
            //do function for turning bluetooth on/off
            bluetoothMenu();
            break;
          case 1:
            currentMenu = MAIN_MENU;
            mainMenu();
            break;
        }
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
    }
  }
}

//********** MENUS **********//

void mainMenu(void) {
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

void exitConfirmMenu(){
  prevMenu = currentMenu;
  currentMenu = EXIT_MENU;
  currentMenuText = exitConfirmText;
  currentMenuLength = 2;
  cursorStart = 2;
  currentSelection = 0;
  displayMenu();

  while (!buttonSelPressed){
    readButtons();
    inputNext();
  }

  while (buttonSelPressed){
    readButtons(); // wait until Sel button is released
  }
  
  if (currentSelection == 1){
    currentSelection = 0;
    mainMenu();
  } else {
    //some function to exit
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Exiting");
    display.display();
    delay(2000);

    mainMenu();
  }
}

void calibMenu(void) {
  currentMenu = CALIB_MENU;
  if (prevMenu != currentMenu) {
    currentMenuLength = calibMenuLen;
    currentMenuText = calibMenuText;
    cursorStart = 0;
    currentSelection = 0;
  
    displayMenu();
  }

}

void modeMenu(void) {
  currentMenu = MODE_MENU;

  currentMenuLength = modeMenuLen;
  currentMenuText = modeMenuText;
  cursorStart = 0;

  displayMenu();

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' coloured text
  if (mode == MODE_MOUSE_USB){
      display.setCursor(12, 0);
      display.print(modeMenuText[MODE_MOUSE_USB]);
  } else if (mode == MODE_MOUSE_BT){
      display.setCursor(12, 16);
      //display.print(" MOUSE BLUETOOTH ");
      display.print(modeMenuText[MODE_MOUSE_BT]);
  } else if (mode == MODE_GAMEPAD){
      display.setCursor(12, 16*2);
      //display.print(" GAMEPAD ");
      display.print(modeMenuText[MODE_GAMEPAD]);
  }

  display.display();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Reset text colour to white on black
  
}

void confirmModeChange() {
  currentMenuText = modeConfirmText;
  currentMenuLength = 2;
  cursorStart = 2;
  currentSelection = 0;
  displayMenu();

  while (!buttonSelPressed){
    readButtons();
    inputNext();
  }
  
  if (currentSelection == 1){
    currentSelection = 0;
    buttonSelPressed=false;
    modeMenu();
  } else {
    changeMode();
  }
}

void changeMode(){
  mode = tempMode;
  
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

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Resetting");
  display.println("device");
  display.display();

  delay(2000);
  
  //conduct mode change
  //save mode into flash

  //remove this for final code, becuase device will reset
  readButtons();
  currentMenu = MAIN_MENU;
  mainMenu();

  //software reset
}

void cursorSpeedMenu(void) { 
  currentMenu = CURSOR_SP_MENU;
  
  cursorSpMenuText[0] = "Speed: " + String(cursorSpeedLevel);
  
  currentMenuLength = cursorSpMenuLen;
  currentMenuText = cursorSpMenuText;
  cursorStart = 1;
  currentSelection = 0;

  displayMenu();

}

void bluetoothMenu(void) { 
  currentMenu = BLUETOOTH_MENU;
  
  if (bluetoothOn) {
    bluetoothMenuText[1] = "ON";
    bluetoothMenuText[2] = "Turn off";
  } else {
    bluetoothMenuText[1] = "OFF";
    bluetoothMenuText[2] = "Turn on";
  }

  currentMenuLength = bluetoothMenuLen;
  currentMenuText = bluetoothMenuText;
  cursorStart = 2;

  displayMenu();

}

void advancedMenu(){
  currentMenu = ADVANCED_MENU;

    if (prevMenu != currentMenu) {
    currentMenuLength = advancedMenuLen;
    currentMenuText = advancedMenuText;
    cursorStart = 0;

    displayMenu();
  }
}

// ----- CALIBRATION PAGES ----- //
void centerResetPage(void){
  display.clearDisplay();
  display.setTextSize(2);                                   // 2x scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  //display.setTextWrap(true);
  display.stopscroll();
  display.setCursor(0,0);

  display.println("Center");
  display.println("reset, do");
  display.println("not move");
  display.println("joystick");

  display.display();

  //function to actually do center reset
  centerReset();

  delay(4000);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Center");
  display.println("reset");
  display.println("complete");

  display.display();

  delay(2000);

  mainMenu();
  
}

void fullCalibrationPage(void){
  display.clearDisplay();
  display.setTextSize(2);                                   // 2x scale text
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);       // Draw white text on solid black background
  display.stopscroll();
  display.setCursor(0,0);

  display.println("Full");
  display.println("Calibration");

  display.display();

  delay(1000);

  mainMenu();
}

void centerReset(void){
  //function to actually perform center reset
}

// ----- ADVANCED SETTINGS MENUS ----- //

void soundMenu(){
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

  displayMenu();
}
