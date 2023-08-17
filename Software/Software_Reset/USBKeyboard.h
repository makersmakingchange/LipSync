#include "Adafruit_TinyUSB.h"

#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

#define RID_KEYBOARD 1
  
typedef struct ATTRIBUTE_PACKED {
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const keyboard_desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID keyboard_usb_hid(keyboard_desc_hid_report, sizeof(keyboard_desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);


class USBKeyboard {
  private:
    KeyReport _keyReport;
    inline void sendKeyboardReport(KeyReport* keys);
    uint8_t const _ascii2keycode[128][2] = {HID_ASCII_TO_KEYCODE};
  public:
    inline USBKeyboard(void);
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
    uint32_t startMillis;
    Adafruit_USBD_HID keyboard_usb_hid;
};


  USBKeyboard::USBKeyboard(void) { 
  }
  
  void USBKeyboard::begin(void)
  {
    this->keyboard_usb_hid.setPollInterval(2);
    this->keyboard_usb_hid.setReportDescriptor(keyboard_desc_hid_report, sizeof(keyboard_desc_hid_report));
    this->keyboard_usb_hid.setStringDescriptor("OpenAT Key");
    this->keyboard_usb_hid.begin();
    while( !USBDevice.mounted() ) delay(1);
  }
  
  void USBKeyboard::end(void)
  {
  }
  
  void USBKeyboard::sendKeyboardReport(KeyReport* keys)
  {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!keyboard_usb_hid.ready()) delay(1);
    keyboard_usb_hid.keyboardReport(RID_KEYBOARD,keys->modifiers,keys->keys);
    delay(2);
  }

void USBKeyboard::wakeup(void)
{
  if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

size_t USBKeyboard::press(uint8_t m, uint8_t k) 
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
  }
  sendKeyboardReport(&_keyReport);
  return 1;
}


size_t USBKeyboard::release(uint8_t m, uint8_t k) 
{
  uint8_t i;
  _keyReport.modifiers = 0x00;
  //Check to see if the key is present and clear it if it exists.
  for (i=0; i<6; i++) {
    if (0 != k && _keyReport.keys[i] == k) {
    _keyReport.keys[i] = 0x00;
    }
  }
  sendKeyboardReport(&_keyReport);
  return 1;
}

void USBKeyboard::releaseAll(void)
{
  _keyReport.keys[0] = 0;
  _keyReport.keys[1] = 0; 
  _keyReport.keys[2] = 0;
  _keyReport.keys[3] = 0; 
  _keyReport.keys[4] = 0;
  _keyReport.keys[5] = 0; 
  _keyReport.modifiers = 0;
  sendKeyboardReport(&_keyReport);
}
  
size_t USBKeyboard::write(uint8_t c)
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

size_t USBKeyboard::write(const uint8_t *buffer, size_t size) {
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

bool USBKeyboard::isReady(void)
{
  if (keyboard_usb_hid.ready()) 
    return true;
  return false;
}
