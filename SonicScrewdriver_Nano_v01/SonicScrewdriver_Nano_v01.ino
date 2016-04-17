/*
* Sonic Screwdriver
* Version History
*
* Version M 2.5
* For ATTiny85 16Mhz
*/

#include "pitches.h"

const int SPEAKER_PIN = 5;
const int SWITCH_PIN = 4;

const int RGB_LED[] = { 7, 8, 9 };
const int numLEDs = 3;

const int PIN_AXIS_X = A1;                  // x-axis of the accelerometer
const int PIN_AXIS_Y = A2;                  // y-axis
const int PIN_AXIS_Z = A3;                  // z-axis (only on 3-axis models)
const int AXIS_SAMPLE_SIZE = 10;

const int melody[] = {
  1471,
  1575,
  1470,
  1575
//  NOTE_F7, 
//  NOTE_G4, 
//  NOTE_A2
};

const int noteDurations[] = { 9, 4, 7, 4 };
const int numNotes = 4;

const float ZERO_G = 512.0; //ADC is 0~1023  the zero g output equal to Vs/2
                            //ADXL335 power supply by Vs 3.3V
const float SCALE = 102.3;  //ADXL335330 Sensitivity is 330mv/g
                            //330 * 1024/3.3/1000

float x = 0;

//int const potPin = A0;
//int potVal;

// Experimentally detirmined values
int xRawMin = 392;
int xRawMax = 658;

int yRawMin = 315;
int yRawMax = 639;
 
int zRawMin = 314;
int zRawMax = 804;

void setup() {
  Serial.begin(9600);

  // Setup RGB Light
  for (int i = 0; i < numLEDs; i++) {
    pinMode(RGB_LED[i], OUTPUT);
  }

  // Setup Switch
  pinMode(SWITCH_PIN, INPUT);
  
}

int lightCount = 0;
int switchState = 0;

void loop() {
  
  switchState = digitalRead(SWITCH_PIN);
  //Serial.println(switchState);

  if (switchState == 0) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(RGB_LED[i], HIGH);
    }
    //delay(500);
    return;
  }

  calculateOrientation();

  // 
  // Perform Light & Sound Calcualtions
  //lightAndSound();
}

void calculateOrientation() {

  // 3-Axis Acce
  int x = readAxis(PIN_AXIS_X);  
  int y = readAxis(PIN_AXIS_Y);  
  int z = readAxis(PIN_AXIS_Z);

  int distanceToFlat = abs(363-x);
  int distanceToUp = abs(294-x);
  int distanceToDown = abs(440-x);

  if (distanceToFlat < distanceToUp && distanceToFlat < distanceToDown) {
    Serial.print("Flat -- ");
    digitalWrite(RGB_LED[0], LOW);
  } else if (distanceToUp < distanceToFlat && distanceToUp < distanceToDown) {
    Serial.print("Up -- ");
    digitalWrite(RGB_LED[1], LOW);
  } else {
    Serial.print("Down -- ");
    digitalWrite(RGB_LED[2], LOW);
  }


  // Convert raw values to 'milli-Gs"
  long xScaled = map(x, xRawMin, xRawMax, -1000, 1000);
  long yScaled = map(y, yRawMin, yRawMax, -1000, 1000);
  long zScaled = map(z, zRawMin, zRawMax, -1000, 1000);
  
  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;

  Serial.print("x:");
  Serial.print(x); 
  Serial.print("\ty:");
  Serial.print(y);
  Serial.print("\tz:");
  Serial.print(z);  
  
  Serial.print(" :: ");
  Serial.print(xAccel);
  Serial.print("G, ");
  Serial.print(yAccel);
  Serial.print("G, ");
  Serial.print(zAccel);
  Serial.print("G");

  
  
  Serial.print("\n");
}

int readAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1); 
  for (int i = 0; i < AXIS_SAMPLE_SIZE; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/AXIS_SAMPLE_SIZE;
}
 

void lightAndSound() {
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

