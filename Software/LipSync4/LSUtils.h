
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