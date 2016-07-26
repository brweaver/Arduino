#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;

Zumo32U4LineSensors lineSensors;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
LSM303 compass;
L3G gyro;

int movAvgIndex = 0;

#define NUM_SENSORS 3
#define MOVING_AVG 10
unsigned int lineSensorValues[NUM_SENSORS];
unsigned int lineSensorValuesAvg[NUM_SENSORS];
unsigned int lineSensorMovingAvg[MOVING_AVG][NUM_SENSORS];

void setupCompassGyro() {
  Wire.begin();

  if (!compass.init())
  {
    // Failed to detect the compass.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect the compass."));
      delay(100);
    }
  }

  compass.enableDefault();

  if (!gyro.init())
  {
    // Failed to detect the gyro.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect gyro."));
      delay(100);
    }
  }

  gyro.enableDefault();

  compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
}

unsigned long stateInitTime = micros();

void setup() {


  
  // put your setup code here, to run once:
  lineSensors.initThreeSensors();
  buzzer.play(">g32>>c32");

  lcd.clear();
  lcd.print(F("Press A"));
  lcd.gotoXY(0, 1);
  buttonA.waitForButton();
  lcd.clear();

  setupCompassGyro();

  // Wait for button A to be pressed and released.
//  lcd.clear();
//  lcd.print(F("Press A"));
//  lcd.gotoXY(0, 1);
//  lcd.print(F("to calib"));
//  buttonA.waitForButton();
//  calibrateSensors();

  

}
char report[120];
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};
// empirical values: min: { -6936,  +7234,  -9877}    max: { -2111, +10431,  -7570}

void loop() {
  int speed = 500;
  motors.setSpeeds(speed, speed);
  delay(600);
  motors.setSpeeds(speed, -speed);
  delay(160);

  // used to measure turning speed as related to physical direction.
  // 160 delay turned 90 degrees. 
//  for (int i = 0; i < 2000; i+=10) {
//    motors.setSpeeds(-speed, speed);
//    delay(i);
//
//    lcd.clear();
//    lcd.gotoXY(0,1);
//    lcd.print(i);
//    motors.setSpeeds(0,0);
//    delay(2000);
//  }
  

//  if (micros() - stateInitTime > 10000) {
//    motors.setSpeeds(0,0);
//  }
}

void loop_old3() {  
  compass.read();
  
  running_min.x = min(running_min.x, compass.m.x);
  running_min.y = min(running_min.y, compass.m.y);
  running_min.z = min(running_min.z, compass.m.z);

  running_max.x = max(running_max.x, compass.m.x);
  running_max.y = max(running_max.y, compass.m.y);
  running_max.z = max(running_max.z, compass.m.z);
  
  snprintf(report, sizeof(report), "min: {%+6d, %+6d, %+6d}    max: {%+6d, %+6d, %+6d}",
    running_min.x, running_min.y, running_min.z,
    running_max.x, running_max.y, running_max.z);

  compass.m_min = running_min;
  compass.m_max = running_max;
  
  Serial.print(report);
  Serial.print(" ");
  Serial.println(compass.heading());
  
  delay(100);
}

void loop_old2()
{
  compass.read();
  gyro.read();

  snprintf_P(report, sizeof(report),
    PSTR("A: %6d %6d %6d    M: %6d %6d %6d    G: %6d %6d %6d"),
    compass.a.x, compass.a.y, compass.a.z,
    compass.m.x, compass.m.y, compass.m.z,
    gyro.g.x, gyro.g.y, gyro.g.z);
  Serial.println(report);

  delay(100);
}


int maxValueSeen = 0;
void loop_old() {
  
  lineSensors.read(lineSensorValues);
  updateMovingAvg(lineSensorValues);

  Serial.print("Values: ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(lineSensorValues[i]);
    Serial.print(" ");
  }
  Serial.print(" avg: ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(lineSensorValuesAvg[i] / MOVING_AVG);
    Serial.print(" ");
  }
  Serial.println("");

  bool hitBorder=false;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (lineSensorValues[i] > maxValueSeen) {
      lcd.clear();
      lcd.gotoXY(0,1);
      lcd.print(maxValueSeen);
      maxValueSeen = lineSensorValues[i];
    }
    if (lineSensorValues[i] > 325) hitBorder=true;
    if (lineSensorValues[i] > lineSensorValuesAvg[i]*1.1) hitBorder=true;
  }
  
  if (hitBorder) {
    lcd.print(F("Hit!"));
    lcd.gotoXY(0, 1);
    motors.setSpeeds(0,0);
    while (!buttonA.getSingleDebouncedRelease()) {
      delay(100);
    }
    motors.setSpeeds(-200, 200);
    delay(100);
  }
  motors.setSpeeds(500, 500);


//  if (buttonA.getSingleDebouncedRelease()) {
//    motors.setSpeeds(0,0);
//    m
//  }
}

void updateMovingAvg(unsigned int *values) {
  unsigned int i;
  
  for (i = 0; i < NUM_SENSORS; i++) {
    lineSensorValuesAvg[i] -= lineSensorMovingAvg[movAvgIndex][i];
    lineSensorMovingAvg[movAvgIndex][i] = values[i];
    lineSensorValuesAvg[i] += lineSensorMovingAvg[movAvgIndex][i];
  }

  movAvgIndex += 1;
  if (movAvgIndex >= MOVING_AVG) {
    movAvgIndex = 0;
  }
}


void calibrateSensors()
{
  lcd.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      //motors.setSpeeds(-200, 200);
    }
    else
    {
      //motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  //motors.setSpeeds(0, 0);
}
