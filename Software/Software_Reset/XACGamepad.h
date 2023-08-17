/* 
* File: XACGamepad.h
* Firmware: Enabled_Controller_Mini_Joystick_Software
* Developed by: Milad Hajihassan
* Version Number: 1.2 (01/6/2023)
*/


#include "Adafruit_TinyUSB.h"

#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

typedef struct ATTRIBUTE_PACKED {
    uint8_t buttons;
    uint8_t	xAxis;
    uint8_t	yAxis;
} gamepadReport;

// HID report descriptor 8 buttons and 2 axis joystick 
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


class XACGamepad {
  public:
    inline XACGamepad(void);
    inline void begin(void);
    inline void end(void);
	  inline void wakeup(void);
    inline void send(void);
    inline bool sendGamepadReport(void* data, size_t length) {
        return this->gamepad_usb_hid.sendReport(0, data, (uint8_t)length);
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
  protected:
    gamepadReport _report;
    uint32_t startMillis;
    Adafruit_USBD_HID gamepad_usb_hid;
};

XACGamepad::XACGamepad(void)
{

}

void XACGamepad::begin(void)
{
  this->gamepad_usb_hid.setPollInterval(1);
  this->gamepad_usb_hid.setReportDescriptor(gamepad_desc_hid_report, sizeof(gamepad_desc_hid_report));
  this->gamepad_usb_hid.setStringDescriptor("OpenAT Gamepad");
  this->gamepad_usb_hid.begin();
  while( !USBDevice.mounted() ) delay(1);
  //Release all the buttons and center joystick
  end();
  startMillis = millis();
}

void XACGamepad::send(void)
{
  if (startMillis != millis()) {
  wakeup();
  while(!isReady()) delay(1);
  sendGamepadReport(&_report, sizeof(_report));
    startMillis = millis();
  }
}

void XACGamepad::end(void)
{
  _report.buttons = 0;
  _report.xAxis = 128;
  _report.yAxis = 128;
  sendGamepadReport(&_report, sizeof(_report));
}

void XACGamepad::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void XACGamepad::write(void)
{
  wakeup();
  while(!isReady()) delay(1);
  sendGamepadReport(&_report, sizeof(_report));
}

void XACGamepad::write(void *report)
{
  wakeup();
  while(!isReady()) delay(1);
  memcpy(&_report, report, sizeof(_report));
  sendGamepadReport(&_report, sizeof(_report));
}

void XACGamepad::press(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons |= (uint8_t)1 << b;
}


void XACGamepad::release(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons &= ~((uint8_t)1 << b);
}


void XACGamepad::releaseAll(void)
{
  _report.buttons = 0;
  _report.xAxis = 128;
  _report.yAxis = 128;
}

void XACGamepad::buttons(uint8_t b)
{
  _report.buttons = b;
}


void XACGamepad::xAxis(uint8_t a)
{
  _report.xAxis = 128 + a;
}


void XACGamepad::yAxis(uint8_t a)
{
  _report.yAxis = 128 + a;
}

void XACGamepad::move(uint8_t x,uint8_t y)
{
  _report.xAxis = 128 + x;
  _report.yAxis = 128 + y;
}

bool XACGamepad::isReady(void)
{
	if (gamepad_usb_hid.ready()) 
	  return true;
	return false;
}
