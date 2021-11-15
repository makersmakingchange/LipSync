#include "TinyUSB_Gamepad.h"

LipSyncGamepad Gamepad;
uint8_t count = 0;
void setup()
{
  Gamepad.begin();
  
}


void loop()
{
  // 8 buttons
  if (count > 7) {
    Gamepad.releaseAll();
    count = 0;
  }
  Gamepad.press(count);
  count++;

  // Move x/y Axis to a random position
  Gamepad.xAxis(random(256));
  Gamepad.yAxis(random(256));

  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.send();
  delay(100);
}
