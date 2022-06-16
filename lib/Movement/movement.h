#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "motor.h"
#include "pins.h"
#include "defines.h"
#include "Arduino.h"

class Movement {
    public:
        Movement();
        void move(int speed, int direction, int rotation);
    private:
        Motor bl, fl, br, fr;
};

#endif