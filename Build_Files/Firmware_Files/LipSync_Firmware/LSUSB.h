/* 
* File: LSUSB.h
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

#include "Adafruit_TinyUSB.h"
#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

#define RID_KEYBOARD 1
#define RID_MOUSE 2
  
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)
#define MOUSE_DESCRIPTOR "LipSync Mouse" // TODO 2025-Feb-21 Unused due to Tiny USB library hang

#define GAMEPAD_DESCRIPTOR "LipSync Gamepad" // TODO 2025-Feb-21 Unused due to Tiny USB library hang

extern unsigned int g_usbAttempt;  // global variable to keep track of USB connection attempts
extern void usbCheckConnection(void);


// https://github.com/hathach/tinyusb/blob/master/examples/device/hid_generic_inout/src/usb_descriptors.c

//uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};

uint8_t const mouse_desc_hid_report[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) )
};


class LSUSBMouse {
  private:
    void mouseReport(signed char b, signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
  public:
    inline LSUSBMouse(void);
    inline void begin(void);
    inline void end(void);
	  inline void wakeup(void);
    inline void move(signed char x, signed char y);
    inline void moveAll(signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
    inline void scroll(signed char wheel = 0);
    inline void pan(signed char pan = 0); 
    inline void click(uint8_t b = MOUSE_LEFT);
    inline void press(uint8_t b = MOUSE_LEFT);     // press LEFT by default
    inline void release(uint8_t b = MOUSE_LEFT);   // release LEFT by default
    inline bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
	  inline bool isReady(void);
    inline bool isConnected(void);
    bool usbRetrying = false;
    bool showTestPage = false;
    bool timedOut = false;
  protected:
    uint8_t _buttons;
    void buttons(uint8_t b);
    Adafruit_USBD_HID usb_hid;
};

typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;


class LSUSBKeyboard : public Print
{
	private:
		KeyReport _keyReport;
		void keyboardReport(KeyReport* keys);
    uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};
	public:
		inline LSUSBKeyboard(void);
		inline void begin(void);
		inline void end(void);
		inline void wakeup(void);
		inline size_t write(uint8_t k);
		inline size_t write(const uint8_t *buffer, size_t size);
		inline size_t press(uint8_t m, uint8_t k);
		inline size_t release(uint8_t m, uint8_t k);
		inline void releaseAll(void);
		inline bool isReady(void);
  protected:
    Adafruit_USBD_HID usb_hid;
};

typedef struct ATTRIBUTE_PACKED {
    uint8_t buttons;
    uint8_t	xAxis;
    uint8_t	yAxis;
} HID_GamepadReport_Data_t;

// HID report descriptor for XAC Compatible gamepad with 8 buttons and 2 axis joystick 
uint8_t const gamepad_desc_hid_report[] =
{
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x05,        // Usage (Gamepad)
    0xA1, 0x01,        // Collection (Application)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x35, 0x00,        //   Physical Minimum (0)
    0x45, 0x01,        //   Physical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8)
    0x05, 0x09,        //   Usage Page (Button)
    0x19, 0x01,        //   Usage Minimum (0x01)
    0x29, 0x08,        //   Usage Maximum (0x08)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
    0x26, 0xFF, 0x00,  //   Logical Maximum (255)
    0x46, 0xFF, 0x00,  //   Physical Maximum (255)
    0x09, 0x30,        //   Usage (X)
    0x09, 0x31,        //   Usage (Y)
    0x75, 0x08,        //   Report Size (8)
    0x95, 0x02,        //   Report Count (2)
    0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};


class LSUSBGamepad {
  public:
    inline LSUSBGamepad(void);
    inline void begin(void);
    inline void end(void);
	  inline void wakeup(void);
    inline void send(void);
    inline bool GamepadReport(void* data, size_t length) {
        return this->usb_hid.sendReport(0, data, (uint8_t)length);
    };
    inline void write(void);
    inline void write(void *report);
    inline void press(uint8_t b);
    inline void release(uint8_t b);
    inline void releaseAll(void);
    inline void buttons(uint8_t b);
    inline void xAxis(uint8_t a);
    inline void yAxis(uint8_t a);
    inline void move(uint8_t x,uint8_t y);
    inline bool isReady(void);
    inline bool isConnected(void);
    bool usbRetrying = false;
  protected:
    HID_GamepadReport_Data_t _report;
    uint32_t startMillis;
    Adafruit_USBD_HID usb_hid;
};




/*****************************
 *   MOUSE SECTION
 *****************************/ 

LSUSBMouse::LSUSBMouse(void)
{

}

void LSUSBMouse::begin(void)
{
  _buttons = 0;
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(mouse_desc_hid_report, sizeof(mouse_desc_hid_report));
  //this->usb_hid.setStringDescriptor(MOUSE_DESCRIPTOR); // TODO this causes TinyUSB to crash 2025-Jan-20
  this->usb_hid.begin();
  if (USB_DEBUG) { Serial.println("USBDEBUG: Initializing USB HID Mouse");  }

  unsigned long timerHidTimeoutBegin = millis();
  unsigned long usbTimeoutMillis;

  g_usbAttempt++;
    
  if (usbRetrying) {
    usbTimeoutMillis = 200;                     // when reattemping to connect the USB, uses a smaller value to not freeze up the code for long but will keep retrying 
  } else {
    usbTimeoutMillis = CONF_USB_HID_TIMEOUT;    // the first time the usb tried to connect, use the default timeout
  }
  
  while( !USBDevice.mounted() ) { //  Wait for USB device to mount
    delay(1);

    if ((millis() - timerHidTimeoutBegin) > usbTimeoutMillis) { 
      usbRetrying = true;
      break;
    } 
  }

  if (USBDevice.mounted()) { // If USB device mounts, send blank report
    usbRetrying = false;
    g_usbAttempt = 0;
    move(0,0);
  }
  
}


bool LSUSBMouse::isConnected(void) {
  return this->usb_hid.ready() && !USBDevice.suspended();
}


void LSUSBMouse::end(void)
{
}

void LSUSBMouse::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void LSUSBMouse::mouseReport(int8_t b, int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
	wakeup();
  unsigned long timerTimeoutBegin = millis();
    
    while(!isReady() && !usbRetrying && !timedOut) {
      delay(1);
      if ((millis() - timerTimeoutBegin) > CONF_USB_HID_TIMEOUT){
        timedOut = true;
        showTestPage=true;
        usbCheckConnection();
        break;
      }
    }
    if (isReady()){
      usb_hid.mouseReport(RID_MOUSE,b,x,y,wheel,pan);
      timedOut = false;
    }
    
}

void LSUSBMouse::move(int8_t x, int8_t y) 
{
    mouseReport(_buttons, x, y, 0, 0);
}
void LSUSBMouse::moveAll(int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
    mouseReport(_buttons, x, y, wheel, pan);
}

void LSUSBMouse::scroll(int8_t wheel) 
{
    mouseReport(_buttons, 0, 0, wheel, 0);
}

void LSUSBMouse::pan(int8_t pan) 
{
    mouseReport(_buttons, 0, 0, 0, pan);
}

void LSUSBMouse::click(uint8_t b)
{
  _buttons = b;
  mouseReport(_buttons, 0, 0, 0, 0);
  _buttons = 0;
  mouseReport(_buttons, 0, 0, 0, 0);
}

void LSUSBMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
	  _buttons = b;
	  mouseReport(_buttons, 0, 0, 0, 0);
	}
}

void LSUSBMouse::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void LSUSBMouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool LSUSBMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
	  return true;
	return false;
}

bool LSUSBMouse::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}

/*****************************
 *   KEYBOARD SECTION
 *****************************/ 

LSUSBKeyboard::LSUSBKeyboard(void) 
{
}

void LSUSBKeyboard::begin(void)
{
	usb_hid.setPollInterval(2);
	usb_hid.setReportDescriptor(mouse_desc_hid_report, sizeof(mouse_desc_hid_report));
	usb_hid.begin();
	while( !USBDevice.mounted() ) delay(1);
}

void LSUSBKeyboard::end(void)
{
}

void LSUSBKeyboard::keyboardReport(KeyReport* keys)
{
	wakeup();
	while(!isReady()) delay(1);
	usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
	delay(2);
}

void LSUSBKeyboard::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

size_t LSUSBKeyboard::press(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = m;

	// Add key if the it's not already present and if there is an empty spot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k && 
	  _keyReport.keys[2] != k && _keyReport.keys[3] != k &&
	  _keyReport.keys[4] != k && _keyReport.keys[5] != k) {
	  
	  for (i=0; i<6; i++) {
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


size_t LSUSBKeyboard::release(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = 0x00;
	// Check to see if the key is present and clear it if it exists.
	for (i=0; i<6; i++) {
	  if (0 != k && _keyReport.keys[i] == k) {
		_keyReport.keys[i] = 0x00;
	  }
	}
	keyboardReport(&_keyReport);
	return 1;
}

void LSUSBKeyboard::releaseAll(void)
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
  
size_t LSUSBKeyboard::write(uint8_t c)
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

size_t LSUSBKeyboard::write(const uint8_t *buffer, size_t size) {
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

bool LSUSBKeyboard::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}


/*****************************
 *   GAMEPAD SECTION
 *****************************/ 
LSUSBGamepad::LSUSBGamepad(void)
{
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(gamepad_desc_hid_report, sizeof(gamepad_desc_hid_report));
  //this->usb_hid.setStringDescriptor(GAMEPAD_DESCRIPTOR); // TODO this causes TinyUSB to crash 2025-Jan-20
}

void LSUSBGamepad::begin(void)
{
  this->usb_hid.begin();

  unsigned long timerHidTimeoutBegin = millis();
  unsigned long usbTimeoutMillis;

  g_usbAttempt++;
    
  if (usbRetrying) {
    usbTimeoutMillis = 200;                     // when reattemping to connect the USB, uses a smaller value to not freeze up the code for long but will keep retrying 
  } else {
    usbTimeoutMillis = CONF_USB_HID_TIMEOUT;    // the first time the usb tried to connect, use the default timeout
  }
  
  while( !USBDevice.mounted() ) {
    delay(1);

    if ((millis() - timerHidTimeoutBegin) > usbTimeoutMillis){
      usbRetrying = true;
      break;
    } 
  }

  if (USBDevice.mounted()) {
    usbRetrying = false;
    g_usbAttempt = 0;
  }
  
  // Release all the buttons and center joystick
  end();
  startMillis = millis();
}

void LSUSBGamepad::send(void)
{
  if (startMillis != millis()) {
  wakeup();
  unsigned long timerTimeoutBegin = millis();

  while(!isReady() && !usbRetrying) {
    delay(1);
    if ((millis() - timerTimeoutBegin) > CONF_USB_HID_TIMEOUT){
      break;
    }
  }
  GamepadReport(&_report, sizeof(_report));
    startMillis = millis();
  }
}

void LSUSBGamepad::end(void)
{
  _report.buttons = 0;
  _report.xAxis = 128;
  _report.yAxis = 128;
  GamepadReport(&_report, sizeof(_report));
}

void LSUSBGamepad::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void LSUSBGamepad::write(void)
{
  wakeup();
  unsigned long timerTimeoutBegin = millis();

  while(!isReady() && !usbRetrying) {
    delay(1);
    if ((millis() - timerTimeoutBegin) > CONF_USB_HID_TIMEOUT){
      break;
    }
  }
  GamepadReport(&_report, sizeof(_report));
}

void LSUSBGamepad::write(void *report)
{
  wakeup();
  unsigned long timerTimeoutBegin = millis();

  while(!isReady() && !usbRetrying) {
    delay(1);
    if ((millis() - timerTimeoutBegin) > CONF_USB_HID_TIMEOUT){
      break;
    }
  }
  memcpy(&_report, report, sizeof(_report));
  GamepadReport(&_report, sizeof(_report));
}

void LSUSBGamepad::press(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons |= (uint8_t)1 << b;
}


void LSUSBGamepad::release(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons &= ~((uint8_t)1 << b);
}


void LSUSBGamepad::releaseAll(void)
{
  _report.buttons = 0;
}

void LSUSBGamepad::buttons(uint8_t b)
{
  _report.buttons = b;
}


void LSUSBGamepad::xAxis(uint8_t a)
{
  _report.xAxis = 128 + a;
}


void LSUSBGamepad::yAxis(uint8_t a)
{
  _report.yAxis = 128 + a;
}

void LSUSBGamepad::move(uint8_t x,uint8_t y)
{
  _report.xAxis = 128 + x;
  _report.yAxis = 128 + y;
}

bool LSUSBGamepad::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}

bool LSUSBGamepad::isConnected(void) {
  	if (usb_hid.ready()) { 
	    return true;
    } else {
      return false;
    }	
}
