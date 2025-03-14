/* 
* File: LSMemory.h
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
#ifndef _LSMEMORY_H
#define _LSMEMORY_H

#include <ArduinoJson.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

#define BUFFER_SIZE  512

JsonDocument doc;

// Initialize FileSystem
File file(InternalFS);

class LSMemory {
  public:
    LSMemory();   
    void begin();                                                   
    void initialize(String fileString, String jsonString);   
    void clear(String fileString);
    void format();
    String readAll(String fileString);
    void writeAll(String fileString, String jsonString);
    JsonObject readObject(String fileString);
    int readInt(String fileString, String key);
    float readFloat(String fileString, String key);
    String readString(String fileString, String key);
    pointFloatType readPoint(String fileString, String key);
    void writeObject(String fileString, String key, JsonObject obj);
    void writeInt(String fileString, String key, int value);
    void writeFloat(String fileString, String key, float value);
    void writeString(String fileString, String key, String value);
    void writePoint(String fileString, String key, pointFloatType value);
};


//*********************************//
// Function   : LSMemory 
// 
// Description: Construct LSMemory
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
LSMemory::LSMemory() {

}

//*********************************//
// Function   : begin 
// 
// Description: Begins internal file structure.
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSMemory::begin() {
  InternalFS.begin();
}


//*********************************//
// Function   : initialize 
// 
// Description: Initializes memory file.
// 
// Arguments :  fileString : String : the name of the file
//              jsonString : String : the default json string with list of key and value pairs 
// 
// Return     : void
//*********************************//
void LSMemory::initialize(String fileString, String jsonString) {
  const char* fileName = fileString.c_str();

  file.open(fileName, FILE_O_READ);
  if ( file )
  {
    uint32_t readlen;
    char buffer[BUFFER_SIZE] = { 0 };
    readlen = file.read(buffer, sizeof(buffer));

    delay(5);
    buffer[readlen] = 0;
    file.close();
    delay(5);
  } else
  {
    if( file.open(fileName, FILE_O_WRITE) )
    {
      const char* jsonChar = jsonString.c_str();
      file.write(jsonChar, strlen(jsonChar));
      delay(1);
      file.close();
      delay(1);
    }
  }
}


//*********************************//
// Function   : clear 
// 
// Description: Deletes the memory file
// 
// Arguments :  fileString : String : the name of the file
// 
// Return     : void
//*********************************//
void LSMemory::clear(String fileString) {
  const char* fileName = fileString.c_str();
  InternalFS.remove(fileName);
  file.close();
  delay(1);  
}

//***FORMAT ALL FILES FUNCTION***//
//*********************************//
// Function   : format 
// 
// Description: Formats all files in the internal file system
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
void LSMemory::format(){
  InternalFS.format();
  delay(1);
}


//*********************************//
// Function   : readAll 
// 
// Description: Reads all Json data from the settings file and stores it in a buffer
// 
// Arguments :  fileString : String : the name of the file
// 
// Return     : buffer : String array : Output from file
//*********************************//
String LSMemory::readAll(String fileString){
  const char* fileName = fileString.c_str();

  file.open(fileName, FILE_O_READ);
  uint32_t readlen;
  char buffer[BUFFER_SIZE] = { 0 };
  readlen = file.read(buffer, sizeof(buffer));

  delay(1);
  buffer[readlen] = 0;
  file.close();
  delay(1);
  return buffer;
}


//*********************************//
// Function   : writeAll 
// 
// Description: Replaces the entire settings file with new values
// 
// Arguments :  fileString : String : the name of the file
//              jsonString : String : a json string with list of key and value pairs 
// 
// Return     : void
//*********************************//
void LSMemory::writeAll(String fileString, String jsonString){

    const char* fileName = fileString.c_str();

    const char* jsonChar = jsonString.c_str();
    
    InternalFS.remove(fileName);
    delay(1);
    
    file.open(fileName, FILE_O_WRITE);
    delay(1);
    
    file.write(jsonChar, strlen(jsonChar));
    delay(1);
    file.close();

}


//*********************************//
// Function   : readObject 
// 
// Description: Reads the settings file and returns it as a Json Object
// 
// Arguments :  fileString : String : the name of the file
// 
// Return     : obj : JsonObject : A Json Object of the file contents
//*********************************//
JsonObject LSMemory::readObject(String fileString){
  
  uint32_t readLenght;
  const char* fileName = fileString.c_str();
  char buffer[BUFFER_SIZE] = { 0 };
  file.open(fileName, FILE_O_READ);
  delay(1);
  readLenght = file.read(buffer, sizeof(buffer));
  delay(1);
  buffer[readLenght] = 0;
  deserializeJson(doc, String(buffer));
  JsonObject obj = doc.as<JsonObject>();
  return obj;
}


//*********************************//
// Function   : readInt 
// 
// Description: Returns the value of the corresponding key within the settings file.
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json value
// 
// Return     : value : int : The returned value corresponding to the key.
//*********************************//
int LSMemory::readInt(String fileString, String key){
  int value = readObject(fileString)[key];
  return value;
}


//*********************************//
// Function   : readFloat 
// 
// Description: Returns the value of the corresponding key within the settings file.
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json value
// 
// Return     : value : float : The returned value corresponding to the key.
float LSMemory::readFloat(String fileString, String key){
  float value = readObject(fileString)[key];
  return value;
}


//*********************************//
// Function   : readString 
// 
// Description: Returns the value of the corresponding key within the settings file.
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json value
// 
// Return     : value : string : The returned value corresponding to the key.
String LSMemory::readString(String fileString, String key){
  String value = readObject(fileString)[key];
  return value;
}

//*********************************//
// Function   : readPoint 
// 
// Description: Returns the value of the corresponding key within the settings file.
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json value
// 
// Return     : value : point : The returned value corresponding to the key.
pointFloatType LSMemory::readPoint(String fileString, String key){
  JsonObject obj = readObject(fileString);
  return {obj[key][0], obj[key][1]};
}


//*********************************//
// Function   : writeObject 
// 
// Description: General function for writing an individual attribute-value pair to the settings file
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json attribute
//              obj : JsonObject : the Json Object value
// 
// Return     : void
//*********************************//
void LSMemory::writeObject(String fileString, String key, JsonObject obj){

    const char* fileName = fileString.c_str();
    
    String jsonString;
    serializeJson(obj, jsonString);
    const char* jsonChar = jsonString.c_str();
    
    InternalFS.remove(fileName);
    delay(1);
    
    file.open(fileName, FILE_O_WRITE);
    delay(1);
    
    file.write(jsonChar, strlen(jsonChar));
    delay(1);
    file.close();
}


//*********************************//
// Function   : writeInt 
// 
// Description: Writing an individual int value to the corresponding attribute-value pair given by the key to the settings file
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json attribute
//              value : int : The integer to write
// 
// Return     : void
//*********************************//
void LSMemory::writeInt(String fileString, String key, int value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

//*********************************//
// Function   : writeFloat 
// 
// Description: Writing an individual float value to the corresponding attribute-value pair given by the key to the settings file
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json attribute
//              value : float : The float value to write
// 
// Return     : void
//*********************************//
void LSMemory::writeFloat(String fileString, String key, float value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

//*********************************//
// Function   : writeString 
// 
// Description: Writing an individual string value to the corresponding attribute-value pair given by the key to the settings file
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json attribute
//              value : string : The string to write to the settings file
// 
// Return     : void
//*********************************//
void LSMemory::writeString(String fileString, String key, String value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

//*********************************//
// Function   : writePoint 
// 
// Description: Writing an individual point value to the corresponding attribute-value pair given by the key to the settings file
// 
// Arguments :  fileString : String : the name of the file
//              key : String : the key of the desired Json attribute
//              value : pointFloatType : The point to write to the settings file
// 
// Return     : void
//*********************************//
void LSMemory::writePoint(String fileString, String key, pointFloatType value){
  JsonObject obj = readObject(fileString);
  JsonArray point = obj[key].to<JsonArray>();
  point.add(value.x);
  point.add(value.y);
  writeObject(fileString,key,obj);
}

#endif 
