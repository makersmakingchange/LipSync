#include "Adafruit_TinyUSB.h"

#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE)

#define RID_MOUSE 2

typedef struct ATTRIBUTE_PACKED {
  uint8_t buttons;
  int8_t x;
  int8_t y;
  int8_t wheel;
} miceReport;


// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const mouse_desc_hid_report[] =
{
    TUD_HID_REPORT_DESC_MOUSE( HID_REPORT_ID(RID_MOUSE) )
};

Adafruit_USBD_HID mouse_usb_hid(mouse_desc_hid_report, sizeof(mouse_desc_hid_report), HID_ITF_PROTOCOL_MOUSE, 2, false);


class USBMouse {
  public:
    inline USBMouse(void);
    inline void begin(void);
    inline void end(void);
    inline void click(uint8_t b = MOUSE_LEFT);
    inline void move(signed char x, signed char y, signed char wheel = 0); 
    inline void press(uint8_t b);
    inline void release(uint8_t b);
    inline bool isPressed(uint8_t b);
    inline void sendMouseReport(miceReport* mouse);
  protected:
    miceReport _report; 
    uint32_t startMillis;
    Adafruit_USBD_HID mouse_usb_hid;
};



USBMouse::USBMouse(void) {
}

void USBMouse::begin(void) {
  _report.buttons = 0;
  this->mouse_usb_hid.setPollInterval(2);
  this->mouse_usb_hid.setReportDescriptor(mouse_desc_hid_report, sizeof(mouse_desc_hid_report));
  this->mouse_usb_hid.setStringDescriptor("OpenAT Mouse");
  this->mouse_usb_hid.begin();
  while( !USBDevice.mounted() ) delay(1);
}

  void USBMouse::sendMouseReport(miceReport* mouse)
  {
    if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
    while(!mouse_usb_hid.ready()) delay(1);
    mouse_usb_hid.mouseReport(RID_MOUSE,mouse->buttons,mouse->x,mouse->y,mouse->wheel,0);
    delay(2);
  }

void USBMouse::move(int8_t x, int8_t y, int8_t wheel) {
  if ( USBDevice.suspended() )  {
    USBDevice.remoteWakeup();
  }
  while(!mouse_usb_hid.ready()) delay(1);
  _report.buttons = _report.buttons;
  _report.x = x;
  _report.y = y;
  _report.wheel = wheel;
  sendMouseReport(&_report);
}

void USBMouse::end(void) 
{
}

void USBMouse::click(uint8_t b)
{
  _report.buttons = b;
  _report.x = 0;
  _report.y = 0;
  _report.wheel = 0;
  sendMouseReport(&_report);
  _report.buttons = 0;
  sendMouseReport(&_report);
}


void USBMouse::press(uint8_t b) 
{
  _report.buttons = _report.buttons | b; 
  sendMouseReport(&_report);
}

void USBMouse::release(uint8_t b)
{
  _report.buttons = _report.buttons & ~b; 
  sendMouseReport(&_report);
}

bool USBMouse::isPressed(uint8_t b)
{
  if ((b & _report.buttons) > 0) 
    return true;
  return false;
}
