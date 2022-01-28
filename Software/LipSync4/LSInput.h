//Header definition
#ifndef _LSINPUT_H
#define _LSINPUT_H


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

#define INPUT_REACTION_TIME 20

#define INPUT_ACTION_TIMEOUT 60000

typedef struct {
  int mainState;            //button1 + 2*button2 + 4*button3
  int secondaryState;       //waiting = 0, started = 1, detected = 2
  unsigned long elapsedTime;     //in ms
} inputStruct;



LSCircularBuffer <inputStruct> inputBuffer(5);

class LSInput {
  private: 
    int *_inputPinArray;
    int _inputNumber;
    int inputState[3];
    int inputAllState;
    inputStruct inputCurrState = {0, 0, 0};
    inputStruct inputPrevState = {0, 0, 0};
    LSTimer inputStateTimer;
    int inputStateTimerId;
  public:
    LSInput(int* inputPinArray, int inputNumber);
    ~LSInput();
    void begin();                                    
    void clear();  
    void update();    
    inputStruct getInputState();
};

LSInput::LSInput(int* inputPinArray, int inputNumber)
{
  _inputPinArray = new int[inputNumber];

  _inputNumber = inputNumber;
  
  for (int i = 0; i < inputNumber; i++){
    pinMode(inputPinArray[i], INPUT_PULLUP);
    _inputPinArray[i] = inputPinArray[i];
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

  inputBuffer.pushElement(inputPrevState);

  inputStateTimerId = inputStateTimer.startTimer();
}


void LSInput::update() {
  inputStateTimer.run();

  inputAllState = 0;


  inputState[0] = !digitalRead(_inputPinArray[0]);
  inputState[1] = !digitalRead(_inputPinArray[1]);
  inputState[2] = !digitalRead(_inputPinArray[2]);
  
  for (int i = 0; i < _inputNumber; i++){
    inputState[i] = !digitalRead(_inputPinArray[i]);
    inputAllState+= pow(2,i) * inputState[i];
  }  
  
  inputPrevState = inputBuffer.getLastElement();
  

  //inputAllState = inputState[0] + 2 * inputState[1] + 4 * inputState[2];
 
  // prev: none,waiting  current : none 
  // prev: press x,started  current : press x 
  if((inputAllState == inputPrevState.mainState && inputPrevState.secondaryState != INPUT_SEC_STATE_RELEASED)){
    inputCurrState = {inputAllState, inputPrevState.secondaryState, inputStateTimer.elapsedTime(inputStateTimerId)};
    inputBuffer.updateLastElement(inputCurrState);  
    //Serial.println("a");
  } // prev: press x,started  current : press y and elapsed time<reaction time ms
  else if(inputAllState != INPUT_MAIN_STATE_NONE && inputPrevState.secondaryState == INPUT_SEC_STATE_STARTED && inputPrevState.elapsedTime<INPUT_REACTION_TIME){
    inputCurrState = {inputAllState, inputPrevState.secondaryState, inputStateTimer.elapsedTime(inputStateTimerId)};

    inputBuffer.updateLastElement(inputCurrState);  
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
      
      inputBuffer.pushElement(inputCurrState);   

      //Reset and start the timer
      inputStateTimer.restartTimer(inputStateTimerId);
  }
  
  //No action in 1 minute : reset timer
  if(inputPrevState.secondaryState==INPUT_SEC_STATE_WAITING && inputStateTimer.elapsedTime(inputStateTimerId)>INPUT_ACTION_TIMEOUT){
      //Reset and start the timer         
      inputStateTimer.restartTimer(inputStateTimerId);
  }
  
}


inputStruct LSInput::getInputState() {
  inputStruct inputCurrState = inputBuffer.getLastElement();  //Get the state
  return inputCurrState;
}

#endif 
