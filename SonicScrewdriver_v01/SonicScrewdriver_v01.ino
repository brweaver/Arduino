/*
* Sonic Screwdriver
* Version History
*
* Version M 2.5
* For ATTiny85 16Mhz
*/

#include "pitches.h"

int const SPEAKER_PIN = 6;
int const SWITCH_PIN = 12;

int LED_PINS[] = { 5, 4, 3, 2};
int numLEDs = 4;

int melody[] = {
  NOTE_F7, 
  NOTE_G4, 
  NOTE_A2
};

int noteDurations[] = { 9, 12, 7 };
float x = 0;

//int const potPin = A0;
//int potVal;

void setup() {
 
  for (int i = 0; i < numLEDs; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  
  pinMode(SWITCH_PIN, INPUT);
  Serial.begin(9600);
}

int switchState = 0;
void loop() {
  
  switchState = digitalRead(switchPin);
  Serial.println(switchState);
//  if (switchState == HIGH) {
//    delay(100);
//    return;
//  }
  
//  if (x < 4) {
//    digitalWrite(led1, HIGH);
//    digitalWrite(led2, LOW);
//    digitalWrite(led3, LOW);
//    digitalWrite(led4, LOW);
//  } else if ( x < 5 ) {
//    digitalWrite(led1, LOW);
//    digitalWrite(led2, HIGH);
//    digitalWrite(led3, LOW);
//    digitalWrite(led4, LOW);
//  } else if ( x < 7 ) {
//    digitalWrite(led1, LOW);
//    digitalWrite(led2, LOW);
//    digitalWrite(led3, HIGH);
//    digitalWrite(led4, LOW);
//  } else if ( x < 9 ) {
//    digitalWrite(led1, LOW);
//    digitalWrite(led2, LOW);
//    digitalWrite(led3, LOW);
//    digitalWrite(led4, HIGH);
//  } else {
//    x = 0;
//  }
//  x+=0.8;
  
  //potVal = analogRead(potPin);
  float factor = 1; // /*-0.5 +*/ potVal / 1023.0; 

  for (int thisNote = 0; thisNote < 3; thisNote++) {
    //int noteDuration = 1000/noteDurations[thisNote];
    
    digitalWrite(ledPin[thisNote], HIGH);
    
    int noteFreq = factor * 100 + melody[thisNote];
    int noteDur  = 1000/noteDurations[thisNote];
    noteDur += noteDur * factor * factor;
    
    tone(speakerPin, noteFreq, noteDur);

    int pauseBetweenNotes = noteDur * .14;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
    
    digitalWrite(ledPin[thisNote], LOW);
  } 

}
