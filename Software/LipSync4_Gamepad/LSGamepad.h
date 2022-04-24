/* 
* File: LSGamepad.h
* Firmware: LipSync4
* Developed by: MakersMakingChange
* Version: Closed-Beta (20 April 2022)
* Copyright Neil Squire Society 2022. 
* License: This work is licensed under the CC BY SA 4.0 License: http://creativecommons.org/licenses/by-sa/4.0 .
*/


#include "Adafruit_TinyUSB.h"

#pragma once

#define ATTRIBUTE_PACKED  __attribute__((packed, aligned(1)))

typedef struct ATTRIBUTE_PACKED {
    uint8_t buttons;
    uint8_t	xAxis;
    uint8_t	yAxis;
} HID_GamepadReport_Data_t;

// HID report descriptor 8 buttons and 2 axis joystick 
uint8_t const desc_hid_report[] =
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


class LSGamepad {
  public:
    inline LSGamepad(void);
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
  protected:
    HID_GamepadReport_Data_t _report;
    uint32_t startMillis;
    Adafruit_USBD_HID usb_hid;
};

LSGamepad::LSGamepad(void)
{
  this->usb_hid.setPollInterval(1);
  this->usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  this->usb_hid.setStringDescriptor("LipSync Gamepad");
}

void LSGamepad::begin(void)
{
  this->usb_hid.begin();
  while( !USBDevice.mounted() ) delay(1);
  //Release all the buttons and center joystick
  end();
  startMillis = millis();
}

void LSGamepad::send(void)
{
  if (startMillis != millis()) {
  wakeup();
  while(!isReady()) delay(1);
  GamepadReport(&_report, sizeof(_report));
    startMillis = millis();
  }
}

void LSGamepad::end(void)
{
  _report.buttons = 0;
  _report.xAxis = 128;
  _report.yAxis = 128;
  GamepadReport(&_report, sizeof(_report));
}

void LSGamepad::wakeup(void)
{
	if ( USBDevice.suspended() )  {
      USBDevice.remoteWakeup();
    }
}

void LSGamepad::write(void)
{
  wakeup();
  while(!isReady()) delay(1);
  GamepadReport(&_report, sizeof(_report));
}

void LSGamepad::write(void *report)
{
  wakeup();
  while(!isReady()) delay(1);
  memcpy(&_report, report, sizeof(_report));
  GamepadReport(&_report, sizeof(_report));
}

void LSGamepad::press(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons |= (uint8_t)1 << b;
}


void LSGamepad::release(uint8_t b)
{
  b &= 0x7; // Limit value between 0..7
  _report.buttons &= ~((uint8_t)1 << b);
}


void LSGamepad::releaseAll(void)
{
  _report.buttons = 0;
}

void LSGamepad::buttons(uint8_t b)
{
  _report.buttons = b;
}


void LSGamepad::xAxis(uint8_t a)
{
  _report.xAxis = 128 + a;
}


void LSGamepad::yAxis(uint8_t a)
{
  _report.yAxis = 128 + a;
}

void LSGamepad::move(uint8_t x,uint8_t y)
{
  _report.xAxis = 128 + x;
  _report.yAxis = 128 + y;
}

bool LSGamepad::isReady(void)
{
	if (usb_hid.ready()) 
	  return true;
	return false;
}
