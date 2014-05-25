/*
Project: Arduino Shift Register Library - Example
File: shiftRegisterLib_example.ino
Author: Leonardo Banderali
Created: May 24, 2014
Last Modified: May 24, 2014

Description: This is an example sketch demonstrating the use of the shift register library.
          This sketch shows how to control two daisy-chained shift registers connected to an
          Arduino as follows:
          
          +------------+-------------------+---------------------------------
          |Arduino pin |Shift register pin |Pin number on 595 shift register
          +------------+-------------------+---------------------------------
          | 8          | data              | 14
          | 10         | clk (clock)       | 11
          | 9          | latch             | 12
          | 12         | reset             | 10
          +------------+-------------------+---------------------------------
          
          Connecting LEDs to the output pins of the shift registers will display the patterns shifted.
*/

//include library
#include <ShiftRegister.h>

//object declaration/instantiation
ShiftRegister sr(2, 8, 10, 9, 12);
/*
-parameters:
  2 shift register
  pin 8 -> data
  pin 10 -> clock
  pin 9 -> latch
  pin 12 -> reset
*/



//~Setup loop~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Serial.begin(9600);
  sr.setShiftOrder(MSBFIRST);  //set order in which bits are shifted (MSBFIRST/LSBFIRST)
  sr.clear();                  //set all bits to low
}



//~Main loop~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  blinkEachOnce();             //set each pin individually HIGH for 0.5s and then LOW, wait for 0.5s between each pin
  
  fourAtATime();               //sets all pins HIGH, four at a time, and then sets all low at once
  
  fourAtATime_Improved();      //sets all pins HIGH, four at a time, and them sets all low at once
  
  setAlternating();            //sets pins to alternating state
  
  shiftAlternating();          //shifts alternating state on pins
  
  setAlternatingWithArray();   //sets pins to alternating state using an array
  
  fourAlternatingWithArray();  //shifts alternating pattern of four and then sets all low at once
}



//~functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void blinkEachOnce() {
/* -set each pin individually HIGH for 0.5s and then LOW, wait for 0.5s between each pin */
  for (int i = 0; i < 16; i++) {  //for every pin on the shift registers
    sr.setBit(i, HIGH);             //set pin HIGH
    delay(500);                     //wait
    sr.setBit(i, LOW);              //set pin LOW
    delay(500);                     //wait
  }
}


void fourAtATime() {
/* -sets all pins HIGH, four at a time, and then sets all low at once */
  sr.setBits(0, 4, HIGH);
  delay(500);
  sr.setBits(4, 4, HIGH);
  delay(500);
  sr.setBits(8, 4, HIGH);
  delay(500);
  sr.setBits(12, 4, HIGH);
  delay(500);
  sr.clear();
  delay(500);
}


void fourAtATime_Improved() {
/* -sets all pins HIGH, four at a time, and them sets all low at once */
  for (int i = 0; i < 4; i++) { //repeate four times
    sr.shiftState(4, HIGH);       //shift four HIGH bits
    delay(500);                   //wait
  }
  sr.clear();  //set all pins LOW
  delay(500);  //wait
}


void setAlternating() {
/* -sets pins to alternating state */
  bool state = HIGH;
  for (int i = 0; i < 16; i++) {  //for every pin on the shift registers
    sr.shiftOneBit(state);          //shift state
    state = !state;                 //alternate the state
  }
  sr.refresh();//push/latch state to pins
  delay(1500); //wait
  sr.clear();  //set all pins LOW
  delay(500);  //wait
}


void shiftAlternating() {
/* -shifts alternating state on pins */
  bool state = HIGH;
  for (int i = 0; i < 16; i++) {  //for every pin on the shift registers
    sr.shiftOneBit(state);          //shift state
    sr.refresh();                   //push/latch state to pins
    delay(500);                     //wait
    state = !state;                 //alternate the state
  }
  sr.clear();  //set all pins LOW
  delay(500);  //wait
}


void setAlternatingWithArray() {
/* -sets pins to alternating state using an array */
  byte pattern[] = {B01010101, B01010101};  //alternating state pattern
  sr.aset(pattern);                         //set the pattern
  delay(1500);                              //wait
  sr.clear();  //set all pins LOW
  delay(500);  //wait
}


void fourAlternatingWithArray() {
/* -shifts alternating pattern of four and then sets all low at once */
  for (int i = 0; i < 4; i++) {  //repeat four times
    byte pattern[] = {B00001111};  //state pattern
    sr.ashift(pattern);            //shift pattern
    delay(500);                    //wait
  }
  sr.clear();  //set all pins LOW
  delay(500);  //wait
}
