#include "Bluetooth.h"

void Bluetooth::init() {
    Serial1.setRX(27);
    Serial1.setTX(26);
    Serial1.begin(BT_BAUD);
}

void Bluetooth::update(float ballDir, float ballStr){
    thisData.ballDir = (uint16_t)ballDir;
    thisData.ballStr = (uint8_t)ballStr;
    if(sendTimer.timeHasPassed()) {
        send();
    }
    recieve();
    decideRole();

    isConnected = connectedTimer.timeHasPassedNoUpdate() ? false : true;
}

void Bluetooth::send(){
    Serial1.write(BT_START_BYTE);
    Serial1.write(BT_START_BYTE);
    Serial1.write(highByte(thisData.ballDir));
    Serial1.write(lowByte(thisData.ballDir));
    Serial1.write(thisData.ballStr);
    Serial1.write(thisData.role);
    Serial1.write(thisData.goalDist);
}

void Bluetooth::recieve() {
    uint8_t buffer[BT_PACKET_SIZE - 2] = {255, 255, 255, 255, 255};
    if(Serial1.available() >= BT_PACKET_SIZE) {
        uint8_t firstByte = Serial1.read();
        uint8_t secondByte = Serial1.peek();
        if(firstByte == BT_START_BYTE && secondByte == BT_START_BYTE) {
            Serial1.read();
            for(uint8_t i = 0; i < BT_PACKET_SIZE - 2; i++) {
                buffer[i] = Serial1.read();
            }
            connectedTimer.update();
            otherData.ballDir = buffer[0] << 8 | buffer[1];
            otherData.ballDir = (otherData.ballDir == -1 ? -1 : otherData.ballDir);
            otherData.ballStr = buffer[2];
            bool roleBefore = otherData.role;
            otherData.role = buffer[3] == DEFENSE_MODE ? DEFENSE_MODE : ATTACK_MODE;
            otherData.goalDist = buffer[4];
            if(!roleBefore && otherData.role){
                if(otherData.role == thisData.role){
                    isSwitching = true;
                }
            }
        }
    }
}

void Bluetooth::decideRole() {
    if(thisData.role == DEFENSE_MODE){
        if(thisData.ballStr > DEFENSE_SURGE_STRENGTH && (thisData.ballDir <= 80 || thisData.ballDir >= 280)){
            thisData.role = ATTACK_MODE;
        } else {
            if ((thisData.role == otherData.role)){
                if(thisData.ballStr > otherData.ballStr){
                    thisData.role = ATTACK_MODE;
                } else{
                    thisData.role = DEFENSE_MODE;
                }
            }
        }
    } else {
        if(thisData.role == otherData.role){
            if (thisData.ballStr > otherData.ballStr){
                thisData.role = ATTACK_MODE;
            }
        }
    }
    if (!isConnected){
        thisData.role = DEFENSE_MODE;
        otherData.role = -1;
        otherData.ballDir = -1;
        otherData.ballStr = 0;
        sameRole = 0;
    } else if (isSwitching){
        thisData.role = !thisData.role;
        isSwitching = false;
        switchTimer.update();
        sameRole = 0;
    }
}