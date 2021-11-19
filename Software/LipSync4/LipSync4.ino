#include <Adafruit_TinyUSB.h>
#include "Config.h"

bool g_settingsEnabled = false;                        //Serial input settings command mode enabled or disabled

//***API FUNCTIONS***// - DO NOT CHANGE
typedef void (*FunctionPointer)(bool,bool,int*);        //Type definition for API function pointer

typedef struct {                                  //Type definition for API function list
  String _endpoint;                                //Unique two character end point  
  String _command;                                //Unique one character command code
  String _parameter;                              //Parameter that is passed to function
  FunctionPointer _function;                      //API function pointer
} _functionList;                                  

// Declare individual API functions with command, parameter, and corresponding function
_functionList getModelNumberFunction =          {"MN","0","0",&getModelNumber};
_functionList getVersionNumberFunction =        {"VN","0","0",&getVersionNumber};

_functionList getCursorSpeedFunction =          {"SS","0","0",&getCursorSpeed};
_functionList setCursorSpeedFunction =          {"SS","1","",&setCursorSpeed};
_functionList getReactionTimeFunction =         {"RT","0","0",&getReactionTime};
_functionList setReactionTimeFunction =         {"RT","1","",&setReactionTime};
_functionList getScrollLevelFunction =          {"SL","0","0",&getScrollLevel};
_functionList setScrollLevelFunction =          {"SL","1","",&setScrollLevel};

_functionList getCursorInitializationFunction = {"IN","0","0",&getCursorInitialization};
_functionList setCursorInitializationFunction = {"IN","1","1",&setCursorInitialization};
_functionList getCursorCalibrationFunction =    {"CA","0","0",&getCursorCalibration};
_functionList setCursorCalibrationFunction =    {"CA","1","1",&setCursorCalibration};
_functionList getCursorDeadZoneFunction =       {"DZ","0","0",&getCursorDeadZone};
_functionList setCursorDeadZoneFunction =       {"DZ","1","",&setCursorDeadZone};

_functionList getRotationAngleFunction =        {"RA","0","0",&getRotationAngle};
_functionList setRotationAngleFunction =        {"RA","1","",&setRotationAngle};

_functionList getPressureValueFunction =        {"PV","0","0",&getPressureValue};
_functionList getPressureThresholdFunction =    {"PT","0","0",&getPressureThreshold};
_functionList setSipPressureThresholdFunction = {"PT","1","",&setSipPressureThreshold};
_functionList setPuffPressureThresholdFunction ={"PT","2","",&setPuffPressureThreshold};

_functionList getCommunicationMethodFunction =  {"CM","0","0",&getCommunicationMethod};
_functionList setCommunicationMethodFunction =  {"CM","1","",&setCommunicationMethod};

_functionList getHIDOutputFunction =            {"HO","0","0",&getHIDOutput};
_functionList setHIDOutputFunction =            {"HO","1","",&setHIDOutput};


_functionList getMappingFunction =              {"MP","0","0",&getMapping};
_functionList setJoystickMappingFunction =      {"MP","1","",&setJoystickMapping}; 
_functionList setPressureMappingFunction =      {"MP","2","",&setPressureMapping}; 
_functionList setSwitchMappingFunction =        {"MP","3","",&setSwitchMapping}; 

_functionList getLEDProfileFunction =           {"LP","0","0",&getLEDProfile};
_functionList setLEDProfileFunction =           {"LP","1","",&setLEDProfile};
_functionList getLEDBrightnessFunction =        {"LB","0","0",&getLEDBrightness};
_functionList setLEDBrightnessFunction =        {"LB","1","",&setLEDBrightness};

_functionList getSoundFunction =                {"SD","0","0",&getSound};
_functionList setSoundFunction =                {"SD","1","",&setSound};

_functionList getDebugModeFunction =            {"DM","0","0",&getDebugMode};
_functionList setDebugModeFunction =            {"DM","1","",&setDebugMode};
_functionList getRawModeFunction =              {"RM","0","0",&getRawMode};
_functionList setRawModeFunction =              {"RM","1","",&setRawMode};
_functionList getDiagnosticFunction =           {"DI","0","0",&getDiagnostic};

_functionList softResetFunction =               {"RS","1","",&softReset};
_functionList hardResetFunction =               {"RS","2","",&hardReset};
_functionList factoryResetFunction =            {"FR","1","",&factoryReset};

// Declare array of API functions
_functionList apiFunction[41] = {
  getModelNumberFunction,
  getVersionNumberFunction,
  getCursorSpeedFunction,
  setCursorSpeedFunction,
  getCursorSpeedFunction,
  setCursorSpeedFunction,
  getScrollLevelFunction,
  setScrollLevelFunction,
  getCursorInitializationFunction,
  setCursorInitializationFunction,
  getCursorCalibrationFunction,
  setCursorCalibrationFunction,
  getCursorDeadZoneFunction,
  setCursorDeadZoneFunction,
  getRotationAngleFunction,
  setRotationAngleFunction,
  getPressureValueFunction,
  getPressureThresholdFunction,
  setSipPressureThresholdFunction,
  getCommunicationMethodFunction,
  setCommunicationMethodFunction,
  getHIDOutputFunction,
  setHIDOutputFunction,
  getMappingFunction,
  setJoystickMappingFunction,
  setPressureMappingFunction,
  setSwitchMappingFunction,
  getLEDProfileFunction,
  setLEDProfileFunction,
  getLEDBrightnessFunction,
  setLEDBrightnessFunction,
  getSoundFunction,
  setSoundFunction,
  getDebugModeFunction,
  setDebugModeFunction,
  getRawModeFunction,
  setRawModeFunction,
  getDiagnosticFunction,
  softResetFunction,
  hardResetFunction,
  factoryResetFunction
  };


void setup() {
   Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  while (!Serial) { delay(1); }
  Serial.println("API Unit test.");
}

void loop() {
    g_settingsEnabled=serialSettings(g_settingsEnabled); //Check to see if setting option is enabled in Lipsync

}

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
    
    //Set the input parameter to the flag returned. This will help to detect that the settings actions should be performed.
     if (Serial.available()>0)  
     {  
       //Check if serial has received or read input string and word "SETTINGS" is in input string.
       commandString = Serial.readString();            
       if (settingsFlag==false && commandString=="SETTINGS") {
        //SETTING received
        //Set the return flag to true so settings actions can be performed in the next call to the function
        printResponseSingle(true,true,true,0,commandString,false,0);
        settingsFlag=true;                         
       } else if (settingsFlag==true && commandString=="EXIT") {
        //EXIT Recieved
        //Set the return flag to false so settings actions can be exited
        printResponseSingle(true,true,true,0,commandString,false,0);
        settingsFlag=false;                         
       } else if (settingsFlag==true && isValidCommandFormat(commandString)){ //Check if command's format is correct and it's in settings mode
        performCommand(commandString);                  //Sub function to process valid strings
        settingsFlag=false;   
       } else {
        printResponseSingle(true,true,false,0,commandString,false,0);
        settingsFlag=false;      
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
  int inputCommandIndex = inputString.indexOf(':');

  //Extract command string from input string
  String inputCommandString = inputString.substring(0, inputCommandIndex);
  
  //Extract parameter string from input string
  String inputParameterString = inputString.substring(inputCommandIndex+1);
  
  // Determine total number of API commands
  int totalCommandNumber=sizeof(apiFunction)/sizeof(apiFunction[0]);

  //Iterate through each API command
  for(int apiIndex = 0; apiIndex < totalCommandNumber; apiIndex++){
    
    // Test if input command string matches API command and input parameter string matches API parameter string
    if( inputCommandString == apiFunction[apiIndex]._command 
    && (inputParameterString == apiFunction[apiIndex]._parameter 
    || apiFunction[apiIndex]._parameter == "" || apiFunction[apiIndex]._parameter == "r" )){
      
      // Matching Command String found
      if( isValidCommandParameter( inputParameterString )) {   //Check if parameter is valid
        //Valid Parameter
        
        //Handle parameters that are an array as a special case.
        if(apiFunction[apiIndex]._parameter=="r"){   //"r" denotes an array parameter 
          
          int inputParameterArray[inputParameterString.length() + 1];
          for(unsigned int arrayIndex=0; arrayIndex<inputParameterString.length(); arrayIndex++)
          {
            inputParameterArray[arrayIndex]=inputParameterString.charAt(arrayIndex)-'0';
          }
          
          // Call matching API function with input parameter array
          apiFunction[apiIndex]._function(true, true, inputParameterArray);
          delay(5);     
        } else {
          int tempParameterArray[1] = {(int)inputParameterString.toInt()};
          // Call matching API function with input parameter string
          apiFunction[apiIndex]._function(true, true, tempParameterArray);
          delay(5);
        }
      } else { // Invalid input parameter
      
      // Outut error message
      printResponseSingle(true,true,false,2,inputString,false,0);

      delay(5);
      }
      break;
    } else if(apiIndex== (totalCommandNumber-1)) { // command doesn’t exist
    
    //Output error message
    printResponseSingle(true,true,false,1,inputString,false,0);

    delay(5);
    break;
    }
  } //end iterate through API functions

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
  bool isValidFormat = false;;
  if ((inputCommandString.length()==(6) || //XX,d:d
       inputCommandString.length()==(7) || //XX,d:dd
       inputCommandString.length()==(8) || //XX,d:ddd
       inputCommandString.length()==(11)) && inputCommandString.charAt(2)==',' && inputCommandString.charAt(4)==':'){ 
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
bool isValidCommandParameter(String inputParamterString) {
  if (isStrNumber(inputParamterString)){ 
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
boolean isStrNumber(String str){
  
  for(byte i=0;i<str.length();i++)
  {
    if(!isDigit(str.charAt(i))) return false;
  }
  return true;
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
bool isValidDelimiter(char inputDelimiter){
  bool validOutput;
  
  (inputDelimiter == ',' || inputDelimiter == ':' || inputDelimiter == '-') ? validOutput = true : validOutput = false;

  return validOutput;
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH SINGLE PARAMETER FUNCTION***//
// Function   : printResponseSingle 
// 
// Description: Serial Print output of the responses from APIs with single parameter as the output 
// 
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true. 
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL) 
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : int : The response parameters printed as output.
// 
// Return     : void
//***********************************************************************//
void printResponseSingle(bool responseEnabled,bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand,bool responseParameterEnabled,int responseParameter) {
  if(responseEnabled) {
    
    if(responseStatus){
      (apiEnabled) ? Serial.print("SUCCESS") : Serial.print("MANUAL");
    }else{
      Serial.print("FAIL");
    } 
    Serial.print(",");
    Serial.print(responseNumber);
    Serial.print(":");
    Serial.print(responseCommand);
    
    if(responseParameterEnabled){
      Serial.print(":");
      Serial.println(responseParameter);    
    } else {
      Serial.println("");  
    } 
  }
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

  printResponseSingle(responseEnabled,apiEnabled,true,0,"MN,0",true,LIPSYNC_MODEL);

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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getModelNumber(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
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
 
  printResponseSingle(responseEnabled,apiEnabled,true,0,"VN,0",true,LIPSYNC_VERSION);
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getVersionNumber(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getVersionNumber(responseEnabled, apiEnabled);
  }
}

//***GET CURSOR SPEED FUNCTION***//
// Function   : getCursorSpeed 
// 
// Description: This function retrieves the current cursor speed level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
int getCursorSpeed(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"SS,0",true,SPEED_COUNTER);

  return SPEED_COUNTER;
}
//***GET CURSOR SPEED API FUNCTION***//
// Function   : getCursorSpeed 
// 
// Description: This function is redefinition of main getCursorSpeed function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getCursorSpeed(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCursorSpeed(responseEnabled, apiEnabled);
  }
}

//***SET CURSOR SPEED FUNCTION***//
// Function   : setCursorSpeed 
// 
// Description: This function sets the current cursor speed level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSpeedCounter : bool : The new cursor speed level.
// 
// Return     : void
//*********************************//
void setCursorSpeed(bool responseEnabled, bool apiEnabled, int inputSpeedCounter) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"SS,1",true,inputSpeedCounter);
}
//***SET CURSOR SPEED API FUNCTION***//
// Function   : setCursorSpeed 
// 
// Description: This function is redefinition of main setCursorSpeed function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSpeedCounter : int* : The array of one element which contains the new cursor speed level.
// 
// Return     : void
void setCursorSpeed(bool responseEnabled, bool apiEnabled, int* inputSpeedCounter){
}

//***INCREASE CURSOR SPEED LEVEL FUNCTION***//
// Function   : increaseCursorSpeed 
// 
// Description: This function increases the cursor speed level by one.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void increaseCursorSpeed(bool responseEnabled, bool apiEnabled) {
}

//***DECREASE CURSOR SPEED LEVEL FUNCTION***//
// Function   : decreaseCursorSpeed 
// 
// Description: This function decreases the cursor speed level by one.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void decreaseCursorSpeed(bool responseEnabled, bool apiEnabled) {
}

//***GET REACTION TIME FUNCTION***//
// Function   : getReactionTime
// 
// Description: This function retrieves the reaction time.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
int getReactionTime(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"RT,0",true,REACTION_TIME);

  return SCROLL_LEVEL;
}

//***GET REACTION TIME API FUNCTION***//
// Function   : getReactionTime 
// 
// Description: This function is redefinition of main getReactionTime function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getReactionTime(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getReactionTime(responseEnabled, apiEnabled);
  }
}

//***SET REACTION TIME FUNCTION***//
// Function   : setReactionTime
// 
// Description: This function sets the reaction time.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputScrollCounter : bool : The new the reaction time.
// 
// Return     : void
//*********************************//
void setReactionTime(bool responseEnabled, bool apiEnabled, int inputReactionTime) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"RT,1",true,inputReactionTime);
  delay(5); 
}
//***SET REACTION TIME API FUNCTION***//
// Function   : setReactionTime
// 
// Description: This function is redefinition of main setReactionTime function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputScrollLevel : int* : The array of one element which contains the reaction time.
// 
// Return     : void
void setReactionTime(bool responseEnabled, bool apiEnabled, int* inputReactionTime) {
  setReactionTime(responseEnabled, apiEnabled, inputReactionTime[0]);
}

//***GET SCROLL LEVEL FUNCTION***//
// Function   : getScrollLevel
// 
// Description: This function retrieves the current cursor scroll level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
int getScrollLevel(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"SL,0",true,SCROLL_LEVEL);

  return SCROLL_LEVEL;
}

//***GET SCROLL LEVEL API FUNCTION***//
// Function   : getScrollLevel 
// 
// Description: This function is redefinition of main getScrollLevel function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getScrollLevel(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getScrollLevel(responseEnabled, apiEnabled);
  }
}

//***SET SCROLL LEVEL FUNCTION***//
// Function   : setScrollLevel
// 
// Description: This function sets the scroll level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputScrollCounter : bool : The new the cursor speed level.
// 
// Return     : void
//*********************************//
void setScrollLevel(bool responseEnabled, bool apiEnabled, int inputScrollLevel) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"SL,1",true,inputScrollLevel);
  delay(5); 
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
//               inputScrollLevel : int* : The array of one element which contains the new scroll speed level.
// 
// Return     : void
void setScrollLevel(bool responseEnabled, bool apiEnabled, int* inputScrollLevel) {
  setScrollLevel(responseEnabled, apiEnabled, inputScrollLevel[0]);
}

//***GET CURSOR INITIALIZATION FUNCTION***//
/// Function   : getCursorInitialization 
// 
// Description: This function retrieves the FSR Neutral values from joystick Initialization.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getCursorInitialization(bool responseEnabled, bool apiEnabled) {
  delay(10);  
}
//***GET CURSOR INITIALIZATION API FUNCTION***//
// Function   : getCursorInitialization 
// 
// Description: This function is redefinition of main getCursorInitialization function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getCursorInitialization(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCursorInitialization(responseEnabled, apiEnabled);
  }
}

//***SET CURSOR INITIALIZATION FUNCTION***//
/// Function   : setCursorInitialization 
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
void setCursorInitialization(bool responseEnabled, bool apiEnabled) {

}
//***SET CURSOR INITIALIZATION API FUNCTION***//
// Function   : setCursorInitialization 
// 
// Description: This function is redefinition of main setCursorInitialization function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setCursorInitialization(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    setCursorInitialization(responseEnabled, apiEnabled);
  }
}

//*** GET CURSOR CALIBRATION FUNCTION***//
/// Function   : getCursorCalibration 
// 
// Description: This function retrieves FSR maximum values from joystick Calibration.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getCursorCalibration(bool responseEnable, bool apiEnabled) {

  delay(10);
}
//***GET CURSOR CALIBRATION API FUNCTION***//
// Function   : getCursorCalibration 
// 
// Description: This function is redefinition of main getCursorCalibration function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getCursorCalibration(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCursorCalibration(responseEnabled, apiEnabled);
  }
}

//*** SET CURSOR CALIBRATION FUNCTION***//
/// Function   : setCursorCalibration 
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
void setCursorCalibration(bool responseEnabled, bool apiEnabled) {

  delay(10);
}
//***SET CURSOR CALIBRATION API FUNCTION***//
// Function   : setCursorCalibration 
// 
// Description: This function is redefinition of main setCursorCalibration function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setCursorCalibration(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==1){
    setCursorCalibration(responseEnabled, apiEnabled);
  }
}

//*** GET CURSOR DEADZONE FUNCTION***//
/// Function   : getCursorDeadZone
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
void getCursorDeadZone(bool responseEnable, bool apiEnabled) {

  delay(10);
}
//***GET CURSOR DEADZONE API FUNCTION***//
// Function   : getCursorDeadZone 
// 
// Description: This function is redefinition of main getCursorDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getCursorDeadZone(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCursorDeadZone(responseEnabled, apiEnabled);
  }
}

//*** SET CURSOR DEADZONE FUNCTION***//
/// Function   : getCursorDeadZone 
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
void setCursorDeadZone(bool responseEnabled, bool apiEnabled) {

  delay(10);
}
//***SET CURSOR DEADZONE API FUNCTION***//
// Function   : setCursorDeadZone  
// 
// Description: This function is redefinition of main setCursorDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setCursorDeadZone(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==1){
    setCursorCalibration(responseEnabled, apiEnabled);
  }
}

//***GET ROTATION ANGLE FUNCTION***///
// Function   : getRotationAngle 
// 
// Description: This function gets the current rotation angle ( 0,90,180,270,360)
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : tempRotationAngle : int : The current rotation angle ( 0,90,180,270,360)
//*********************************//
int getRotationAngle(bool responseEnabled, bool apiEnabled) {

   printResponseSingle(responseEnabled,apiEnabled,true,0,"RA,0",true,ROTATION_ANGLE);

   delay(5); 
    
   return ROTATION_ANGLE;
}
//***GET ROTATION ANGLE API FUNCTION***//
// Function   : getRotationAngle 
// 
// Description: This function is redefinition of main getRotationAngle function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getRotationAngle(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getRotationAngle(responseEnabled, apiEnabled);
  }
}

//***SET ROTATION ANGLE FUNCTION***///  
// Function   : setRotationAngle 
// 
// Description: This function sets a new rotation angle ( 0,90,180,270,360)
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputRotationAngle : int : The input rotation angle ( 0,90,180,270,360) requested.
// 
// Return     : void
//*********************************//
void setRotationAngle(bool responseEnabled, bool apiEnabled, int inputRotationAngle) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"RA,1",true,ROTATION_ANGLE); 
  

}
//***SET ROTATION ANGLE API FUNCTION***//
// Function   : setRotationAngle 
// 
// Description: This function is redefinition of main setRotationAngle function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputRotationAngle : int* : The array of one element which contains the new rotation angle.
// 
// Return     : void
void setRotationAngle(bool responseEnabled, bool apiEnabled, int* inputRotationAngle) {
  setRotationAngle(responseEnabled, apiEnabled, inputRotationAngle[0]);
}

//***GET PRESSURE VALUE FUNCTION***//
// Function   : getPressureValue 
// 
// Description: This function returns pressure value in volts [0.0V - 5.0V]. The pressure multiplied by 100.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getPressureValue(bool responseEnabled, bool apiEnabled) {

  int tempPressureValue = 0;

  printResponseSingle(responseEnabled,apiEnabled,true,0,"PV,0",true,tempPressureValue);

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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getPressureValue(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getPressureValue(responseEnabled, apiEnabled);
  }
}


//***GET PRESSURE THRESHOLD FUNCTION***//
// Function   : getPressureThreshold 
// 
// Description: This function returns the current pressure threshold in percentage and the nominal pressure [0.0V - 5.0V] multiplied by 100.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getPressureThreshold(bool responseEnabled, bool apiEnabled) {

  
}
//***GET PRESSURE THRESHOLD API FUNCTION***//
// Function   : getPressureThreshold 
// 
// Description: This function is redefinition of main getPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getPressureThreshold(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getPressureThreshold(responseEnabled, apiEnabled);
  }
}

//***SET SIP PRESSURE THRESHOLD FUNCTION***//
// Function   : setSipPressureThreshold 
// 
// Description: This function sets the current Sip pressure threshold in percentage.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSipPressureThreshold : bool : The new sip pressure threshold in percentage.
// 
// Return     : void
//*********************************//
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, int inputSipPressureThreshold) {

  delay(5); 
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
//               inputSipPressureThreshold : int* : The array of one element which contains the new sip pressure threshold.
// 
// Return     : void
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, int* inputSipPressureThreshold) {
  setSipPressureThreshold(responseEnabled,apiEnabled, inputSipPressureThreshold[0]);
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
//               inputPuffPressureThreshold : bool : The new sip pressure threshold in percentage.
// 
// Return     : void
//*********************************//
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, int inputPuffPressureThreshold) {

  delay(5); 
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
//               inputPuffPressureThreshold : int* : The array of one element which contains the new puff pressure threshold.
// 
// Return     : void
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, int* inputPuffPressureThreshold) {
  setPuffPressureThreshold(responseEnabled,apiEnabled, inputPuffPressureThreshold[0]);
}

//***GET COMMUNICATION METHOD FUNCTION***//
// Function   : getCommunicationMethod 
// 
// Description: This function retrieves the state of communication method.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : communicationMethod : bool : The current state of communication method.
//*********************************//
bool getCommunicationMethod(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"CM,0",true,COM_METHOD);

  return COM_METHOD;
}
//***GET COMMUNICATION METHOD API FUNCTION***//
// Function   : getCommunicationMethod
// 
// Description: This function is redefinition of main getCommunicationMethod function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getCommunicationMethod(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCommunicationMethod(responseEnabled, apiEnabled);
  }
}

//***SET COMMUNICATION METHOD FUNCTION***//
// Function   : setCommunicationMethod
// 
// Description: This function sets the state of communication method.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputCommunicationMethod : int : The new communication method state ( 0 = USB , 1 = BLE )
// 
// Return     : void
//*********************************//
void setCommunicationMethod(bool responseEnabled, bool apiEnabled, int inputCommunicationMethod) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "CM,1", true, COM_METHOD);
  
  delay(5); 
}
//***SET COMMUNICATION METHOD API FUNCTION***//
// Function   : setCommunicationMethod 
// 
// Description: This function is redefinition of main setCommunicationMethod function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputCommunicationMethod : int* : The array of one element which contains the new communication method state.
// 
// Return     : void
void setCommunicationMethod(bool responseEnabled, bool apiEnabled, int* inputCommunicationMethod){
  setCommunicationMethod(responseEnabled, apiEnabled, inputCommunicationMethod[0]);
}

//***GET HID OUTPUT FUNCTION***//
// Function   : getHIDOutput
// 
// Description: This function retrieves the state of HID Output.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : HIDOutput: bool : The current state of HID Output.
//*********************************//
bool getHIDOutput(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"HO,0",true,HID_OUTPUT);

  return HID_OUTPUT;
}
//***GET HID OUTPUT API FUNCTION***//
// Function   : getHIDOutput
// 
// Description: This function is redefinition of main getHIDOutputfunction to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getHIDOutput(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getHIDOutput(responseEnabled, apiEnabled);
  }
}

//***SET HID OUTPUT FUNCTION***//
// Function   : setHIDOutput
// 
// Description: This function sets the state of HID Output.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputHIDOutput: int : The new HID Output state ( false = HID disabled , true = HID enabled )
// 
// Return     : void
//*********************************//
void setHIDOutput(bool responseEnabled, bool apiEnabled, int inputHIDOutput) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "HO,1", true, HID_OUTPUT);
  
  delay(5); 
}
//***SET HID OUTPUT API FUNCTION***//
// Function   : setHIDOutput
// 
// Description: This function is redefinition of main setHIDOutput function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputHIDOutput: int* : The array of one element which contains the new HID Output state.
// 
// Return     : void
void setHIDOutput(bool responseEnabled, bool apiEnabled, int* inputHIDOutput){
  setHIDOutput(responseEnabled, apiEnabled, inputHIDOutput[0]);
}

//***GET MAPPING FUNCTION***//
// Function   : getMapping 
// 
// Description: This function retrieves a new input action mapping.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getMapping(bool responseEnabled, bool apiEnabled) {

  delay(5); 
}
//***GET MAPPING API FUNCTION***//
// Function   : getMapping 
// 
// Description: This function is redefinition of main getMapping function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getMapping(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getMapping(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK MAPPING FUNCTION***//
// Function   : setJoystickMapping
// 
// Description: This function sets a new input joystick action mapping.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputJoystickMapping : int array : The input joystick action mapping requested.
// 
// Return     : void
//*********************************//
void setJoystickMapping(bool responseEnabled, bool apiEnabled, int inputJoystickMapping[]) {
  
  delay(5); 
}

//***SET PRESSURE MAPPING FUNCTION***//
// Function   : setPressureMapping
// 
// Description: This function sets a new input pressure action mapping.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputPressureMapping : int array : The input pressure action mapping requested.
// 
// Return     : void
//*********************************//
void setPressureMapping(bool responseEnabled, bool apiEnabled, int inputPressureMapping[]) {
  
  delay(5); 
}

//***SET SWITCH MAPPING FUNCTION***//
// Function   : setSwitchMapping
// 
// Description: This function sets a new input switch action mapping.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSwitchMapping : int array : The input switch action mapping requested.
// 
// Return     : void
//*********************************//
void setSwitchMapping(bool responseEnabled, bool apiEnabled, int inputSwitchMapping[]) {
  
  delay(5); 
}

//***GET LED PROFILE FUNCTION***//
// Function   : getLEDProfile
// 
// Description: This function retrieves the current LED Color profile.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : LEDProfile: bool : The current LED Color profile.
//*********************************//
bool getLEDProfile(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"LP,0",true,LED_PROFILE);

  return LED_PROFILE;
}
//***GET LED PROFILE API FUNCTION***//
// Function   : getLEDProfile
// 
// Description: This function is redefinition of main getLEDProfile function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getLEDProfile(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getLEDProfile(responseEnabled, apiEnabled);
  }
}

//***SET LED PROFILE FUNCTION***//
// Function   : setLEDProfile
// 
// Description: This function sets the state of LED Color profile.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLEDProfile: int : The new LED Color profile
// Return     : void
//*********************************//
void setLEDProfile(bool responseEnabled, bool apiEnabled, int inputLEDProfile) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "LP,1", true, LED_PROFILE);
  
  delay(5); 
}
//***SET LED PROFILE API FUNCTION***//
// Function   : setLEDProfile
// 
// Description: This function is redefinition of main setLEDProfile function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inpuLEDProfile: int* : The array of one element which contains the new LED Color profile.
// 
// Return     : void
void setLEDProfile(bool responseEnabled, bool apiEnabled, int* inpuLEDProfile){
  setLEDProfile(responseEnabled, apiEnabled, inpuLEDProfile[0]);
}

//***GET LED BRIGHTNESS FUNCTION***//
// Function   : getLEDBrightness
// 
// Description: This function retrieves the current LED Color Brightness.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : LEDBrightness: bool : The current LED Color brightness.
//*********************************//
bool getLEDBrightness(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"LB,0",true,LED_BRIGHTNESS);

  return LED_BRIGHTNESS;
}
//***GET LED BRIGHTNESS API FUNCTION***//
// Function   : getLEDBrightness
// 
// Description: This function is redefinition of main getLEDBrightness function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getLEDBrightness(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getLEDBrightness(responseEnabled, apiEnabled);
  }
}

//***SET LED BRIGHTNESS FUNCTION***//
// Function   : setLEDBrightness
// 
// Description: This function sets the state of LED Color Brightness.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLEDBrightness: int : The new LED Color Brightness
// Return     : void
//*********************************//
void setLEDBrightness(bool responseEnabled, bool apiEnabled, int inputLEDBrightness) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "LB,1", true, LED_BRIGHTNESS);
  
  delay(5); 
}
//***SET LED BRIGHTNESS API FUNCTION***//
// Function   : setLEDBrightness
// 
// Description: This function is redefinition of main setLEDBrightness function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputLEDBrightness: int* : The array of one element which contains the new LED Color Brightness.
// 
// Return     : void
void setLEDBrightness(bool responseEnabled, bool apiEnabled, int* inputLEDBrightness){
  setLEDBrightness(responseEnabled, apiEnabled, inputLEDBrightness[0]);
}


//***GET SOUND STATE FUNCTION***//
// Function   : getSound  
// 
// Description: This function retrieves the state of sound ( ON or OFF ).
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : soundState : bool : The current state of sound ( ON or OFF )
//*********************************//
bool getSound(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"SD,0",true,SOUND_ENABLED);

  return SOUND_ENABLED;
}
//***GET SOUND STATE API FUNCTION***//
// Function   : getSound 
// 
// Description: This function is redefinition of main getSound function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getSound(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getSound(responseEnabled, apiEnabled);
  }
}

//***SET SOUND STATE FUNCTION***//
// Function   : setSound 
// 
// Description: This function sets the state of sound ( ON or OFF ).
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inpuSoundState : int : The new sound state ( true = ON , false = OFF )
// 
// Return     : void
//*********************************//
void setSound(bool responseEnabled, bool apiEnabled, int inpuSoundState) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "SD,1", true, SOUND_ENABLED);
  
  delay(5); 
}
//***SET SOUND STATE API FUNCTION***//
// Function   : setSound 
// 
// Description: This function is redefinition of main setSound function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inpuSoundState : int* : The array of one element which contains the new sound state.
// 
// Return     : void
void setSound(bool responseEnabled, bool apiEnabled, int* inputSoundState){
  setSound(responseEnabled, apiEnabled, inputSoundState[0]);
}

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
// Return     : debugState : bool : The current state of debug mode.
//*********************************//
bool getDebugMode(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"DM,0",true,DEBUG_MODE);

  return DEBUG_MODE;
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getDebugMode(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
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
//               inputDebugState : int : The new debug mode state ( true = ON , false = OFF )
// 
// Return     : void
//*********************************//
void setDebugMode(bool responseEnabled, bool apiEnabled, int inputDebugState) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "DM,1", true, DEBUG_MODE);
  
  delay(5); 
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
//               inputDebugState : int* : The array of one element which contains the new debug mode state.
// 
// Return     : void
void setDebugMode(bool responseEnabled, bool apiEnabled, int* inputDebugState){
  setDebugMode(responseEnabled, apiEnabled, inputDebugState[0]);
}

//***GET RAW MODE STATE FUNCTION***//
// Function   : getRawMode 
// 
// Description: This function retrieves the state of raw mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : rawState : bool : The current state of raw mode.
//*********************************//
bool getRawMode(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"RM,0",true,RAW_MODE);

  return RAW_MODE;
}
//***GET RAW MODE STATE API FUNCTION***//
// Function   : getRawMode 
// 
// Description: This function is redefinition of main getRawMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getRawMode(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getRawMode(responseEnabled, apiEnabled);
  }
}

//***SET RAW MODE STATE FUNCTION***//
// Function   : setRawMode 
// 
// Description: This function sets the state of raw mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputRawState : int : The new raw mode state ( true = ON , false = OFF )
// 
// Return     : void
//*********************************//
void setRawMode(bool responseEnabled, bool apiEnabled, int inputRawState) {

  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "RM,1", true, RAW_MODE);
  
  delay(5); 
}
//***SET RAW MODE STATE API FUNCTION***//
// Function   : setRawMode 
// 
// Description: This function is redefinition of main setRawMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inpuRawState : int* : The array of one element which contains the new raw mode state.
// 
// Return     : void
void setRawMode(bool responseEnabled, bool apiEnabled, int* inputRawState){
  setRawMode(responseEnabled, apiEnabled, inputRawState[0]);
}

//***GET DIAGNOSTICS FUNCTION***//
// Function   : getDiagnostic 
// 
// Description: This function retrieves diagnostics.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getDiagnostic(bool responseEnabled, bool apiEnabled) {

  printResponseSingle(responseEnabled,apiEnabled,true,0,"DI,0",true,0);

}
//***GET DIAGNOSTICS API FUNCTION***//
// Function   : getDiagnostic 
// 
// Description: This function is redefinition of main getDiagnostic function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getDiagnostic(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getDiagnostic(responseEnabled, apiEnabled);
  }
}

//***SOFT RESET FUNCTION***//
// Function   : softReset 
// 
// Description: This function performs soft reset. 
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//***************************//
void softReset(bool responseEnabled, bool apiEnabled) { 

      
  printResponseSingle(responseEnabled,apiEnabled,true,0,"RS,1",true,0);

  delay(5); 
}
//***SOFT RESET API FUNCTION***//
// Function   : softReset 
// 
// Description: This function is redefinition of main softReset function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
void softReset(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    softReset(responseEnabled, apiEnabled);
  }
 }
 
 //***HARD RESET FUNCTION***//
// Function   : hardReset 
// 
// Description: This function performs hard reset. 
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//***************************//
void hardReset(bool responseEnabled, bool apiEnabled) { 

      
  printResponseSingle(responseEnabled,apiEnabled,true,0,"RS,2",true,0);

  delay(5); 
}
//***HARD RESET API FUNCTION***//
// Function   : hardReset 
// 
// Description: This function is redefinition of main hardReset function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
void hardReset(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    hardReset(responseEnabled, apiEnabled);
  }
 }

//***FACTORY RESET FUNCTION***//
// Function   : factoryReset 
// 
// Description: This function performs factory reset. It can perform a soft or hard reset.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//***************************//
void factoryReset(bool responseEnabled, bool apiEnabled) { 

      
  printResponseSingle(responseEnabled,apiEnabled,true,0,"FR,1",true,0);

  delay(5); 
}
//***FACTORY RESET API FUNCTION***//
// Function   : factoryReset 
// 
// Description: This function is redefinition of main factoryReset function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
void factoryReset(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    factoryReset(responseEnabled, apiEnabled);
  }
}
