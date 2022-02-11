

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

// Declare array of API functions
_functionList apiFunction[3] = {
  getModelNumberFunction,
  getVersionNumberFunction
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
