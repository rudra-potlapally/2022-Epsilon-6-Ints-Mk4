#ifndef TSSPS_H
#define TSSPS_H

#include <Arduino.h>
#include "defines.h"
#include "pins.h"
#include "Bluetooth.h"

class TSSPs {
public:
    TSSPs();
    void update();
    float calculateAngleAddition(bool mode);
    float speedMultiplier(int lowSpd, int highSpd);
    float ballDir;
    float ballStr;
    float angleAddition;
    bool ballVisible;
private:
    uint8_t tsspPins[TSSP_NUM] = {IR0, IR1, IR2, IR3, IR4, IR5, IR6, IR7, IR8, IR9, IR10, IR11, IR12, IR13, IR14, IR15};
    float tsspXValues[TSSP_NUM] = {TSSP_X_0, TSSP_X_1, TSSP_X_2, TSSP_X_3, TSSP_X_4, TSSP_X_5, TSSP_X_6, TSSP_X_7, TSSP_X_8, TSSP_X_9, TSSP_X_10, TSSP_X_11, TSSP_X_12, TSSP_X_13, TSSP_X_14, TSSP_X_15};
    float tsspYValues[TSSP_NUM] = {TSSP_Y_0, TSSP_Y_1, TSSP_Y_2, TSSP_Y_3, TSSP_Y_4, TSSP_Y_5, TSSP_Y_6, TSSP_Y_7, TSSP_Y_8, TSSP_Y_9, TSSP_Y_10, TSSP_Y_11, TSSP_Y_12, TSSP_Y_13, TSSP_Y_14, TSSP_Y_15};
    uint8_t tsspCounter;
    uint8_t tsspValues[TSSP_NUM] = {0};
    uint8_t tsspSortedValues[TSSP_NUM] = {0};
    uint8_t tsspIndexes[TSSP_NUM] = {0};
};

#endif // TSSPS_H