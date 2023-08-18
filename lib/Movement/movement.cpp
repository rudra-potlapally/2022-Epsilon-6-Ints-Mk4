#include "movement.h"

Movement::Movement(){
    bl = Motor(BLINA, BLINB);
    br = Motor(BRINA, BRINB);
    fl = Motor(FLINA, FLINB);
    fr = Motor(FRINA, FRINB);
}

void Movement::move(int speed, int direction, int rotation){
    int blspeed, brspeed, flspeed, frspeed;

    blspeed = (cos(DEG2RAD*(225-90-direction))*speed)-rotation;
    flspeed = (cos(DEG2RAD*(315-90-direction))*speed)-rotation;
    brspeed = (cos(DEG2RAD*(135-90-direction))*speed)-rotation;
    frspeed = (cos(DEG2RAD*(45-90-direction))*speed)-rotation;
    int maxSpeed=0;
    if (abs(blspeed)>maxSpeed) {
        maxSpeed = abs(blspeed);
    }
    if (abs(flspeed)>maxSpeed) {
        maxSpeed = abs(flspeed);
    }
    if (abs(brspeed)>maxSpeed) {
        maxSpeed = abs(brspeed);
    }
    if (abs(frspeed)>maxSpeed) {
        maxSpeed = abs(frspeed);
    }
    if (maxSpeed>255) {
        blspeed = (blspeed/maxSpeed)*255;
        flspeed = (flspeed/maxSpeed)*255;
        brspeed = (brspeed/maxSpeed)*255;
        frspeed = (frspeed/maxSpeed)*255;
    }
    bl.move(blspeed);
    br.move(brspeed);
    fl.move(flspeed);
    fr.move(frspeed);
}