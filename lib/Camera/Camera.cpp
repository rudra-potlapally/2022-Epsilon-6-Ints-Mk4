#include "Camera.h"

Camera::Camera() {}

void Camera::init() {
    Serial6.begin(CAMERA_BAUD);
}

void Camera::update(bool attackBlue) {
    if(Serial6.available() >= 7) {
        uint8_t firstByte = Serial6.read();
        uint8_t secondByte = Serial6.peek();
        if(firstByte == 255 && secondByte == 255) {
            Serial6.read();
            int yellowX = Serial6.read();
            int yellowY = Serial6.read();
            yellowAngle = floatMod((RAD2DEG * atan2(120-yellowY-60, 120-yellowX-60)), 360);
            if(yellowAngle < 0){
                yellowAngle += 360;
            }
            yellowDist = sqrt(((120-yellowX-60)*(120-yellowX-60))+((120-yellowY-60)*(120-yellowY-60)));
            int blueX = Serial6.read();
            int blueY = Serial6.read();
            blueAngle = RAD2DEG * atan2(120-blueY-60, 120-blueX-60);
            if(blueAngle < 0){
                blueAngle += 360;
            }
            blueDist = sqrt(((120-blueX-60)*(120-blueX-60))+((120-blueY-60)*(120-blueY-60)));

            attackVis = (attackBlue ? (blueAngle != 225 ? true : false) : (yellowAngle != 225 ? true : false));
            defendVis = (attackBlue ? (yellowAngle != 225 ? true : false) : (blueAngle != 225 ? true : false));
            
            if(yellowAngle != 225){
                prevAngY = yellowAngle;
            } else{
                yellowAngle = prevAngY;
            }
            // if(yellowDist < 60){
            //     prevDistY = yellowDist;
            // } else{
            //     yellowDist = prevDistY;
            // }
            if(blueAngle != 225){
                prevAngB = blueAngle;
            } else{
                blueAngle = prevAngB;
            }
            // if(blueDist < 60){
            //     prevDistB = blueDist;
            // } else{
            //     blueDist = prevDistB;
            // }

            attackAngle = (attackBlue ? blueAngle : yellowAngle);
            attackDist = (attackBlue ? blueDist : yellowDist);
            defendAngle = (attackBlue ? yellowAngle : yellowDist);
            defendDist = (attackBlue ? yellowDist : blueDist);
        }
    }
    // delay(10);
}

float Camera::ballPixeltoCM(float dist){
    return (dist != 0) ? 0.0195649 * expf(0.0634054 * (dist + 30.1281)) + 21.2566 : 0;
}

float Camera::goalPixeltoCM(float dist){
    return (dist != 0) ? 5.7478f * powf(10.0f, -13.0f) * expf(0.0494379*(dist + 552.825f)) + 13.8327f : 0;
}