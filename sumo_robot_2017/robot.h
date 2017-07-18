#include <Wire.h>
#include <Zumo32U4.h>
#include "sumo_robot.h"

/*
 * Class for managing robot 
 * 
 */
class Robot {
    
  public:

    enum RobotState {
      rs_forward,
      rs_backward,
      rs_ramming,
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
    
    // Basic Operations
    void forward();
    void turnLeft();
    void turnRight();
    void turnLeft(unsigned long delay);
    void turnRight(unsigned long delay);
    void nudgeLeft();
    void nudgeRight();
    void deadStop();
    void taanabManeuver();
    void backupManeuver();
    void refresh();

    void registerFrontLineHit();
    void registerLeftLineHit();
    void registerRightLineHit();
    bool hasRecentFrontLineHit();
    bool hasRecentLeftLineHit();
    bool hasRecentRightLineHit();

    void setProxResult();
    void setLineResult();
    
    RobotState state();
    
  private:

    void resetDefaultDelay();
    void resetDelay(int);
    bool randomBool();
    
    static const uint16_t maxSpeed = 1024;
    static const unsigned long defaultDelay = 20; // ms
    
    int16_t speed = 400;

    Zumo32U4Motors motors;
    
    uint16_t period;
    unsigned long stateInitTime;
    unsigned long stateDelayTime;

    unsigned long lastLeftLineHit; 
    unsigned long lastRightLineHit;
    unsigned long lastFrontLineHit;

    RobotState currentState;
    ProxResult proxResult;
    LineResult lineResult;

};

