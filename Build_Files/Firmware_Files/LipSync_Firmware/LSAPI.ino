/* 
* File: LSAPI.h
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
  If not, see <http://www.gnu.org/licenses/>.
*/


//***API FUNCTIONS***// - DO NOT CHANGE
typedef void (*FunctionPointer)(bool, bool, String);      // Type definition for API function pointer

typedef struct {                                  // Type definition for API function list
  String endpoint;                                // Unique two character end point
  String code;                                    // Unique one character command code
  String parameter;                               // Parameter that is passed to function
  FunctionPointer function;                       // API function pointer
} _functionList;

// Declare individual API functions with command, parameter, and corresponding function
_functionList getModelNumberFunction =            {"MN", "0", "0", &getModelNumber};
_functionList getVersionNumberFunction =          {"VN", "0", "0", &getVersionNumber};
_functionList getDeviceIDFunction =               {"ID", "0", "0", &getDeviceID};
_functionList getOperatingModeFunction =          {"OM", "0", "0", &getOperatingMode};
_functionList setOperatingModeFunction =          {"OM", "1", "",  &setOperatingMode};
_functionList getCommunicationModeFunction =      {"CM", "0", "0", &getCommunicationMode};
_functionList setCommunicationModeFunction =      {"CM", "1", "",  &setCommunicationMode};

_functionList getJoystickInitializationFunction = {"IN", "0", "0", &getJoystickInitialization};
_functionList setJoystickInitializationFunction = {"IN", "1", "1", &setJoystickInitialization};
_functionList getJoystickCalibrationFunction =    {"CA", "0", "0", &getJoystickCalibration};
_functionList setJoystickCalibrationFunction =    {"CA", "1", "1", &setJoystickCalibration};

_functionList getJoystickInnerDeadzoneFunction =  {"IZ", "0", "0", &getJoystickInnerDeadzone};
_functionList setJoystickInnerDeadzoneFunction =  {"IZ", "1", "",  &setJoystickInnerDeadzone};
_functionList getJoystickOuterDeadzoneFunction =  {"OZ", "0", "0", &getJoystickOuterDeadzone};
_functionList setJoystickUpperDeadzoneFunction =  {"OZ", "1", "",  &setJoystickOuterDeadzone};
_functionList getJoystickAccelerationFunction =   {"AV", "0", "0", &getJoystickAcceleration};
_functionList setJoystickAccelerationFunction =   {"AV", "1", "0", &setJoystickAcceleration};

_functionList getCursorSpeedFunction =            {"SS", "0", "0", &getCursorSpeed};
_functionList setCursorSpeedFunction =            {"SS", "1", "",  &setCursorSpeed};
_functionList getScrollLevelFunction =            {"SL", "0", "0", &getScrollLevel};
_functionList setScrollLevelFunction =            {"SL", "1", "",  &setScrollLevel};

_functionList getPressureValueFunction =          {"PV", "0", "0", &getPressureValue};
_functionList getPressureModeFunction =           {"PM", "0", "0", &getPressureMode};
_functionList setPressureModeFunction =           {"PM", "1", "",  &setPressureMode};
_functionList getSipPressureThresholdFunction =   {"ST", "0", "0", &getSipPressureThreshold};
_functionList setSipPressureThresholdFunction =   {"ST", "1", "",  &setSipPressureThreshold};
_functionList getPuffPressureThresholdFunction =  {"PT", "0", "0", &getPuffPressureThreshold};
_functionList setPuffPressureThresholdFunction =  {"PT", "1", "",  &setPuffPressureThreshold};

_functionList getSoundModeFunction =              {"SM", "0", "0", &getSoundMode};
_functionList setSoundModeFunction =              {"SM", "1", "",  &setSoundMode};
_functionList getLightModeFunction =              {"LM", "0", "0", &getLightMode};
_functionList setLightModeFunction =              {"LM", "1", "",  &setLightMode};
_functionList getLightBrightnessLevelFunction =   {"LL", "0", "0", &getLightBrightnessLevel};
_functionList setLightBrightnessLevelFunction =   {"LL", "1", "",  &setLightBrightnessLevel};

_functionList controlHubMenuFunction =            {"CH", "1", "",  &controlHubMenu};

_functionList getDebugModeFunction =              {"DM", "0", "0", &getDebugMode};
_functionList setDebugModeFunction =              {"DM", "1", "",  &setDebugMode};
_functionList getJoystickValueFunction =          {"JV", "0", "0", &getJoystickValue};

_functionList runTestFunction =                   {"RT", "1", "",  &runTest};
_functionList softResetFunction =                 {"SR", "1", "1", &softReset};
_functionList resetSettingsFunction =             {"RS", "1", "1", &resetSettings};
_functionList factoryResetFunction =              {"FR", "1", "1", &doFactoryReset};

// Declare array of API functions
_functionList apiFunction[] = { //Let compiler determine number of functions
  getModelNumberFunction,
  getVersionNumberFunction,
  getDeviceIDFunction,
  getOperatingModeFunction,
  setOperatingModeFunction,
  getCommunicationModeFunction,
  setCommunicationModeFunction,
  getJoystickInitializationFunction,
  setJoystickInitializationFunction,
  getJoystickCalibrationFunction,
  setJoystickCalibrationFunction,
  getJoystickInnerDeadzoneFunction,
  setJoystickInnerDeadzoneFunction,
  getJoystickOuterDeadzoneFunction,
  setJoystickUpperDeadzoneFunction,
  getCursorSpeedFunction,
  setCursorSpeedFunction,
  getScrollLevelFunction,
  setScrollLevelFunction,
  getJoystickValueFunction,
  getPressureModeFunction,
  setPressureModeFunction,
  getPressureValueFunction,
  getSipPressureThresholdFunction,
  setSipPressureThresholdFunction,
  getPuffPressureThresholdFunction,
  setPuffPressureThresholdFunction,
  getJoystickAccelerationFunction,
  setJoystickAccelerationFunction,
  getSoundModeFunction,
  setSoundModeFunction,
  getLightModeFunction,
  setLightModeFunction,
  getLightBrightnessLevelFunction,
  setLightBrightnessLevelFunction,
  controlHubMenuFunction,
  getDebugModeFunction,
  setDebugModeFunction,
  runTestFunction,
  softResetFunction,
  resetSettingsFunction,
  factoryResetFunction
};

//***SERIAL SETTINGS FUNCTION TO CHANGE SPEED AND COMMUNICATION MODE USING SOFTWARE***//
// Function   : serialSettings
//
// Description: This function confirms if serial settings should be enabled.
//              It returns true if it's in the settings mode and is waiting for a command.
//              It returns false if it's not in the settings mode or it needs to exit the settings mode.
//
// Parameters :  enabled : bool : The input flag
//
// Return     : bool
//*************************************************************************************//
bool serialSettings(bool enabled) {

  String commandString = "";
  bool settingsFlag = enabled;

  // Set the input parameter to the flag returned. This will help to detect that the settings actions should be performed.
  if (Serial.available() > 0)
  {
    // Check if serial has received or read input string and word "SETTINGS" is in input string.
    commandString = Serial.readString();
    if (settingsFlag == false && commandString == "SETTINGS") {
      // SETTING received
      // Set the return flag to true so settings actions can be performed in the next call to the function
      printResponseInt(true, true, true, 0, commandString, false, 0);
      settingsFlag = true;
    } else if (settingsFlag == true && commandString == "EXIT") {
      // EXIT Received
      // Set the return flag to false so settings actions can be exited
      printResponseInt(true, true, true, 0, commandString, false, 0);
      settingsFlag = false;
    } else if (settingsFlag == true && isValidCommandFormat(commandString)) { // Check if command's format is correct and it's in settings mode
      performCommand(commandString);                  // Sub function to process valid strings
      settingsFlag = false;
    } else {
      printResponseInt(true, true, false, 0, commandString, false, 0);
      settingsFlag = false;
    }
    Serial.flush();
  }
  return settingsFlag;
}

//***PERFORM COMMAND FUNCTION TO CHANGE SETTINGS USING SOFTWARE***//
// Function   : performCommand
//
// Description: This function takes processes an input string from the serial and calls the
//              corresponding API function, or outputs an error.
//
// Parameters :  inputString : String : The input command as a string.
//
// Return     : void
//*********************************//
void performCommand(String inputString) {
  int inputIndex = inputString.indexOf(':');

  // Extract command string from input string
  String inputCommandString = inputString.substring(0, inputIndex);

  int inputCommandIndex = inputCommandString.indexOf(',');

  String inputEndpointString = inputCommandString.substring(0, inputCommandIndex);

  String inputCodeString = inputCommandString.substring(inputCommandIndex + 1);

  // Extract parameter string from input string
  String inputParameterString = inputString.substring(inputIndex + 1);

  // Determine total number of API commands
  int apiTotalNumber = sizeof(apiFunction) / sizeof(apiFunction[0]);

  // Iterate through each API command
  for (int apiIndex = 0; apiIndex < apiTotalNumber; apiIndex++) {

    // Test if input command string matches API command and input parameter string matches API parameter string
    if (inputEndpointString == apiFunction[apiIndex].endpoint &&
        inputCodeString == apiFunction[apiIndex].code) {

      // Matching Command String found
      if (!isValidCommandParameter( inputParameterString )) {
        printResponseInt(true, true, false, 2, inputString, false, 0);
      }
      else if (inputParameterString == apiFunction[apiIndex].parameter || apiFunction[apiIndex].parameter == "") {
        apiFunction[apiIndex].function(true, true, inputParameterString);
      }
      else { // Invalid input parameter

        // Outut error message
        printResponseInt(true, true, false, 3, inputString, false, 0);
      }
      break;
    } else if (apiIndex == (apiTotalNumber - 1)) { // api doesn’t exist

      // Output error message
      printResponseInt(true, true, false, 1, inputString, false, 0);

      //delay(5);
      break;
    }
  } // end iterate through API functions

}

//***VALIDATE INPUT COMMAND FORMAT FUNCTION***//
// Function   : isValidCommandFormat
//
// Description: This function confirms command string has correct format.
//              It returns true if the string has a correct format.
//              It returns false if the string doesn't have a correct format.
//
// Parameters :  inputCommandString : String : The input string
//
// Return     : boolean
//***********************************************//
bool isValidCommandFormat(String inputCommandString) {
  bool isValidFormat = false;
  if ((inputCommandString.length() == (6) || // XX,d:d
       inputCommandString.length() == (7) || // XX,d:dd
       inputCommandString.length() == (8) || // XX,d:ddd
       inputCommandString.length() == (9) || // XX,d:dddd
       inputCommandString.length() == (11)) && inputCommandString.charAt(2) == ',' && inputCommandString.charAt(4) == ':') { // XX,d:dddddd
    isValidFormat = true;
  }
  return isValidFormat;
}

//***VALIDATE INPUT COMMAND PARAMETER FUNCTION***//
// Function   : isValidCommandParameter
//
// Description: This function checks if the input string is a valid command parameters.
//              It returns true if the string includes valid parameters.
//              It returns false if the string includes invalid parameters.
//
// Parameters :  inputParamterString : String : The input string
//
// Return     : boolean
//*************************************************//
bool isValidCommandParameter(String inputParameterString) {
  if (isStrNumber(inputParameterString)) {
    return true;
  }
  return false;
}

//***CHECK IF STRING IS A NUMBER FUNCTION***//
// Function   : isStrNumber
//
// Description: This function checks if the input string is a number.
//              It returns true if the string includes all numeric characters.
//              It returns false if the string includes a non numeric character.
//
// Parameters :  str : String : The input string
//
// Return     : boolean
//******************************************//
boolean isStrNumber(String str) {
  boolean isNumber = false;
  for (byte i = 0; i < str.length(); i++) {
    isNumber = isDigit(str.charAt(i)) || str.charAt(i) == '+' || str.charAt(i) == '.' || str.charAt(i) == '-';
    if (!isNumber) {
      return false; // Non numeric character detected
    }
  }
  return true; // All numeric characters detected
}

//***CHECK IF CHAR IS A VALID DELIMITER FUNCTION***//
// Function   : isValidDelimiter
//
// Description: This function checks if the input char is a valid delimiter.
//              It returns true if the character is a valid delimiter.
//              It returns false if the character is not a valid delimiter.
//
// Parameters :  inputDelimiter : char : The input char delimiter
//
// Return     : boolean
//******************************************//
bool isValidDelimiter(char inputDelimiter) {
  bool validOutput;

  (inputDelimiter == ',' || inputDelimiter == ':' || inputDelimiter == '-') ? validOutput = true : validOutput = false;

  return validOutput;
}


//***GET MODEL NUMBER FUNCTION***//
// Function   : getModelNumber
//
// Description: This function retrieves the current LipSync firmware model number.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getModelNumber(bool responseEnabled, bool apiEnabled) {
  String commandKey = "MN";
  int tempModelNumber = mem.readInt(CONF_SETTINGS_FILE, commandKey);
  if (tempModelNumber != CONF_LIPSYNC_MODEL) {                          // If the previous firmware was different model then reset the settings
    resetSettings(false, false);

    tempModelNumber = CONF_LIPSYNC_MODEL;                               // And store the model number
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempModelNumber);
  }
  printResponseInt(responseEnabled, apiEnabled, true, 0, "MN,0", true, tempModelNumber);

}
//***GET MODEL NUMBER API FUNCTION***//
// Function   : getModelNumber
//
// Description: This function is redefinition of main getModelNumber function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getModelNumber(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getModelNumber(responseEnabled, apiEnabled);
  }
}

//***GET VERSION FUNCTION***//
// Function   : getVersionNumber
//
// Description: This function retrieves the current LipSync firmware version number.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getVersionNumber(bool responseEnabled, bool apiEnabled) {
  String commandKey = "VN";
  
  int tempMajorVersionNumber = mem.readInt(CONF_SETTINGS_FILE, "VN1");
  int tempMinorVersionNumber = mem.readInt(CONF_SETTINGS_FILE, "VN2");
  int tempRevVersionNumber = mem.readInt(CONF_SETTINGS_FILE, "VN3");
  
  if (tempMajorVersionNumber != CONF_LIPSYNC_VERSION_MAJOR) {                          // If the previous firmware was different version then reset the settings
    // could add factory reset here if version number saved in memory is different
    tempMajorVersionNumber = CONF_LIPSYNC_VERSION_MAJOR;                               // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN1", tempMajorVersionNumber);
    
    tempMinorVersionNumber = CONF_LIPSYNC_VERSION_MINOR;                               // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN2", tempMinorVersionNumber);

    tempRevVersionNumber = CONF_LIPSYNC_VERSION_REV;                               // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN3", tempRevVersionNumber);
  }

  else if (tempMinorVersionNumber != CONF_LIPSYNC_VERSION_MINOR) {                     // If the previous firmware was different version then reset the settings
    // could reset some settings here if version number saved in memory is different
    tempMinorVersionNumber = CONF_LIPSYNC_VERSION_MINOR;                               // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN2", tempMinorVersionNumber);

    tempRevVersionNumber = CONF_LIPSYNC_VERSION_REV;                                    // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN3", tempRevVersionNumber);
  }

  else if (tempRevVersionNumber != CONF_LIPSYNC_VERSION_REV) {                          // If the previous firmware was different version then reset the settings
    tempRevVersionNumber = CONF_LIPSYNC_VERSION_REV;                                    // And store the version number
    mem.writeInt(CONF_SETTINGS_FILE, "VN3", tempRevVersionNumber);
  }
  

  String tempLipsyncVersionStr = String(tempMajorVersionNumber) + "." + String(tempMinorVersionNumber) + "." + String(tempRevVersionNumber);
  
  printResponseString(responseEnabled, apiEnabled, true, 0, "VN,0", true, tempLipsyncVersionStr);
}
//***GET VERSION API FUNCTION***//
// Function   : getVersionNumber
//
// Description: This function is redefinition of main getVersionNumber function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getVersionNumber(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getVersionNumber(responseEnabled, apiEnabled);
  }
}

//***GET DEVICE ID FUNCTION***//
// Function   : getDeviceID
//
// Description: This function retrieves the UID from the device
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getDeviceID(bool responseEnabled, bool apiEnabled) {
  String commandKey = "ID";
  String tempDeviceID = mem.readString(CONF_SETTINGS_FILE, commandKey);
  String deviceID = readUID();
 
  if (tempDeviceID != deviceID) {  // If string doesn't match, store to memory
    mem.writeString(CONF_SETTINGS_FILE, commandKey, deviceID);
  }
  g_deviceUID = deviceID; // Update global variable
  
  printResponseString(responseEnabled, apiEnabled, true, 0, "ID,0", true, tempDeviceID);

}
//***GET DEVICE ID API FUNCTION***//
// Function   : getDeviceID
//
// Description: This function is redefinition of main getDeviceID function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getDeviceID(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getDeviceID(responseEnabled, apiEnabled);
  }
}

//***GET OPERATING MODE STATE FUNCTION***//
// Function   : getOperatingMode
//
// Description: This function retrieves the state of operating mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : operatingState : intol : The current state of operating mode.
//*********************************//
int getOperatingMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "OM";
  int tempOperatingMode;
  tempOperatingMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempOperatingMode < CONF_OPERATING_MODE_MIN) || (tempOperatingMode > CONF_OPERATING_MODE_MAX)) {
    tempOperatingMode = CONF_OPERATING_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempOperatingMode);
  }

  printResponseInt(responseEnabled, apiEnabled, true, 0, "OM,0", true, tempOperatingMode);

  return tempOperatingMode;
}
//***GET OPERATING MODE STATE API FUNCTION***//
// Function   : getOperatingMode
//
// Description: This function is redefinition of main getOperatingMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getOperatingMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getOperatingMode(responseEnabled, apiEnabled);
  }
}

//***SET OPERATING MODE STATE FUNCTION***//
// Function   : setOperatingMode
//
// Description: This function sets the state of operating mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputOperatingMode : int : The new operating mode state 
//
// Return     : void
//*********************************//
void setOperatingMode(bool responseEnabled, bool apiEnabled, int inputOperatingMode) {
  String commandKey = "OM";

  if ((inputOperatingMode >= CONF_OPERATING_MODE_MIN) && (inputOperatingMode <= CONF_OPERATING_MODE_MAX)) {   
    if ((g_comMode == CONF_COM_MODE_BLE) && (inputOperatingMode == CONF_OPERATING_MODE_GAMEPAD)){
      printResponseInt(responseEnabled, apiEnabled, false, 3, "OM,1", true, inputOperatingMode);    // Return error if user tries to change to Bluetooth mode while in Gamepad mode
    } else {
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputOperatingMode);
      printResponseInt(responseEnabled, apiEnabled, true, 0, "OM,1", true, inputOperatingMode);
      g_operatingMode = inputOperatingMode;
      changeOperatingMode(inputOperatingMode);
    }
  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "OM,1", true, inputOperatingMode);
  }

  // TODO: move softwareReset() here instead of in changeOperatingMode?

}
//***SET OPERATING MODE STATE API FUNCTION***//
// Function   : setOperatingMode
//
// Description: This function is redefinition of main setOperatingMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setOperatingMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setOperatingMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}


//***GET JOYSTICK CURSOR SPEED FUNCTION***//
// Function   : getCursorSpeed
//
// Description: This function retrieves the current joystick speed level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
int getCursorSpeed(bool responseEnabled, bool apiEnabled) {
  String commandKey = "SS";
  int tempCursorSpeedLevel = CONF_JOY_CURSOR_SPEED_LEVEL_DEFAULT;
  if (CONF_API_ENABLED) {
    tempCursorSpeedLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    if ((tempCursorSpeedLevel < CONF_JOY_CURSOR_SPEED_LEVEL_MIN) || (tempCursorSpeedLevel > CONF_JOY_CURSOR_SPEED_LEVEL_MAX)) {
      tempCursorSpeedLevel = CONF_JOY_CURSOR_SPEED_LEVEL_DEFAULT;
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempCursorSpeedLevel);
    }
    
  }
  js.setOutputRange(tempCursorSpeedLevel);
  printResponseInt(responseEnabled, apiEnabled, true, 0, "SS,0", true, tempCursorSpeedLevel);

  return tempCursorSpeedLevel;
}
//***GET JOYSTICK SPEED API FUNCTION***//
// Function   : getCursorSpeed
//
// Description: This function is redefinition of main getCursorSpeed function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getCursorSpeed(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getCursorSpeed(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK SPEED FUNCTION***//
// Function   : setCursorSpeed
//
// Description: This function sets the current joystick speed level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSpeedLevel : bool : The new joystick speed level.
//
// Return     : void
//*********************************//
void setCursorSpeed(bool responseEnabled, bool apiEnabled, int inputSpeedLevel) {
  String commandKey = "SS";
  bool isValidSpeed = true;
  int tempCursorSpeedLevel = inputSpeedLevel;
  
  if ((tempCursorSpeedLevel >= CONF_JOY_CURSOR_SPEED_LEVEL_MIN) && (tempCursorSpeedLevel <= CONF_JOY_CURSOR_SPEED_LEVEL_MAX)) { // Check if inputSpeedCounter is valid
    // Valid inputSpeedLevel
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempCursorSpeedLevel);
    if (!CONF_API_ENABLED) {
      tempCursorSpeedLevel = CONF_JOY_CURSOR_SPEED_LEVEL_DEFAULT;
    }
    performLedAction(ledCurrentState);
    isValidSpeed = true;
  }
  else {
    // Invalid inputSpeedLevel
    tempCursorSpeedLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    performLedAction(ledCurrentState);
    isValidSpeed = false;
  }

  js.setOutputRange(tempCursorSpeedLevel);

  int responseCode = 0;
  (isValidSpeed) ? responseCode = 0 : responseCode = 3;
  printResponseInt(responseEnabled, apiEnabled, isValidSpeed, responseCode, "SS,1", true, tempCursorSpeedLevel);
}
//***SET JOYSTICK SPEED API FUNCTION***//
// Function   : setCursorSpeed
//
// Description: This function is redefinition of main setCursorSpeed function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setCursorSpeed(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setCursorSpeed(responseEnabled, apiEnabled, optionalParameter.toInt());
}


//***GET SCROLL LEVEL FUNCTION***//
// Function   : getScrollLevel
//
// Description: This function retrieves the current scroll level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
int getScrollLevel(bool responseEnabled, bool apiEnabled) {
  String commandKey = "SL";
  int tempScrollLevel = CONF_SCROLL_LEVEL_DEFAULT;
  if (CONF_API_ENABLED) {
    tempScrollLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    if ((tempScrollLevel < CONF_SCROLL_LEVEL_MIN) || (tempScrollLevel > CONF_SCROLL_LEVEL_MAX)) {
      tempScrollLevel = CONF_SCROLL_LEVEL_DEFAULT;
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempScrollLevel);
    }
    
  }
  //js.setOutputRange(tempScrollLevel);
  printResponseInt(responseEnabled, apiEnabled, true, 0, "SL,0", true, tempScrollLevel);

  return tempScrollLevel;
}
//***GET JOYSTICK LEVEL FUNCTION***//
// Function   : getScrollLevel
//
// Description: This function is redefinition of main getScrollLevel function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getScrollLevel(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getScrollLevel(responseEnabled, apiEnabled);
  }
}

//***SET SCROLL LEVEL FUNCTION***//
// Function   : setScrollLevel
//
// Description: This function sets the current scroll level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputScrollLevel : bool : The new scroll speed level.
//
// Return     : void
//*********************************//
void setScrollLevel(bool responseEnabled, bool apiEnabled, int inputScrollLevel) {
  String commandKey = "SL";
  bool isValidLevel = true;
  int tempScrollLevel = inputScrollLevel;
  
  if ((tempScrollLevel >= CONF_SCROLL_LEVEL_MIN) && (tempScrollLevel <= CONF_SCROLL_LEVEL_MAX)) { // Check if inputLevelCounter is valid
    // Valid inputScrollLevel
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempScrollLevel);
    if (!CONF_API_ENABLED) {
      tempScrollLevel = CONF_SCROLL_LEVEL_DEFAULT;
    }
    performLedAction(ledCurrentState);
    isValidLevel = true;
    g_scrollLevel = tempScrollLevel;
  }
  else {
    // Invalid inputScrollLevel
    tempScrollLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    performLedAction(ledCurrentState);
    isValidLevel = false;
  }

  //js.setOutputRange(tempScrollLevel);
  int responseCode = 0;
  (isValidLevel) ? responseCode = 0 : responseCode = 3;
  printResponseInt(responseEnabled, apiEnabled, isValidLevel, responseCode, "SL,1", true, tempScrollLevel);
}
//***SET SCROLL LEVEL API FUNCTION***//
// Function   : setScrollLevel
//
// Description: This function is redefinition of main setScrollLevel function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setScrollLevel(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setScrollLevel(responseEnabled, apiEnabled, optionalParameter.toInt());
}


//***INCREASE JOYSTICK CURSOR SPEED LEVEL FUNCTION***//
// Function   : increaseCursorSpeed
//
// Description: This function increases the joystick cursor speed level by one.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void increaseCursorSpeed(bool responseEnabled, bool apiEnabled) {
  int tempCursorSpeedLevel = js.getOutputRange();

  tempCursorSpeedLevel++;
  if(tempCursorSpeedLevel <= CONF_JOY_CURSOR_SPEED_LEVEL_MAX) {  // Check validity of input
    setLedState(LED_ACTION_BLINK, 
    CONF_JOY_SPEED_INC_LED_COLOR, 
    CONF_JOY_SPEED_INC_LED_NUMBER, 
    CONF_JOY_SPEED_CHANGE_LED_BLINK, 
    CONF_JOY_SPEED_CHANGE_LED_DELAY, 
    led.getLedBrightness());   // Blink once
  } 
  else{
    setLedState(LED_ACTION_BLINK, 
    CONF_JOY_SPEED_LIMIT_LED_COLOR, 
    CONF_JOY_SPEED_INC_LED_NUMBER, 
    CONF_JOY_SPEED_LIMIT_LED_BLINK, 
    CONF_JOY_SPEED_LIMIT_LED_DELAY, 
    led.getLedBrightness());   // Blink 3 times
  }

  setCursorSpeed(responseEnabled, apiEnabled, tempCursorSpeedLevel);
}

//***DECREASE JOYSTICK CURSOR SPEED LEVEL FUNCTION***//
// Function   : decreaseCursorSpeed
//
// Description: This function decreases the joystick cursor speed level by one.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void decreaseCursorSpeed(bool responseEnabled, bool apiEnabled) {
  int tempCursorSpeedLevel = js.getOutputRange();

  tempCursorSpeedLevel--;
  if(tempCursorSpeedLevel >= CONF_JOY_CURSOR_SPEED_LEVEL_MIN) {
    setLedState(LED_ACTION_BLINK, 
    CONF_JOY_SPEED_DEC_LED_COLOR, 
    CONF_JOY_SPEED_DEC_LED_NUMBER, 
    CONF_JOY_SPEED_CHANGE_LED_BLINK, 
    CONF_JOY_SPEED_CHANGE_LED_DELAY, 
    led.getLedBrightness());   // Blink once
  } 
  else{
    setLedState(LED_ACTION_BLINK, 
    CONF_JOY_SPEED_LIMIT_LED_COLOR, 
    CONF_JOY_SPEED_DEC_LED_NUMBER, 
    CONF_JOY_SPEED_LIMIT_LED_BLINK, 
    CONF_JOY_SPEED_LIMIT_LED_DELAY, 
    led.getLedBrightness());   // Blink 3 times
  }
  
    
  setCursorSpeed(responseEnabled, apiEnabled, tempCursorSpeedLevel);
}


//***GET JOYSTICK INITIALIZATION FUNCTION***//
// Function   : getJoystickInitialization
//
// Description: This function retrieves the neutral values from joystick initialization.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getJoystickInitialization(bool responseEnabled, bool apiEnabled) {
  pointFloatType tempCenterPoint = js.getInputCenter();
  js.setMinimumRadius();                                                                        // Update the minimum cursor operating radius 
  printResponseFloatPoint(responseEnabled, apiEnabled, true, 0, "IN,0", true, tempCenterPoint);
}
//***GET JOYSTICK INITIALIZATION API FUNCTION***//
// Function   : getJoystickInitialization
//
// Description: This function is redefinition of main getJoystickInitialization function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickInitialization(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK INITIALIZATION FUNCTION***//
// Function   : setJoystickInitialization
//
// Description: This function performs joystick Initialization.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void setJoystickInitialization(bool responseEnabled, bool apiEnabled) {
  int stepNumber = 0;
  canOutputAction = false;
  calibrationTimerId[0] = calibrationTimer.setTimeout(CONF_JOY_INIT_START_DELAY, performJoystickCenter, (int *)stepNumber);  
}
//***SET JOYSTICK INITIALIZATION API FUNCTION***//
// Function   : setJoystickInitialization
//
// Description: This function is redefinition of main setJoystickInitialization function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setJoystickInitialization(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 1) {
    setJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//*** GET JOYSTICK CALIBRATION FUNCTION***//
// Function   : getJoystickCalibration
//
// Description: This function retrieves calibration values from joystick Calibration.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getJoystickCalibration(bool responseEnabled, bool apiEnabled) {
  String commandKey;
  pointFloatType calibrationPointArray[5];
  calibrationPointArray[0] = js.getInputCenter();
  for (int i = 1; i < 5; i++)
  {
    commandKey = "CA" + String(i);
    calibrationPointArray[i] = mem.readPoint(CONF_SETTINGS_FILE, commandKey);
    js.setInputMax(i, calibrationPointArray[i]);
  }
  js.setMinimumRadius();                                                                              // Update the minimum cursor operating radius 
  printResponseFloatPointArray(responseEnabled, apiEnabled, true, 0, "CA,0", true, "", 5, ',', calibrationPointArray);

}
//***GET JOYSTICK CALIBRATION API FUNCTION***//
// Function   : getJoystickCalibration
//
// Description: This function is redefinition of main getJoystickCalibration function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickCalibration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickCalibration(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK CALIBRATION FUNCTION***//
// Function   : setJoystickCalibration
//
// Description: This function starts the joystick Calibration.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void setJoystickCalibration(bool responseEnabled, bool apiEnabled) {
  js.clear();                                                                                           // Clear previous calibration values
  int stepNumber = 0;
  canOutputAction = false;
  calibrationTimerId[0] = calibrationTimer.setTimeout(CONF_JOY_CALIB_START_DELAY, performJoystickCalibration, (int *)stepNumber);  // Start the process
}
//***SET JOYSTICK CALIBRATION API FUNCTION***//
// Function   : setJoystickCalibration
//
// Description: This function is redefinition of main setJoystickCalibration function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setJoystickCalibration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 1) {
    setJoystickCalibration(responseEnabled, apiEnabled);
  }
}


//*** GET JOYSTICK DEADZONE FUNCTION***//
// Function   : getJoystickInnerDeadzone
//
// Description: This function retrieves the joystick deadzone.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
float getJoystickInnerDeadzone(bool responseEnabled, bool apiEnabled) {
  String deadzoneCommand = "IZ";
  float tempDeadzone;
  tempDeadzone = mem.readFloat(CONF_SETTINGS_FILE, deadzoneCommand);

  if ((tempDeadzone <= CONF_JOY_DEADZONE_MIN) || (tempDeadzone >= CONF_JOY_DEADZONE_MAX)) {
    tempDeadzone = CONF_JOY_DEADZONE_INNER_DEFAULT;
    mem.writeFloat(CONF_SETTINGS_FILE, deadzoneCommand, tempDeadzone);
  }
  js.setInnerDeadzone(true, tempDeadzone);
  printResponseFloat(responseEnabled, apiEnabled, true, 0, "IZ,0", true, tempDeadzone);
  return tempDeadzone;
}
//***GET JOYSTICK DEADZONE API FUNCTION***//
// Function   : getJoystickInnerDeadzone
//
// Description: This function is redefinition of main getJoystickInnerDeadzone function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickInnerDeadzone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickInnerDeadzone(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK DEADZONE FUNCTION***//
// Function   : setJoystickInnerDeadzone
//
// Description: This function starts the joystick deadzone.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void setJoystickInnerDeadzone(bool responseEnabled, bool apiEnabled, float inputDeadzone) {
  String deadzoneCommand = "IZ";
  if ((inputDeadzone > CONF_JOY_DEADZONE_MIN) && (inputDeadzone <= CONF_JOY_DEADZONE_MAX)) {
    mem.writeFloat(CONF_SETTINGS_FILE, deadzoneCommand, inputDeadzone);
    js.setInnerDeadzone(true, inputDeadzone);
    printResponseFloat(responseEnabled, apiEnabled, true, 0, "IZ,1", true, inputDeadzone);
  }
  else {
    printResponseFloat(responseEnabled, apiEnabled, false, 3, "IZ,1", true, inputDeadzone);
  }
}
//***SET JOYSTICK DEADZONE API FUNCTION***//
// Function   : setJoystickInnerDeadzone
//
// Description: This function is redefinition of main setJoystickInnerDeadzone function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setJoystickInnerDeadzone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setJoystickInnerDeadzone(responseEnabled, apiEnabled, optionalParameter.toFloat());
}

//*** GET JOYSTICK OUTER DEADZONE FUNCTION***//
// Function   : getJoystickOuterDeadzone
//
// Description: This function retrieves the joystick outer deadzone for max output.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
float getJoystickOuterDeadzone(bool responseEnabled, bool apiEnabled) {
  String outerDeadzoneCommand = "OZ";
  float tempOuterDeadzone;
  tempOuterDeadzone = mem.readFloat(CONF_SETTINGS_FILE, outerDeadzoneCommand);

  if ((tempOuterDeadzone <= CONF_JOY_DEADZONE_MIN) || (tempOuterDeadzone >= CONF_JOY_DEADZONE_MAX)) {
    tempOuterDeadzone = CONF_JOY_DEADZONE_OUTER_DEFAULT;
    mem.writeFloat(CONF_SETTINGS_FILE, outerDeadzoneCommand, tempOuterDeadzone);
  }
  js.setOuterDeadzone(true, tempOuterDeadzone);
  printResponseFloat(responseEnabled, apiEnabled, true, 0, "OZ,0", true, tempOuterDeadzone);
  return tempOuterDeadzone;
}
//***GET JOYSTICK OUTER DEADZONE API FUNCTION***//
// Function   : getJoystickInnerDeadzone
//
// Description: This function is redefinition of main getJoystickOuterDeadzone function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickOuterDeadzone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickOuterDeadzone(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK OUTER DEADZONE FUNCTION***//
// Function   : setJoystickOuterDeadzone
//
// Description: This function starts the joystick deadzone.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputUpperDeadZone : float : The new upper deadzone value.
//
// Return     : void
//*********************************//
void setJoystickOuterDeadzone(bool responseEnabled, bool apiEnabled, float inputUpperDeadZone) {
  String outerDeadzoneCommand = "OZ";
  if ((inputUpperDeadZone > CONF_JOY_DEADZONE_MIN) && (inputUpperDeadZone <= CONF_JOY_DEADZONE_MAX)) {
    mem.writeFloat(CONF_SETTINGS_FILE, outerDeadzoneCommand, inputUpperDeadZone);
    js.setOuterDeadzone(true, inputUpperDeadZone);
    printResponseFloat(responseEnabled, apiEnabled, true, 0, "OZ,1", true, inputUpperDeadZone);
  }
  else {
    printResponseFloat(responseEnabled, apiEnabled, false, 3, "OZ,1", true, inputUpperDeadZone);
  }
}
//***SET JOYSTICK OUTER DEADZONE API FUNCTION***//
// Function   : setJoystickOuterDeadzone
//
// Description: This function is redefinition of main setJoystickOuterDeadzone function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setJoystickOuterDeadzone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setJoystickOuterDeadzone(responseEnabled, apiEnabled, optionalParameter.toFloat());
}

//***GET JOYSTICK VALUE FUNCTION***//
// Function   : getJoystickValue
//
// Description: This function returns raw joystick value.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getJoystickValue(bool responseEnabled, bool apiEnabled) {
  js.update();      // Request new values from joystick class
  
  const int outputArraySize = 6; 
  float tempJoystickArray[outputArraySize];
  
  pointFloatType tempJoystickRaw = js.getXYRaw();                                 // Read the raw values
  pointIntType tempJoystickIn = js.getXYIn();                                     // Read the input
  pointIntType tempJoystickProcessed = js.getXYOut();                             // Read the filtered values  
  
  tempJoystickArray[0] = tempJoystickRaw.x;
  tempJoystickArray[1] = tempJoystickRaw.y;
  tempJoystickArray[2] = (float)tempJoystickIn.x;
  tempJoystickArray[3] = (float)tempJoystickIn.y;
  tempJoystickArray[4] = (float)tempJoystickProcessed.x;
  tempJoystickArray[5] = (float)tempJoystickProcessed.y;
  
  printResponseFloatArray(responseEnabled, apiEnabled, true, 0, "JV,0", true, "", outputArraySize, ',', tempJoystickArray);
  
}

//***GET JOYSTICK VALUE API FUNCTION***//
// Function   : getJoystickValue
//
// Description: This function is redefinition of main getJoystickValue function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickValue(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickValue(responseEnabled, apiEnabled);
  }
}

//***GET PRESSURE VALUE FUNCTION***//
// Function   : getPressureValue
//
// Description: This function returns pressure difference.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void getPressureValue(bool responseEnabled, bool apiEnabled) {

  ps.update();      // Request new values from pressure class
  
  const int outputArraySize = 3;   
  float tempSapPressure[outputArraySize];
  tempSapPressure[0] = ps.getSapPressureAbs();    // Read the mouthpiece pressure 
  tempSapPressure[1] = ps.getAmbientPressure();   // Read the ambient pressure
  tempSapPressure[2] = ps.getSapPressure();       // Read the diff pressure

  
  printResponseFloatArray(responseEnabled, apiEnabled, true, 0, "PV,0", true, "", outputArraySize, ',', tempSapPressure);
}

//***GET PRESSURE VALUE API FUNCTION***//
// Function   : getPressureValue
//
// Description: This function is redefinition of main getPressureValue function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getPressureValue(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getPressureValue(responseEnabled, apiEnabled);
  }
}

//***GET PRESSURE MODE FUNCTION***//
// Function   : getPressureMode
//
// Description: This function retrieves the state of pressure mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : PressureMode : int : The current state of pressure mode.
//*********************************//
int getPressureMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "PM";
  int tempPressureMode;
  tempPressureMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempPressureMode < PRESS_MODE_MIN) || (tempPressureMode > PRESS_MODE_MAX)) {
    tempPressureMode = CONF_PRESS_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempPressureMode);
  }
  ps.setPressureMode(tempPressureMode);

  printResponseInt(responseEnabled, apiEnabled, true, 0, "PM,0", true, tempPressureMode);

  return tempPressureMode;
}

//***GET PRESSURE MODE API FUNCTION***//
// Function   : getPressureMode
//
// Description: This function is redefinition of main getPressureMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getPressureMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getPressureMode(responseEnabled, apiEnabled);
  }
}

//***SET PRESSURE MODE FUNCTION***//
// Function   : setPressureMode
//
// Description: This function sets the state of Pressure mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputPressureMode : int : The new pressure mode state ( 0 = None, 1 = Abs , 2 = Diff )
//
// Return     : void
//*********************************//
void setPressureMode(bool responseEnabled, bool apiEnabled, int inputPressureMode) {
  String commandKey = "PM";
  if ((inputPressureMode >= PRESS_MODE_MIN) && (inputPressureMode <= PRESS_MODE_MAX)) {
    //g_comMode = inputPressureMode;  // TODO: fix
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputPressureMode);
    printResponseInt(responseEnabled, apiEnabled, true, 0, "PM,1", true, inputPressureMode);
  ps.setPressureMode(inputPressureMode);
  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "PM,1", true, inputPressureMode);

  }

}

//***SET PRESSURE MODE API FUNCTION***//
// Function   : setPressureMode
//
// Description: This function is redefinition of main setPressureMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setPressureMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setPressureMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}


//***GET SIP PRESSURE THRESHOLD FUNCTION***//
// Function   : getSipPressureThreshold
//
// Description: This function returns the current sip pressure threshold.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
float getSipPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String commandKey = "ST";
  float tempSipThreshold;
  tempSipThreshold = mem.readFloat(CONF_SETTINGS_FILE, commandKey);

  if ((tempSipThreshold <= 0.0) || (tempSipThreshold >= CONF_PRESS_MAX_THRESHOLD)) {
    tempSipThreshold = CONF_SIP_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE, commandKey, tempSipThreshold);
  }
  ps.setSipThreshold(tempSipThreshold);
  printResponseFloat(responseEnabled, apiEnabled, true, 0, "ST,0", true, tempSipThreshold);
  return tempSipThreshold;
}

//***GET PRESSURE THRESHOLD API FUNCTION***//
// Function   : getSipPressureThreshold
//
// Description: This function is redefinition of main getSipPressureThreshold function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getSipPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getSipPressureThreshold(responseEnabled, apiEnabled);
  }
}

//***SET SIP PRESSURE THRESHOLD FUNCTION***//
// Function   : setSipPressureThreshold
//
// Description: This function sets the current Sip pressure threshold.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSipThreshold : float : The new sip pressure threshold.
//
// Return     : void
//*********************************//
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, float inputSipThreshold) {
  String commandKey = "ST";

  if (inputSipThreshold >= CONF_PRESS_MIN_THRESHOLD && inputSipThreshold < CONF_PRESS_MAX_THRESHOLD) {
    mem.writeFloat(CONF_SETTINGS_FILE, commandKey, inputSipThreshold);
    ps.setSipThreshold(inputSipThreshold);
    printResponseFloat(responseEnabled, apiEnabled, true, 0, "ST,1", true, inputSipThreshold);
  }
  else {
    printResponseFloat(responseEnabled, apiEnabled, false, 3, "ST,1", true, inputSipThreshold);

  }
}

//***SET SIP PRESSURE THRESHOLD API FUNCTION***//
// Function   : setSipPressureThreshold
//
// Description: This function is redefinition of main setSipPressureThreshold function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setSipPressureThreshold(responseEnabled, apiEnabled, optionalParameter.toFloat());
}

//***GET PUFF THRESHOLD FUNCTION***//
// Function   : getPuffPressureThreshold
//
// Description: This function returns the puff pressure threshold.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
float getPuffPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String commandKey = "PT";
  float tempPuffThreshold;
  tempPuffThreshold = mem.readFloat(CONF_SETTINGS_FILE, commandKey);

  if ((tempPuffThreshold <= 0.0) || (tempPuffThreshold >= CONF_PRESS_MAX_THRESHOLD)) {
    tempPuffThreshold = CONF_PUFF_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE, commandKey, tempPuffThreshold);
  }
  ps.setPuffThreshold(tempPuffThreshold);
  printResponseFloat(responseEnabled, apiEnabled, true, 0, "PT,0", true, tempPuffThreshold);

  return tempPuffThreshold;
}

//***GET PUFF THRESHOLD API FUNCTION***//
// Function   : getPuffPressureThreshold
//
// Description: This function is redefinition of main getPuffPressureThreshold function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getPuffPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getPuffPressureThreshold(responseEnabled, apiEnabled);
  }
}


//***SET PUFF PRESSURE THRESHOLD FUNCTION***//
// Function   : setPuffPressureThreshold
//
// Description: This function sets the current Puff pressure threshold in percentage.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputPuffThreshold : float : The new sip pressure threshold in percentage.
//
// Return     : void
//*********************************//
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, float inputPuffThreshold) {
  String commandKey = "PT";

  if (inputPuffThreshold >= CONF_PRESS_MIN_THRESHOLD && inputPuffThreshold < CONF_PRESS_MAX_THRESHOLD) {
    mem.writeFloat(CONF_SETTINGS_FILE, commandKey, inputPuffThreshold);
    ps.setPuffThreshold(inputPuffThreshold);
    printResponseFloat(responseEnabled, apiEnabled, true, 0, "PT,1", true, inputPuffThreshold);
  }
  else {
    printResponseFloat(responseEnabled, apiEnabled, false, 3, "PT,1", true, inputPuffThreshold);

  }
}
//***SET PUFF PRESSURE THRESHOLD API FUNCTION***//
// Function   : setPuffPressureThreshold
//
// Description: This function is redefinition of main setPuffPressureThreshold function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setPuffPressureThreshold(responseEnabled, apiEnabled, optionalParameter.toFloat());
}

//***GET JOYSTICK ACCELERATION FUNCTION***//
// Function   : getJoystickAcceleration
//
// Description: This function retrieves the current joystick speed level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
int getJoystickAcceleration(bool responseEnabled, bool apiEnabled) {
  String commandKey = "AV";
  int tempJoystickAccelerationLevel = CONF_JOY_ACCELERATION_LEVEL_DEFAULT;
  if (CONF_API_ENABLED) {
    tempJoystickAccelerationLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    if ((tempJoystickAccelerationLevel < CONF_JOY_ACCELERATION_LEVEL_MIN) || (tempJoystickAccelerationLevel > CONF_JOY_ACCELERATION_LEVEL_MAX)) {
      tempJoystickAccelerationLevel = CONF_JOY_ACCELERATION_LEVEL_DEFAULT;
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempJoystickAccelerationLevel);
    }
    
  }
  printResponseInt(responseEnabled, apiEnabled, true, 0, "AV,0", true, tempJoystickAccelerationLevel);

  return tempJoystickAccelerationLevel;
}
//***GET JOYSTICK ACCELERATION IO API FUNCTION***//
// Function   : getJoystickAcceleration
//
// Description: This function is redefinition of main getJoystickAcceleration function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getJoystickAcceleration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getJoystickAcceleration(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK ACCELERATION FUNCTION***//
// Function   : setJoystickAcceleration
//
// Description: This function sets the current joystick acceleration level.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputAccelerationLevel : bool : The new joystick acceleration level.
//
// Return     : void
//*********************************//
void setJoystickAcceleration(bool responseEnabled, bool apiEnabled, int inputAccelerationLevel) {
  String commandKey = "AV";
  bool isValidAcceleration = true;
  int tempJoystickAccelerationLevel = inputAccelerationLevel;
  
  if ((tempJoystickAccelerationLevel >= CONF_JOY_ACCELERATION_LEVEL_MIN) && (tempJoystickAccelerationLevel <= CONF_JOY_ACCELERATION_LEVEL_MAX)) { // Check if inputAccelerationCounter is valid
    // Valid inputAccelerationLevel
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempJoystickAccelerationLevel);
    if (!CONF_API_ENABLED) {
      tempJoystickAccelerationLevel = CONF_JOY_ACCELERATION_LEVEL_DEFAULT;
    }
    isValidAcceleration = true;
  }
  else {
    // Invalid inputAccelerationLevel
    tempJoystickAccelerationLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    isValidAcceleration = false;
  }


  int responseCode = 0;
  (isValidAcceleration) ? responseCode = 0 : responseCode = 3;
  printResponseInt(responseEnabled, apiEnabled, isValidAcceleration, responseCode, "AV,1", true, tempJoystickAccelerationLevel);
}
//***SET JOYSTICK ACCELERATION API FUNCTION***//
// Function   : setJoystickAcceleration
//
// Description: This function is redefinition of main setJoystickAcceleration function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setJoystickAcceleration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setJoystickAcceleration(responseEnabled, apiEnabled, optionalParameter.toInt());
}



//***INCREASE/ACCELERATE JOYSTICK MOVEMENT FUNCTION***//
// Function   : increaseJoystickAcceleration
//
// Description: This function increases the joystick acceleration level by one.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void increaseJoystickAcceleration(bool responseEnabled, bool apiEnabled) {
  int tempJoystickAccelerationLevel=acceleration;
  tempJoystickAccelerationLevel++;
  if(tempJoystickAccelerationLevel <= CONF_JOY_ACCELERATION_LEVEL_MAX) {
    setJoystickAcceleration(responseEnabled, apiEnabled, tempJoystickAccelerationLevel);
  } 
  else {
  }

}

//***DECREASE/DECELERATE JOYSTICK MOVEMENT FUNCTION***//
// Function   : decreaseJoystickAcceleration
//
// Description: This function decreases the joystick acceleration level by one.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//*********************************//
void decreaseJoystickAcceleration(bool responseEnabled, bool apiEnabled) {
  int tempJoystickAccelerationLevel=acceleration;
  tempJoystickAccelerationLevel--;
  if(tempJoystickAccelerationLevel <= CONF_JOY_ACCELERATION_LEVEL_MIN){
    setJoystickAcceleration(responseEnabled, apiEnabled, tempJoystickAccelerationLevel);
  } 
  else{
  }

}

//***GET COMMUNICATION MODE FUNCTION***//
// Function   : getCommunicationMode
//
// Description: This function retrieves the state of communication mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : communicationMode : int : The current state of communication method.
//*********************************//
int getCommunicationMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "CM";
  int tempComMode;
  tempComMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempComMode < CONF_COM_MODE_MIN) || (tempComMode > CONF_COM_MODE_MAX)) {
    tempComMode = CONF_COM_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempComMode);
  }

  printResponseInt(responseEnabled, apiEnabled, true, 0, "CM,0", true, tempComMode);

  return tempComMode;
}
//***GET COMMUNICATION MODE API FUNCTION***//
// Function   : getCommunicationMode
//
// Description: This function is redefinition of main getCommunicationMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getCommunicationMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getCommunicationMode(responseEnabled, apiEnabled);
  }
}

//***SET COMMUNICATION MODE FUNCTION***//
// Function   : setCommunicationMode
//
// Description: This function sets the state of communication mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputCommunicationMode : int : The new communication mode state ( 0 = None, 1 = USB , 2 = BLE )
//
// Return     : void
//*********************************//
void setCommunicationMode(bool responseEnabled, bool apiEnabled, int inputCommunicationMode) {
  String commandKey = "CM";
  
  if ((inputCommunicationMode >= CONF_COM_MODE_MIN) && (inputCommunicationMode <= CONF_COM_MODE_MAX)) {
    if ((inputCommunicationMode == CONF_COM_MODE_BLE) && (g_operatingMode == CONF_OPERATING_MODE_GAMEPAD)){
      printResponseInt(responseEnabled, apiEnabled, false, 3, "CM,1", true, inputCommunicationMode);    // Return error if user tries to change to Bluetooth mode while in Gamepad mode
    } else {
      g_comMode = inputCommunicationMode;
      setCommunicationModeLed(g_comMode);
      setLedDefault();
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputCommunicationMode);
      printResponseInt(responseEnabled, apiEnabled, true, 0, "CM,1", true, inputCommunicationMode);
    }

    // TODO: move this?
    releaseOutputAction();
    switch(g_comMode) {
      case CONF_COM_MODE_USB:       // USB Mouse
        btmouse.end();
        usbmouse.begin();    
        break;
      case CONF_COM_MODE_BLE:       // Bluetooth Mouse
        usbmouse.end();
        btmouse.begin();
        break;
    }

  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "CM,1", true, inputCommunicationMode);

  }

}

//***SET COMMUNICATION MODE LED FUNCTION***//
// Function   : setCommunicationModeLed
//
// Description: This function sets the color communication mode blink LED action.
//
// Parameters :  inputCommunicationMode : int : The new communication mode state ( 0 = None, 1 = USB , 2 = BLE )
//
// Return     : void
//*********************************//
void setCommunicationModeLed(int inputCommunicationMode) {
    int modeLedColor = LED_CLR_NONE;
    if(inputCommunicationMode==CONF_COM_MODE_USB) {
      modeLedColor = LED_CLR_WHITE;
    } else if (inputCommunicationMode==CONF_COM_MODE_BLE) {
      modeLedColor = LED_CLR_BLUE;
    }
    setLedState(LED_ACTION_BLINK, modeLedColor, CONF_COM_MODE_LED_NUMBER, CONF_COM_MODE_LED_BLINK, CONF_COM_MODE_LED_BLINK_DELAY,led.getLedBrightness());    
    performLedAction(ledCurrentState);   
}
//***SET COMMUNICATION MODE API FUNCTION***//
// Function   : setCommunicationMode
//
// Description: This function is redefinition of main setCommunicationMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setCommunicationMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setCommunicationMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}


//***TOGGLE COMMUNICATION MODE FUNCTION***//
// Function   : toggleCommunicationMode
//
// Description: This function is used to toggle or increment communication mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
void toggleCommunicationMode(bool responseEnabled, bool apiEnabled) {
  if (g_comMode < CONF_COM_MODE_MAX) {
    g_comMode++;
  }
  else {
    g_comMode = CONF_COM_MODE_MIN;
  }
  setCommunicationMode(responseEnabled, apiEnabled, g_comMode);
}

//***GET SOUND MODE STATE FUNCTION***//
// Function   : getSoundMode
//
// Description: This function retrieves the state of sound mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : debugState : intol : The current state of sound mode.
//*********************************//
int getSoundMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "SM";
  int tempSoundMode;
  tempSoundMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempSoundMode < CONF_SOUND_MODE_MIN) || (tempSoundMode > CONF_SOUND_MODE_MAX)) {
    tempSoundMode = CONF_SOUND_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempSoundMode);
  }

  printResponseInt(responseEnabled, apiEnabled, true, 0, "SM,0", true, tempSoundMode);

  return tempSoundMode;
}

//***GET SOUND MODE STATE API FUNCTION***//
// Function   : getSoundMode
//
// Description: This function is redefinition of main getSoundMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getSoundMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getSoundMode(responseEnabled, apiEnabled);
  }
}

//***SET SOUND MODE STATE FUNCTION***//
// Function   : setSoundMode
//
// Description: This function sets the state of sound mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSoundMode : int : The new sound mode state.
//
// Return     : void
//*********************************//
void setSoundMode(bool responseEnabled, bool apiEnabled, int inputSoundMode) {
  String commandKey = "SM";

  if ((inputSoundMode >= CONF_SOUND_MODE_MIN) && (inputSoundMode <= CONF_SOUND_MODE_MAX)) {
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputSoundMode);
    g_soundMode = inputSoundMode;  // Update global variable TODO - currently unused
    buzzer.setSoundModeLevel(inputSoundMode);  // Update sound mode level within buzzer instance
    printResponseInt(responseEnabled, apiEnabled, true, 0, "SM,1", true, inputSoundMode);

  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "SM,1", true, inputSoundMode);

  }

}
//***SET SOUND MODE STATE API FUNCTION***//
// Function   : setSoundMode
//
// Description: This function is redefinition of main setSoundMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setSoundMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setSoundMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}

//***GET LIGHT MODE STATE FUNCTION***//
// Function   : getLightMode
//
// Description: This function retrieves the state of light mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : tempLightMode : int : The current state of light mode.
//*********************************//
int getLightMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "LM";
  int tempLightMode;
  tempLightMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempLightMode < CONF_LIGHT_MODE_MIN) || (tempLightMode > CONF_LIGHT_MODE_MAX)) {
    tempLightMode = CONF_LIGHT_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempLightMode);
  }

  printResponseInt(responseEnabled, apiEnabled, true, 0, "LM,0", true, tempLightMode);

  return tempLightMode;
}

//***GET LIGHT MODE STATE API FUNCTION***//
// Function   : getLightMode
//
// Description: This function is redefinition of main getLightMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getLightMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getLightMode(responseEnabled, apiEnabled);
  }
}

//***SET LIGHT MODE STATE FUNCTION***//
// Function   : setLightMode
//
// Description: This function sets the state of sound mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLightMode : int : The new light mode state 
//
// Return     : void
//*********************************//
void setLightMode(bool responseEnabled, bool apiEnabled, int inputLightMode) {
  String commandKey = "LM";

  if ((inputLightMode >= CONF_LIGHT_MODE_MIN) && (inputLightMode <= CONF_LIGHT_MODE_MAX)) {
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputLightMode);
    g_lightMode = inputLightMode;
    led.setLightModeLevel(inputLightMode);
    printResponseInt(responseEnabled, apiEnabled, true, 0, "LM,1", true, inputLightMode);
  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "LM,1", true, inputLightMode);

  }

}

//***SET LIGHT MODE STATE API FUNCTION***//
// Function   : setLightMode
//
// Description: This function is redefinition of main setLightMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setLightMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setLightMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}

// *********************************************************************************

//***GET LIGHT BRIGHTNESS LEVEL FUNCTION***//
// Function   : getLightBrightnessLevel
//
// Description: This function retrieves the level of the light brightness.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : tempLightLevel : int : The current light brightness level (0-10)
//*********************************//
int getLightBrightnessLevel(bool responseEnabled, bool apiEnabled) {
  String commandKey = "LL";
  int tempLightLevel;
  tempLightLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempLightLevel < CONF_LED_BRIGHTNESS_LEVEL_MIN) || (tempLightLevel > CONF_LED_BRIGHTNESS_LEVEL_MAX)) {
    tempLightLevel = CONF_LED_BRIGHTNESS_LEVEL_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempLightLevel);
  }

  printResponseInt(responseEnabled, apiEnabled, true, 0, "LL,0", true, tempLightLevel);

  return tempLightLevel;
}

//***GET LIGHT BRIGHTNESS LEVEL API FUNCTION***//
// Function   : getLightBrightnessLevel
//
// Description: This function is redefinition of main getLightBrightnessLevel function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getLightBrightnessLevel(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getLightBrightnessLevel(responseEnabled, apiEnabled);
  }
}

//***SET LIGHT BRIGHTNESS LEVEL FUNCTION***//
// Function   : setLightBrightnessLevel
//
// Description: This function sets the level of the light brightness.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLightLevel : int : The new light brightness level (0-10)
//
// Return     : void
//*********************************//
void setLightBrightnessLevel(bool responseEnabled, bool apiEnabled, int inputLightLevel) {
  String commandKey = "LL";

  if ((inputLightLevel >= CONF_LED_BRIGHTNESS_LEVEL_MIN) && (inputLightLevel <= CONF_LED_BRIGHTNESS_LEVEL_MAX)) {
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputLightLevel);
    led.setLedBrightnessLevel(inputLightLevel);
    printResponseInt(responseEnabled, apiEnabled, true, 0, "LL,1", true, inputLightLevel);
  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "LL,1", true, inputLightLevel);

  }

  //TODO: Add LED blink
  setLedState(LED_ACTION_BLINK,
            LED_CLR_RED, 
            CONF_LED_ALL, 
            1,                     // number of blinks
            1000,                     // blink time
            led.getLedBrightness());  // brightness
  // Perform led action
  performLedAction(ledCurrentState);

}

//***SET LIGHT BRIGHTNESS LEVEL API FUNCTION***//
// Function   : setLightBrightnessLevel
//
// Description: This function is redefinition of main setLightBrightnessLevel function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of the input light level.
//
// Return     : void
void setLightBrightnessLevel(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setLightBrightnessLevel(responseEnabled, apiEnabled, optionalParameter.toInt());
}

// *********************************************************************************

//***OPEN HUB MENU FUNCTION***//
// Function   : openHubMenu
//
// Description: This function opens the Hub Menu.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLightMode : int : The new debug mode state ( true = ON , false = OFF )
//
// Return     : void
//*********************************//
void controlHubMenu(bool responseEnabled, bool apiEnabled, int inputMenuControl) {
  String commandKey = "CH";

  if ((inputMenuControl >= CONF_MENU_CONTROL_MIN) && (inputMenuControl <= CONF_MENU_CONTROL_MAX)) {
    switch(inputMenuControl) {
      case CONF_MENU_CONTROL_OPEN: 
        screen.activateMenu();
        break;     
      case CONF_MENU_CONTROL_SELECT:
        screen.selectMenuItem();
        break;
      case CONF_MENU_CONTROL_NEXT:
        screen.nextMenuItem();
        break;     
      case CONF_MENU_CONTROL_CLOSE:
        screen.deactivateMenu();
        break;      
    } // end switch
   
    printResponseInt(responseEnabled, apiEnabled, true, 0, "CH,1", true, inputMenuControl);

  } // end if valid inputMenuControl
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "CH,1", true, inputMenuControl);

  }

}
//***CONTROL HUB MENU API FUNCTION***//
// Function   : controlHubMenu
//
// Description: This function is redefinition of main controlHubMenu function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void controlHubMenu(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  controlHubMenu(responseEnabled, apiEnabled, optionalParameter.toInt());
}

// *********************************************************************************

//***GET DEBUG MODE STATE FUNCTION***//
// Function   : getDebugMode
//
// Description: This function retrieves the state of debug mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : debugState : intol : The current state of debug mode.
//*********************************//
int getDebugMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "DM";
  int tempDebugMode;
  tempDebugMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if ((tempDebugMode < CONF_DEBUG_MODE_MIN) || (tempDebugMode > CONF_DEBUG_MODE_MAX)) {
    tempDebugMode = CONF_DEBUG_MODE_DEFAULT;
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempDebugMode);
  }

  setDebugState(tempDebugMode);

  printResponseInt(responseEnabled, apiEnabled, true, 0, "DM,0", true, tempDebugMode);

  return tempDebugMode;
}

//***GET DEBUG MODE STATE API FUNCTION***//
// Function   : getDebugMode
//
// Description: This function is redefinition of main getDebugMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void getDebugMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 0) {
    getDebugMode(responseEnabled, apiEnabled);
  }
}

//***SET DEBUG MODE STATE FUNCTION***//
// Function   : setDebugMode
//
// Description: This function sets the state of debug mode.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputDebugStateMode : int : The new debug mode state ( true = ON , false = OFF )
//
// Return     : void
//*********************************//
void setDebugMode(bool responseEnabled, bool apiEnabled, int inputDebugMode) {
  String commandKey = "DM";

  if ((inputDebugMode >= CONF_DEBUG_MODE_MIN) && (inputDebugMode <= CONF_DEBUG_MODE_MAX)) {
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, inputDebugMode);
    g_debugMode = inputDebugMode;
    setDebugState(inputDebugMode);
    printResponseInt(responseEnabled, apiEnabled, true, 0, "DM,1", true, inputDebugMode);
  }
  else {
    printResponseInt(responseEnabled, apiEnabled, false, 3, "DM,1", true, inputDebugMode);
  }

}

//***SET DEBUG MODE STATE API FUNCTION***//
// Function   : setDebugMode
//
// Description: This function is redefinition of main setDebugMode function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void setDebugMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setDebugMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}

//***RUN TEST FUNCTION***//
// Function   : runTest
//
// Description: This function activates an internal test.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               intputTest : int : The id of test to run.
//
// Return     : void
//*********************************//
void runTest(bool responseEnabled, bool apiEnabled, int inputTest) {
  String commandKey = "RT";

  if ((inputTest >= CONF_TEST_MODE_MIN) && (inputTest <= CONF_TEST_MODE_MAX)) { 
    printResponseInt(responseEnabled, apiEnabled, true, 0, "RT,1", true, inputTest);
    activateTest(inputTest); // run the test
  }
  else { // error message
    printResponseInt(responseEnabled, apiEnabled, false, 3, "RT,1", true, inputTest);
  }
}

//***RUN TEST API FUNCTION***//
// Function   : runTest
//
// Description: This function is redefinition of main runTest function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void runTest(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  runTest(responseEnabled, apiEnabled, optionalParameter.toInt());
}

//***SOFT RESET FUNCTION***//
// Function   : softReset
//
// Description: This function performs a software reset.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//***************************//
void softReset(bool responseEnabled, bool apiEnabled) {
  printResponseInt(responseEnabled, apiEnabled, true, 0, "SR,1", true, 1);
  softwareReset();
}

//***FACTORY RESET API FUNCTION***//
// Function   : softReset
//
// Description: This function is redefinition of main softRest function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void softReset(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 1) {
    softReset(responseEnabled, apiEnabled);
  }
}

//***RESET SETTINGS FUNCTION***//
// Function   : resetSettings
//
// Description: This function performs reset.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//***************************//
void resetSettings(bool responseEnabled, bool apiEnabled) {
  printResponseInt(responseEnabled, apiEnabled, true, 0, "RS,1", true, 0);
}

//***RESET SETTINGS API FUNCTION***//
// Function   : resetSettings
//
// Description: This function is redefinition of main resetSettings function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void resetSettings(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 1) {
    resetSettings(responseEnabled, apiEnabled);
  }
}

//***DO FACTORY RESET FUNCTION***//
// Function   : doFactoryReset
//
// Description: This function does a factory reset.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//
// Return     : void
//***************************//
void doFactoryReset(bool responseEnabled, bool apiEnabled) {

  // Set all LEDs to red to indicate factory reset process 
  setLedState(LED_ACTION_ON, LED_CLR_RED, CONF_LED_ALL, 0, 0, CONF_LED_BRIGHTNESS_MAX);                           
  performLedAction(ledCurrentState);  

  // Factory reset process 
  Serial.println("FACTORY RESET TRIGGERED");
  
  resetMemory();  // Format and reinitialize internal file system 

  setCommunicationMode(false, false, CONF_COM_MODE_DEFAULT);
  setOperatingMode(false, false, CONF_OPERATING_MODE_DEFAULT);
  setDebugMode(false, false, CONF_DEBUG_MODE_DEFAULT);
  setJoystickInnerDeadzone(false, false, CONF_JOY_DEADZONE_INNER_DEFAULT);
  setJoystickOuterDeadzone(false, false, CONF_JOY_DEADZONE_OUTER_DEFAULT);
  setSipPressureThreshold(false, false, CONF_SIP_THRESHOLD);
  setPuffPressureThreshold(false, false, CONF_PUFF_THRESHOLD);
  setCursorSpeed(false, false, CONF_JOY_CURSOR_SPEED_LEVEL_DEFAULT);  
  setScrollLevel(false, false, CONF_SCROLL_LEVEL_DEFAULT);
  setLightBrightnessLevel(false, false, CONF_LED_BRIGHTNESS_LEVEL_DEFAULT);
  setJoystickAcceleration(false, false, CONF_JOY_ACCELERATION_LEVEL_DEFAULT);
  printResponseInt(responseEnabled, apiEnabled, true, 0, "FR,1", true, 1);

  // Clear all LEDs to indicate factory reset process is finished 
  setLedState(LED_ACTION_OFF, LED_CLR_NONE, CONF_JOY_CALIB_LED_NUMBER, 0, 0, CONF_LED_BRIGHTNESS_MAX);                           
  performLedAction(ledCurrentState);  

  Serial.println("Activating software reset");

  softwareReset();

}

//***FACTORY RESET API FUNCTION***//
// Function   : doFactoryReset
//
// Description: This function is redefinition of main doFactoryReset function to match the types of API function arguments.
//
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
//
// Return     : void
void doFactoryReset(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if (optionalParameter.length() == 1 && optionalParameter.toInt() == 1) {
    doFactoryReset(responseEnabled, apiEnabled);
  }
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH STRING PARAMETER FUNCTION***//
// Function   : printResponseString
//
// Description: Serial Print output of the responses from APIs with string parameter as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : String : The response parameters printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseString(bool responseEnabled, 
                         bool apiEnabled,
                         bool responseStatus,
                         int responseNumber,
                         String responseCommand, 
                         bool responseParameterEnabled, 
                         String responseParameter) {
  if (responseEnabled) {

    if (responseStatus) {
      (apiEnabled) ? Serial.print("SUCCESS") : Serial.print("MANUAL");
    } else {
      Serial.print("FAIL");
    }
    Serial.print(",");
    Serial.print(responseNumber);
    Serial.print(":");
    Serial.print(responseCommand);
    if (responseParameterEnabled) {
      Serial.print(":");
      Serial.println(responseParameter);
    } else {
      Serial.println("");
    }
  }
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH INT PARAMETER FUNCTION***//
// Function   : printResponseInt
//
// Description: Serial Print output of the responses from APIs with int parameter as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : int : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseInt(bool responseEnabled, 
                      bool apiEnabled,
                      bool responseStatus, 
                      int responseNumber, 
                      String responseCommand, 
                      bool responseParameterEnabled, 
                      int responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH INT PARAMETER FUNCTION***//
// Function   : printResponseIntArray
//
// Description: Serial Print output of the responses from APIs with an int array as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : int[] : The array response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseIntArray(bool responseEnabled, 
                           bool apiEnabled, 
                           bool responseStatus, 
                           int responseNumber, 
                           String responseCommand, 
                           bool responseParameterEnabled, 
                           String responsePrefix, 
                           int responseParameterSize, 
                           char responseParameterDelimiter, 
                           int responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0] = {responseParameterDelimiter} : tempParameterDelimiter[0] = {'\0'};

  String responseParameterString = String(responsePrefix);
  for (int parameterIndex = 0; parameterIndex < responseParameterSize; parameterIndex++) {
    responseParameterString.concat(responseParameter[parameterIndex]);
    if (parameterIndex < (responseParameterSize - 1)) {
      responseParameterString.concat(tempParameterDelimiter[0]);
    };
  }

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH POINT PARAMETER FUNCTION***//
// Function   : printResponseIntPoint
//
// Description: Serial Print output of the responses from APIs with an point as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : pointIntType : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseIntPoint(bool responseEnabled,
                           bool apiEnabled, 
                           bool responseStatus, 
                           int responseNumber, 
                           String responseCommand, 
                           bool responseParameterEnabled, 
                           pointIntType responseParameter) {
  String responseParameterString = "";
  responseParameterString.concat(responseParameter.x);
  responseParameterString.concat("|");
  responseParameterString.concat(responseParameter.y);
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH INT POINT ARRAY PARAMETER FUNCTION***//
// Function   : printResponseIntPointArray
//
// Description: Serial Print output of the responses from APIs with an point array as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : pointIntType : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseIntPointArray(bool responseEnabled,
                                bool apiEnabled, 
                                bool responseStatus, 
                                int responseNumber, 
                                String responseCommand, 
                                bool responseParameterEnabled, 
                                String responsePrefix, 
                                int responseParameterSize, 
                                char responseParameterDelimiter, 
                                pointIntType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0] = {responseParameterDelimiter} : tempParameterDelimiter[0] = {'\0'};

  String responseParameterString = String(responsePrefix);
  for (int parameterIndex = 0; parameterIndex < responseParameterSize; parameterIndex++) {
    responseParameterString.concat(responseParameter[parameterIndex].x);
    responseParameterString.concat("|");
    responseParameterString.concat(responseParameter[parameterIndex].y);
    if (parameterIndex < (responseParameterSize - 1)) {
      responseParameterString.concat(tempParameterDelimiter[0]);
    };
  }

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH FLOAT PARAMETER FUNCTION***//
// Function   : printResponseFloat
//
// Description: Serial Print output of the responses from APIs with float parameter as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : float : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseFloat(bool responseEnabled,
                        bool apiEnabled,
                        bool responseStatus,
                        int responseNumber,
                        String responseCommand,
                        bool responseParameterEnabled,
                        float responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH FLOAT ARRAY PARAMETER FUNCTION***//
// Function   : printResponseFloat
//
// Description: Serial Print output of the responses from APIs with float parameter as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responsePrefix : String : 
//               responseParameterSize : int : Number of responses in array
//               responseParameterDelimiter : char : Delimter character to separate responses
//               responseParameter : float[] : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseFloatArray(bool responseEnabled, 
                             bool apiEnabled,
                             bool responseStatus,
                             int responseNumber,
                             String responseCommand,
                             bool responseParameterEnabled,
                             String responsePrefix,
                             int responseParameterSize,
                             char responseParameterDelimiter,
                             float responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0] = {responseParameterDelimiter} : tempParameterDelimiter[0] = {'\0'};

  String responseParameterString = String(responsePrefix);
  for (int parameterIndex = 0; parameterIndex < responseParameterSize; parameterIndex++) {
    responseParameterString.concat(responseParameter[parameterIndex]);
    if (parameterIndex < (responseParameterSize - 1)) {
      responseParameterString.concat(tempParameterDelimiter[0]);
    };
  }

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH FLOAT POINT PARAMETER FUNCTION***//
// Function   : printResponseFloatPoint
//
// Description: Serial Print output of the responses from APIs with float point as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : pointFloatType : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseFloatPoint(bool responseEnabled, 
                             bool apiEnabled, 
                             bool responseStatus, 
                             int responseNumber, 
                             String responseCommand, 
                             bool responseParameterEnabled, 
                             pointFloatType responseParameter) {
  String responseParameterString = "";
  responseParameterString.concat(responseParameter.x);
  responseParameterString.concat("|");
  responseParameterString.concat(responseParameter.y);

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH FLOAT POINT ARRAY PARAMETER FUNCTION***//
// Function   : printResponseFloatPointArray
//
// Description: Serial Print output of the responses from APIs with float point as the output
//
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true.
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL)
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : pointFloatType[] : The response parameter printed as output.
//
// Return     : void
//***********************************************************************//
void printResponseFloatPointArray(bool responseEnabled, 
                                  bool apiEnabled,
                                  bool responseStatus,
                                  int responseNumber,
                                  String responseCommand,
                                  bool responseParameterEnabled,
                                  String responsePrefix,
                                  int responseParameterSize,
                                  char responseParameterDelimiter,
                                  pointFloatType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0] = {responseParameterDelimiter} : tempParameterDelimiter[0] = {'\0'};

  String responseParameterString = String(responsePrefix);
  for (int parameterIndex = 0; parameterIndex < responseParameterSize; parameterIndex++) {
    responseParameterString.concat(responseParameter[parameterIndex].x);
    responseParameterString.concat("|");
    responseParameterString.concat(responseParameter[parameterIndex].y);
    if (parameterIndex < (responseParameterSize - 1)) {
      responseParameterString.concat(tempParameterDelimiter[0]);
    };
  }

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);
}