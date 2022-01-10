#include <ArduinoJson.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

using namespace Adafruit_LittleFS_Namespace;

#define BUFFER_SIZE  512


//Initialize FileSystem
File file(InternalFS);

class LSMemory {
  public:
    LSMemory();   
    void begin();                                                   
    void initialize(String fileString,String jsonString);                                                                                                 //Pop last dot or dash from the stack    
    void clear(String fileString);
    void format();
    String readAll(String fileString);
    void writeAll(String fileString,String jsonString);
    String read(String fileString,String key);
    int readInt(String fileString,String key);
    float readFloat(String fileString,String key);
    void write(String fileString,String key,String value);
    void writeInt(String fileString,String key,int value);
    void writeFloat(String fileString,String key,float value);
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

String LSMemory::read(String fileString,String key){
  
  uint32_t readLenght;
  const char* fileName = fileString.c_str();
  
  char buffer[BUFFER_SIZE] = { 0 };
  DynamicJsonDocument doc(1024);
  file.open(fileName, FILE_O_READ);
  delay(1);
  readLenght = file.read(buffer, sizeof(buffer));
  delay(1);
  buffer[readLenght] = 0;
  deserializeJson(doc, String(buffer));
  JsonObject obj = doc.as<JsonObject>();
  String value = obj[key];
  return value;
}

int LSMemory::readInt(String fileString,String key){
  return read(fileString,key).toInt();
}


float LSMemory::readFloat(String fileString,String key){
  return read(fileString, key).toFloat();
}

//***WRITE MEMORY FILE FUNCTION***//

void LSMemory::write(String fileString,String key,String value){

    uint32_t readLenght;
    const char* fileName = fileString.c_str();
    
    char buffer[BUFFER_SIZE] = { 0 };
    DynamicJsonDocument doc(1024);
    file.open(fileName, FILE_O_READ);
    delay(1);
    
    readLenght = file.read(buffer, sizeof(buffer));
    file.close();
    delay(1);
    
    buffer[readLenght] = 0;
    deserializeJson(doc, String(buffer));
    JsonObject obj = doc.as<JsonObject>();
    
    obj[String(key)] = serialized(value);
    String jsonString;
    serializeJson(doc, jsonString);
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
  write(fileString,key,String(value));
}

void LSMemory::writeFloat(String fileString,String key,float value){
  write(fileString,key,String(value));
}
