#include <Arduino.h>
#include <math.h>
#include <IntervalTimer.h>
#include "defines.h"
#include "pins.h"
#include "vect.h"
#include "Adafruit_BNO055.h"
#include "Motor.h"
#include "movement.h"
#include "PID.h"
#include "LightSensor.h"
#include "lineavoidance.h"
#include "TSSPs.h"
#include "orbit.h"
#include "Camera.h"

Movement motors;
Adafruit_BNO055 bno = Adafruit_BNO055 (55, 0x29, &Wire);
PID pid = PID(COMPASS_P, COMPASS_I, COMPASS_D);
PID sidewayPid = PID(SIDEWAY_P, SIDEWAY_I, SIDEWAY_D, 130);
PID yupPid = PID(yup_P, yup_I, yup_D);
PID defendPid = PID(defend_P, defend_I, defend_D);
PID attackPid = PID(attack_P, attack_I, attack_D);
LightSensor lightsensor;
oAvoidance outAvoidance;
TSSPs tssps;
Orbit orbit;
Camera camera;
Vect vector;
float previousLine;
struct Move{
	int speed;
	int dir;
	int rot;
};

long long start;
int counter = 0;
sensors_event_t event;
int bnoCtr = 0;

int compass_correct(float targetHeading = 0, float orient = 0) {
	if (orient > 180){
		orient = orient -360;
	}
	
	return pid.update(orient, targetHeading);
}

int sideways_correct(float orient = tssps.ballDir, float targetHeading = 0) {
	if (orient > 180){
		orient -= 360;
	}
	return sidewayPid.update(orient, targetHeading);
}

float centre_correct(float targetHeading = 180){
	float orient = -1 * (camera.defendAngle) + 360;
	return sidewayPid.update(orient, targetHeading);
}

int forwards_correct() {
	float targetDist = (ROBOT == 1 ? 45 : 22);
	int currentDist = camera.defendDist;
	if(ROBOT == 1 ? currentDist < 43 : currentDist < 22){
		currentDist = (ROBOT == 1 ? -10 : -30);
	} if (ROBOT == 1 ? currentDist > 45 : currentDist > 26){
		currentDist = (ROBOT == 1 ? 90 : 80);
	}
	return yupPid.update(currentDist, targetDist);
}


float defend_correct(float targetHeading = 180) {
	int orient = camera.defendAngle;
	
	return defendPid.update(orient, targetHeading);
}

int attack_correct(float targetHeading = 0){
	int orient = camera.attackAngle;
	if (orient > 180){
		orient -= 360;
	}
	return attackPid.update(orient, targetHeading);
}

void ERROR(int pin){
	pinMode(pin, OUTPUT);
	while(1){
		digitalWrite(pin, HIGH);
		delay(300);
		digitalWrite(pin, LOW);
		delay(300);
	}
}

void debug(){
	if (bno.begin(bno.OPERATION_MODE_IMUPLUS)){
		LEDoff(COMPASS_DEBUG);
		delay(200);
		if (true){
			delay(200);
			LEDoff(LS_DEBUG);
			if (true){
				LEDoff(TSSP_DEBUG);
				delay(200);
				if(true){
					delay(200);
					LEDoff(BLUETOOTH_DEBUG);
				}
				else{
					ERROR(BLUETOOTH_DEBUG);
				}
			}
			else{
				ERROR(TSSP_DEBUG);
			}
		}
		else{
			ERROR(LS_DEBUG);
		}
	}
	else{
		ERROR(COMPASS_DEBUG);
	}
}

void setup() {
	LEDon(COMPASS_DEBUG);
	LEDon(LS_DEBUG);
	LEDon(BLUETOOTH_DEBUG);
	LEDon(TSSP_DEBUG);

	Serial.begin(9600);
	lightsensor.init();
	camera.init();
	pinMode(FLINA, OUTPUT);
	pinMode(FLINB, OUTPUT);
	pinMode(FLPWM, OUTPUT);
	pinMode(KICKER_SIG, OUTPUT);

	debug();
}

Move attack(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, double lineAngle){
	Move movement;
	if (lineAngle != -1) {
		if (ballDir > floatMod(lineAngle+100, 360) && ballDir < floatMod(lineAngle-100, 360)){
			if (tssps.ballVisible) {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.dir = floatMod((-1*(tssps.ballDir+tssps.calculateAngleAddition()))+360,360);
				movement.speed = orbitData.speed;
			}
		} else{
			movement.dir = outDir;
			movement.speed = outSpd;
		}
	}
	else {
		if (tssps.ballVisible) {
			Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
			movement.dir = floatMod((-1*(tssps.ballDir+tssps.calculateAngleAddition()))+360,360);
			movement.speed = orbitData.speed;
		}
		else {
			movement.dir = 0;
			movement.speed = 0;
		}
	}
	return movement;
}

Move defend(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, bool defendVis, int defendDist){
	Move move;
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}
	
	if(outDir == -1){
		if(ballVis && defendVis){
			if (ballStr > DEFENSE_SURGE_STRENGTH){
					if(defendDist < (ROBOT == 1 ? 50 : 40)){
						move.dir = tssps.ballDir; 
						move.speed = DEFENSE_SUGRE_SPEED;
						move.rot = compass_correct(0, (float)event.orientation.x);
					} else{
					move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct())));
					move.dir = floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360);
					move.rot = defend_correct();
				}
			} else {
				move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct())));
				move.dir = floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360);
				move.rot = defend_correct();
			}
		} else{
			move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((centre_correct())*(centre_correct())));
			move.dir = floatMod(RAD_TO_DEG * atan2(centre_correct(), forwards_correct()), 360);
			move.rot = compass_correct(0, (float)event.orientation.x);
		}
	} else{
		move.dir = outDir;
		move.speed = outSpd;
		move.rot = defend_correct();
	}

	return move;	
}

void loop() {
	// if(micros() - start > 1000000) {
	// 	Serial.printf("Freq %iHz\n", counter);
	// 	Serial.printf("Period: %fms\n", 1000/(float)counter);
	// 	start = micros();
	// 	counter = 0;
	// }
	// counter++;

	// ----- SETUP -----//
	
	// tssps.update();
	// bnoCtr++;
	// float ol = lightsensor.update();
	// if(bnoCtr % 5 == 0) {
	// 	bno.getEvent(&event);
	// }
	// float orient = -1* ((float)event.orientation.x) +360;
	// if (orient > 180){
	// 	orient = orient -360;
	// }
  	// float lineAngle = (ol != -1 ? floatMod(ol+orient, 360) : -1.00);
  	// oAvoidance::Movement outavoidance = outAvoidance.moveDirection(lineAngle);
  	// outavoidance.direction = (outavoidance.direction != -1 ? -1* (floatMod(outavoidance.direction+orient, 360)) + 360 : -1.00);	
	// camera.update(true);

	// Move att = attack(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, lineAngle);
	// motors.move(att.speed, att.dir, attack_correct());

	// Move def = defend(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, camera.defendVis, camera.defendDist);
	// motors.move(def.speed, def.dir, def.rot);

	// motors.move(outavoidance.speed, outavoidance.direction, compass_correct());

	digitalWrite(KICKER_SIG, LOW);
	delay(2000);
	digitalWrite(KICKER_SIG, HIGH);
	delay(50);

}