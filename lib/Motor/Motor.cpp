#include "motor.h"

Motor::Motor(int ina, int inb){
    inaPin = ina;
    inbPin = inb;

    pinMode(inaPin, OUTPUT);
    pinMode(inbPin, OUTPUT);
}

void Motor::move(float speed) {
    if (speed >= 0) {
        analogWrite(inaPin, constrain(abs(speed),0,255));
        digitalWriteFast(inbPin, LOW);
    }
    else if (speed < 0) {
        digitalWriteFast(inaPin, LOW);
        analogWrite(inbPin, constrain(abs(speed),0,255));
    }
}

void Motor::brake(){
    digitalWriteFast(inaPin, HIGH);
    digitalWriteFast(inbPin, HIGH);
}   