/* 
* File: LSMemory.h
* Firmware: LipSync
* Developed by: MakersMakingChange
* Version: v4.0.rc1 (26 January 2024)
  License: GPL v3.0 or later

  Copyright (C) 2024 Neil Squire Society
  This program is free software: you can redistribute it and/or modify it under the terms of
  the GNU General Public License as published by the Free Software Foundation,
  either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <http://www.gnu.org/licenses/>
*/

//Header definition
#ifndef _LSMEMORY_H
#define _LSMEMORY_H

#include <ArduinoJson.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

#define BUFFER_SIZE  512

DynamicJsonDocument doc(1024);
//Initialize FileSystem
File file(InternalFS);
class LSMemory {
  public:
    LSMemory();   
    void begin();                                                   
    void initialize(String fileString,String jsonString);   
    void clear(String fileString);
    void format();
    String readAll(String fileString);
    void writeAll(String fileString,String jsonString);
    JsonObject readObject(String fileString);
    int readInt(String fileString,String key);
    float readFloat(String fileString,String key);
    String readString(String fileString,String key);
    pointFloatType readPoint(String fileString,String key);
    void writeObject(String fileString,String key,JsonObject obj);
    void writeInt(String fileString,String key,int value);
    void writeFloat(String fileString,String key,float value);
    void writeString(String fileString,String key,String value);
    void writePoint(String fileString,String key,pointFloatType value);
};

LSMemory::LSMemory() {

}

//***BEGIN FILE SYSTEM FUNCTION***//


void LSMemory::begin() {
  InternalFS.begin();
}

//***INITIALIZE MEMORY FILE FUNCTION***//

void LSMemory::initialize(String fileString,String jsonString) {
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
  }else
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

//***DELETE MEMORY FILE FUNCTION***//

void LSMemory::clear(String fileString) {
  const char* fileName = fileString.c_str();
  InternalFS.remove(fileName);
  file.close();
  delay(1);  
}

//***FORMAT ALL FILES FUNCTION***//

void LSMemory::format(){
  InternalFS.format();
  delay(1);
}

//***READ JSON STRING FROM MEMORY FILE FUNCTION***//

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

//***WRITE JSON STRING TO MEMORY FILE FUNCTION***//

void LSMemory::writeAll(String fileString,String jsonString){

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

//***READ MEMORY FILE FUNCTION***//
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

int LSMemory::readInt(String fileString,String key){
  int value = readObject(fileString)[key];
  return value;
}


float LSMemory::readFloat(String fileString,String key){
  float value = readObject(fileString)[key];
  return value;
}

String LSMemory::readString(String fileString,String key){
  String value = readObject(fileString)[key];
  return value;
}

pointFloatType LSMemory::readPoint(String fileString,String key){
  JsonObject obj = readObject(fileString);
  return {obj[key][0], obj[key][1]};
}

//***WRITE MEMORY FILE FUNCTION***//

void LSMemory::writeObject(String fileString,String key,JsonObject obj){

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

void LSMemory::writeInt(String fileString,String key,int value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

void LSMemory::writeFloat(String fileString,String key,float value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

void LSMemory::writeString(String fileString,String key,String value){
  JsonObject obj = readObject(fileString);
  obj[String(key)] = value;
  writeObject(fileString,key,obj);
}

void LSMemory::writePoint(String fileString,String key,pointFloatType value){
  JsonObject obj = readObject(fileString);
  JsonArray point = obj.createNestedArray(key);
  point.add(value.x);
  point.add(value.y);
  writeObject(fileString,key,obj);
}

#endif 
