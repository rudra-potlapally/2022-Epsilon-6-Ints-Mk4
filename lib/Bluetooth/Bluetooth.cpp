#include "Bluetooth.h"

void Bluetooth::init() {
    Serial2.begin(BT_BAUD);
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
    Serial2.write(BT_START_BYTE);
    Serial2.write(BT_START_BYTE);
    Serial2.write(highByte(thisData.ballDir));
    Serial2.write(lowByte(thisData.ballDir));
    Serial2.write(thisData.ballStr);
    Serial2.write(thisData.role);
}

void Bluetooth::recieve() {
    uint8_t buffer[BT_PACKET_SIZE - 2] = {255, 255, 255, -1};
    if(Serial2.available() >= BT_PACKET_SIZE) {
        uint8_t firstByte = Serial2.read();
        uint8_t secondByte = Serial2.peek();
        if(firstByte == BT_START_BYTE && secondByte == BT_START_BYTE) {
            Serial2.read();
            for(uint8_t i = 0; i < BT_PACKET_SIZE - 2; i++) {
                buffer[i] = Serial2.read();
            }
            connectedTimer.update();
            otherData.ballDir = buffer[0] << 8 | buffer[1];
            otherData.ballDir = (otherData.ballDir == -1 ? -1 : otherData.ballDir);
            otherData.ballStr = buffer[2];
            int roleBefore = otherData.role;
            otherData.role = buffer[3] == DEFENSE_MODE ? DEFENSE_MODE : ATTACK_MODE;
            if(roleBefore == otherData.role) {
                if(otherData.role == thisData.role) {
                    isSwitching = true;
                }
            }
        }
    }
}

void Bluetooth::decideRole() {
    if (!isConnected){
        thisData.role = DEFENSE_MODE;
    }
    else if (isSwitching){
        if (thisData.role == ATTACK_MODE){
            thisData.role = DEFENSE_MODE;
        } else{
            thisData.role = ATTACK_MODE;
        }
        isSwitching = false;
        switchTimer.update();
        sameRole = 0;
    }
    else if (thisData.ballDir >= 80 && thisData.ballDir <= 280){
        if (thisData.role == ATTACK_MODE){
            thisData.role = DEFENSE_MODE;
        }
        isSwitching = false;
        switchTimer.update();
        sameRole = 0;
    }
    else if (thisData.ballDir <= 80 && thisData.ballDir >= 280){
        if (thisData.role == DEFENSE_MODE){
            thisData.role = ATTACK_MODE;
        }
        isSwitching = false;
        switchTimer.update();
        sameRole = 0;
    }
    else{
        if (thisData.ballStr > otherData.ballStr){
            thisData.role = ATTACK_MODE;
        } else{
            thisData.role = DEFENSE_MODE;
        }
        sameRole = 0;
    }
}