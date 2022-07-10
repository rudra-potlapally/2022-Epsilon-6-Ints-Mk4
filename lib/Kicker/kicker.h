#ifndef KICKER_H
#define KICKER_H

#include "defines.h"
#include "pins.h"
#include "Timer.h"
#include <Arduino.h>

class Kicker {
    public:
        void kick();
        void update();
        void init();
    private:
        bool shouldKick = false;
        bool triggered = false;
        uint8_t i = 0;
        Timer kickDischarge = Timer((unsigned long)KICK_DISCHARGE_TIME);
        Timer kickCharge = Timer((unsigned long)KICK_CHARGE_TIME);
        float lgCalVal;
};

#endif