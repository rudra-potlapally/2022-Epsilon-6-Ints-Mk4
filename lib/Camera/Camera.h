#ifndef CAMERA_H
#define CAMERA_H

#include <defines.h>
#include "Arduino.h"
#include "vect.h"

class Camera {
public:
    Camera();
    void init();
    void update(bool attackBlue);
    Vect robot;
    Vect goal;
    float attackAngle;
    float defendAngle;
    float attackDist;
    float defendDist;
    

private:
    Vect blueGoal;
    Vect yellowGoal;
    float ballPixeltoCM(float dist);
    float goalPixeltoCM(float dist);
};

#endif