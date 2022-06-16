#include "TSSPs.h"

TSSPs::TSSPs() {
    for(uint8_t i = 0; i < TSSP_NUM; i++) {
		pinMode(tsspPins[i], INPUT);
	}
}

void TSSPs::update() {
	for(int i = tsspCounter; i < TSSP_READ_NUM; i++) {
		for(int i = 0; i < TSSP_NUM; i++) {
		    tsspValues[i] += 1 - digitalRead(tsspPins[i]);
	    }
	    tsspCounter++;
	}
	
    for(int i = 0; i < TSSP_NUM; i++) {
		for(int j = 0; j < TSSP_NUM; j++) {
			if(tsspValues[i] > tsspSortedValues[j]) {
				if(j <= i) {
					ARRAYSHIFTDOWN(tsspSortedValues, j, i);
					ARRAYSHIFTDOWN(tsspIndexes, j, i);
				}
				tsspSortedValues[j] = tsspValues[i];
				tsspIndexes[j] = i;
				break;
			}
		}
	}

    ballStr = ((3 * tsspSortedValues[0]) + (2 * tsspSortedValues[1]) + tsspSortedValues[2] + tsspSortedValues[3]) / 7;
	ballVisible = (ballStr != 0);
	float x = ((tsspSortedValues[0] * tsspXValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspXValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspXValues[tsspIndexes[2]]) + (tsspSortedValues[3] * tsspXValues[tsspIndexes[3]])) / 4;
	float y = ((tsspSortedValues[0] * tsspYValues[tsspIndexes[0]]) + (tsspSortedValues[1] * tsspYValues[tsspIndexes[1]]) + (tsspSortedValues[2] * tsspYValues[tsspIndexes[2]]) + (tsspSortedValues[3] * tsspYValues[tsspIndexes[3]])) / 4;
	float bD = (ROBOT == 1 ? (ballVisible ? 360 - floatMod((RAD2DEG * (atan2f(y, x)))-90, 360) : -1) : (ballVisible ? 360 - floatMod((RAD2DEG * (atan2f(y, x)))-90, 360) : -1));
	ballDir = (bD != -1 ? floatMod(((-1 * bD) + 360+TSSP_OFFSET), 360) : -1);
	// if (260 < ballDir && ballDir < 280){
	// 	ballDir = 270;
	// }
	// if (60 < ballDir && ballDir < 100){
	// 	ballDir = 90;
	// }
	tsspCounter = 0;
	for(int i = 0; i < TSSP_NUM; i++) {
		tsspValues[i] = 0;
		tsspSortedValues[i] = 0;
		tsspIndexes[i] = 0;
	}

}

float TSSPs::calculateAngleAddition() {
	float dir = ballDir > 180 ? ballDir - 360 : ballDir;
    float ballAngleDifference = findSign(dir) * fmin(90, 0.4 * expf(0.22 * smallestAngleBetween(ballDir, 0)));
    float strengthFactor = constrain(ballStr / BALL_CLOSE_STRENGTH, 0, 1);
    float distanceMultiplier = constrain((0.02 * strengthFactor * expf(4.5 * strengthFactor)), 0, 1);
	angleAddition = ballAngleDifference * distanceMultiplier;
	return angleAddition;
}