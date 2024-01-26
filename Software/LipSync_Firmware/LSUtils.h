/* 
* File: LSUtils.h
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


//Header definition
#ifndef _LSUTILS_H
#define _LSUTILS_H

//Struct of float point
typedef struct {
  float x;
  float y;
} pointFloatType;

//Struct of int point
typedef struct {
  int x;
  int y;
} pointIntType;

//LED action structure ( Led actions for each output action )
typedef struct
{
  uint8_t ledOutputActionNumber;
  uint8_t ledNumber;
  uint8_t ledStartColor;
  uint8_t ledEndColor;
  uint8_t ledEndAction;
} ledActionStruct;

//LED state structure ( Led state which is sent to perform led action )
typedef struct
{
  int ledAction; //none = 0, off = 1, on = 2, blink = 3
  int ledColorNumber;
  int ledNumber; // LED index / which LED
  int ledBlinkNumber; // number of blinks
  unsigned long ledBlinkTime; //
  int ledBrightness;
} ledStateStruct;

//Input(sip and puff, switches ,buttons) actions relation with output actions structure
typedef struct
{
  uint8_t inputActionState;
  uint8_t mouseOutputActionNumber;
  uint8_t gamepadOutputActionNumber;
  uint8_t menuOutputActionNumber;
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;

//Input (sip and puff, switches ,buttons) states structure
typedef struct {
  int mainState;                 //button1 + 2*button2 + 4*button3  or none : 0 ,sip : 1, puff : 2
  int secondaryState;            //waiting = 0, started = 1, released = 2
  unsigned long elapsedTime;     //in ms
} inputStateStruct;

//acceleration structure
typedef struct
{
  uint8_t accNumber;
  float accCoefficient;
  uint8_t accStartSpeed;
  uint8_t accEndSpeed;
} accStruct;

#endif