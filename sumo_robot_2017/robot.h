#include <Wire.h>
#include <Zumo32U4.h>
#include "sumo_robot.h"

// Can be 3 or 5. 
// Note that 5 means some of the prox sensors are disabled. 
#define NUM_LINE_SENSORS 5 

#define PRINT_LINE_SENSOR_DATA false
#define PRINT_PROX_SENSOR_DATA false
#define PRINT_ENCODER_DATA true

#define LINE_SENSOR_MOVING_AVG 10
#define LINE_SENSOR_THRESHOLD 500

#define IR_BRIGHTNESS_MIN 2 
#define IR_BRIGHTNESS_MAX 410 // 418 is the empirical max, but it's not always triggered. 
#define IR_NUM_BRIGHTNESS_LEVELS 20



/*
 * Class for managing robot 
 * 
 */
class Robot {
    
  public:

    enum RobotState {
      rs_forward,
      rs_ramming,
      rs_backward,
      rs_nudgeleft,
      rs_nudgeright,
      rs_turnleft,
      rs_turnright,
      rs_deadstop,
    };
    
    Robot();

    static const unsigned long TURN_DELAY_90 = 250; //160?
    static const unsigned long TURN_DELAY_180 = 400;
    static const unsigned long RECENT_LINE_HIT_MS = 200;
    static constexpr float NUDGE_REDUCTION = 0.33;

    static constexpr unsigned long mmLineDistance[5][5] { 
      
      { 00, 30, 40, 50, 80},
      { 30, 00, 10, 20, 50},
      { 40, 10, 00, 10, 40},
      { 50, 20, 10, 00, 30},
      { 80, 50, 40, 30, 00},
    };
    
    // Basic Operations
    void forward();
    void turnLeft();
    void turnRight();
    void turn(int degrees);
    void nudgeLeft();
    void nudgeRight();
    void deadStop();
    void taanabManeuver();
    void backupManeuver();
    void refresh();
    bool isMoving();
    void waitForButtonA();

//    void registerFrontLineHit();
//    void registerLeftLineHit();
//    void registerRightLineHit();
//    bool hasRecentFrontLineHit();
//    bool hasRecentLeftLineHit();
//    bool hasRecentRightLineHit();

    void setProxResult();
    void setLineResult();

    void setTargetDistance(int mm);

    void printline(String str);
    void play(char const* str);
    
    RobotState state();
    
  private:

    //void resetDefaultDelay();
    //void resetDelay(int);
    bool randomBool();
    
    void setEncoderCountTarget(int targetLeftCount, int targetRightCount);
    long degreesToRevolutions(int degrees);
    void resetEncoders();

    void readLineSensors();
    void updateLineSensorMovingAvg(unsigned int *values);
    bool lineSensorHit(int sensorIndex);
    bool hasBorderContact(int requiredSensorCount);
    int estimateBorderTangent();

    static const uint16_t maxSpeed = 1024;
    //static const unsigned long defaultDelay = 20; // ms
    static const uint16_t defaultRevolutions = 20; // uh? 
    
    static constexpr long countsPerRevolution = 1200; // 12 CPR on motor shaft, 100:1 gear ratio
    static constexpr long mmPerRevolution = 120; // measured: 120mm/per wheel revolution
    static constexpr long countPerMM = 10; 
    static constexpr float revolutionsPerDegree = 0.00833; // measured: ~0.75 wheel revolution for 90 turn, divided by 90 degrees. 
    static constexpr float mmPerMS = 1; // speed measure...weird
    
    Zumo32U4Motors motors;
    Zumo32U4Encoders encoders;
    Zumo32U4ProximitySensors proxSensors;
    Zumo32U4LineSensors lineSensors;
    Zumo32U4ButtonA buttonA;
    Zumo32U4ButtonB buttonB;
    Zumo32U4ButtonC buttonC;
    Zumo32U4Buzzer buzzer;
    Zumo32U4LCD lcd;

    int16_t speed = 400;
    uint16_t brightnessLevels[IR_NUM_BRIGHTNESS_LEVELS];
    
    unsigned int lineSensorValues[NUM_LINE_SENSORS];
    unsigned int lineSensorValuesAvg[NUM_LINE_SENSORS];
    unsigned int lineSensorMovingAvg[LINE_SENSOR_MOVING_AVG][NUM_LINE_SENSORS];
    unsigned int lineSensorMovAvgIndex = 0;
    unsigned long lineSensorHitTimestamp[NUM_LINE_SENSORS];
    
// Depricated in favor of encoders. 
//    uint16_t period;
//    unsigned long stateInitTime;
//    unsigned long stateDelayTime;

    unsigned long leftEncoderCount;
    unsigned long rightEncoderCount; 
    
// Depricated in favor of modelling
//    unsigned long lastLeftLineHit; 
//    unsigned long lastRightLineHit;
//    unsigned long lastFrontLineHit;
//    ProxResult proxResult;
//    LineResult lineResult;

    

    RobotState currentState;
    


};

