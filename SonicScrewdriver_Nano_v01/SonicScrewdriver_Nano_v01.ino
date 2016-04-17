/*
* Sonic Screwdriver
* Version History
*
* Version M 2.5
* For ATTiny85 16Mhz
*/

#include "pitches.h"

int const SPEAKER_PIN = 5;
int const SWITCH_PIN = 4;

int RGB_LED[] = { 7, 8, 9 };
int numLEDs = 3;

int melody[] = {
  1471,
  1575,
  1470,
  1575
//  NOTE_F7, 
//  NOTE_G4, 
//  NOTE_A2
};

int noteDurations[] = { 9, 12, 7, 4 };
int numNotes = 4;

float x = 0;

//int const potPin = A0;
//int potVal;

void setup() {
 
  for (int i = 0; i < numLEDs; i++) {
    pinMode(RGB_LED[i], OUTPUT);
  }
  
  pinMode(SWITCH_PIN, INPUT);
  Serial.begin(9600);
}
int lightCount = 0;
int switchState = 0;
void loop() {
  
  switchState = digitalRead(SWITCH_PIN);
  Serial.println(switchState);

  if (switchState == 0) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(RGB_LED[i], HIGH);
    }
    
    
    delay(1000);
    return;
  }
  float factor = 1; // /*-0.5 +*/ potVal / 1023.0; 

  int light = lightCount++ % numLEDs;
  digitalWrite(RGB_LED[light], LOW);
  
  for (int thisNote = 0; thisNote < numNotes; thisNote++) {
    
    int noteFreq = factor * 100 + melody[thisNote];
    int noteDur  = 1000/noteDurations[thisNote];
    noteDur += noteDur * factor * factor;
    
    tone(SPEAKER_PIN, noteFreq, noteDur);

    int pauseBetweenNotes = noteDur * .14;
    delay(pauseBetweenNotes);
    noTone(SPEAKER_PIN);
    
  } 

  digitalWrite(RGB_LED[light], HIGH);
}
