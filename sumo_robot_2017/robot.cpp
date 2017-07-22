/*
 * TODO
 * 
 * - Dance
 * - Fix proximity sensors
 * - Calibrate prox/line detection easier. 
 * - More sophisticated seperation between line and proximity detection ???
 * - Better "center of board" strategy: keep bot closer to the middle. 
 * 
 * - Decoy
 * - IR Flak 
 * - 
 * 
 */
 
#include "robot.h"
#include <math.h>
#include <Wire.h>
#include <Zumo32U4.h>

/*
const int strongSignal = IR_NUM_BRIGHTNESS_LEVELS * 0.9;
const int modSignal = IR_NUM_BRIGHTNESS_LEVELS * 0.6;
const int weakSignal = IR_NUM_BRIGHTNESS_LEVELS * 0.25;



*/


// Static Methods
// ----------------
static void deriveLinearBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  uint16_t x = (max - min)/(count-1);
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = min + i*x;
  }
}

static void deriveExponentialBrightnessLevels(uint16_t min, uint16_t max, uint16_t count, uint16_t levels[]) {
  float a = min;
  float b = pow(max/a, 1.0/(count-1.0));
  levels[0] = min;
  for (int i = 1; i < count; i++) {
    levels[i] = a*pow(b, i);
  }
}

Robot::Robot() {
  
  printline("Init...");
  randomSeed(millis());
  
  if (NUM_LINE_SENSORS==3) {
    lineSensors.initThreeSensors();
    proxSensors.initThreeSensors();
  } else {
    lineSensors.initFiveSensors();
    proxSensors.initFrontSensor();
  }

  proxSensors.setPulseOnTimeUs(457); // default is 421; changing for the hell of it
  proxSensors.setPulseOffTimeUs(330); // per http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html#a6d47a41b45a7088916fbeaa5b38c60b3
  //deriveLinearBrightnessLevels(minIRBrightness, maxIRBrightness, numBrightnessLevels, brightnessLevels);
  deriveExponentialBrightnessLevels(IR_BRIGHTNESS_MIN, IR_BRIGHTNESS_MAX, IR_NUM_BRIGHTNESS_LEVELS, brightnessLevels);
  proxSensors.setBrightnessLevels(brightnessLevels, IR_NUM_BRIGHTNESS_LEVELS);

  printline("Ready.");
}

void Robot::printline(String str) {
  lcd.clear();
  lcd.print(str);
}

void Robot::play(char const* str) {
  buzzer.play(str);
}

Robot::RobotState Robot::state() {
  return currentState;
}

bool Robot::isMoving() {
  return currentState != rs_deadstop;
}

void Robot::resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}
void Robot::setEncoderCountTarget(int targetLeftCount, int targetRightCount) {
  resetEncoders();
  leftEncoderCount = targetLeftCount;
  rightEncoderCount = targetRightCount;
}
void Robot::setTargetDistance(int mm) {

  // Adjustments based on emprical runs at a speed of 400. 
  if (mm > 20) {
    mm -= 10;
  } else {
    mm *= 0.5;
  }
  
  long count = mm * Robot::countPerMM;
  
  if (PRINT_ENCODER_DATA) {
    Serial.print("Target Count: " + count);
  }
  setEncoderCountTarget(count, count);
}


//void Robot::resetDefaultDelay() {
//  stateInitTime = micros();
//  stateDelayTime = Robot::defaultDelay;
//}
//void Robot::resetDelay(int delay) {
//  stateInitTime = micros();
//  stateDelayTime = Robot::defaultDelay;
//}

bool Robot::randomBool() {
  int r = random(0,100);
  return r % 2 == 0;
}

//void Robot::registerFrontLineHit() {
//  lastFrontLineHit = millis();
//}
//void Robot::registerLeftLineHit() {
//  lastLeftLineHit = millis();
//}
//void Robot::registerRightLineHit() {
//  lastRightLineHit = millis();
//}
//
//bool Robot::hasRecentFrontLineHit() {
//  return millis() - lastFrontLineHit > RECENT_LINE_HIT_MS;
//}
//bool Robot::hasRecentLeftLineHit() {
//  return millis() - lastLeftLineHit > RECENT_LINE_HIT_MS;
//}
//bool Robot::hasRecentRightLineHit() {
//  return millis() - lastRightLineHit > RECENT_LINE_HIT_MS;
//}

void Robot::refresh() {
  
//  // Check Delay 
//  if (currentState != rs_deadstop) {
//    unsigned long t = micros();
////    Serial.print(stateInitTime);
////    Serial.print(" ");
////    Serial.print(t);
////    Serial.print(" ");
////    Serial.print(t - stateInitTime);
//    if (t - stateInitTime > stateDelayTime * 1000) {
//      deadStop();
//    }
//  }
//  
  if (PRINT_ENCODER_DATA) {
    Serial.print("State: ");
    if (currentState == rs_deadstop) {
      Serial.print("stopped");
    } else {
      Serial.print("moving");
    }
    Serial.print(" Encoders: L-");
    Serial.print(leftEncoderCount);
    Serial.print(",");
    Serial.print(encoders.getCountsLeft());
    Serial.print(" R-");
    Serial.print(rightEncoderCount);
    Serial.print(",");
    Serial.print(encoders.getCountsLeft());
    Serial.print("\n");
  }

  if (PRINT_ENCODER_DATA) {
//    lcd.print("" + encoders.getCountsLeft());
//    printline();
  }
  
  if (currentState != rs_deadstop) {
    bool left = encoders.getCountsLeft() >= leftEncoderCount;
    bool right = encoders.getCountsRight() >= rightEncoderCount;
    if (left) motors.setLeftSpeed(0);
    if (right) motors.setRightSpeed(0);
    if (left && right) {
      Serial.print("deadstop\n");
      deadStop(); 
    }
  }
  
  switch (currentState) {
    case rs_forward:
      motors.setSpeeds(speed, speed);
      break;
    case rs_nudgeleft: 
      motors.setLeftSpeed(speed);
      motors.setRightSpeed(speed * NUDGE_REDUCTION);
      break;
    case rs_nudgeright:
      motors.setLeftSpeed(speed * NUDGE_REDUCTION);
      motors.setRightSpeed(speed);
      break;
    case rs_turnleft:
      motors.setLeftSpeed(-speed);
      motors.setRightSpeed(speed);
      break;
    case rs_turnright:
      motors.setLeftSpeed(speed);
      motors.setRightSpeed(-speed);
      break;
    case rs_deadstop:
      motors.setLeftSpeed(0);
      motors.setRightSpeed(0);
      break;
  }

}


void Robot::forward() {
  currentState = rs_forward;
  //setEncoderCountTarget(Robot::defaultRevolutions, Robot::defaultRevolutions);
}

void Robot::turn(int degrees) {
  if (degrees > 0) {
    currentState = rs_turnright;
  } else if (degrees < 0) {
    currentState = rs_turnleft;
  } else {
    return; // Ignore 0.
  }
  
  int rev = degreesToRevolutions(degrees);
  setEncoderCountTarget(rev, -rev);
}
long Robot::degreesToRevolutions(int degrees) {
  // is this an empiracal value? 
  return (long)(degrees*Robot::revolutionsPerDegree*Robot::countsPerRevolution);
}

// Disabled for now
//void Robot::nudgeLeft() {
//  currentState = rs_nudgeleft;
//  resetDefaultDelay();
//}
//void Robot::nudgeRight() {
//  currentState = rs_nudgeright;
//  resetDefaultDelay();
//}

void Robot::deadStop() {
  currentState = rs_deadstop;
}

void Robot::readLineSensors() {
  lineSensors.read(lineSensorValues);
  //updateLineSensorMovingAvg(lineSensorValues);

  if (PRINT_LINE_SENSOR_DATA) {
    Serial.print("Values: ");
    for (int i = 0; i < NUM_LINE_SENSORS; i++) {
      Serial.print(lineSensorValues[i]);
      Serial.print(" ");
    }
    Serial.print(" avg: ");
    for (int i = 0; i < NUM_LINE_SENSORS; i++) {
      Serial.print(lineSensorValuesAvg[i] / LINE_SENSOR_MOVING_AVG);
      Serial.print(" ");
    }
    Serial.println("");
  }

  // Update last hit across all sensors
  for (int i = 0; i < NUM_LINE_SENSORS; i++) {
    if (lineSensorHit(i)) {
      lineSensorHitTimestamp[i] = millis();
    } 
  }
}

bool Robot::lineSensorHit(int sensorIndex) {
  if (lineSensorValues[sensorIndex] < LINE_SENSOR_THRESHOLD) return true;
  //if (lineSensorValues[sensorIndex] < lineSensorValuesAvg[sensorIndex]*1.1) return true;
  return false;
}

// Return if any sensor has border contact. 
bool Robot::hasBorderContact(int requiredSensorCount) {
  int count = 0;
  for (int i = 0; i < NUM_LINE_SENSORS; i++) {
    if (lineSensorHit(i)) ++count;
    if (count >= requiredSensorCount) return true;
  }
  return false;
}

int Robot::estimateBorderTangent() {

  int indexOldest = -1;
  int indexYoungest = -1;
  
  for (int i = 0; i < NUM_LINE_SENSORS; i++) {
    if (lineSensorHit(i)) {
      if (lineSensorHitTimestamp[i] < indexOldest) {
        indexYoungest = indexOldest;
        indexOldest = i;
      } else if (lineSensorHitTimestamp[i] < indexYoungest) {
        indexYoungest = i;
      }
    }
  }

  if (indexYoungest == -1)  {
    // Not enough data to make calcualtion. 
    return -1;
  }

  float adj = (lineSensorHitTimestamp[indexOldest] - lineSensorHitTimestamp[indexYoungest]) * mmPerMS;
  float opp = mmLineDistance[indexOldest][indexYoungest];

  float rad = atan2(adj, opp);
  return (int)(rad * 57.2957795); //rad * 57296 / 1000
}


void Robot::updateLineSensorMovingAvg(unsigned int *values) {
  unsigned int i;
  
  for (i = 0; i < NUM_LINE_SENSORS; i++) {
    lineSensorValuesAvg[i] -= lineSensorMovingAvg[lineSensorMovAvgIndex][i];
    lineSensorMovingAvg[lineSensorMovAvgIndex][i] = values[i];
    lineSensorValuesAvg[i] += lineSensorMovingAvg[lineSensorMovAvgIndex][i];
  }

  lineSensorMovAvgIndex += 1;
  if (lineSensorMovAvgIndex >= LINE_SENSOR_MOVING_AVG) {
    lineSensorMovAvgIndex = 0;
  }
}

/*

void printProxSensorResults(uint16_t sensor[]) {
    Serial.print(sensor[0]);
    Serial.print(" ");
    Serial.print(sensor[1]);
    Serial.print(" ");
    Serial.print(sensor[2]);
}

String proxResultToString(ProxResult r) {
  switch(r) {
      case AheadClose: return "AheadClose";
      case Ahead: return "Ahead";
      case AheadFar: return "AheadFar";
      case AheadLeft: return "AheadLeft";
      case Left: return "Left";
      case AheadRight: return "AheadRight";
      case Right: return "Right";
      case Nothing: return "Nothing";
      case Confused: return "Confused";
      default: return "oops.";
  }
}



ProxResult readProxSensorsSimple() {
  
  proxSensors.read();

  uint16_t sumLeftLED[] = {
    proxSensors.countsLeftWithLeftLeds(), 
    proxSensors.countsFrontWithLeftLeds(), 
    proxSensors.countsRightWithLeftLeds()}; 
  uint16_t sumRightLED[] = {
    proxSensors.countsLeftWithRightLeds(), 
    proxSensors.countsFrontWithRightLeds(), 
    proxSensors.countsRightWithRightLeds()};

  int diffFrontSignal = sumLeftLED[1] - sumRightLED[1]; 

  if (PRINT_PROX_SENSOR_DATA) {
    Serial.print("Left: ");
    printProxSensorResults(sumLeftLED);
    Serial.print(" Right: ");
    printProxSensorResults(sumRightLED);
    Serial.print("\n");
  }
  
  // 90% 1/4
  // 80% 1/2 means (roughly) midcircle
  // 70% 1/2
  // 40% means 3/4 
  // 20% means (roughly) full circle

  if (!robot.hasRecentFrontLineHit() && sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal) {
      return AheadFar;
  } else if (sumRightLED[2] >= modSignal && !robot.hasRecentRightLineHit()) {
    return Right;
  } else if (sumLeftLED[0] >= modSignal && !robot.hasRecentLeftLineHit()) {
    return Left;
  } else if (diffFrontSignal > weakSignal) {
    return AheadLeft;
  } else if (diffFrontSignal < -weakSignal) {
    return AheadRight;
  } else if (sumLeftLED[1] >= modSignal && sumRightLED[1] >= modSignal && abs(diffFrontSignal) < weakSignal) {
    return Ahead;
  } else if (sumLeftLED[1] >= weakSignal && sumRightLED[1] >= weakSignal) {
    return AheadFar;
  }
  
  return Confused;
}

 */

// Maneuvers are blocking--no sensor input or state will be  
// will be examined until the maneuver has completed. 

// Depricated
//void Robot::taanabManeuver() {
//  if (randomBool()) {
//    motors.setLeftSpeed(-speed);
//    motors.setRightSpeed(-speed*NUDGE_REDUCTION);
//  } else {
//    motors.setRightSpeed(-speed);
//    motors.setLeftSpeed(-speed*NUDGE_REDUCTION);
//  }
//  delay(450); // Whoa!
//  forward();
//}
//
//void Robot::backupManeuver() {
//  motors.setLeftSpeed(-speed);
//  motors.setRightSpeed(-speed);
//  delay(80); // note, this is actually dangerous!!!
//  if (randomBool()) {
//    turnLeft(Robot::TURN_DELAY_180);
//  } else {
//    turnRight(Robot::TURN_DELAY_180);
//  }
//}

void Robot::waitForButtonA() {
  lcd.clear();
  lcd.print(F("Press A"));
  lcd.gotoXY(0, 1);
  buttonA.waitForButton();
  lcd.clear();
}

