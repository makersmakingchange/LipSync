/* 
* File: LSCircularBuffer.h
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

  * Based on: CircularBuffer.h
*/

// Header definition
#ifndef _LSCIRCULARBUFFER_H
#define _LSCIRCULARBUFFER_H


// The definition of the CircularBuffer class.
template<typename T>
class LSCircularBuffer {
  public:
    LSCircularBuffer();
    ~LSCircularBuffer();
    T getElement( uint16_t );  // Zero is the push location of new element 
    void begin( uint16_t inputSize );
    void pushElement(const T elementVal);
    void updateLastElement(const T elementVal);
    T getLastElement( void ); 
    uint16_t getLength( void );

  private:
    uint16_t cBufferSize;
    T *cBufferData;
    int16_t cBufferLastPtr;
    uint8_t cBufferElementsUsed;
};

//*********************************//
// Function   : LSCircularBuffer 
// 
// Description: Construct LSCircularBuffer type with arguments
// 
// Arguments :  uint16_t inputSize: number of elements
// 
// Return     : void
//*********************************//
template<typename T>
LSCircularBuffer<T>::LSCircularBuffer()
{
//  cBufferData = new T[inputSize];
//  cBufferLastPtr = 0;
//  cBufferElementsUsed = 0;  
//  cBufferSize = inputSize;
  
}

//*********************************//
// Function   : ~LSCircularBuffer 
// 
// Description: Destruct LSCircularBuffer
// 
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
LSCircularBuffer<T>::~LSCircularBuffer()
{
  delete[] cBufferData;
}

//*********************************//
// Function   : begin 
// 
// Description: create LSCircularBuffer type with arguments
// 
// Arguments :  uint16_t inputSize: number of elements
// 
// Return     : void
//*********************************//
template<typename T>
void LSCircularBuffer<T>::begin(uint16_t inputSize)
{
  cBufferData = new T[inputSize];
  cBufferLastPtr = 0;
  cBufferElementsUsed = 0;  
  cBufferSize = inputSize; 
}

//*********************************//
// Function   : getElement 
// 
// Description: Get an element at some depth into the circular buffer
//              zero is the push location.  Max: cBufferSize - 1
//
// Arguments :  uint16_t elementNum: number of element in
// 
// Return     : element type T
//*********************************//
template<typename T>
T LSCircularBuffer<T>::getElement( uint16_t elementNum )
{
  // Translate elementNum into terms of cBufferLastPtr.
  int16_t virtualElementNum;
  virtualElementNum = cBufferLastPtr - elementNum;
  if( virtualElementNum < 0 ) {
    virtualElementNum += cBufferSize;
  }
  
  // Output the value
  return cBufferData[virtualElementNum];
}

//*********************************//
// Function   : pushElement 
// 
// Description: Push a new element into the buffer
//              and expand the size up to the max size.
//
// Arguments :  T elementVal: value of new element
// 
// Return     : void
//*********************************//
template<typename T>
void LSCircularBuffer<T>::pushElement(const T elementVal )
{
  // inc. the pointer
  cBufferLastPtr++;

  // deal with roll
  if( cBufferLastPtr >= cBufferSize ) {
    cBufferLastPtr = 0;
  }

  // Write data
  cBufferData[cBufferLastPtr] = elementVal;

  // Increase length up to cBufferSize
  if( cBufferElementsUsed < cBufferSize ) {
    cBufferElementsUsed++;
  }
}

//*********************************//
// Function   : updateLastElement 
// 
// Description: Update the last element pushed into the buffer.
//
// Arguments :  T elementVal: value of new element
// 
// Return     : void
//*********************************//
template<typename T>
void LSCircularBuffer<T>::updateLastElement(const T elementVal){
  // Write data
  cBufferData[cBufferLastPtr] = elementVal; 
}

//*********************************//
// Function   : getLastElement 
// 
// Description: Get the last element pushed into the buffer.
//
// Arguments :  void
// 
// Return     : void
//*********************************//
template<typename T>
T LSCircularBuffer<T>::getLastElement( void )
{
  // Output the value of last element
  return cBufferData[cBufferLastPtr];
}

//*********************************//
// Function   : getLength 
// 
// Description: Return the current size of the buffer
//
// Arguments :  void
// 
// Return     : uint16_t current size of the buffer
//*********************************//
template<typename T>
uint16_t LSCircularBuffer<T>::getLength( void )
{
  return cBufferElementsUsed;
}

#endif // _LSCIRCULARBUFFER_H
