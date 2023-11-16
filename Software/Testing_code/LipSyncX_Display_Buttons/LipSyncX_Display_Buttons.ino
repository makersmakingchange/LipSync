/*******************
  This is an exmaple of the OLED screen menu for the LipSyncX
  Last edited: November 14, 2023
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

#define MAIN_MENU   0
#define CALIB_MENU  1
#define MODE_MENU   2
#define CURSOR_SP_MENU   3
#define BLUETOOTH_MENU   4

//Calibration pages
#define CENTER_RESET_PAGE 11
#define FULL_CALIB_PAGE   12

#define MODE_MOUSE    0
#define MODE_GAMEPAD  1

#define SCROLL_DELAY_MILLIS   100

const int TEXT_ROWS = 4; //Probably a better way to do this with calculations based on text size and screen size

int currentMenu = 0;
int prevMenu = -1;
int currentSelection = 0;
int selectedLine; 

int mode = MODE_GAMEPAD;
int cursorSpeedLevel = 5;
bool bluetoothOn = false;

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

String mainMenuText[4] = {"Calibrate", "Mode", "Cursor speed", "Bluetooth"};
String calibMenuText[4] = {"Center reset", "Full Calibration", "... Back"};
String modeMenuText[4] = {"Mode: ", "New mode:", "<mode>", "... Back"};
String modeConfirmText[4] = {"Change", "mode?", "Confirm", "... Back"};
String cursorSpMenuText[4] = {"  ", "Increase", "Decrease", "... Back"};
String bluetoothMenuText[4] = {"Bluetooth:", "<>", "Turn <>", "... Back"};

const int mainMenuLen = 4;
const int calibMenuLen = 3;
const int modeMenuLen = 2;
const int cursorSpMenuLen = 3;
const int bluetoothMenuLen = 2;

//String mainMenuText = "Hello";

void setup() {
  pinMode(PIN_BUTTON_SEL, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NEXT, INPUT_PULLUP);

  Serial.begin(9600);
  
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
  display.setCursor(0, 16 * cursorStart);  // ADD VARIABLE for start row, so if selection items are only rows 3 and 4 it starts on row 3
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
    delay(500);                           // may need to remove this
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
  if (currentSelection >= currentMenuLength) {       // CHANGE THIS TO USE MENU LENGTH
    currentSelection = 0;
  }

  displayCursor();

}


void scrollLongText(void){
  //use selectedText to display

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
          case CALIB_MENU:
            calibMenu();
            break;
          case MODE_MENU:
            modeMenu();
            break;
          case CURSOR_SP_MENU:
            cursorSpeedMenu();
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
        switch (currentSelection){         
          case 0:             // change mode
             //confirm mode change?
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
              break;
            }
            
            if (mode == MODE_MOUSE){
              mode = MODE_GAMEPAD;
            } else if (mode == MODE_GAMEPAD){
              mode = MODE_MOUSE;
            }

            display.clearDisplay();
            display.setCursor(0,0);
            display.println("Resetting");
            display.println("device");
            display.display();

            delay(2000);
            
            //conduct mode change

            //remove this for final code, becuase device will reset
            readButtons();
            currentMenu = MAIN_MENU;
            mainMenu();
            break;
            
          case 1:             // ... Back
            currentMenu = MAIN_MENU;
            mainMenu();
            break;
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
            //cursorSpeedMenu();    //Refresh menu printout
            break;
          case 1:       //Decrease
            cursorSpeedLevel--;
            if (cursorSpeedLevel<1){
              cursorSpeedLevel=1;
            }
            display.setCursor(0,0);
            display.print("Speed: "); display.print(cursorSpeedLevel); display.println("  ");
            display.display();
            //cursorSpeedMenu();    //Refresh menu printout
            break;
          case 2:       //Back
            currentMenu = MAIN_MENU;
            mainMenu();
            break;
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
    }
  }
}

//********** MENUS **********//

void mainMenu(void) {
  currentMenu = MAIN_MENU;
  //if new menu selection
  if (prevMenu != currentMenu) {
    currentMenuLength = mainMenuLen;
    currentMenuText = mainMenuText;
    cursorStart = 0;

    displayMenu();
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
  if (mode == MODE_MOUSE){
      modeMenuText[0] = "Mode: MOUSE";
      modeMenuText[2] = "GAMEPAD";
  } else if (mode == MODE_GAMEPAD){
      modeMenuText[0] = "Mode: GAMEPAD";
      modeMenuText[2] = "MOUSE";
  }

  currentMenuLength = modeMenuLen;
  currentMenuText = modeMenuText;
  cursorStart = 2;

  displayMenu();
  
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

  currentMenu = MAIN_MENU;
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

  currentMenu = MAIN_MENU;
  mainMenu();
}

void centerReset(void){
  //function to actually perform center reset
}
