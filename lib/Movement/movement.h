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
        void brake(int power) {this->move(0, 0, power); bl.brake(); fl.brake(); br.brake(); fr.brake();}
    private:
        Motor bl, fl, br, fr;
};

#endif