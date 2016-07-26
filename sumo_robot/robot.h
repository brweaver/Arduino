#include <Wire.h>
#include <Zumo32U4.h>

/*
 * Class for managing robot 
 * 
 */


    
 class Robot {
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

  public:
    Robot() {}
    
    // Basic Operations
    void turnLeft();
    void turnRight();
    void nudgeLeft();
    void nudgeRight();
    void deadStop();
    void refresh();
    RobotState state();
    
  private:

    void resetDefaultDelay();
    void resetDelay(int);
    
    static const uint16_t maxSpeed = 1024;
    static const unsigned long defaultDelay = 20; // ms

    Zumo32U4Motors motors;
    
    uint16_t period;
    unsigned long stateInitTime;
    unsigned long stateDelayTime;

    RobotState currentState;

    
//    typedef struct Movement {
//      RobotState state;
//      unsigned long delay;
//    } Movement;
};

