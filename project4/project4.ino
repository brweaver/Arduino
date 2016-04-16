const int gPin = 9;
const int rPin = 10;
const int bPin = 11;

const int rSensor = A0;
const int bSensor = A2;
const int gSensor = A1;

int rv = 0;
int gv = 0;
int bv = 0;

int rx = 0;
int bx = 0;
int gx = 0;


void setup() {
  Serial.begin(9600);
  
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
}

void loop() {
  rx = analogRead(rSensor);
  delay(10);
  gx = analogRead(gSensor);
  delay(10);
  bx = analogRead(bSensor);
  delay(10);
  
  Serial.print("Raw Sensor Values (");
  Serial.print(rx);
  Serial.print(", ");
  Serial.print(gx);
  Serial.print(", ");
  Serial.print(bx);
  Serial.print(") ");
  
  rv = rx/4;
  gv = gx/4;
  bv = bx/4;
  
  Serial.print(" -- ");
  Serial.print(rv);
  Serial.print(", ");
  Serial.print(gv);
  Serial.print(", ");
  Serial.print(bv);
  Serial.println("");
  
  
  analogWrite(rPin, rv);
  analogWrite(gPin, gv);
  analogWrite(bPin, bv);
}
