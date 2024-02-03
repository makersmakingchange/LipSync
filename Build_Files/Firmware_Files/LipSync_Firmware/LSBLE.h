/* 
* File: LSBLE.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.0.rc1 (26 January 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/
#include <bluefruit.h>

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

BLEDis bledis;
BLEHidAdafruit blehid;
bool needsInitialization = true;

class LSBLEMouse {
  private:
    void mouseReport(signed char b, signed char x, signed char y, signed char wheel = 0, signed char pan = 0);
  public:
    inline LSBLEMouse(void);
    inline void begin(const char* s = "LipSync");
    inline void end(void);
    inline void move(signed char x, signed char y);
    inline void moveAll(signed char x, signed char y, signed char wheel = 0, signed char pan = 0);
    inline void scroll(signed char wheel = 0);
    inline void pan(signed char pan = 0);
    inline void click(uint8_t b = MOUSE_LEFT);
    inline void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
    inline void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
    inline bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
    inline bool isConnected(void);
  protected:
    uint8_t _buttons;
    void buttons(uint8_t b);
};

typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} btKeyReport;


class LSBLEKeyboard : public Print
{
  private:
    btKeyReport _keyReport;
    void keyboardReport(btKeyReport* keys);
    uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};
  public:
    inline LSBLEKeyboard(void);
    inline void begin(const char *s = "LipSync");
    inline void end(void);
    inline size_t write(uint8_t k);
    inline size_t write(const uint8_t *buffer, size_t size);
    inline size_t press(uint8_t m, uint8_t k);
    inline size_t release(uint8_t m, uint8_t k);
    inline void releaseAll(void);
    inline bool isConnected(void);
};


void initializeBluefruit(const char* s) {
  Bluefruit.begin();
  Bluefruit.Periph.setConnInterval(9, 16);  // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
  Bluefruit.setTxPower(4);                  // Check bluefruit.h for supported values
  Bluefruit.setName(s);
  bledis.setManufacturer("MakersMakingChange");
  bledis.setModel("LipSync Mouse");
  bledis.begin();
  blehid.begin();
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // 20 ms - in unit of 0.625 ms (Interval:  fast mode = 20 ms, slow mode = 152.5 ms)
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds

  //Serial.println("Initializing Bluetooth");
  if (USB_DEBUG) { Serial.println("Initializing Bluetooth");}
}


/*****************************
     MOUSE SECTION
 *****************************/

LSBLEMouse::LSBLEMouse(void)
{

}

void LSBLEMouse::begin(const char* s)
{
  _buttons = 0;
  if (needsInitialization) {
    initializeBluefruit(s);
    needsInitialization = false;
  }
}


void LSBLEMouse::end(void)
{
}

void LSBLEMouse::mouseReport(int8_t b, int8_t x, int8_t y, int8_t wheel, int8_t pan)
{
  blehid.mouseReport(b, x, y, wheel, pan);
}

void LSBLEMouse::move(int8_t x, int8_t y)
{
  mouseReport(_buttons, x, y, 0, 0);
}
void LSBLEMouse::moveAll(int8_t x, int8_t y, int8_t wheel, int8_t pan)
{
  mouseReport(_buttons, x, y, wheel, pan);
}

void LSBLEMouse::scroll(int8_t wheel)
{
  mouseReport(_buttons, 0, 0, wheel, 0);
}

void LSBLEMouse::pan(int8_t pan)
{
  mouseReport(_buttons, 0, 0, 0, pan);
}

void LSBLEMouse::click(uint8_t b)
{
  _buttons = b;
  mouseReport(_buttons, 0, 0, 0, 0);
  _buttons = 0;
  mouseReport(_buttons, 0, 0, 0, 0);
}

void LSBLEMouse::buttons(uint8_t b)
{
  if (b != _buttons)
  {
    _buttons = b;
    mouseReport(_buttons, 0, 0, 0, 0);
  }
}

void LSBLEMouse::press(uint8_t b)
{
  buttons(_buttons | b);
}

void LSBLEMouse::release(uint8_t b)
{
  buttons(_buttons & ~b);
}

bool LSBLEMouse::isPressed(uint8_t b)
{
  if ((b & _buttons) > 0)
    return true;
  return false;
}

bool LSBLEMouse::isConnected(void) {
  return Bluefruit.connected();
}



/*****************************
     KEYBOARD SECTION
 *****************************/

LSBLEKeyboard::LSBLEKeyboard(void)
{
}

void LSBLEKeyboard::begin(const char* s)
{
  if (needsInitialization) {
    initializeBluefruit(s);
    needsInitialization = false;
  }
}

void LSBLEKeyboard::end(void)
{
}

void LSBLEKeyboard::keyboardReport(btKeyReport* keys)
{
  blehid.keyboardReport(keys->modifiers, keys->keys);
  delay(2);
}


size_t LSBLEKeyboard::press(uint8_t m, uint8_t k)
{
  uint8_t i;
  _keyReport.modifiers = m;

  //Add key if the it's not already present and if there is an empty spot.
  if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
      _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
      _keyReport.keys[4] != k && _keyReport.keys[5] != k) {

    for (i = 0; i < 6; i++) {
      if (_keyReport.keys[i] == 0x00) {
        _keyReport.keys[i] = k;
        break;
      }
    }
    if (i == 6) {
      setWriteError();
      return 0;
    }
  }
  keyboardReport(&_keyReport);
  return 1;
}


size_t LSBLEKeyboard::release(uint8_t m, uint8_t k)
{
  uint8_t i;
  _keyReport.modifiers = 0x00;
  //Check to see if the key is present and clear it if it exists.
  for (i = 0; i < 6; i++) {
    if (0 != k && _keyReport.keys[i] == k) {
      _keyReport.keys[i] = 0x00;
    }
  }
  keyboardReport(&_keyReport);
  return 1;
}

void LSBLEKeyboard::releaseAll(void)
{
  _keyReport.keys[0] = 0;
  _keyReport.keys[1] = 0;
  _keyReport.keys[2] = 0;
  _keyReport.keys[3] = 0;
  _keyReport.keys[4] = 0;
  _keyReport.keys[5] = 0;
  _keyReport.modifiers = 0;
  keyboardReport(&_keyReport);
}

size_t LSBLEKeyboard::write(uint8_t c)
{
  uint8_t keycode = 0;
  uint8_t modifier = 0;
  uint8_t uch = (uint8_t)c;

  if (_ascii2keycode[c][0]) {
    modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  }

  keycode = _ascii2keycode[uch][1];

  uint8_t p = press(modifier, keycode);  // Keydown
  release(modifier, keycode);            // Keyup
  return p;              // Return the result of press()
}

size_t LSBLEKeyboard::write(const uint8_t *buffer, size_t size) {
  size_t n = 0;
  while (size--) {
    if (*buffer != '\r') {
      if (write(*buffer)) {
        n++;
      } else {
        break;
      }
    }
    buffer++;
  }
  return n;
}

bool LSBLEKeyboard::isConnected(void) {
  return Bluefruit.connected();
}
