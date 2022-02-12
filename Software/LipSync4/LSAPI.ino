

//***API FUNCTIONS***// - DO NOT CHANGE
typedef void (*FunctionPointer)(bool,bool,int*);        //Type definition for API function pointer

typedef struct {                                  //Type definition for API function list
  String endpoint;                               //Unique two character end point  
  String code;                                   //Unique one character command code
  String parameter;                              //Parameter that is passed to function
  FunctionPointer function;                      //API function pointer
} _functionList;                                  

// Declare individual API functions with command, parameter, and corresponding function
_functionList getModelNumberFunction =            {"MN","0","0",&getModelNumber};
_functionList getVersionNumberFunction =          {"VN","0","0",&getVersionNumber};
_functionList getJoystickInitializationFunction = {"IN","0","0",&getJoystickInitialization};
_functionList setJoystickInitializationFunction = {"IN","1","1",&setJoystickInitialization};
_functionList getJoystickCalibrationFunction =    {"CA","0","0",&getJoystickCalibration};
_functionList setJoystickCalibrationFunction =    {"CA","1","1",&setJoystickCalibration};
_functionList getJoystickDeadZoneFunction =       {"DZ","0","0",&getJoystickDeadZone};
_functionList setJoystickDeadZoneFunction =       {"DZ","1","",&setJoystickDeadZone};

_functionList getPressureValueFunction =          {"PV","0","0",&getPressureValue};
_functionList getSipPressureThresholdFunction =   {"ST","0","0",&getSipPressureThreshold};
_functionList setSipPressureThresholdFunction =   {"ST","1","",&setSipPressureThreshold};
_functionList getPuffPressureThresholdFunction =  {"PT","1","0",&getPuffPressureThreshold};
_functionList setPuffPressureThresholdFunction =  {"PT","1","",&setPuffPressureThreshold};

_functionList getCommunicationMethodFunction =    {"CM","0","0",&getCommunicationMethod};
_functionList setCommunicationMethodFunction =    {"CM","1","",&setCommunicationMethod};

_functionList getDebugModeFunction =              {"DM","0","0",&getDebugMode};
_functionList setDebugModeFunction =              {"DM","1","",&setDebugMode};

// Declare array of API functions
_functionList apiFunction[18] = {
  getModelNumberFunction,
  getVersionNumberFunction,
  getJoystickInitializationFunction,
  setJoystickInitializationFunction,
  getJoystickCalibrationFunction,
  setJoystickCalibrationFunction,
  getJoystickDeadZoneFunction,
  setJoystickDeadZoneFunction,
  getPressureValueFunction,
  getSipPressureThresholdFunction,
  setSipPressureThresholdFunction,
  getPuffPressureThresholdFunction,
  setPuffPressureThresholdFunction,
  getCommunicationMethodFunction,
  setCommunicationMethodFunction,
  getDebugModeFunction,
  setDebugModeFunction
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
  int inputIndex = inputString.indexOf(':');

  //Extract command string from input string
  String inputCommandString = inputString.substring(0, inputIndex);

  int inputCommandIndex = inputCommandString.indexOf(',');
  
  String inputEndpointString = inputCommandString.substring(0, inputCommandIndex);

  String inputCodeString = inputCommandString.substring(inputCommandIndex+1);
  
  //Extract parameter string from input string
  String inputParameterString = inputString.substring(inputIndex+1);
  Serial.println(inputEndpointString);  
  Serial.println(inputCodeString); 
  // Determine total number of API commands
  int apiTotalNumber=sizeof(apiFunction)/sizeof(apiFunction[0]);

  //Iterate through each API command
  for(int apiIndex = 0; apiIndex < apiTotalNumber; apiIndex++){
    
    // Test if input command string matches API command and input parameter string matches API parameter string
    if(inputEndpointString == apiFunction[apiIndex].endpoint && 
    inputCodeString == apiFunction[apiIndex].code && 
    (inputParameterString == apiFunction[apiIndex].parameter ||
    apiFunction[apiIndex].parameter == "" || 
    apiFunction[apiIndex].parameter == "r" )){
      
      // Matching Command String found
      if( isValidCommandParameter( inputParameterString )) {   //Check if parameter is valid
        //Valid Parameter
        
        //Handle parameters that are an array as a special case.
        if(apiFunction[apiIndex].parameter=="r"){   //"r" denotes an array parameter 
          
          int inputParameterArray[inputParameterString.length() + 1];
          for(unsigned int arrayIndex=0; arrayIndex<inputParameterString.length(); arrayIndex++)
          {
            inputParameterArray[arrayIndex]=inputParameterString.charAt(arrayIndex)-'0';
          }
          
          // Call matching API function with input parameter array
          apiFunction[apiIndex].function(true, true, inputParameterArray);
          delay(5);     
        } else {
          int tempParameterArray[1] = {(int)inputParameterString.toInt()};
          // Call matching API function with input parameter string
          apiFunction[apiIndex].function(true, true, tempParameterArray);
          delay(5);
        }
      } else { // Invalid input parameter
      
      // Outut error message
      printResponseSingle(true,true,false,2,inputString,false,0);

      delay(5);
      }
      break;
    } else if(apiIndex== (apiTotalNumber-1)) { // api doesn’t exist
    
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
  bool isValidFormat = false;
  if ((inputCommandString.length()==(6) || //XX,d:d
       inputCommandString.length()==(7) || //XX,d:dd
       inputCommandString.length()==(8) || //XX,d:ddd
       inputCommandString.length()==(9) || //XX,d:dddd
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
  boolean isNumber = false;
  for(byte i=0;i<str.length();i++)
  {
    isNumber = isDigit(str.charAt(i)) || str.charAt(i) == '+' || str.charAt(i) == '.' || str.charAt(i) == '-';
    if(!isNumber) return false;
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

  printResponseSingle(responseEnabled,apiEnabled,true,0,"MN,0",true,CONF_LIPSYNC_MODEL);

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
 
  printResponseSingle(responseEnabled,apiEnabled,true,0,"VN,0",true,CONF_LIPSYNC_VERSION);
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

//***GET JOYSTICK INITIALIZATION FUNCTION***//
/// Function   : getJoystickInitialization 
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
void getJoystickInitialization(bool responseEnabled, bool apiEnabled) {
  pointFloatType centerPoint = js.getInputComp();
  printJoystickFloatData(centerPoint);
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getJoystickInitialization(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK INITIALIZATION FUNCTION***//
/// Function   : setJoystickInitialization 
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
  js.updateInputComp();
  pointFloatType centerPoint = js.getInputComp();
  printJoystickFloatData(centerPoint);
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setJoystickInitialization(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    setJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//*** GET JOYSTICK CALIBRATION FUNCTION***//
/// Function   : getJoystickCalibration 
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
void getJoystickCalibration(bool responseEnable, bool apiEnabled) {
  String commandKey;
  pointFloatType maxPoint;
  for (int i = 1; i < 5; i++)
  {
    commandKey = "CA" + String(i);
    maxPoint = mem.readPoint(CONF_SETTINGS_FILE, commandKey);
    printJoystickFloatData(maxPoint);
    js.setInputMax(i, maxPoint);
  }
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getJoystickCalibration(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getJoystickCalibration(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK CALIBRATION FUNCTION***//
/// Function   : setJoystickCalibration 
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
  js.clear();                                                                                           //Clear previous calibration values 
  int stepNumber = 0;
  canOutputAction = false;
  calibTimerId[0] = calibTimer.setTimeout(CONF_JOY_CALIB_BLINK_TIME, performCalibration, (int *)stepNumber);  //Start the process  
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
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setJoystickCalibration(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==1){
    setJoystickCalibration(responseEnabled, apiEnabled);
  }
}


//*** GET JOYSTICK DEADZONE FUNCTION***//
/// Function   : getJoystickDeadZone
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
float getJoystickDeadZone(bool responseEnable, bool apiEnabled) {
  String deadZoneCommand = "DZ";  
  float tempDeadzone;
  tempDeadzone = mem.readFloat(CONF_SETTINGS_FILE, deadZoneCommand);

  if(tempDeadzone=<0 || tempDeadzone>=1){
    tempDeadzone = CONF_JOY_DEADZONE;
    mem.writeFloat(CONF_SETTINGS_FILE,deadZoneCommand,tempDeadzone);   
  }
  ps.setDeadzone(true,tempDeadzone);
  return tempDeadzone;
}
//***GET JOYSTICK DEADZONE API FUNCTION***//
// Function   : getJoystickDeadZone 
// 
// Description: This function is redefinition of main getJoystickDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void getJoystickDeadZone(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getCursorDeadZone(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK DEADZONE FUNCTION***//
/// Function   : setJoystickDeadZone 
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
void setJoystickDeadZone(bool responseEnabled, bool apiEnabled, float inputDeadZone) {
  String deadZoneCommand = "DZ";  
  if(inputDeadZone>0 && inputDeadZone<1){
    mem.writeFloat(CONF_SETTINGS_FILE,deadZoneCommand,inputDeadZone); 
    ps.setDeadzone(true,inputDeadZone);  
  }

}
//***SET JOYSTICK DEADZONE API FUNCTION***//
// Function   : setJoystickDeadZone  
// 
// Description: This function is redefinition of main setJoystickDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalArray : int* : The array of int which should contain one element with value of zero.
// 
// Return     : void
void setJoystickDeadZone(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==1){
    setCursorCalibration(responseEnabled, apiEnabled);
  }
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

  int tempPressureValue = (int) ps.getDiffPressure();

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
// Function   : getSipPressureThreshold 
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
float getSipPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String sipThresholdCommand = "ST";  
  float tempSipThreshold;
  tempSipThreshold = mem.readFloat(CONF_SETTINGS_FILE, sipThresholdCommand);

  if(tempSipThreshold=<0 || tempSipThreshold>=100.0){
    tempSipThreshold = CONF_SIP_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE,sipThresholdCommand,tempSipThreshold);   
  }
  ps.setSipThreshold(tempSipThreshold);
  return tempSipThreshold;
}
//***GET PRESSURE THRESHOLD API FUNCTION***//
// Function   : getSipPressureThreshold 
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
void getSipPressureThreshold(bool responseEnabled, bool apiEnabled, int* optionalArray) {
  if(optionalArray[0]==0){
    getPressureThreshold(responseEnabled, apiEnabled);
  }
}

float getPuffPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String puffThresholdCommand = "PT";  
  float tempPuffThreshold;
  tempPuffThreshold = mem.readFloat(CONF_SETTINGS_FILE, puffThresholdCommand);

  if(tempSipThreshold=<0 || tempSipThreshold>=100.0){
    tempPuffThreshold = CONF_PUFF_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE,puffThresholdCommand,tempPuffThreshold);   
  }
  ps.setPuffThreshold(tempPuffThreshold);
  return tempPuffThreshold;
}

void getSipPressureThreshold(bool responseEnabled, bool apiEnabled, int* optionalArray) {
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
//               inputSipThreshold : float : The new sip pressure threshold in percentage.
// 
// Return     : void
//*********************************//
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, float inputSipThreshold) {
  String sipThresholdCommand = "ST";  

  if(inputSipThreshold>1 && inputSipThreshold<100.0){
    mem.writeFloat(CONF_SETTINGS_FILE,sipThresholdCommand,inputSipThreshold);   
    ps.setSipThreshold(inputSipThreshold);
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
//               inputPuffThreshold : float : The new sip pressure threshold in percentage.
// 
// Return     : void
//*********************************//
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, float inputPuffThreshold) {
  String puffThresholdCommand = "PT";  

  if(inputPuffThreshold>1 && inputPuffThreshold<100.0){
    mem.writeFloat(CONF_SETTINGS_FILE,puffThresholdCommand,inputPuffThreshold);   
    ps.setPuffThreshold(inputPuffThreshold);
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
// Return     : communicationMethod : int : The current state of communication method.
//*********************************//
int getCommunicationMethod(bool responseEnabled, bool apiEnabled) {
  String comModeCommand = "CM";   
  int tempComMode;
  tempComMode = mem.readInt(CONF_SETTINGS_FILE, comModeCommand);

  if(tempComMode<1 || tempComMode>2){
    tempComMode = CONF_COM_MODE;
    mem.writeInt(CONF_SETTINGS_FILE,comModeCommand,tempComMode);   
  }

  printResponseSingle(responseEnabled,apiEnabled,true,0,"CM,0",true,tempComMode);

  return tempComMode;
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
  String comModeCommand = "CM";   
  if (comMode < 2)
  {
    comMode++;
  }
  else
  {
    comMode = 1;
  }
  mem.writeInt(CONF_SETTINGS_FILE,comModeCommand,comMode);    
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "CM,1", true, comMode);
 
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
  String debugModeCommand = "DM";   
  int tempDebugState;
  tempDebugState = mem.readInt(CONF_SETTINGS_FILE, debugModeCommand);

  if(tempDebugState<0 || tempDebugState>1){
    tempDebugState = CONF_DEBUG_MODE;
    mem.writeInt(CONF_SETTINGS_FILE,debugModeCommand,tempDebugState);   
  }

  printResponseSingle(responseEnabled,apiEnabled,true,0,"CM,0",true,tempDebugState);

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
  String debugModeCommand = "DM";   
  
  if(inputDebugState>=0 && inputDebugState<=1){
    mem.writeInt(CONF_SETTINGS_FILE,debugModeCommand,inputDebugState);   
  }
  
  printResponseSingle(responseEnabled, apiEnabled, true, 0, "DM,1", true, inputDebugState);
  
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
