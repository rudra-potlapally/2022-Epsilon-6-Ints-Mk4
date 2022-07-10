#include "kicker.h"

void Kicker::init(){
    pinMode(KICKER_SIG, OUTPUT);
    pinMode(LG_SIG, INPUT);
    digitalWrite(KICKER_SIG, LOW);
    int sum = 0;
    for (int i = 0; i < 10; i++){
        sum += analogRead(LG_SIG);
    }
    sum /= 10;
    lgCalVal = sum + LDR_OFSET;
}

void Kicker::update(){
    if(this->kickDischarge.timeHasPassedNoUpdate()) {
        if(this->kickCharge.timeHasPassedNoUpdate()) {
            if(analogRead(LG_SIG) > lgCalVal){
                digitalWrite(KICKER_SIG, HIGH);
                this->kickDischarge.resetTime();
                this->kickCharge.resetTime();
            }
        } else {
            digitalWrite(KICKER_SIG, LOW);
        }
    }
}

void Kicker::kick(){
    // if(shouldKick == true){
    //     if(i < 50){
    //         digitalWrite(KICKER_SIG, HIGH);
    //         i++;
    //     }else{
    //         digitalWrite(KICKER_SIG, LOW);
    //         i = 0;
    //     }
    // } else{
    //     digitalWrite(KICKER_SIG, LOW);
    // }
    return;
}