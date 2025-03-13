/* 
* File: LSBuzzer.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.1rc (10 March 2025)
  License: GPL v3.0 or later

  Copyright (C) 2024 - 2025 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/

// Header definition
#ifndef _LSBUZZER_H
#define _LSBUZZER_H

// Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

class LSBuzzer {
  public: 
    LSBuzzer();
    void begin();
    void update();
    void clear();
    void playStartupSound();
    void playReadySound();
    void playErrorSound();
    void playShutdownSound();
    void enable();
    void disable();
    void setSoundModeLevel(int inputSoundMode);
    void calibCornerTone();
    void calibCenterTone();

  private:
    boolean _buzzerOn = true; // Sound feedback is on by default
    int _soundModeLevel; // Levels that correspond to volume
};

//*********************************//
// Function   : LSBuzzer 
// 
// Description: Construct LSBuzzer
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
LSBuzzer::LSBuzzer() {
}

//*********************************//
// Function   : begin function 
// 
// Description: Initialize LSBuzzer with default settings 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::begin(){
  pinMode(CONF_BUZZER_PIN, OUTPUT);
  // Read sound mode level from memory
  _soundModeLevel = getSoundMode(false, false);
}

//*********************************//
// Function   : update
// 
// Description: Updates buzzer (currently not implemented)
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::update(){
  
}

//*********************************//
// Function   : clear function 
// 
// Description: Clears buzzer (currently no implemented) 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::clear(){
  // No current implementation  
}

//*********************************//
// Function   : enable
// 
// Description: Enable sounds feedback
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::enable(){
  _buzzerOn = true;
}

//*********************************//
// Function   : disable
// 
// Description: Disable sound feedback
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::disable(){
  _buzzerOn = false;
}

//*********************************//
// Function   : setSoundModeLevel
// 
// Description: Set sound level
// 
// Arguments :  inputSoundMode : int : desired sound level
// 
// Return     : void
//*********************************//
void LSBuzzer::setSoundModeLevel(int inputSoundMode){
  _soundModeLevel = inputSoundMode;
}

//*********************************//
// Function   : playStartupSound 
// 
// Description: Sound to play to indicate LipSync startup.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::playStartupSound(){ 
  if (_buzzerOn && (_soundModeLevel != CONF_SOUND_MODE_OFF)){
    tone(CONF_BUZZER_PIN, NOTE_F5, 200);
  }
}

//*********************************//
// Function   : playReadySound 
// 
// Description: Sound to play to indicate that LipSync is ready to use.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::playReadySound(){ 
  if (_buzzerOn && (_soundModeLevel != CONF_SOUND_MODE_OFF)){
    tone(CONF_BUZZER_PIN, NOTE_F5, 500);
    delay(500);  // TODO: add timer instead of delay
    tone(CONF_BUZZER_PIN, NOTE_C6, 250);
  }
}

//*********************************//
// Function   : playErrorSound 
// 
// Description: Sound to play to indicate LipSync error.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::playErrorSound(){
    tone(CONF_BUZZER_PIN, NOTE_G4, 500);
    delay(500);
    tone(CONF_BUZZER_PIN, NOTE_C4, 500);       
}

//*********************************//
// Function   : playShutdownSound 
// 
// Description: Sound to play to indicate LipSync shutdown.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::playShutdownSound(){
  if (_buzzerOn && (_soundModeLevel != CONF_SOUND_MODE_OFF)) {
    tone(CONF_BUZZER_PIN, NOTE_C6, 500);
    delay(250);
    tone(CONF_BUZZER_PIN, NOTE_G5, 500);       
    delay(250);
    tone(CONF_BUZZER_PIN, NOTE_C5, 300);
    delay(500);  
  } 
}

//*********************************//
// Function   : calibCornerTone 
// 
// Description: Sound to play to indicate corner during full calibration. 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::calibCornerTone(){
  if (_buzzerOn && (_soundModeLevel != CONF_SOUND_MODE_OFF)){
    tone(CONF_BUZZER_PIN, NOTE_A4, 300);
  }
}

//*********************************//
// Function   : calibCenterTone 
// 
// Description: Sound to play to indicate center during full calibration. 
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSBuzzer::calibCenterTone(){
  if (_buzzerOn && (_soundModeLevel != CONF_SOUND_MODE_OFF)){
    tone(CONF_BUZZER_PIN, NOTE_A6, 500);
  }
}

#endif
