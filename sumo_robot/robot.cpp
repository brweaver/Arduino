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
  
  // Check Delay 
  if (currentState != rs_deadstop) {
    unsigned long t = micros();
    if (t - stateInitTime > stateDelayTime) {
      deadStop();
    }
  }

  int16_t speed = 400;
  
  switch (currentState) {
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

void Robot::turnLeft() {
  currentState = rs_turnleft;
  resetDefaultDelay();
}
void Robot::turnRight() {
  currentState = rs_turnright;
  resetDefaultDelay();
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


