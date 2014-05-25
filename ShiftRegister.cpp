/*
Project:  Arduino Shift Register Library
Version:  0.1
Author:  Leonardo Banderali
Created:  December 16, 2013
Last Modified: May 24, 2014
Description:  This is an arduino library writen to siplify the use of shift registers.
Note: In the comments, a 'shift register' is considered to be a set of one or more 8 bit shift register chips daisy chained together,
    thereby acting as a single register.
*/

//include necessary files
#include "ShiftRegister.h"



//public method definitions

ShiftRegister::ShiftRegister(int _rNum, uint8_t _dataPin, uint8_t _clockPin, uint8_t _latchPin, uint8_t _resetPin) {
/*
-class constructor initializes data members, allocates memory for the object, and sets Arduino and shift register pins to
thier logical initial state
-paramters:
    _rNum: number of 8 bit shift register chips to be programmed
    _dataPin: pin number on the Arduino connected to the shift register data pin
    _clockPin: pin number on the Arduino connected to the shift register data clock pin
    _latchPin: pin number on the Arduino connected to the shift register latch (storage clock) pin
    _resetPin: pin number on the Arduino connected to the shift register reset pin
*/
    //assign pin numbers to private data members
    dataPin = _dataPin;
    clockPin = _clockPin;
    latchPin = _latchPin;
    resetPin = _resetPin;
    
    //assign other data to private data members
    rNum = _rNum;
    shiftOrder = MSBFIRST;    //set bit shift order flag to "Most Significant Bit FIRST"
    
    //set pins as outputs
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    
    //allocate memory
    stateArray = (uint8_t*) calloc(rNum, sizeof(uint8_t) );   //memory for the data array
    
    //set Arduino pins to initial state
    digitalWrite(resetPin, HIGH);
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, LOW);
    digitalWrite(latchPin, LOW);
    
    //clear all shift register pins
    clear();
}

ShiftRegister::~ShiftRegister() {
/* -class destructor deallocates memmory for the class */
    free(stateArray);
}

void ShiftRegister::clear() {
/*
-set all bits to low state
*/
    //reset shift register
    digitalWrite(resetPin, LOW);
    digitalWrite(resetPin, HIGH);
    
    //latch low state to shift register output pins
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    
    //clear 'stateArray'
    for(int i = 0; i < rNum; i++) stateArray[i] = 0;
}

void ShiftRegister::refresh() {
/*
-latch shifted bits to shift register output pins
*/
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
}

void ShiftRegister::setShiftOrder(bool order) {
/*
-set the flag to control the order in which bit patterns are shifted
-parameter:
    order: -if '0' or 'LSBFIRST' is passed then the least significant bit (index 0) is shifted first
           -other wise ('1' or 'MSBFIRST' passed) the most significant bit is shifted first.
*/
    shiftOrder = order;
}

void ShiftRegister::set(uint8_t* data, unsigned int size) {
/*
-set all bits by passing a pointer to an array in which each bit holds the state to be assigned to the corresponding
 shift register output pin
-will overwright the previous state of all bits with the new state
-it is recommended to use the 'aset' macro instead of directly calling this function/method
-parameter:
    data: pointer to array of bits
    size: size of array in bytes (not required when using 'aset' macro)
*/
    if( !(size == rNum) ) return;   //check the passed array is the correct size
    
    if(shiftOrder == MSBFIRST){
        //shift the most significant bit first
        for(int i = size*8 - 1; i >= 0; i--) {  //for every bit in the array
            shiftBit( readBit(data, i) );        //shift the bit into the shift register
            saveToArray(data, i);                 //save the state of the shifted bit to 'stateArray'
        }
    }
    else{
        //shift the least significant bit first
        for(int i = 0; i < size*8; i++) {   //for every bit in the array
            shiftBit( readBit(data, i) );    //shift the bit into the shift register
            saveToArray(data, i);             //save the state of the shifted bit to 'stateArray'
        }
    }
    
    refresh();  //latch the shifted bits to the shift register output pins
}

void ShiftRegister::setBit(unsigned int bitIndex, bool state) {
/*
-set a specific bit to a given state
-only overwrights the previous state of the specified bit; does not change the state of other bits
-parameter:
    bitNum: the index of the bit (0 for least significant) to be set
    state: the state (HIGH/LOW, 1/0) at which the bit is to be set
*/
    bitWrite( stateArray[bitIndex/8], bitIndex%8, state);//save the bit state in 'stateArray' to create a new bit pattern

    //shift the new bit pattern
    for(int i = rNum*8 - 1; i >= 0; i--) {  //for every bit in the array
        shiftBit( readBit(stateArray, i) );  //shift the bit into the shift register
    }
    
    refresh();  //latch the shifted bits to the shift register output pins
}

void ShiftRegister::setBits(unsigned int bitIndex, unsigned int bitNum, bool state) {
/*
-set a number of bits to a given state
-only overwrights the previous state of specified bits; does not change the state of other bits
-parameter:
    bitIndex: the index of the first bit (0 for least significant) to be set
    bitNum: number of bits to be set after the first bit
    state: the state (HIGH/LOW, 1/0) at which the bit is to be set
*/
    for(int i = 0; i < bitNum; i++){                //for every bit to be set
        //bitWrite( *stateArray, bitIndex + i, state);  //save the bit state in 'stateArray' to create a new bit pattern
        bitWrite( stateArray[bitIndex/8], bitIndex%8 + i, state);
    }
    
    //shift the new bit pattern
    for(int i = rNum*8 - 1; i >= 0; i--) {  //for every bit in the array
        shiftBit( readBit(stateArray, i) );  //shift the bit into the shift register
    }
    
    refresh();  //latch the shifted bits to the shift register output pins
}

void ShiftRegister::shift(uint8_t* pattern, unsigned int size) {
/*
-shift a bit pattern into the shift register
-will not overwright the previous states of bits but will shift states to the next available bits
-it is recommended to use the 'ashift' macro instead of directly calling this function/method
-parameters:
    pattern: pointer to an array in which the bit pattern (state of the bits) is stored
    size: size of the array in bytes 
        (note that because 1 byte is the smallest posible size for the array (other than 0), only a multiple
         of 8 bits can be shifted)
*/
    //shift currently saved states
    shiftStateArray(size*8);
    
    if(shiftOrder == MSBFIRST){
        //shift the most significant bit first
        for(int i = size*8 - 1; i >= 0; i--) {  //for every bit in the array
            shiftBit( readBit(pattern, i) );     //shift the bit into the shift register
            saveToArray(pattern, i);              //save the state of the shifted bit to 'stateArray'
        }
    }
    else{
        //shift the least significant bit first
        for(int i = 0; i < size*8; i++) {       //for every bit in the array
            shiftBit( readBit(pattern, i) );     //shift the bit into the shift register
            saveToArray(pattern, i);              //save the state of the shifted bit to 'stateArray'
        }
    }
    
    refresh();  //latch the shifted bits to the shift register output pins
}

void ShiftRegister::shiftState(unsigned int bitNum, bool state){
/*
-shift a number of bits of the given state into the shift register
-will not overwright the previous states of bits but will shift states to the next available bits
-parameter:
    bitNum: the number of bits to be shifted
    state: the state of the bits to be shifted
*/
    //shift currently saved states
    shiftStateArray(bitNum);

    for(int i = bitNum - 1; i >= 0; i--) {  //for every bit to be shifted
        shiftBit( state );                    //shift the bit of the given state into the shift register
        bitWrite( *stateArray, i, state);     //save the new state of the bit in 'stateArray'
    }
    
    refresh();  //latch the shifted bits to the shift register output pins
}

void ShiftRegister::shiftOneBit(bool state) {
/*
-public method to shift one bit of state 'state' (HIGH/LOW, 1/0) 
-used instead of 'shiftBit(bool)' because it changes the internally saved state of the shift register
-'refresh()' must be called in order to latch the shifted data to the shift register output pins
*/
    shiftStateArray(1);                 //save the states in the array
    bitWrite( stateArray[0], 1, state); //add the new bit to the array
    shiftBit(state);                    //shift the bit into the shift registor
}



//private method definitions

void ShiftRegister::shiftBit(bool state) {
/*
-shift one bit of state 'state' (HIGH/LOW, 1/0);
*/
    digitalWrite(dataPin, state);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
}

void ShiftRegister::shiftStateArray(unsigned int bitNum) {
/*
-shift currently saved states in 'stateArray' forward by 'bitNum' bits
*/
    for(int i = rNum*8 - 1; i > bitNum; i--){   //for every bit in 'stateArray'
        saveToArray(stateArray, i - bitNum);      //shift the state forwarf by 'size*8' bits
    }
}

bool ShiftRegister::readBit(uint8_t* array, int bitIndex) {
/*
-read the state of a particular bit in an array and return the value
-parameters:
    array: pointer to the array from which the bit is read
    bitIndex: the index of the bit to be read
*/
    return array[bitIndex/8] & (1<<bitIndex%8); //use a bit mask to retrieve the state of the bit
}
