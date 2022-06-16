#include "orbit.h"

Orbit::OrbitData Orbit::update(double ballAngle, double ballStrength){
    OrbitData output;
    TSSPs tssps;
    if (ballAngle != -1){
        if (ballAngle < 360-ORBIT_STRIKE_ANGLE && ballAngle > ORBIT_STRIKE_ANGLE){
            double angleDiff = (ballAngle < 180 ? output.angle - ballAngle : ballAngle - output.angle);
            double gradient = abs(ORBIT_FAR_SPEED-ORBIT_CLOSE_SPEED)/(90-ORBIT_STRIKE_ANGLE);
            double cVal = ORBIT_CLOSE_SPEED + gradient;
            output.speed = constrain(-gradient * (angleDiff) + cVal, ORBIT_CLOSE_SPEED, ORBIT_FAR_SPEED);
        } else{
            output.speed = ORBIT_STRIKE_SPEED;
        }
    } else{
        output.speed = 0;
    }

    return output;
}