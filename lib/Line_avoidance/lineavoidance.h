
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
        Movement moveDirection(double lineAngle);
    private:
        int botlocation = 0;
        double original_line = -1;

};

#endif