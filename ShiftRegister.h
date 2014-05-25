/*
Project:  Arduino Shift Register Library
Author:  Leonardo Banderali
Created:  December 16, 2013
Last Modified: May 24, 2014
Description:  This is an arduino library writen to siplify the use of shift registers

Copyright (C) 2014  Leonardo Banderali

Usage Agreement:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Note: In the comments, a 'shift register' is considered to be a set of one or more 8 bit shift register chips daisy chained together,
    thereby acting as a single register.
*/

#ifndef SHIFTREGISTER_H //prevent the file from being included more than once
#define SHIFTREGISTER_H

//include Arduino language files
#if defined(ARDUINO) && ARDUINO >= 100 //if using Arduino1.0 or later
#include "Arduino.h"    
#else                                  //if using Arudino0023 or an older versions
#include "WProgram.h"   
#endif

//macro definitions
#define aset(a) set((uint8_t*) a, sizeof(a))                        //use to avoid having to explicitly pass the size of array 'a'
#define ashift(a) shift( (uint8_t*) a, sizeof(a))                   //use to avoid having to explicitly pass the size of array 'a'
#define saveToArray(a, i) bitWrite(*stateArray, i, bitRead(*a, i) ) //copy bits from array 'a' to 'stateArray'; used when shifting bits to save 
                                                                    //  the shifted bit pattern



//class definition
class ShiftRegister {
    public:
        ShiftRegister(int _rNum, uint8_t _dataPin, uint8_t _clockPin, uint8_t _latchPin, uint8_t _resetPin);
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
        
        ~ShiftRegister();
            /* -class destructor deallocates memmory for the class */
        
        void clear();
            /* -set all bits to low state*/
        
        void refresh();
            /* -latch shifted bits to output pins */
        
        void setShiftOrder(bool order);
            /*
            -set the flag to control the order in which bit patterns are shifted
            -parameter:
                order: -if '0' or 'LSBFIRST' is passed then the least significant bit (index 0) is shifted first
                       -other wise ('1' or 'MSBFIRST' passed) the most significant bit is shifted first.
            */
        
        void set(uint8_t* data, unsigned int size);
            /*
            -set all bits by passing a pointer to an array in which each bit holds the state to be assigned to the corresponding
             shift register output pin
            -will overwright the previous state of all bits with the new state
            -it is recommended to use the 'aset' macro instead of directly calling this function/method
            -parameter:
                data: pointer to array of bits
                size: size of array in bytes (not required when using 'aset' macro)
            */
        
        void setBit(unsigned int bitIndex, bool state);
            /*
            -set a specific bit to a given state
            -only overwrights the previous state of specified bit; does not change the state of other bits
            -parameter:
                bitIndex: index of the bit (0 for least significant) to be set
                state: the state (HIGH/LOW, 1/0) at which the bit is to be set
            */
        
        void setBits(unsigned int bitIndex, unsigned int bitNum, bool state);
            /*
            -set a number of bits to a given state
            -only overwrights the previous state of specified bits; does not change the state of other bits
            -parameter:
                bitIndex: the index of the first bit (0 for least significant) to be set
                bitNum: number of bits to be set after the first bit
                state: the state (HIGH/LOW, 1/0) at which the bit is to be set
            */
        
        void shift(uint8_t* pattern, unsigned int size);
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
            
        void shiftState(unsigned int bitNum, bool state);
            /*
            -shift a number of bits of the given state into the shift register
            -will not overwright the previous states of bits but will shift states to the next available bits
            -parameter:
                bitNum: the number of bits to be shifted
                state: the state of the bits to be shifted
            */
            
        void shiftOneBit(bool state);
            /*
            -public method to shift one bit of state 'state' (HIGH/LOW, 1/0) 
            -used instead of 'shiftBit(bool)' because it changes the internally saved state of the shift register
            -'refresh()' must be called in order to latch the shifted data to the shift register output pins
            */
    
    private:
        //data members
        uint8_t dataPin;    //variables to store the pin numbers for the data, latch, clock, and reset pins
        uint8_t clockPin;   //
        uint8_t latchPin;   //
        uint8_t resetPin;   //
        
        int rNum;           //variable to store the nuber of shift register to be programmed
        uint8_t* stateArray;//pointer to an array in which the shifted bit pattern is stored.  This used by the program to "remember"
                            //  the state of the shift register output pins after the data has been shifted and latched.
        bool shiftOrder;    //flag used to set shifting of most significant bit or least significant bit first
        
        //methods
        void shiftBit(bool state);
            /* -shift one bit of state 'state' (HIGH/LOW, 1/0) */
        
        void shiftStateArray(unsigned int bitNum);
            /* -shift currently saved states in 'stateArray' forward by 'bitNum' bits */
        
        bool readBit(uint8_t* array, int bitIndex);
            /*
            -read the state of a particular bit in an array and return the value
            -parameters:
                array: pointer to the array from which the bit is read
                bitIndex: the index of the bit to be read
            */
            
};

#endif
