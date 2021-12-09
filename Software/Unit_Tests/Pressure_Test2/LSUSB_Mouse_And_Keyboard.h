#include "Adafruit_TinyUSB.h"
#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

#define RID_KEYBOARD 1
#define RID_MOUSE 2
  
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};

uint8_t const desc_hid_report[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) )
};


class LSMouse {
  private:
    void mouseReport(signed char b, signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
  public:
    inline LSMouse(void);
    inline void begin(void);
    inline void end(void);
	  inline void wakeup(void);
    inline void move(signed char x, signed char y);
    inline void moveAll(signed char x, signed char y, signed char wheel = 0,signed char pan = 0); 
    inline void scroll(signed char wheel = 0);
    inline void pan(signed char pan = 0); 
    inline void click(uint8_t b = MOUSE_LEFT);
    inline void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
    inline void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
    inline bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
	  inline bool isReady(void);
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


class LSKeyboard : public Print
{
	private:
		KeyReport _keyReport;
		void keyboardReport(KeyReport* keys);
	public:
		inline LSKeyboard(void);
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

/*****************************
 *   MOUSE SECTION
 *****************************/ 

LSMouse::LSMouse(void)
{
  _buttons = 0;
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));

}

void LSMouse::begin(void)
{
  this->usb_hid.begin();
  while( !USBDevice.mounted() ) delay(1);
}


void LSMouse::end(void)
{
}

void LSMouse::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void LSMouse::mouseReport(int8_t b, int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
	wakeup();
    while(!isReady()) delay(1);
    usb_hid.mouseReport(RID_MOUSE,b,x,y,wheel,pan);
}

void LSMouse::move(int8_t x, int8_t y) 
{
    mouseReport(_buttons, x, y, 0, 0);
}
void LSMouse::moveAll(int8_t x, int8_t y, int8_t wheel, int8_t pan) 
{
    mouseReport(_buttons, x, y, wheel, pan);
}

void LSMouse::scroll(int8_t wheel) 
{
    mouseReport(_buttons, 0, 0, wheel, 0);
}

void LSMouse::pan(int8_t pan) 
{
    mouseReport(_buttons, 0, 0, 0, pan);
}

void LSMouse::click(uint8_t b)
{
  _buttons = b;
  mouseReport(_buttons, 0, 0, 0, 0);
  _buttons = 0;
  mouseReport(_buttons, 0, 0, 0, 0);
}

void LSMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
	  _buttons = b;
	  mouseReport(_buttons, 0, 0, 0, 0);
	}
}

void LSMouse::press(uint8_t b) 
{
	buttons(_buttons | b);
}

void LSMouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool LSMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0) 
	  return true;
	return false;
}

bool LSMouse::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}

/*****************************
 *   KEYBOARD SECTION
 *****************************/ 

LSKeyboard::LSKeyboard(void) 
{
}

void LSKeyboard::begin(void)
{
	usb_hid.setPollInterval(2);
	usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
	usb_hid.begin();
	while( !USBDevice.mounted() ) delay(1);
}

void LSKeyboard::end(void)
{
}

void LSKeyboard::keyboardReport(KeyReport* keys)
{
	wakeup();
	while(!isReady()) delay(1);
	usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
	delay(2);
}

void LSKeyboard::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

size_t LSKeyboard::press(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = m;

	//Add key if the it's not already present and if there is an empty spot.
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


size_t LSKeyboard::release(uint8_t m, uint8_t k) 
{
	uint8_t i;
	_keyReport.modifiers = 0x00;
	//Check to see if the key is present and clear it if it exists.
	for (i=0; i<6; i++) {
	  if (0 != k && _keyReport.keys[i] == k) {
		_keyReport.keys[i] = 0x00;
	  }
	}
	keyboardReport(&_keyReport);
	return 1;
}

void LSKeyboard::releaseAll(void)
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
  
size_t LSKeyboard::write(uint8_t c)
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

size_t LSKeyboard::write(const uint8_t *buffer, size_t size) {
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

bool LSKeyboard::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}
