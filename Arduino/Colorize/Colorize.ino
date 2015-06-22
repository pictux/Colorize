/*
  Colorize is an interactive installation that allows to learn (by playing) how Arduino, 
  conductive paint, capacitive sensor and RGB led strip work.
  Project description / images / code : 
  https://github.com/pictux/Colorize 
  
  Based on :
  - Bare Conductive Proximity MP3 player, code by Jim Lindblom and plenty of inspiration from the Freescale 
  Semiconductor datasheets and application notes
  - LedDriver example code by Frankie Chu/Grove/Seeedstudio
    
  This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
  Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
*/

#include "RGBdriver.h"
#include <MPR121.h>
#include <Wire.h>

#define MPR121_ADDR 0x5A

#define CLK 2//pins definitions for the driver        
#define DIO 3

// mapping and filter definitions
#define LOW_DIFF 0
#define HIGH_DIFF 50
#define filterWeight 0.3f // 0.0f to 1.0f - higher value = more smoothing
float lastProx3 = 0;
float lastProx6 = 0;
float lastProx9 = 0;

// the electrodes to monitor for proximity
#define PROX_ELECTRODE3 3
#define PROX_ELECTRODE6 6
#define PROX_ELECTRODE9 9

#define LED_BUILTIN 13

RGBdriver Driver(CLK, DIO);

//#define VERBOSE

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  //while (!Serial) ; {} //uncomment when using the serial monitor
#ifdef VERBOSE
  Serial.println("Bare Conductive Touch MP3 player");
#endif
  if (!MPR121.begin(MPR121_ADDR)) Serial.println("error setting up MPR121");
  //MPR121.setInterruptPin(MPR121_INT);

  // slow down some of the MPR121 baseline filtering to avoid
  // filtering out slow hand movements
  MPR121.setRegister(NHDF, 0x01); //noise half delta (falling)
  MPR121.setRegister(FDLF, 0x3F); //filter delay limit (falling)
}

void loop() {
  readTouchInputs();
}


void readTouchInputs() {

  MPR121.updateAll();

  for (int i = 1; i < 12; i++) { // Check which electrodes were pressed
    if (MPR121.isNewTouch(i)) {
#ifdef VERBOSE
      //pin i was just touched
      Serial.print("pin ");
      Serial.print(i);
      Serial.println(" was just touched");
      digitalWrite(LED_BUILTIN, HIGH);
#endif

    } else {
      if (MPR121.isNewRelease(i)) {
#ifdef VERBOSE
        Serial.print("pin ");
        Serial.print(i);
        Serial.println(" is no longer being touched");
        digitalWrite(LED_BUILTIN, LOW);
#endif
      }
    }
  }

  // read the difference between the measured baseline and the measured continuous data
  int reading3 = MPR121.getBaselineData(PROX_ELECTRODE3) - MPR121.getFilteredData(PROX_ELECTRODE3);
  int reading6 = MPR121.getBaselineData(PROX_ELECTRODE6) - MPR121.getFilteredData(PROX_ELECTRODE6);
  int reading9 = MPR121.getBaselineData(PROX_ELECTRODE9) - MPR121.getFilteredData(PROX_ELECTRODE9);

  // constrain the reading between our low and high mapping values
  unsigned int prox3 = constrain(reading3, LOW_DIFF, HIGH_DIFF);
  unsigned int prox6 = constrain(reading6, LOW_DIFF, HIGH_DIFF);
  unsigned int prox9 = constrain(reading9, LOW_DIFF, HIGH_DIFF);

  // implement a simple (IIR lowpass) smoothing filter
  lastProx3 = (filterWeight * lastProx3) + ((1 - filterWeight) * (float)prox3);
  lastProx6 = (filterWeight * lastProx6) + ((1 - filterWeight) * (float)prox6);
  lastProx9 = (filterWeight * lastProx9) + ((1 - filterWeight) * (float)prox9);

  // map the LOW_DIFF..HIGH_DIFF range to 0..254 (max range for RGB channels)
  uint8_t thisOutput3 = (uint8_t)map(lastProx3, LOW_DIFF, HIGH_DIFF, 0, 254);
  uint8_t thisOutput6 = (uint8_t)map(lastProx6, LOW_DIFF, HIGH_DIFF, 0, 254);
  uint8_t thisOutput9 = (uint8_t)map(lastProx9, LOW_DIFF, HIGH_DIFF, 0, 254);

#ifdef VERBOSE
  Serial.print("Proximity electrode: ");
#endif

  Serial.print(thisOutput3);
  Serial.print(";");
  Serial.print(thisOutput6);
  Serial.print(";");
  Serial.print(thisOutput9);
  Serial.println(";");

  Driver.begin();
  Driver.SetColor(thisOutput3, thisOutput6, thisOutput9);
  Driver.end();
}

