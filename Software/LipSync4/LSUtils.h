
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
  int ledNumber;
  int ledBlinkNumber;
  unsigned long ledBlinkTime;
  int ledBrightness;
} ledStateStruct;

//Input(sip and puff, switches ,buttons) actions relation with output actions structure
typedef struct
{
  uint8_t inputActionNumber;
  uint8_t inputActionState;
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;

//Input (sip and puff, switches ,buttons) states structure
typedef struct {
  int mainState;                 //button1 + 2*button2 + 4*button3  or none : 0 ,sip : 1, puff : 2
  int secondaryState;            //waiting = 0, started = 1, released = 2
  unsigned long elapsedTime;     //in ms
} inputStateStruct;


void printResponseInt(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, int responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));

}

void printResponseIntArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, int responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix) + ":";
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex]);  
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}


void printResponseIntPoint(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, pointIntType responseParameter) {
  String responseParameterString = responseParameter.x + "|" + responseParameter.y;
  
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseIntPointArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, pointIntType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix) + ":";
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex].x);  
    responseParameterString.concat("|"); 
    responseParameterString.concat(responseParameter[parameterIndex].y); 
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseFloat(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, float responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));

}

void printResponseFloatArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, float responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix) + ":";
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex]);  
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseFloatPoint(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, pointFloatType responseParameter) {
  String responseParameterString = responseParameter.x + "|" + responseParameter.y;
  
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseIntPointArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, pointFloatType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix) + ":";
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex].x);  
    responseParameterString.concat("|"); 
    responseParameterString.concat(responseParameter[parameterIndex].y); 
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseString(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responseParameter) {
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