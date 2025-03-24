/* 
* File: LSJoystick.h
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

// Header definition
#ifndef _LSJOYSTICK_H
#define _LSJOYSTICK_H

#include <Tlv493d.h>                    // Infinion TLV493 magnetic sensor
#include <Arduino.h>
#include "LSCircularBuffer.h"           // LSCircularBuffer
#include "LSUtils.h"                    // pointIntType

#define JOY_RAW_BUFF_SIZE 10            // The size of _joystickRawBuffer
#define JOY_INPUT_BUFF_SIZE 5           // The size of _joystickInputBuffer
#define JOY_OUTPUT_BUFF_SIZE 5          // The size of _joystickOutputBuffer
#define JOY_CENTER_BUFF_SIZE 5          // The size of _joystickCenterBuffer

#define JOY_CALIBR_ARRAY_SIZE 5         // The _magnetInputCalibration array size
#define JOY_MAG_SAMPLE_SIZE 5           // The sample size used for averaging magnet samples 

// Joystick magnetic directions
#define JOY_DIRECTION_DEFAULT 1
#define JOY_DIRECTION_INVERSE -1
#define JOY_DIRECTION_FAULT 0           // Can be used to notify the end user that there is an issue with hall sensor setup

#define JOY_DIRECTION_X -1              // Coordinate system for joystic axes. Depends on sensor arrangement
#define JOY_DIRECTION_Y -1              // Coordinate system for joystic axes. Depends on sensor arrangement

#define JOY_Z_DIRECTION_THRESHOLD 9     // The threshold used in z axis direction detection 

#define JOY_RAW_XY_MAX 30.0             // The max raw x or y of a calibration point in mT

#define JOY_INPUT_XY_MAX 1024           // The max range of mapped input from float to int(-1024 to 1024)

#define JOY_INPUT_CHANGE_TOLERANCE 0.1  // The output change tolerance in mT

#define JOY_INPUT_DEADZONE 0.5          // The input deadzone in mT

#define JOY_OUTPUT_DEADZONE_STATUS true // The default output deadzone state (True = enable , False = disable)

#define JOY_OUTPUT_DEADZONE_FACTOR 0.05 // The default output deadzone factor (5% of JOY_INPUT_XY_MAX)

#define JOY_OUTPUT_RANGE_LEVEL 5        // The default output mouse cursor range level or output movement upper range 

#define JOY_OUTPUT_XY_MAX_GAMEPAD  127


class LSJoystick {
  public:
    LSJoystick();                                                         // Constructor
    void begin();
    void clear();   
    int getMagnetXDirection();                                            // Get the X direction of the magnet.
    void setMagnetXDirection(int magnetXDirection);                       // Set or update the magnet X direction variable.
    int getMagnetYDirection();                                            // Get the Y direction of the magnet.
    void setMagnetYDirection(int magnetYDirection);                       // Set or update the magnet Y direction variable.
    int getMagnetZDirection();                                            // Get the Z direction of the magnet.
    void setMagnetZDirection();                                           // Update the magnet Z direction variable.
    void setMagnetDirection(int magnetXDirection, int magnetYDirection);  // Set magnet direction based on orientation of magnet (z axis), X and Y direction variables.
    void setInnerDeadzone(bool deadzoneEnabled,float deadzoneFactor);     // Enable or disable deadzone and set deadzone scale factor (0-100), default 0.12
    float getInnerDeadzoneFactor(void);                                   // Get the inner deadzone factor ()
    void setOuterDeadzone(bool upperDeadzoneEnabled,float outerDeadzoneFactor);  // Enable or disable deadzone and set deadzone scale factor  Default 0.95 
    int getOutputRange();                                                 // Get the output range or speed levels.
    void setOutputRange(int rangeLevel);                                  // Set the output range or speed levels.
    int getMouseSpeedRange();                                             // Get the maximum cursor change
    int getMinimumRadius();                                               // Get the minimum input radius for square to circle mapping.
    void setMinimumRadius();                                              // Set or update the minimum input radius for square to circle mapping.
    pointFloatType getInputCenter();                                      // Get the updated center compensation point.
    void evaluateInputCenter();                                           // Evaluate the center compensation point.
    void updateInputCenterBuffer();                                       // Push new center compensation point to joystickCenter
    pointFloatType getInputMax(int quad);                                 // Get the updated maximum input reading from the selected corner of joystick using the input quadrant. (Calibration purposes)
    void setInputMax(int quad, pointFloatType point);                     // Set the maximum input reading for each corner of joystick using the input quadrant. 
    void zeroInputMax(int quad);                                          // Zero the maximum input reading for each corner of joystick using the input quadrant. 
    void update();                                                        // Update the joystick reading to get new input from the magnetic sensor and calculate the output.
    int getXOut();                                                        // Get the output x value.
    int getYOut();                                                        // Get the output y value.
    pointFloatType getXYRaw();                                            // Get the raw x and y values.
    pointIntType getXYIn();                                               // Get the mapped and filtered x and y values.
    pointIntType getXYOut();                                              // Get the output x and y values.
    int mapRoundInt(int input, int inputStart, int inputEnd, int outputStart, int outputEnd);       // Custom map function that rounds the results instead of truncating

  private:
    Tlv493d _Tlv493dSensor = Tlv493d();                                   // Create an object of Tlv493d class
    LSCircularBuffer <pointFloatType> _joystickRawBuffer;                 // Create a buffer of type pointFloatType to push raw readings 
    LSCircularBuffer <pointIntType> _joystickInputBuffer;                 // Create a buffer of type pointIntType to push mapped and filtered readings 
    LSCircularBuffer <pointIntType> _joystickOutputBuffer;                // Create a buffer of type pointIntType to push mapped readings 
    LSCircularBuffer <pointFloatType> _joystickCenterBuffer;              // Create a buffer of type pointFloatType to push center input readings     
    bool canSkipInputChange(pointFloatType inputPoint);                   // Check if the output change can be skipped (Low-Pass Filter)
    pointIntType applyRadialDeadzone(pointIntType inputPoint, float inputPointMagnitude, float inputPointAngle);    // Apply radial deadzone to the input based on deadzoneValue and upperDeadzoneValue
    pointIntType processInputReading(pointFloatType inputPoint);          // Process the input readings and map the input reading from square to circle. (-1024 to 1024 output )
    pointIntType linearizeOutput(pointIntType inputPoint);                // Linearize the output.
    pointIntType scaleOutput(pointIntType inputPoint, float inputMagnitude, float inputAngle);                    // Scales the output from -1024 1024 to operating mode requirements of gamepad or curosor
    pointIntType processOutputResponse(pointIntType inputPoint);          // Process the output (Including linearizeOutput methods and speed control)
    int mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd); // Custom map function to map float to int.
    float mapIntToFloat(int input, int inputStart, int inputEnd, int outputStart, int outputEnd);    // Custom map function that takes integers and outputs a float
    pointFloatType absPoint(pointFloatType inputPoint);                   // Get the absolute value of the point.
    float magnitudePoint(pointFloatType inputPoint);                      // Magnitude of a point from er (0,0)
    float magnitudePoint(pointIntType inputPoint);                        // Magnitude of a point from er (0,0)
    float magnitudePoint(pointFloatType inputPoint, pointFloatType offsetPoint); // Magnitude of a point from offset center point / Compensation point
    pointFloatType pointFloatFromMagnitudeAngle(float inputMagnitude, float inputPointAngle);
    pointIntType pointIntFromMagnitudeAngle(float inputMagnitude, float inputPointAngle);
    int sgn(float val);                                                   // Get the sign of the value.
    pointFloatType _magnetInputCalibration[JOY_CALIBR_ARRAY_SIZE];        // Array of calibration points.
    pointFloatType _rawPoint;                                             // Raw x and y values used for debugging purposes.
    pointIntType _inputPoint;                                             // Mapped and filtered x and y values
    pointIntType _outputPoint;                                            // Output x and y values
    int _joystickXDirection;                                              // Corrected x value after applying _magnetXDirection
    int _joystickYDirection;                                              // Corrected y value after applying _magnetYDirection
    int _magnetZDirection;                                                // Direction of z ( if the board has flipped and has resulted in z-axis being flipped )
    int _magnetXDirection;                                                // Direction of x ( if the board has flipped and has resulted in x axis being flipped )
    int _magnetYDirection;                                                // Direction of y ( if the board has flipped and has resulted in y axis being flipped )
    bool _innerDeadzoneEnabled;                                           // Is inner deadzone enabled?
    bool _outerDeadzoneEnabled;                                           // Is outer deadzone enabled?
    float _innerDeadzoneFactor;                                           // Deadzone factor in percent of total value or max reading JOY_INPUT_XY_MAX
    int _innerDeadzoneValue;                                              // The calculated deadzone value based on deadzone factor and maximum value JOY_INPUT_XY_MAX.
    float _outerDeadzoneFactor;                                           // Upper deadzone factor in percent of total value or max reading JOY_INPUT_XY_MAX
    int _outerDeadzoneValue;                                              // The calculated upper deadzone value based on upper deadzone factor and maximum value JOY_INPUT_XY_MAX.
    int _rangeLevel;                                                      // The range level from 0 to 10 which is used as speed levels.
    int _rangeValue;                                                      // The calculated range value based on range level and an equation. This is maximum output value for each range level. (Cursor or gamepad)
    float _inputRadius;                                                   // The minimum radius of operating area calculated using calibration points.
    bool _skipInputChange;                                                // The flag to low-pass filter the input changes 
    int _operatingMode;                                                   // Operating mode, gamepad or mouse  //TODO 2025-Mar-06 Remove - Joystick class should be independent of operating mode

};

//*********************************//
// Function   : LSJoystick 
// 
// Description: Construct LSJoystick
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
LSJoystick::LSJoystick() {
  _joystickRawBuffer.begin(JOY_RAW_BUFF_SIZE);                       // Initialize _joystickRawBuffer
  _joystickInputBuffer.begin(JOY_INPUT_BUFF_SIZE);                   // Initialize _joystickInputBuffer
  _joystickOutputBuffer.begin(JOY_OUTPUT_BUFF_SIZE);                 // Initialize _joystickOutputBuffer
  _joystickCenterBuffer.begin(JOY_CENTER_BUFF_SIZE);                 // Initialize _joystickCenterBuffer
}

//*********************************//
// Function   : begin 
// 
// Description: Initialize LSJoystick with default settings 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::begin() {

  _inputRadius = 0.0;                                                  // Initialize _inputRadius
  _skipInputChange = false;                                            // Initialize _skipInputChange
  _operatingMode = getOperatingMode(false, false); //TODO 2025-Mar-06 Remove - Joystick class should be independent of operating mode

  _Tlv493dSensor.begin();  // TODO 2025-Feb-25 This will likely hang if it fails. Ideally replace with something that returns error/success.
  setMagnetDirection(JOY_DIRECTION_DEFAULT, JOY_DIRECTION_DEFAULT);      // Set default magnet direction.
  setInnerDeadzone(JOY_OUTPUT_DEADZONE_STATUS, JOY_OUTPUT_DEADZONE_FACTOR);   // Set default deadzone status and deadzone factor.
  setOuterDeadzone(JOY_OUTPUT_DEADZONE_STATUS, 1.0 - JOY_OUTPUT_DEADZONE_FACTOR);   // Set default deadzone status and deadzone factor.
  setOutputRange(JOY_OUTPUT_RANGE_LEVEL);                               // Set default output range level or speed level.
  clear();                                                              // Clear calibration array and _joystickOutputBuffer.
}

//*********************************//
// Function   : clear 
// 
// Description: Clear LSJoystick class
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::clear() {

  // Initialize _magnetInputCalibration array
  _magnetInputCalibration[0] = {0.00, 0.00};           // Center point
  _magnetInputCalibration[1] = {0.00, 0.00};
  _magnetInputCalibration[2] = {0.00, 0.00};
  _magnetInputCalibration[3] = {0.00, 0.00};
  _magnetInputCalibration[4] = {0.00, 0.00};


  _joystickRawBuffer.pushElement({0.0, 0.0});           // Initialize _joystickRawBuffer
  _joystickInputBuffer.pushElement({0, 0});            // Initialize _joystickInputBuffer
  _joystickOutputBuffer.pushElement({0, 0});           // Initialize _joystickOutputBuffer
  
}


//*********************************//
// Function   : getMagnetXDirection 
// 
// Description: Get the set magnet x direction ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  void
// 
// Return     : direction : int : Magnet x direction
//*********************************//
int LSJoystick::getMagnetXDirection() {
  return _magnetXDirection;
}

//*********************************//
// Function   : setMagnetXDirection 
// 
// Description: Set magnet x direction ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  magnetXDirection : int : Magnet x direction
// 
// Return     : void
//*********************************//
void LSJoystick::setMagnetXDirection(int magnetXDirection){
   _magnetXDirection = magnetXDirection;
}

//*********************************//
// Function   : getMagnetYDirection 
// 
// Description: Get the set magnet y direction ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  void
// 
// Return     : direction : int : Magnet y direction
//*********************************//
int LSJoystick::getMagnetYDirection() {
  return _magnetYDirection;
}

//*********************************//
// Function   : setMagnetYDirection 
// 
// Description: Set magnet y direction ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  magnetYDirection : int : Magnet y direction
// 
// Return     : void
//*********************************//
void LSJoystick::setMagnetYDirection(int magnetYDirection){
   _magnetYDirection = magnetYDirection;
}

//*********************************//
// Function   : getMagnetZDirection 
// 
// Description: Get the set magnet z direction ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  void
// 
// Return     : direction : int : Magnet z direction
//*********************************//
int LSJoystick::getMagnetZDirection() {
  return _magnetZDirection;
}


//*********************************//
// Function   : setMagnetZDirection 
// 
// Description: Set magnet z direction based on z direction and JOY_Z_DIRECTION_THRESHOLD.
//              ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::setMagnetZDirection() {

  float zReading = 0.0;
  for (int i = 0 ; i < JOY_MAG_SAMPLE_SIZE ; i++){        // Get the average of 5 z direction reading 
    _Tlv493dSensor.updateData();
    zReading += _Tlv493dSensor.getZ();
  }
  zReading = ((float) zReading) / JOY_MAG_SAMPLE_SIZE;
   
  if (zReading < -1 * JOY_Z_DIRECTION_THRESHOLD)          // Set z direction to inverse 
  {
    _magnetZDirection = JOY_DIRECTION_INVERSE;
  }
  else if (zReading > JOY_Z_DIRECTION_THRESHOLD){         // Set z direction to default 
    _magnetZDirection = JOY_DIRECTION_DEFAULT;
  }
  else {
    _magnetZDirection = JOY_DIRECTION_FAULT;             // Set z direction to fault
  }

}

//*********************************//
// Function   : setMagnetDirection 
// 
// Description: Set joystick x and y final magnet directions based on x,y, and z magnet directions.
//              ( Default = 1, Inverse = -1, Fault = 0)
// 
// Arguments :  magnetXDirection : int : Magnet x direction.
//              magnetYDirection : int : Magnet y direction.
// 
// Return     : void
//*********************************//
void LSJoystick::setMagnetDirection(int magnetXDirection, int magnetYDirection) {

  // Set new x,y, and z magnet directions
  setMagnetXDirection(magnetXDirection);
  setMagnetYDirection(magnetYDirection);
  setMagnetZDirection();
  
  // Evaluate joystick x and y final magnet directions
  _joystickXDirection = -1 * _magnetZDirection * _magnetXDirection; // Flip x- axis due to flipped sensor
  _joystickYDirection = -1 * _magnetZDirection * _magnetYDirection; // Flip y- axis due to flipped sensor
}


//*********************************//
// Function   : setInnerDeadzone 
// 
// Description: Set the deadzone value based on deadzone status and deadzone factor.
// 
// Arguments :  deadzoneEnabled : bool  : is deadzone enabled?
//              deadzoneFactor  : float : deadzoneFactor from 0.01 to 1.0
// 
// Return     : void
//*********************************//
void LSJoystick::setInnerDeadzone(bool innerDeadzoneEnabled, float innerDeadzoneFactor){
  _innerDeadzoneEnabled = innerDeadzoneEnabled;
  _innerDeadzoneFactor = innerDeadzoneFactor;

  // Set the deadzone value if it's enabled. The deadzone value is zero if it's disabled.
  // deadzone value is the _innerDeadzoneFactor multiplied by JOY_INPUT_XY_MAX.
  if (_innerDeadzoneEnabled) {
    _innerDeadzoneValue = round(JOY_INPUT_XY_MAX * _innerDeadzoneFactor);
  } else {
    _innerDeadzoneValue = 0; 
  }    
}

//*********************************//
// Function   : getInnerDeadzoneFactor 
// 
// Description: Get the inner deadzone factor.
// 
// Arguments :  void
// 
// Return     : deadzoneFactor  : float : deadzoneFactor from 0.01 to 1.0
//*********************************//
float LSJoystick::getInnerDeadzoneFactor(void){
  return _innerDeadzoneFactor;  
}

//*********************************//
// Function   : setOuterDeadzone 
// 
// Description: Set the upper deadzone value based on deadzone status and upper deadzone factor.
// 
// Arguments :  deadzoneEnabled : bool  : is deadzone enabled?
//              deadzoneFactor  : float : deadzoneFactor from 0.01 to 1.0
// 
// Return     : void
//*********************************//
void LSJoystick::setOuterDeadzone(bool outerDeadzoneEnabled, float outerDeadzoneFactor){
  _outerDeadzoneEnabled = outerDeadzoneEnabled;
  _outerDeadzoneFactor = outerDeadzoneFactor;

  // Set the deadzone value if it's enabled. The deadzone value is zero if it's disabled.
  // deadzone value is the _outerDeadzoneFactor multiplied by JOY_INPUT_XY_MAX.
  if (_outerDeadzoneEnabled) {
    _outerDeadzoneValue = round(JOY_INPUT_XY_MAX * _outerDeadzoneFactor);
  } else {
    _outerDeadzoneValue = JOY_INPUT_XY_MAX; 
  } 

}

//*********************************//
// Function   : getOutputRange
// 
// Description: Get the output range level or speed level ( 0 to 10 )
// 
// Arguments :  
// 
// Return     : range level : int : range level or speed level ( 0 to 10 )
//*********************************//
int LSJoystick::getOutputRange(){
  return _rangeLevel;
}

//*********************************//
// Function   : setOutputRange 
// 
// Description: Set the output range value based on range level or speed level ( 0 to 10 )
// 
// Arguments :  rangeLevel : int : range level or speed level ( 0 to 10 )
// 
// Return     : void
//*********************************//
void LSJoystick::setOutputRange(int rangeLevel){
  if (USB_DEBUG) { Serial.print("USBDEBUG: setOutputRange("); Serial.print(rangeLevel); Serial.println(")"); }

  // When operating as a mouse, the following function sets the upper limit of cursor value output each update
  if (_operatingMode == CONF_OPERATING_MODE_MOUSE){
    // Calculate the output range value
    _rangeValue = (int)((0.125 * sq(rangeLevel)) + ( 0.3 * rangeLevel ) + 2);       // Polynomial 
    // [0:2; 1:2; 2:3; 3:4; 4:5; 5:7; 6:8; 7:10; 8:12; 9:15; 10:18]
    
    // _rangeValue = (int)((1.05 * exp(( 0.175 * rangeLevel) + 1.1)) - 1);           // Exponential   
    //Serial.print("_rangeValue:");
    //Serial.println(_rangeValue);

    // When operating as gamepad, the output range is 127 and is not affected by rangeLevel
  } else if (_operatingMode == CONF_OPERATING_MODE_GAMEPAD){
    _rangeValue = JOY_OUTPUT_XY_MAX_GAMEPAD ;
  }
  _rangeLevel = rangeLevel;
}

//*********************************//
// Function   : getMouseSpeedRange
// 
// Description: Get the output range level based on mouse speed level.
// 
// Arguments :  void
// 
// Return     : int : _rangeValue;
//*********************************//
int LSJoystick::getMouseSpeedRange(){
  return _rangeValue;
}

//*********************************//
// Function   : getMinimumRadius 
// 
// Description: Get the minimum operating range as radius of circle from center point using calibration points.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
int LSJoystick::getMinimumRadius(){
  return _inputRadius;
}

//*********************************//
// Function   : setMinimumRadius 
// 
// Description: Set the minimum operating range as radius of circle from center point using calibration points.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::setMinimumRadius(){
  float tempRadius = 0.0;
  pointFloatType tempPoint = {JOY_RAW_XY_MAX, JOY_RAW_XY_MAX};
  _inputRadius = magnitudePoint(tempPoint, _magnetInputCalibration[0]) / sqrt(2.0);

  // Loop through calibration points and calculate the smallest magnitude from center point
  for (int i = 1; i < JOY_CALIBR_ARRAY_SIZE; i++) {
    tempRadius = (sqrt(sq(_magnetInputCalibration[i].x - _magnetInputCalibration[0].x) + sq(_magnetInputCalibration[i].y - _magnetInputCalibration[0].y)) / sqrt(2.0));
    // Make sure the calibration point is valid and initial value of _inputRadius is charged 
    if(magnitudePoint(_magnetInputCalibration[i]) > 0.0 && (tempRadius < _inputRadius)) {  // Set smallest magnitude as radius
      _inputRadius = tempRadius; 
    }
  }
  //Serial.println(_inputRadius);
}


//*********************************//
// Function   : getInputCenter 
// 
// Description: Get the compensation center point from _magnetInputCalibration array.
// 
// Arguments :  void
// 
// Return     : center point : pointFloatType : The center point
//*********************************//
pointFloatType LSJoystick::getInputCenter() {
  return _magnetInputCalibration[0];
}

//*********************************//
// Function   : evaluateInputCenter
// 
// Description: Evaluate the compensation center point from_joystickCenterBuffer.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::evaluateInputCenter() {
  float centerX = 0.0;
  float centerY = 0.0;
  pointFloatType centerPoint = {centerX, centerY};
  for(int centerIndex = 0; centerIndex < JOY_CENTER_BUFF_SIZE; centerIndex++){
    centerPoint = _joystickCenterBuffer.getElement(centerIndex);
    centerX += centerPoint.x;
    centerY += centerPoint.y;
  }
  centerX = centerX / JOY_CENTER_BUFF_SIZE;
  centerY = centerY / JOY_CENTER_BUFF_SIZE;
  _magnetInputCalibration[0] = {centerX, centerY};
}


//*********************************//
// Function   : updateInputCenterBuffer
// 
// Description: Update the compensation center point buffer by pushing new value into _joystickCenterBuffer.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::updateInputCenterBuffer() {
  _Tlv493dSensor.updateData();
  _joystickCenterBuffer.pushElement({_Tlv493dSensor.getY(), _Tlv493dSensor.getX()});   // Joystick direction mapping
}


//*********************************//
// Function   : getInputMax 
// 
// Description: Get the max point of the input quadrant from _magnetInputCalibration array.
// 
// Arguments :  quad : int : input quadrant
// 
// Return     : max point : pointFloatType : The max point
//*********************************//
pointFloatType LSJoystick::getInputMax(int quad) {
  _Tlv493dSensor.updateData();
  // Get new x and y reading
  pointFloatType tempCalibrationPoint = {_Tlv493dSensor.getY(), _Tlv493dSensor.getX()};
//  Serial.print("x:");
//  Serial.print(tempCalibrationPoint.x);
//  Serial.print("y:");
//  Serial.println(tempCalibrationPoint.y);

  // Update the calibration point and minimum radius 
  // Make sure it's a valid quadrant and if new point has larger magnitude from center ( calibration timer loop )


  if( (quad >= 0) && 
  (quad < JOY_CALIBR_ARRAY_SIZE) && 
  magnitudePoint(tempCalibrationPoint, _magnetInputCalibration[0]) > magnitudePoint(_magnetInputCalibration[quad], _magnetInputCalibration[0])){           // The point with larger magnitude is sent as output 
    _magnetInputCalibration[quad] = tempCalibrationPoint;
  }
  
  return _magnetInputCalibration[quad];
}


//*********************************//
// Function   : setInputMax 
// 
// Description: Set the max point of the input quadrant in _magnetInputCalibration array.
// 
// Arguments :  quad : int : input quadrant
//              inputPoint : pointFloatType : The max point
// 
// Return     : void
//*********************************//
void LSJoystick::setInputMax(int quad, pointFloatType inputPoint) {
 // Update the calibration point 
  _magnetInputCalibration[quad] = inputPoint;
}

//*********************************//
// Function   : zeroInputMax 
// 
// Description: Zero the max point of the input quadrant in _magnetInputCalibration array.
// 
// Arguments :  quad : int : input quadrant
// 
// Return     : void
//*********************************//
void LSJoystick::zeroInputMax(int quad) {
 // Reset or zero the calibration point 
  _magnetInputCalibration[quad] = {0, 0};
}

//*********************************//
// Function   : update 
// 
// Description: Set the new sensor readings, process data and push to _joystickOutputBuffer
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSJoystick::update() {

  _Tlv493dSensor.updateData();
  // Get the new readings as a point
  _rawPoint = {_Tlv493dSensor.getY(), _Tlv493dSensor.getX()};  // TODO 2025-Feb-25 This should be abstracted to a dedicated function   
  
  _skipInputChange = canSkipInputChange(_rawPoint);
  _joystickRawBuffer.pushElement(_rawPoint);                  // Add raw points to _joystickRawBuffer : DON'T MOVE THIS


  if(!_skipInputChange){  // If latest measurement has changed more than the change threshold, process and add to output buffer 
    _inputPoint = processInputReading(_rawPoint);             // Filtered and scaled input readings
    _joystickInputBuffer.pushElement(_inputPoint);            // Add new input point to _joystickInputBuffer
    _outputPoint = processOutputResponse(_inputPoint);        // Process output by applying deadzone, speed control, and linearization
    _joystickOutputBuffer.pushElement(_outputPoint);          // Add new output point to _joystickOutputBuffer    
  } 
}

//*********************************//
// Function   : getXOut 
// 
// Description: Get the last processed output x value from _joystickOutputBuffer
// 
// Arguments :  void
// 
// Return     : x value : int : The output x value 
//*********************************//
int LSJoystick::getXOut() {
  return _joystickOutputBuffer.getLastElement().x;
}

//*********************************//
// Function   : getYOut 
// 
// Description: Get the last processed output y value from _joystickOutputBuffer
// 
// Arguments :  void
// 
// Return     : x value : int : The output x value 
//*********************************//
int LSJoystick::getYOut() {
  return _joystickOutputBuffer.getLastElement().y;
}

//*********************************//
// Function   : getXYRaw
// 
// Description: Get the last raw x and y values from _joystickRawBuffer
// 
// Arguments :  void
// 
// Return     : output point : pointFloatType : The raw x and y point
//*********************************//
pointFloatType LSJoystick::getXYRaw() {
  return _joystickRawBuffer.getLastElement();
}


//*********************************//
// Function   : getXYIn 
// 
// Description: Get the mapped x and y input point
// 
// Arguments :  void
// 
// Return     : input point : pointIntType : The input x and y point
//*********************************//
pointIntType LSJoystick::getXYIn() {
  return _joystickInputBuffer.getLastElement();
}


//*********************************//
// Function   : getXYOut 
// 
// Description: Get the last processed output x and y values from _joystickOutputBuffer
// 
// Arguments :  void
// 
// Return     : output point : pointIntType : The output x and y point
//*********************************//
pointIntType LSJoystick::getXYOut() {
  return _joystickOutputBuffer.getLastElement();
}


//*********************************//
//********Private******************//
//*********************************//


//*********************************//
// Function   : applyRadialDeadzone 
// 
// Description: Apply the radial deadzone to the output value 
// 
// Arguments :  inputPoint : pointIntType : Input point coordinates
//              inputPointMagnitude : float : Magnitude of input point
//              inputPointAngle : float : Angle of input point
// 
// Return     : outputPoint : pointIntType : Output with deadzone applied
//*********************************//
pointIntType LSJoystick::applyRadialDeadzone(pointIntType inputPoint, float inputPointMagnitude, float inputPointAngle) {

 
  pointIntType outputPoint = {0,0};

   // Output the input if Deadzone is not enabled
   // if Deadzone is not enabled, if it is enabled it will be overwritten
  outputPoint = inputPoint;  

  int innerDeadzoneValue; 
  int outerDeadzoneValue; 

  if(_innerDeadzoneEnabled) { 
    innerDeadzoneValue = _innerDeadzoneValue;
  }
  else {
    innerDeadzoneValue = 0;
  } 

  if (_outerDeadzoneEnabled) {
    outerDeadzoneValue = _outerDeadzoneValue;
  } else {
    outerDeadzoneValue = JOY_INPUT_XY_MAX;
  }

  //float deadzoneScalingFactor = (inputPointMagnitude - innerDeadzoneValue) / (outerDeadzoneValue - innerDeadzoneValue);

  // Apply deadzone

  if (inputPointMagnitude <= innerDeadzoneValue) {
    // No output
    outputPoint = {0,0};
  } else if (inputPointMagnitude >= outerDeadzoneValue) {
    // Set x,y value of output point on circle of radius JOY_INPUT_XY_MAX at angle of input point
    outputPoint = pointIntFromMagnitudeAngle(JOY_INPUT_XY_MAX, inputPointAngle);

    //outputPoint.x = sgn(inputPoint.x) * abs(round(cos(inputPointAngle) * JOY_INPUT_XY_MAX));   
    //outputPoint.y = sgn(inputPoint.y) * abs(round(sin(inputPointAngle) * JOY_INPUT_XY_MAX));   

  } else { // (innerDeadzoneValue < inputPointMagnitude < outerDeadzoneValue)
    // Map the input magnitudes between the lower and outer deadzones to between 0 and the maximum value
    
    int outputMagnitude = mapRoundInt(inputPointMagnitude, _innerDeadzoneValue, _outerDeadzoneValue, 0, JOY_INPUT_XY_MAX);
    outputPoint = pointIntFromMagnitudeAngle(outputMagnitude, inputPointAngle);

  }

 return outputPoint;
}

//*********************************//
// Function   : canSkipInputChange 
// 
// Description: Check if the input change can be skipped  
// 
// Arguments :  inputPoint : pointFloatType : raw input reading
// 
// Return     : skipInputChange : bool : skip processing if it's true
//*********************************//
bool LSJoystick::canSkipInputChange(pointFloatType inputPoint) {      
  pointFloatType prevInputPoint = _joystickRawBuffer.getLastElement();           // Get the previous point from buffer 

  // If latest joystick measurement is less than JOY_INPUT_CHANGE_TOLERANCE (0.1 mT), then don't process 
  bool skipInputChange = abs(_rawPoint.x - prevInputPoint.x) < JOY_INPUT_CHANGE_TOLERANCE 
                      && abs(_rawPoint.y - prevInputPoint.y) < JOY_INPUT_CHANGE_TOLERANCE;

  return skipInputChange;
}


//*********************************//
// Function   : linearizeOutput 
// 
// Description: Linearize raw joystick readings
// 
// Arguments :  inputPoint : pointIntType : Input points 
// 
// Return     : outputPoint : pointIntType : Output with linearization and speed control applied
//*********************************//
pointIntType LSJoystick::linearizeOutput(pointIntType inputPoint){                              
  
  // Linearize the input joystick values 
  pointIntType linearizedPoint = {0,0};

  linearizedPoint = inputPoint; //TODO 2025-Mar-07 Replace with linearization 
    
  return linearizedPoint;  
}

//*********************************//
// Function   : scaleOutput 
// 
// Description: Scales output value to CONF_JOY_OUTPUT_XY_MAX
// 
// Arguments :  inputPoint : pointIntType : Output without linearization
//              inputPointMagnitude : float : Magnitude of input point
//              inputPointAngle : float : Angle of input point
// 
// Return     : outputPoint : pointIntType : Scaled output point
//*********************************//
pointIntType LSJoystick::scaleOutput(pointIntType inputPoint, float inputMagnitude, float inputAngle){                              
  
  pointIntType outputPoint = {0,0};  // Initialize outputPoint

  // Map input magnitude (between 0 and 1024) to output magnitudes (between 0 and _rangeValue)
  float outputMagnitude = mapIntToFloat(inputMagnitude, 0, JOY_INPUT_XY_MAX, 0, CONF_JOY_OUTPUT_XY_MAX);
  outputMagnitude = constrain(outputMagnitude, 0, CONF_JOY_OUTPUT_XY_MAX);

  outputPoint = pointIntFromMagnitudeAngle(outputMagnitude,inputAngle);
   
  return outputPoint;  
}

//*********************************//
// Function   : processInputReading 
// 
// Description: Process input value and map the readings
// 
// Arguments :  inputPoint : pointFloatType : Raw magnet input reading
// 
// Return     : outputPoint : pointIntType : Output with mapped reading 
//*********************************//
pointIntType LSJoystick::processInputReading(pointFloatType inputPoint) {

  // Initialize limitPoint, centeredPoint and outputPoint
  pointFloatType limitPoint = {0.00, 0.00};
  pointFloatType centeredPoint = {0.00, 0.00};
  pointIntType outputPoint = {0,0};
  pointFloatType center = _magnetInputCalibration[0];  // Center / neutral point reading of joystick from calibration
  
  if ((sq(inputPoint.x) + sq(inputPoint.y)) >= sq(JOY_INPUT_DEADZONE)) {  //  TODO 2025-Mar-07 Look at this constant / fixed input deadzone
    // Center the input point
    centeredPoint = {(inputPoint.x - center.x) * _joystickXDirection, 
                     (inputPoint.y - center.y) * _joystickYDirection};
  }

  float thetaVal = atan2(centeredPoint.y, centeredPoint.x);         // Get the angle of the point

  // Find the limiting point on perimeter of circle
  limitPoint.x = sgn(centeredPoint.x) * abs(cos(thetaVal) * _inputRadius);
  limitPoint.y = sgn(centeredPoint.y) * abs(sin(thetaVal) * _inputRadius);

  // Compare the magnitude of two points from center
  // Output point on perimeter of circle if it's outside
  if ((sq(centeredPoint.x) + sq(centeredPoint.y)) >= sq(_inputRadius)) {
    centeredPoint.x = limitPoint.x; 
    centeredPoint.y = limitPoint.y; 
//    Serial.print(centeredPoint.x);  
//    Serial.print(",");  
//    Serial.println(centeredPoint.y); 
  }
    // Scale the centered point to int ( -1024 to 1024 )
    outputPoint.x = mapFloatInt(centeredPoint.x, -1*_inputRadius, _inputRadius, -1*JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
    outputPoint.y = mapFloatInt(centeredPoint.y, -1*_inputRadius, _inputRadius, -1*JOY_INPUT_XY_MAX, JOY_INPUT_XY_MAX);
 
  return outputPoint;
}


//*********************************//
// Function   : processOutputResponse 
// 
// Description: Process output value by applying Deadzone and linearization
// 
// Arguments :  inputPoint : pointIntType : Output before applying Deadzone and linearization
// 
// Return     : outputPoint : pointIntType : Processed Output with mapped reading 
//*********************************//
pointIntType LSJoystick::processOutputResponse(pointIntType inputPoint){
  
  // Initialize outputPoint and deadzonedPoint
  pointIntType outputPoint, deadzonedPoint, linearizedPoint = {0,0};

  // Apply linearization
  linearizedPoint = linearizeOutput(inputPoint);

  float linearizedPointMagnitude = magnitudePoint(linearizedPoint);
  float inputAngle = atan2(linearizedPoint.y, linearizedPoint.x); 
  
  // Apply Deadzone
  deadzonedPoint = applyRadialDeadzone(linearizedPoint, linearizedPointMagnitude, inputAngle);
  float deadzonedPointMagnitude = magnitudePoint(deadzonedPoint);

  // Apply Scaling to output device range
  outputPoint = scaleOutput(deadzonedPoint, deadzonedPointMagnitude, inputAngle);
  

  return outputPoint;
}


//*********************************//
// Function   : mapFloatInt 
// 
// Description: Map float to int
// 
// Arguments :  input        : float : input value 
//              inputStart   : float : lower limit of input
//              inputEnd     : float : upper limit of input
//              outputStart  : int   : lower limit of output
//              outputEnd    : int   : upper limit of input
// 
// Return     : output : int : Mapped input value 
//*********************************//
int LSJoystick::mapFloatInt(float input, float inputStart, float inputEnd, int outputStart, int outputEnd) {

  // Evaluate input and output range
  float inputRange = inputEnd - inputStart;
  int outputRange = outputEnd - outputStart;

  int output = round((input - inputStart) * outputRange / inputRange + outputStart);
  
  return output;
}

//*********************************//
// Function   : mapRoundInt
// 
// Description: Map a value from one range to another, rounding the output
// 
// Arguments :  input        : int   : input value 
//              inputStart   : int   : lower limit of input
//              inputEnd     : int   : upper limit of input
//              outputStart  : int   : lower limit of output
//              outputEnd    : int   : upper limit of input
// 
// Return     : output : int : Mapped input value 
//*********************************//
int LSJoystick::mapRoundInt(int input, int inputStart, int inputEnd, int outputStart, int outputEnd) {
  int output = round(float(input - inputStart) * float(outputEnd - outputStart) / float(inputEnd - inputStart) + outputStart);
  
  return output;
}

//*********************************//
// Function   : mapIntToFloat
// 
// Description: Map a value from one range to another, rounding the output
// 
// Arguments :  input        : int   : input value 
//              inputStart   : int   : lower limit of input
//              inputEnd     : int   : upper limit of input
//              outputStart  : int   : lower limit of output
//              outputEnd    : int   : upper limit of input
// 
// Return     : output : float : Mapped input value 
//*********************************//
float LSJoystick::mapIntToFloat(int input, int inputStart, int inputEnd, int outputStart, int outputEnd) {
  float output = float(input - inputStart) * float(outputEnd - outputStart) / float(inputEnd - inputStart) + float(outputStart);
  
  return output;
}

//*********************************//
// Function   : absPoint 
// 
// Description: The absolute value of float point
// 
// Arguments :  inputPoint  : pointFloatType : float input point
// 
// Return     : output point : pointFloatType : absolute value of float input point
//*********************************//
pointFloatType LSJoystick::absPoint(pointFloatType inputPoint){
  return {abs(inputPoint.x), abs(inputPoint.y)};
}

//*********************************//
// Function   : magnitudePoint 
// 
// Description: The magnitude of float point from center point (0,0)
// 
// Arguments :  inputPoint  : pointFloatType : float input point
// 
// Return     : magnitude : float : magnitude of float point
//*********************************//
float LSJoystick::magnitudePoint(pointFloatType inputPoint){
  return (sqrt(sq(inputPoint.x) + sq(inputPoint.y)));
}

//*********************************//
// Function   : magnitudePoint 
// 
// Description: The magnitude of an int point from center point (0,0)
// 
// Arguments :  inputPoint  : pointIntType : integer input point
// 
// Return     : magnitude : float : magnitude of float point
//*********************************//
float LSJoystick::magnitudePoint(pointIntType inputPoint){
  return (sqrt(sq(float(inputPoint.x)) + sq(float(inputPoint.y))));
}

//*********************************//
// Function   : magnitudePoint 
// 
// Description: The magnitude of float point from offset point
// 
// Arguments :  inputPoint  : pointFloatType : float input point
//              offsetPoint  : pointFloatType : float offset center point
// 
// Return     : magnitude : float : magnitude of float point
//*********************************//
float LSJoystick::magnitudePoint(pointFloatType inputPoint, pointFloatType offsetPoint){
  return (sqrt(sq(inputPoint.x - offsetPoint.x) + sq(inputPoint.y - offsetPoint.y)));
}


//*********************************//
// Function   : sgn 
// 
// Description: The sign of float value ( -1 or +1 )
// 
// Arguments :  val : float : float input value
// 
// Return     : sign : int : sign of float value ( -1 or +1 )
//*********************************//
int LSJoystick::sgn(float val) {
  return (0.0 < val) - (val < 0.0);
}

//*********************************//
// Function   : pointIntFromMagnitudeAngle 
// 
// Description: The sign of float value ( -1 or +1 )
// 
// Arguments :  inputMagnitude : float : Magnitude
//              inputPointAngle : float : Angle of point
// 
// Return     : outputPoint : pointIntType : (x,y) point
//*********************************//
pointIntType LSJoystick::pointIntFromMagnitudeAngle(float inputMagnitude, float inputPointAngle) {
  pointIntType outputPoint= {0,0};
  outputPoint.x = round(inputMagnitude * cos(inputPointAngle));   
  outputPoint.y = round(inputMagnitude * sin(inputPointAngle)); 
  return outputPoint;
}


//*********************************//
// Function   : pointFloatFromMagnitudeAngle 
// 
// Description: Returns a float point based on magnitude and angle
// 
// Arguments :  inputMagnitude : float : Magnitude
//              inputPointAngle : float : Angle of point
// 
// Return     : outputPoint : pointFloatType : (x,y) point
//*********************************//
pointFloatType LSJoystick::pointFloatFromMagnitudeAngle(float inputMagnitude, float inputPointAngle) {
  pointFloatType outputPoint= {0,0};
  outputPoint.x = inputMagnitude * cos(inputPointAngle);   
  outputPoint.y = inputMagnitude * sin(inputPointAngle); 
  return outputPoint;
}


#endif 
