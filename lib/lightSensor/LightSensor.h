#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

#include "defines.h"
#include "pins.h"
#include <Arduino.h>

class LightSensor {
    public:
        LightSensor();
        void readAll();
        double update();
        void init();
        struct Cluster{
            float start = -1;
            float end = -1;
            float midpoint = -1;
        };
        void debug();
        double ls_cal[LS_NUM];
        bool broke;
        void test();
        int clusterNum;
    private:
        bool inCluster;
        uint8_t indexA[LS_NUM_IND] = {24, 25, 26, 27, 28, 29, 30, 31, 0, 7, 6, 5, 4, 3, 2, 1};
        uint8_t indexB[LS_NUM_IND] = {8, 9, 10, 11, 12, 13, 14, 15, 23, 22, 21, 20, 19, 18, 17, 16};
        uint8_t white[LS_NUM] = {0};
        int read[LS_NUM];
        float lineAngle;
        uint16_t readValue[LS_NUM] = {0};
        double ls_x[LS_NUM] = {LS_X_0, LS_X_1,LS_X_2,LS_X_3,LS_X_4,LS_X_5,LS_X_6,LS_X_7,LS_X_8,LS_X_9,LS_X_10,LS_X_11,LS_X_12,LS_X_13,LS_X_14,LS_X_15, LS_X_16,LS_X_17,LS_X_18,LS_X_19,LS_X_20,LS_X_21,LS_X_22,LS_X_23,LS_X_24,LS_X_25,LS_X_26,LS_X_27,LS_X_28,LS_X_29,LS_X_30,LS_X_31};
        double ls_y[LS_NUM] = {LS_Y_0, LS_Y_1,LS_Y_2,LS_Y_3,LS_Y_4,LS_Y_5,LS_Y_6,LS_Y_7,LS_Y_8,LS_Y_9,LS_Y_10,LS_Y_11,LS_Y_12,LS_Y_13,LS_Y_14,LS_Y_15, LS_X_16, LS_X_17,LS_X_18,LS_X_19,LS_X_20,LS_X_21,LS_X_22,LS_X_23,LS_X_24,LS_X_25,LS_X_26,LS_X_27,LS_X_28,LS_X_29,LS_X_30,LS_X_31};
};

#endif