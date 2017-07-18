#ifndef SUMO_ROBOT_H
#define SUMO_ROBOT_H

enum ProxResult
    {
      AheadClose,
      AheadFar,
      AheadLeft,
      AheadRight,
      Ahead,
      Left,
      Right,
      Nothing,
      Confused
    };
    
    enum LineResult
    {
      HitCenter,
      HitLeft,
      HitLeftCenter,
      HitRight,
      HitRightCenter,
      Miss
    };

#endif

