

#define INPUT_MAIN_STATE_NONE           0
#define INPUT_MAIN_STATE_B1_PRESSED     1
#define INPUT_MAIN_STATE_B2_PRESSED     2
#define INPUT_MAIN_STATE_B12_PRESSED    3
#define INPUT_MAIN_STATE_B3_PRESSED     4
#define INPUT_MAIN_STATE_B13_PRESSED    5
#define INPUT_MAIN_STATE_B23_PRESSED    6
#define INPUT_MAIN_STATE_B123_PRESSED   7

#define INPUT_SEC_STATE_WAITING 0
#define INPUT_SEC_STATE_STARTED 1
#define INPUT_SEC_STATE_RELEASED 2

#define INPUT_TYPE_BUTTON 0
#define INPUT_TYPE_SWITCH 1

#define INPUT_REACTION_TIME 150

StopWatch inputTimer[2];
StopWatch testTimer[1];

typedef struct {
  int mainState;            //button1 + 2*button2 + 4*button3
  int secondaryState;       //waiting = 0, started = 1, detected = 2
  unsigned long elapsedTime;     //in ms
} inputStruct;



LSCircularBuffer <inputStruct> inputButtonBuffer(12);
LSCircularBuffer <inputStruct> inputSwitchBuffer(12);

class LSInput {
  private: 
    void resetTimer();
    unsigned long getTime();
    int *_buttonPinArray;
    int *_switchPinArray;
    int _buttonNumber;
    int _switchNumber;
  public:
    LSInput(int* buttonPinArray, int* switchPinArray, int buttonNumber, int switchNumber);
    ~LSInput();
    void begin();                                    
    void clear();  
    void update();    
    void readState(int type); 
    inputStruct getButtonState();
    inputStruct getSwitchState();
};

LSInput::LSInput(int* buttonPinArray, int* switchPinArray, int buttonNumber, int switchNumber)
{
  _buttonPinArray = new int[buttonNumber];
  _switchPinArray = new int[switchNumber];

  _buttonNumber = buttonNumber;
  _switchNumber = switchNumber;
  
  for (int i = 0; i < buttonNumber; i++){
    pinMode(buttonPinArray[i], INPUT_PULLUP);
    _buttonPinArray[i] = buttonPinArray[i];
  }
  for (int i = 0; i < switchNumber; i++){
    pinMode(switchPinArray[i], INPUT_PULLUP);
    _switchPinArray[i] = switchPinArray[i];
  }
}

LSInput::~LSInput()
{

}


void LSInput::begin() {
  clear();
}

void LSInput::clear() {
    
    inputStruct inputPrevState = {INPUT_MAIN_STATE_NONE, INPUT_SEC_STATE_WAITING, 0};

    inputButtonBuffer.pushElement(inputPrevState);
    inputSwitchBuffer.pushElement(inputPrevState);

    
    inputTimer[0].stop();                                      
    inputTimer[0].reset();                                                                        
    inputTimer[0].start(); 
    
    inputTimer[1].stop();                                      
    inputTimer[1].reset();                                                                        
    inputTimer[1].start(); 
}

void LSInput::update() {
  readState(INPUT_TYPE_BUTTON);
  readState(INPUT_TYPE_SWITCH);
}

void LSInput::readState(int type) {
  //resetTimer();

  int inputState[3];
  int inputAllState = 0;
  inputStruct inputCurrState = {0, 0, 0};
  inputStruct inputPrevState = {0, 0, 0};

  if(type==INPUT_TYPE_BUTTON){
    for (int i = 0; i < _buttonNumber; i++){
      inputState[i] = !digitalRead(_buttonPinArray[i]);
      inputAllState+= pow(2,i) * inputState[i];
    }  
  }
  else if(type==INPUT_TYPE_SWITCH){
    for (int i = 0; i < _switchNumber; i++){
      inputState[i] = !digitalRead(_switchPinArray[i]);
      inputAllState+= pow(2,i) * inputState[i];
    }  
  }
  
  //inputAllState = inputState[0] + 2 * inputState[1] + 4 * inputState[2] ;
  
 (type==0) ? inputPrevState = inputButtonBuffer.getLastElement() : inputPrevState = inputSwitchBuffer.getLastElement(); 
  // prev: none,waiting  current : none 
  // prev: press x,started  current : press x 
  if((inputAllState == inputPrevState.mainState && inputPrevState.secondaryState != INPUT_SEC_STATE_RELEASED)){
    inputCurrState = {inputAllState, inputPrevState.secondaryState, inputTimer[type].elapsed()};

    (type==0) ? inputButtonBuffer.updateLastElement(inputCurrState) : inputSwitchBuffer.updateLastElement(inputCurrState);
    //Serial.println("a");
  } // prev: press x,started  current : press y and elapsed time<reaction time ms
  else if(inputAllState != INPUT_MAIN_STATE_NONE && inputPrevState.secondaryState == INPUT_SEC_STATE_STARTED && inputPrevState.elapsedTime<INPUT_REACTION_TIME){
    inputCurrState = {inputAllState, inputPrevState.secondaryState, inputTimer[type].elapsed()};

    (type==0) ? inputButtonBuffer.updateLastElement(inputCurrState) : inputSwitchBuffer.updateLastElement(inputCurrState);
    //Serial.println("b");
  } 
  else { 
      // prev: none,waiting  current : press x 
      if(inputPrevState.secondaryState==INPUT_SEC_STATE_WAITING){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_STARTED, 0};
        //Serial.println("c");
      } // prev: press x,started  current : none  OR prev: press x,started  current : press y and elapsed time>=150
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_STARTED){
        inputCurrState = {inputPrevState.mainState, INPUT_SEC_STATE_RELEASED, inputPrevState.elapsedTime};
        //Serial.println("d");
      } // prev: press x,released  current : none 
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_RELEASED && inputAllState==INPUT_MAIN_STATE_NONE){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_WAITING, 0};
        //Serial.println("e");
      } // prev: press x,released  current : press y 
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_RELEASED && inputAllState!=INPUT_MAIN_STATE_NONE){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_STARTED, 0};
        //Serial.println("f");
      }     
      //Push the new state 
      
      if (type==0){
        inputButtonBuffer.pushElement(inputCurrState);     
      } else{
        inputSwitchBuffer.pushElement(inputCurrState);           
      }

      //Reset and start the timer
      inputTimer[type].stop();      
      inputTimer[type].reset();                                                                        
      inputTimer[type].start(); 
  }

  //No action in 1 minute : reset timer
  if(inputPrevState.secondaryState==INPUT_SEC_STATE_WAITING && inputTimer[type].elapsed()>60000){
      //Reset and start the timer         
      inputTimer[type].stop();      
      inputTimer[type].reset();                                                                        
      inputTimer[type].start();   
  }
  

}


inputStruct LSInput::getButtonState() {
  inputStruct inputCurrState = inputButtonBuffer.getLastElement();  //Get the state
  return inputCurrState;
}

inputStruct LSInput::getSwitchState() {
  inputStruct inputCurrState = inputSwitchBuffer.getLastElement();  //Get the state
  return inputCurrState;
}

//***RESET TIMER FUNCTION***//

void LSInput::resetTimer() {
  testTimer[0].stop();                                //Reset and start the timer         
  testTimer[0].reset();                                                                        
  testTimer[0].start(); 
}

//***GET TIME FUNCTION***//

unsigned long LSInput::getTime() {
  unsigned long finalTime = testTimer[0].elapsed(); 
  testTimer[0].stop();                                //Reset and start the timer         
  testTimer[0].reset(); 
  return finalTime;
}
