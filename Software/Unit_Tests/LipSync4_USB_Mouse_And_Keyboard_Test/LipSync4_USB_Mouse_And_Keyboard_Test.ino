#include "LSUSB_Mouse_And_Keyboard.h"

LSMouse lipSyncMouse;
LSKeyboard lipSyncKeyboard;

// the setup function runs once when you press reset or power the board
void setup()
{
lipSyncMouse.begin();
lipSyncKeyboard.begin();
}

void loop()
{
    delay(12000);
    /*
    lipSyncMouse.moveAll(10, 0, 0, 0);
    delay(1000);
    lipSyncMouse.moveAll(0, 10, 0, 0);
    delay(1000);
    lipSyncMouse.moveAll(-10, 0, 0, 0);
    delay(1000);
    lipSyncMouse.moveAll(0, -10, 0, 0);
    delay(1000);
    lipSyncMouse.moveAll(0, 0, 10, 0);
    delay(1000);
    lipSyncMouse.moveAll(0, 0, -10, 0);
    delay(1000);
    lipSyncMouse.scroll(10);
    delay(1000);
    lipSyncMouse.scroll(-10);
    delay(1000);
    lipSyncMouse.pan(10);
    delay(1000);
    lipSyncMouse.pan(-10);
    delay(1000);
    lipSyncMouse.press(MOUSE_LEFT);
    delay(500);
    lipSyncMouse.move(100, 0);
    delay(500);
    lipSyncMouse.release(MOUSE_LEFT);
    delay(1000);
    lipSyncMouse.click(MOUSE_LEFT);
    delay(1000);
    lipSyncMouse.click(MOUSE_RIGHT);
    delay(1000);
    lipSyncMouse.click(MOUSE_MIDDLE);
    delay(1000);
     

    lipSyncKeyboard.write('B');
    delay(2000);

    lipSyncKeyboard.press(KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_1);
    lipSyncKeyboard.release(KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_1);
    delay(2000);
    lipSyncKeyboard.press(0, HID_KEY_F1);
    lipSyncKeyboard.releaseAll();
    */

}
