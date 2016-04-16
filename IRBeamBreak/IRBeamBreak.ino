/*

http://www.righto.com/2010/03/detecting-ir-beam-break-with-arduino-ir.html

*/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#include <IRremote.h>

#define PIN_IR 3
#define PIN_DETECT 2
#define PIN_STATUS 13
#define PIN_SWITCH 12

// ms that beam will be disabled 
#define BEAM_OFF_TIME 60

IRsend irsend;
Adafruit_7segment matrix = Adafruit_7segment();

void setup()
{
  pinMode(PIN_DETECT, INPUT);
  pinMode(PIN_STATUS, OUTPUT);
  digitalWrite(PIN_IR, HIGH);

  irsend.enableIROut(38);
  irsend.mark(0);
  
  Serial.begin(9600);
  
  matrix.begin(0x70);
}

void loop() {
  Serial.print("Prep...");
  
  digitalWrite(PIN_STATUS, LOW);
  
  // make sure switch is closed
  while (!digitalRead(PIN_SWITCH)) {
    matrix.print(9999);
    matrix.drawColon(true);
    matrix.writeDisplay();
    delay(500);
  }
  
  // ok; wait for switch
  matrix.print(90);
  matrix.writeDisplay();
  
  Serial.print("Ready...");
  while (digitalRead(PIN_SWITCH)) {
    delay(100);
  }
  
  Serial.print("GO!");
  
  // wait for beam break
  unsigned long timeBegin = millis();
  detect(timeBegin);
  unsigned long totalTime = millis() - timeBegin;
  
  digitalWrite(PIN_STATUS, HIGH);
  
  Serial.print(" - Total Time: ");
  Serial.print(totalTime);
  Serial.println("ms");
  
  // output time
  matrix.print(totalTime / 1000.0);
  matrix.writeDisplay();
  
  delay(3000); 
}

void detect(unsigned long timeBegin) {
  
  unsigned long timeStamp = 0;
  
  while (true) {
    
    if (millis()-timeStamp > BEAM_OFF_TIME) {
      irsend.enableIROut(38);
    } 
    irsend.space(0);
    delay(1);
    irsend.mark(0);
    delay(1);
    if (!digitalRead(PIN_DETECT)) {
      return;
    } 
    timeStamp = millis();
    
    // output time
    unsigned long totalTime = millis() - timeBegin;
    matrix.print(totalTime / 1000.0);
    matrix.writeDisplay();
    
    if (totalTime > 15000) return;
  }
}
