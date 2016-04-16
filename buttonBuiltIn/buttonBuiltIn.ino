int const buttonPin = 12;
int const ledPin = 5;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  //read the pushbutton value into a variable
  int sensorVal = digitalRead(buttonPin);
  //print out the value of the pushbutton
  Serial.println(sensorVal);

  // Keep in mind the pullup means the pushbutton's
  // logic is inverted. It goes HIGH when it's open,
  // and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:
  if (sensorVal == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
  
}
