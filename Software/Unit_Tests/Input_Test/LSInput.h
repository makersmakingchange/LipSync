
#define INPUT_BUTTON1_PIN 9 // pushbutton S1
#define INPUT_BUTTON2_PIN 6 // pushbutton S2
#define INPUT_BUTTON3_PIN 5 // pushbutton S3
#define INPUT_SWITCH1_PIN A1 // 3.5mm jack SW1
#define INPUT_SWITCH2_PIN 11 // 3.5mm jack SW2
#define INPUT_SWITCH3_PIN 10 // 3.5mm jack SW3

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

StopWatch inputTimer[2];
StopWatch testTimer[1];

typedef struct {
  int mainState;            //button1 + 2*button2 + 4*button3
  int secondaryState;       //waiting = 0, started = 1, detected = 2
  unsigned long elapsedTime;     //in ms
} inputStruct;



LSQueue <inputStruct> inputButtonQueue;
LSQueue <inputStruct> inputSwitchQueue;

class LSInput {
  private: 
    void resetTimer();
    unsigned long getTime();
  public:
    LSInput();
    void begin();                                    
    void clear();  
    void update(int type);    
    void updateButton();   
    void updateSwitch();  
    inputStruct getButtonState();
    inputStruct getSwitchState();
};

LSInput::LSInput() {

}



void LSInput::begin() {
  
  pinMode(INPUT_SWITCH1_PIN, INPUT_PULLUP);// 3.5mm jack SW1
  pinMode(INPUT_SWITCH2_PIN, INPUT_PULLUP);// 3.5mm jack SW2
  pinMode(INPUT_SWITCH3_PIN, INPUT_PULLUP);// 3.5mm jack SW3
  pinMode(INPUT_BUTTON1_PIN, INPUT_PULLUP);// pushbutton S1
  pinMode(INPUT_BUTTON2_PIN, INPUT_PULLUP);// pushbutton S1
  pinMode(INPUT_BUTTON3_PIN, INPUT_PULLUP); //right 3.5mm jack
  
  clear();
}

void LSInput::clear() {
    
    inputStruct inputPrevState = {INPUT_MAIN_STATE_NONE, INPUT_SEC_STATE_WAITING, 0};

    inputButtonQueue.push(inputPrevState);
    inputSwitchQueue.push(inputPrevState);
    
    inputTimer[0].stop();                                      
    inputTimer[0].reset();                                                                        
    inputTimer[0].start(); 

    inputTimer[1].stop();                                      
    inputTimer[1].reset();                                                                        
    inputTimer[1].start(); 
}

void LSInput::update(int type) {
  //resetTimer();

  int inputState[3];
  int inputAllState;
  inputStruct inputCurrState, inputPrevState;
  
  (type==0) ? inputState[0] = !digitalRead(INPUT_BUTTON1_PIN) : inputState[0] = !digitalRead(INPUT_SWITCH1_PIN);// 3.5mm jack SW1
  (type==0) ? inputState[1] = !digitalRead(INPUT_BUTTON2_PIN) : inputState[1] = !digitalRead(INPUT_SWITCH2_PIN);// 3.5mm jack SW1
  (type==0) ? inputState[2] = !digitalRead(INPUT_BUTTON3_PIN) : inputState[2] = !digitalRead(INPUT_SWITCH3_PIN);// 3.5mm jack SW1


  inputAllState = inputState[0] + 2 * inputState[1] + 4 * inputState[2] ;
  
 (type==0) ? inputPrevState = inputButtonQueue.end() : inputPrevState = inputSwitchQueue.end(); 
  
  if(inputPrevState.mainState == inputAllState){
    inputCurrState = {inputAllState, inputPrevState.secondaryState, inputTimer[type].elapsed()};
    //Serial.println("a");
    (type==0) ? inputButtonQueue.update(inputCurrState) : inputSwitchQueue.update(inputCurrState);
  } else {
      if(inputPrevState.secondaryState==INPUT_SEC_STATE_RELEASED && inputState==INPUT_MAIN_STATE_NONE){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_WAITING, 0};
        //Serial.println("b");
      }
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_RELEASED && inputState!=INPUT_MAIN_STATE_NONE){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_STARTED, 0};
        //Serial.println("c");
      }      
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_WAITING){
        inputCurrState = {inputAllState, INPUT_SEC_STATE_STARTED, 0};
        //Serial.println("d");
      }     
      else if(inputPrevState.secondaryState==INPUT_SEC_STATE_STARTED){
        inputCurrState = {inputPrevState.mainState, INPUT_SEC_STATE_RELEASED, inputPrevState.elapsedTime};
        //Serial.println("e");
      }
      //Push the new state   
      if (type==0){
        inputButtonQueue.push(inputCurrState);
        if(inputButtonQueue.count()==12){inputButtonQueue.pop(); }  //Keep last 12 objects         
      } else{
        inputSwitchQueue.push(inputCurrState);
        if(inputSwitchQueue.count()==12){inputSwitchQueue.pop(); }  //Keep last 12 objects                
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

void LSInput::updateButton(){
   update(INPUT_TYPE_BUTTON);
}

void LSInput::updateSwitch(){
   update(INPUT_TYPE_SWITCH);  
}

inputStruct LSInput::getButtonState() {
  inputStruct inputCurrState = inputButtonQueue.end();  //Get the state
  return inputCurrState;
}

inputStruct LSInput::getSwitchState() {
  inputStruct inputCurrState = inputSwitchQueue.end();  //Get the state
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
