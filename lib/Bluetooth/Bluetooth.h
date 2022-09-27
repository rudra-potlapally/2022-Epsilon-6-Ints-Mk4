#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Timer.h>
#include <defines.h>
#include <pins.h>
#include "SoftwareSerial.h"

class Bluetooth
{
    private:
        void send();
        void recieve();
        void decideRole();
        Timer sendTimer = Timer(BT_SEND_TIMER);
        Timer connectedTimer = Timer(BT_CONNECTED_TIMER);
        uint16_t sameRole = 0;
        
    public:
        void init();
        void update(float ballDir, float ballStr);
        BluetoothData thisData = {0, 0, -1, 0};
        BluetoothData otherData = {0, 0, -1, 0};
        Timer switchTimer = Timer(BT_SWITCH_TIMER);
        bool isConnected = false;
        bool isSwitching = false;

};


#endif