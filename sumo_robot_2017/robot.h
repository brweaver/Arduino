#include <Wire.h>
#include <Zumo32U4.h>
#include "sumo_robot.h"

// Can be 3 or 5. 
// Note that 5 means some of the prox sensors are disabled. 
#define NUM_LINE_SENSORS 5 

#define PRINT_LINE_SENSOR_DATA true
#define PRINT_PROX_SENSOR_DATA false
#define PRINT_ENCODER_DATA false
#define PRINT_MOTOR_DATA false

#define LINE_SENSOR_MOVING_AVG 10

#define LINE_SENSOR_THRESHOLD 500
#define LINE_SENSOR_HIT_WAIT 500

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

    // measured off the robot; distance between line sensors 
    int mmLineDistance[5][5] { 
      { 00, 35, 44, 53, 88},
      { 35, 00,  9, 18, 53},
      { 44,  9, 00,  9, 44},
      { 53, 18,  9, 00, 35},
      { 88, 53, 44, 35, 00},
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

    // These are all instant, blocking moves. 
    void danceForward(int mm, int speed);
    void danceBackward(int mm, int speed);
    //void danceForward(int mmLeft, mmRight);
    void danceTurn(int degrees);
    void danceShimmy(int delay);

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

    bool hasBorderContact(int requiredSensorCount);
    int lineSensorValue(int sensorIndex);
    int estimateBorderTangent();

    RobotState state();
    
  private:

    //void resetDefaultDelay();
    //void resetDelay(int);
    bool randomBool();
    
    void setEncoderCountTarget(int16_t targetLeftCount, int16_t targetRightCount);
    long degreesToCounts(int degrees);
    void resetEncoders();
    int16_t getAvgEncoderCount();
    void stopIfEncoderTargetReached();
    void delayUntilEncoderTargetReached();

    void readLineSensors();
    void updateLineSensorMovingAvg(unsigned int *values);
    bool lineSensorHit(int sensorIndex);
    float radiansToDegrees(float radians);
    
    static constexpr long countsPerRevolution = 1200; // 12 CPR on motor shaft, 100:1 gear ratio
    static constexpr long mmPerRevolution = 120; // measured: 120mm/per wheel revolution
    static constexpr long countPerMM = 10; 
    static constexpr float revolutionsPerDegree = 0.00845; //0.00833; // measured: ~0.75 wheel revolution for 90 turn, divided by 90 degrees. 
    
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
    unsigned long lineSensorHitEncoderCount[NUM_LINE_SENSORS];
    
// Depricated in favor of encoders. 
//    uint16_t period;
//    unsigned long stateInitTime;
//    unsigned long stateDelayTime;

    int16_t leftEncoderCount;
    int16_t rightEncoderCount; 

    //int16_t currentSpeed;
    int16_t speedEncoderStart; 
    unsigned long speedTimeStart;

    
// Depricated in favor of modelling
//    unsigned long lastLeftLineHit; 
//    unsigned long lastRightLineHit;
//    unsigned long lastFrontLineHit;
//    ProxResult proxResult;
//    LineResult lineResult;

    

    RobotState currentState;
    


};

