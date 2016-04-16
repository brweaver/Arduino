int PIN_SPEAKER = 6;

int count = 0;
void setup() {
  pinMode(PIN_SPEAKER, OUTPUT);
}

void loop() {
  count = count+1;
  if (count < 5) {
    tone(PIN_SPEAKER,1471);
    delay(10);
    tone(PIN_SPEAKER,1575);
    delay(5);
  } else {
    tone(PIN_SPEAKER,1470);
    delay(10);
    tone(PIN_SPEAKER,1575);
    delay(5);
    if(count > 10) {
      count = 0;
    }
  }

//  digitalWrite(PIN_SPEAKER, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(PIN_SPEAKER, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);              // wait for a second
  
}


