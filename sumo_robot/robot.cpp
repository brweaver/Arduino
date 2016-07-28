#include "robot.h"
#include <Wire.h>
#include <Zumo32U4.h>

void Robot::resetDefaultDelay() {
  stateInitTime = micros();
  stateDelayTime = Robot::defaultDelay;
}
void Robot::resetDelay(int delay) {
  stateInitTime = micros();
  stateDelayTime = Robot::defaultDelay;
}

Robot::RobotState Robot::state() {
  return currentState;
}

void Robot::refresh() {

  Serial.print(currentState);
  Serial.print("  ");
  
  // Check Delay 
  if (currentState != rs_deadstop) {
    unsigned long t = micros();
    Serial.print(stateInitTime);
    Serial.print(" ");
    Serial.print(t);
    Serial.print(" ");
    Serial.print(t - stateInitTime);
    if (t - stateInitTime > stateDelayTime * 1000) {
      deadStop();
    }
  }
  
  Serial.print(" ");
  Serial.println(currentState);
  
  switch (currentState) {
    case rs_forward:
      motors.setSpeeds(speed, speed);
      //motors.setRightSpeed(speed);
      break;
    case rs_nudgeleft: 
      motors.setLeftSpeed(speed);
      motors.setRightSpeed(speed/2);
      break;
    case rs_nudgeright:
      motors.setLeftSpeed(speed/2);
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
  resetDefaultDelay();
}
void Robot::turnLeft() {
  currentState = rs_turnleft;
  resetDefaultDelay();
}
void Robot::turnLeft(unsigned long delay) {
  currentState = rs_turnleft;
  resetDelay(delay);
}
void Robot::turnRight() {
  currentState = rs_turnright;
  resetDefaultDelay();
}
void Robot::turnRight(unsigned long delay) {
  currentState = rs_turnright;
  resetDelay(delay);
}
void Robot::nudgeLeft() {
  currentState = rs_nudgeleft;
  resetDefaultDelay();
}
void Robot::nudgeRight() {
  currentState = rs_nudgeright;
  resetDefaultDelay();
}
void Robot::deadStop() {
  currentState = rs_deadstop;
}

// blocking 
void Robot::taanabManeuver() {
  int dir = random(0,1);
  if (dir == 0) {
    motors.setLeftSpeed(-speed);
    motors.setRightSpeed(-speed/2);
    turnRight(TURN_DELAY_90);
  } else {
    motors.setRightSpeed(-speed);
    motors.setLeftSpeed(-speed/2);
    turnLeft(TURN_DELAY_90);
  }
  delay(200);
}

void Robot::backupManeuver() {
  motors.setLeftSpeed(-speed);
  motors.setRightSpeed(-speed);
  delay(80); // note, this is actually dangerous!!!
  int dir = random(0,1);
  if (dir == 0) {
    turnLeft(Robot::TURN_DELAY_180);
  } else {
    turnRight(Robot::TURN_DELAY_180);
  }
}

