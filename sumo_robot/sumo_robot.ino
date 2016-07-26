#include "sumo_robot.h"
#include "robot.h"
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4LCD lcd;

Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;

Robot robot;

const bool shouldPrintProxSensorResults = false;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

void setup() {
  // put your setup code here, to run once:
  lcd.clear();
  lcd.print(">");

  ledRed(1);
  delay(500);

  ledRed(0);
  ledYellow(1);
  lcd.print(">");
  delay(500);

  ledRed(0);
  ledYellow(0);
  ledGreen(1);
  lcd.print(">");

  proxSensors.initThreeSensors();

  // Reset defaults:
  proxSensors.setPulseOnTimeUs(457); // default is 421; changing for the hell of it
  proxSensors.setPulseOffTimeUs(330); // per http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html#a6d47a41b45a7088916fbeaa5b38c60b3
}

void loop() {
  
  ProxResult proxResult = readProxSensorsSimple();
  
  Serial.print(" proxResult: ");
  Serial.println(proxResultToString(proxResult));

  switch (proxResult) {
    case Confused:
    case Nothing:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("No Target");
      robot.deadStop();
      break;
    case AheadQuarter:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("A");
      robot.deadStop();
      break;
    case AheadHalf:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("^");
      robot.deadStop();
      break;
    case AheadFull:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("|");
      robot.deadStop();
      break;
    case Left:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("<<");
      robot.turnLeft();
      break;
    case NudgeLeft:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("<");
      robot.nudgeLeft();
      break;
    case Right:
      lcd.clear();
      lcd.gotoXY(4, 1);
      lcd.print(">>");
      robot.turnRight();
      break;
    case NudgeRight:
      lcd.clear();
      lcd.gotoXY(5, 1);
      lcd.print(">");
      robot.nudgeRight();
      break;
  }

  robot.refresh();
}

//void loop() {
//  robot.turnLeft();
//  robot.refresh();
//  delay(40);
//  robot.refresh();
//  
//  Serial.println(robot.state());
//  buzzer.playFrequency(440, 200, 15);
//  delay(1000);
//}

void printProxSensorResults(uint16_t sensor[]) {
    Serial.print(sensor[0]);
    Serial.print(" ");
    Serial.print(sensor[1]);
    Serial.print(" ");
    Serial.print(sensor[2]);
}

String proxResultToString(ProxResult r) {
  switch(r) {
      case AheadQuarter: return "AheadQuarter";
      case AheadHalf: return "AheadHalf";
      case AheadFull: return "AheadFull";
      case NudgeLeft: return "NudgeLeft";
      case Left: return "Left";
      case NudgeRight: return "NudgeRight";
      case Right: return "Right";
      case Nothing: return "Nothing";
      case Confused: return "Confused";
      default: return "oops.";
  }
}

ProxResult readProxSensorsSimple() {
  uint16_t minIRBrightness = 2; 
  uint16_t maxIRBrightness = 410; // 418 is the empirical max, but it's not always triggered. 
  uint16_t numBrightnessLevels = 20;

  uint16_t brightnessLevels[numBrightnessLevels];
  //deriveLinearBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  deriveExponentialBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  
  proxSensors.setBrightnessLevels(brightnessLevels, numBrightnessLevels);
  proxSensors.read();

  uint16_t sumLeftLED[] = {
    proxSensors.countsLeftWithLeftLeds(), 
    proxSensors.countsFrontWithLeftLeds(), 
    proxSensors.countsRightWithLeftLeds()}; 
  uint16_t sumRightLED[] = {
    proxSensors.countsLeftWithRightLeds(), 
    proxSensors.countsFrontWithRightLeds(), 
    proxSensors.countsRightWithRightLeds()};

  const int strongSignal = numBrightnessLevels * 0.9;
  const int modSignal = numBrightnessLevels * 0.6;
  const int weakSignal = numBrightnessLevels * 0.25;

  int diffFrontSignal = sumLeftLED[1] - sumRightLED[1]; 

  Serial.print("Left: ");
  printProxSensorResults(sumLeftLED);
  Serial.print(" Right: ");
  printProxSensorResults(sumRightLED);
  
  // 90% 1/4
  // 80% 1/2 means (roughly) midcircle
  // 70% 1/2
  // 40% means 3/4 
  // 20% means (roughly) full circle

  if (sumLeftLED[1] >= strongSignal && sumRightLED[1] >= strongSignal) {
      return AheadQuarter;
  } else if (sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal && abs(diffFrontSignal) < weakSignal) {
      return AheadHalf;
  } else if (diffFrontSignal > weakSignal) {
    return NudgeLeft;
  } else if (diffFrontSignal < -weakSignal) {
    return NudgeRight;
  } else if (sumLeftLED[0] >= modSignal) {
    return Left;
  } else if (sumRightLED[2] >= modSignal) {
    return Right;
  } else if (sumLeftLED[1] >= weakSignal && sumRightLED[1] >= weakSignal) {
    return AheadFull;
  }
  
  return Confused;
}

void deriveLinearBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  uint16_t x = (max - min)/(count-1);
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = min + i*x;
  }
}

void deriveExponentialBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  float a = min;
  float b = pow(max/a, 1.0/(count-1.0));
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = a*pow(b, i);
  }
}

void calibrateLineSensors()
{
  lcd.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-200, 200);
    }
    else
    {
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}


void loop_old() {

  // Notes: 
  // Roughly two milliseconds per brightness level. 

  unsigned long StartTime = micros();
  proxSensors.read();
  unsigned long CurrentTime = micros();
  unsigned long ElapsedTime = CurrentTime - StartTime;

  
  Serial.println("");
  Serial.print("Elapsed Time: ");
  Serial.println(ElapsedTime);
  Serial.print("Number of Brightness Levels: ");
  Serial.println(proxSensors.getNumBrightnessLevels());
  Serial.println("");
  Serial.print("Basic Left: ");
  Serial.println(proxSensors.readBasicLeft());
  Serial.print("Basic Right: ");
  Serial.println(proxSensors.readBasicRight());
  Serial.print("Basic Front: ");
  Serial.println(proxSensors.readBasicFront());
  Serial.println("");
  Serial.print("Front w/Left: ");
  Serial.println(proxSensors.countsFrontWithLeftLeds());
  Serial.print("Front w/Right: ");
  Serial.println(proxSensors.countsFrontWithRightLeds());
  Serial.print("Left w/Left: ");
  Serial.println(proxSensors.countsLeftWithLeftLeds());
  Serial.print("Left w/Right: ");
  Serial.println(proxSensors.countsLeftWithRightLeds());
  Serial.print("Right w/Right: ");
  Serial.println(proxSensors.countsRightWithRightLeds());
  Serial.print("Right w/Left: ");
  Serial.println(proxSensors.countsRightWithLeftLeds());
  
  while (!buttonA.getSingleDebouncedRelease()) {
    delay(50);

    if (buttonC.getSingleDebouncedRelease()) {

      calibrateProxSensors();
      
      //uint16_t brightnessLevels[] = { 4, 15, 32, 55, 85, 120 };
      //uint16_t brightnessLevels[] = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1012 };
      //proxSensors.setBrightnessLevels(brightnessLevels, 10);
      //Serial.println("Set new brightness levels.");
    }
  }

  

  
  if (proxSensors.countsFrontWithLeftLeds() >= 2
        || proxSensors.countsFrontWithRightLeds() >= 2)
      {
        lcd.clear();
        lcd.gotoXY(3, 1);
        lcd.print("^");
      }

  else if (proxSensors.countsLeftWithLeftLeds() >= 2)
      {
        // Detected something to the left.
        lcd.clear();
        lcd.gotoXY(0, 1);
        lcd.print("<");
      }

  else if (proxSensors.countsRightWithRightLeds() >= 2)
      {
        // Detected something to the right.
        lcd.clear();
        lcd.gotoXY(5, 1);
        lcd.print(">");
      }
      
//   else {
//    uint8_t sum = proxSensors.countsFrontWithRightLeds() + proxSensors.countsFrontWithLeftLeds();
//    int8_t diff = proxSensors.countsFrontWithRightLeds() - proxSensors.countsFrontWithLeftLeds();
//if (sum == 0)
//   }
   
  
  if (buttonB.isPressed())
  {
    // Whenever the button is pressed, turn on the yellow LED.
    ledYellow(1);
  }
  else
  {
    // Whenever the button is not pressed, turn off the yellow
    // LED.
    ledYellow(0);
  }

  static int cPressedCount = 0;
  if (buttonC.getSingleDebouncedPress())
  {
    cPressedCount += 1;
    Serial.print(F("Button C was pressed "));
    Serial.print(cPressedCount);
    Serial.println(F(" times."));
    // Start playing a tone with frequency 440 Hz at maximum
    // volume (15) for 200 milliseconds.
    buzzer.playFrequency(440, 200, 15);
    
    lcd.clear();
    lcd.print(cPressedCount);
  }
}
