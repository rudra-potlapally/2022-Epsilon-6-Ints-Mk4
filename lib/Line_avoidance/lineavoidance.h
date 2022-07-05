
#ifndef LINEAVOIDANCE_H
#define LINEAVOIDANCE_H
#include "defines.h"
#include "pins.h"
#include "LightSensor.h"
#include <Arduino.h>
class oAvoidance {
    public:
        struct Movement{
            double speed;
            double direction;
        };
        int botlocation = 0;
        Movement moveDirection(double lineAngle);
    private:
        
        double original_line = -1;

};

#endif