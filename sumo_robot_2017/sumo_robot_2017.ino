#include "sumo_robot.h"
#include "robot.h"
#include <Wire.h>
#include <Zumo32U4.h>

// The robot class wraps all the underlying hardware on the robot. 
Robot robot;

char const* CANT_STOP_THE_FEELING = "T130O5MSL8E4.C<ACC<A2CCDEDCDEDCC<A2EDCEG4.GEDDC16<A2CCDEDCDEDCC<A2O4MSL8GG.A4>CA>C16A>CA.GA>CA16>CA16>C>E4.GA>CA>C16A>CA.GA>C4.";
// 2) O4MSL8E2C<ACC<A2CCDEDCDEDCC<A2EDCEG4.GEDDC16<A2CCDEDCDEDCC<A2
// 1) O5MSL8DDC16DMLE4R2O4MSC16D4D4D4D4O5DDC16DMLE4D2O4MSC16D4D4D4D4C16A<C
// Can't stop"O5MSL8DDC16DMLE4R2O4MSC16D4D4D4D4O5DDC16DMLE4D2"
// transition O4MSC16D4D4D4D4C16A<C

void setup() {


  //systemCheck();
  
  robot.waitForButtonA();
  robot.play(">g32>>c32");
  
  // Delay 5-seconds!
  robot.printline("2sec...");
  delay(2000);

  //dance01();
  
  //testEncoderConstantsForwardMovement();
  testEncoderConstantsTurn();
  //testLineDetectionSensor();
  //testLineDetectionWithMovement();
}

void loop() {
  strategyTestBackAndForth();
  robot.refresh();
}

void systemCheck() {
  
  Zumo32U4Motors motors;
  Zumo32U4Encoders encoders;
  ledRed(1);
  delay(1000);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(-200, 200);
  int16_t left;
  int16_t right;

  do {
    left = encoders.getCountsLeft();
    right = encoders.getCountsRight();
    Serial.print("Left: ");
    Serial.print(left);
    Serial.print("  Right: ");
    Serial.print(right);
    Serial.print("\n");
    
  } while (left > 100 || right < 100);
  motors.setSpeeds(0,0);
  
}


void dance01() {
  robot.play(CANT_STOP_THE_FEELING);
  robot.danceTurn(-10);
  robot.danceTurn(10);
  robot.danceTurn(-10);
  robot.danceTurn(10);
  robot.danceTurn(-10);
  robot.danceTurn(10);
  robot.danceForward(10, 100);
  delay(100);
  robot.danceForward(10, 200);
  delay(100);
  robot.danceForward(10, 300);
  delay(100);
  robot.danceForward(10, 400);
  delay(100);
  robot.turn(180);
  robot.danceForward(10, 100);
  delay(100);
  robot.danceForward(10, 200);
  delay(100);
  robot.danceForward(10, 300);
  delay(100);
  robot.danceForward(10, 400);
  delay(100);
}

void strategyTestBackAndForth() {
  
}

void testLineDetectionSensor() {

  long t = millis();
  while (true) {
    
    if (millis() - t > 500) {
      //String str = robot.lineSensorValue(0);
      if (robot.hasBorderContact(2)) {
        robot.printline("contact!");
      } else {
        robot.printline("...");
      }
      t = millis();
    }
    robot.refresh();
  }
}

void testLineDetectionWithMovement() {
  
  robot.setTargetDistance(200); // MM
  robot.forward();
  robot.printline("...");
  
  do {
    robot.refresh();
    if (robot.hasBorderContact(2)) {
      robot.deadStop();
      robot.printline("contact");
      break;
    }
  } while (robot.isMoving());
  
  //robot.waitForButtonA();
  //delay(2000);
  
  // black test: (all values should be black)
  // white test: (all values should be white)
  // angle test: robot moves forward until two sensors hit; angle calculated. 
  // 
}

void exerciseProxDetection() {
  // follow test: attempt to follow object 1ft away
  // attempt to lead moving object?
  // attempt to dectect pushable object
}

void testEncoderConstantsForwardMovement() {

  // Expected Result: Robot moves forward 10mm. 
  robot.printline("Fwd1cm");
  robot.setTargetDistance(10); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot moves forward 20mm. 
  robot.printline("Fwd2cm");
  robot.setTargetDistance(20); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot moves forward 30mm. 
  robot.printline("Fwd3cm");
  robot.setTargetDistance(30); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);
  
  // Expected Result: Robot moves forward 60mm. 
  robot.printline("Fwd6cm");
  robot.setTargetDistance(60); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot moves forward 120mm. 
  robot.printline("Fwd12cm");
  robot.setTargetDistance(120); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);
  
  // Expected Result: Robot moves forward 240mm. 
  robot.printline("Fwd24cm");
  robot.setTargetDistance(240); // MM
  robot.forward();
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);
}

void testEncoderConstantsTurn() {
  
  // Expected Result: Robot turns exactly 90 degrees to the right
  robot.printline("R90*");
  robot.turn(90);
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot turns exactly 90 degrees to the left
  robot.printline("L90*");
  robot.turn(-90);
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot turns exactly 30 degrees to the right
  robot.printline("R30*");
  robot.turn(30);
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000);

  // Expected Result: Robot turns exactly 30 degrees to the left
  robot.printline("L30*");
  robot.turn(-30);
  refreshLoop(); 
  robot.waitForButtonA();
  delay(2000); 
}

void refreshLoop() {
  while (robot.isMoving()) {
    robot.refresh();
  }
}

void openingDance() {

  //buzzer.play(">g32>>c32");
  
}


//void openingAnimation() {
//  lcd.clear();
//  lcd.print(">");
//
//  ledRed(1);
//  delay(500);
//
//  ledRed(0);
//  ledYellow(1);
//  lcd.print(">");
//  delay(500);
//
//  ledRed(0);
//  ledYellow(0);
//  ledGreen(1);
//  lcd.print(">");
//}



/*

// Respond to prox result
  ProxResult proxResult = readProxSensorsSimple();  
  switch (proxResult) {
    case Confused:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("Confused");
      //robot.forward(); // This is the default search behavior. 
      break;
    case Nothing:  // unused
    default:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("No Target");
      //robot.forward();
      break;
    case AheadClose:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("A");
      //robot.forward();
      break;
    case Ahead:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("^");
      // move forward
      //robot.forward();
      break;
    case AheadFar:
      lcd.clear();
      lcd.gotoXY(3, 1);
      lcd.print("|");
      //robot.forward();
      break;
    case Left:
      lcd.clear();
      lcd.gotoXY(0, 1);
      lcd.print("<<");
      robot.turnLeft();
      break;
    case AheadLeft:
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
    case AheadRight:
      lcd.clear();
      lcd.gotoXY(5, 1);
      lcd.print(">");
      robot.nudgeRight();
      break;
  }

  LineResult lineResult = readLineSensorSimple();
  switch (lineResult) {
    case Miss:
      lcd.gotoXY(1, 0);
      lcd.print("no");
      break; 
    case HitCenter:
      lcd.gotoXY(1, 0);
      lcd.print("HC");
      robot.backupManeuver();
      break;
    case HitLeft:
      lcd.gotoXY(1, 0);
      lcd.print("HL");
      robot.turnRight(Robot::TURN_DELAY_90);
      break;
    case HitLeftCenter:
      lcd.gotoXY(1, 0);
      lcd.print("CL");
      robot.turnRight(Robot::TURN_DELAY_180);
      break;
    case HitRight:
      lcd.gotoXY(1, 0);
      lcd.print("HR");
      robot.turnLeft(Robot::TURN_DELAY_90);
      break;
    case HitRightCenter:
      lcd.gotoXY(1, 0);
      lcd.print("CR");
      robot.turnLeft(Robot::TURN_DELAY_180);
      break;
    
  }

 */

