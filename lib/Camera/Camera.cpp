#include "Camera.h"

Camera::Camera() {}

void Camera::init() {
    Serial6.begin(CAMERA_BAUD);
}

void Camera::update(bool attackBlue) {
    if(Serial6.available() >= 6) {
        uint8_t firstByte = Serial6.read();
        uint8_t secondByte = Serial6.peek();
        if(firstByte == 255 && secondByte == 255) {
            int yellowX = Serial6.read() - CENTRE_X;
            int yellowY = Serial6.read() - CENTRE_Y;
            int blueX = Serial6.read() - CENTRE_X;
            int blueY = Serial6.read() - CENTRE_Y;
            attackAngle = (attackBlue ? floatMod(RAD_TO_DEG * atan2f(blueY, blueX), 360.0) : floatMod(RAD_TO_DEG * atan2f(yellowY, yellowX), 360.0));
            defendAngle = (attackBlue ? floatMod(RAD_TO_DEG * atan2f(yellowY, yellowX), 360.0) : floatMod(RAD_TO_DEG * atan2f(blueY, blueX), 360.0));
            attackDist = (attackBlue ? sqrt(((blueX)^2)+((blueY)^2)) : sqrt(((yellowX)^2)+((yellowY)^2)));
            defendDist = (attackBlue ? sqrt(((yellowX)^2)+((yellowY)^2)) : sqrt(((blueX)^2)+((blueY)^2)));
        }
    } else{
        Serial.println("no serial");
    }
}

float Camera::ballPixeltoCM(float dist){
    return (dist != 0) ? 0.0195649 * expf(0.0634054 * (dist + 30.1281)) + 21.2566 : 0;
}

float Camera::goalPixeltoCM(float dist){
    return (dist != 0) ? 5.7478f * powf(10.0f, -13.0f) * expf(0.0494379*(dist + 552.825f)) + 13.8327f : 0;
}