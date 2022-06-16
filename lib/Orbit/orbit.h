#ifndef ORBIT_H
#define ORBIT_H

#include "defines.h"
#include "pins.h"
#include "TSSPs.h"
#include <Arduino.h>

class Orbit {
    public:
        struct OrbitData{
            int speed;
            int angle;
        };
        Orbit() {};

        OrbitData update(double ballAngle, double ballStrength);
        

};

#endif