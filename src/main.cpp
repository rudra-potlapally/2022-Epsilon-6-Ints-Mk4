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
#include "kicker.h"
#include "MotorMusic.h"
#include "time.h"
#include "Bluetooth.h"

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
Kicker kicker;
Bluetooth bluetooth;

struct Move{
	int speed;
	int dir;
	int rot;
};

long long start;
int counter = 0;
sensors_event_t event;
int bnoCtr = 0;
unsigned long startTime;
bool spin;

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
	float orient = camera.defendAngle;
	return sidewayPid.update(orient, targetHeading);
}

int forwards_correct() {
	float targetDist = (ROBOT == 1 ? 15 : 32);
	int currentDist = camera.defendDist;
	if(ROBOT == 1 ? currentDist < 10 : currentDist < 27){
		currentDist = (ROBOT == 1 ? 0 : 0);
	} if (ROBOT == 1 ? currentDist > 20 : currentDist > 37){
		currentDist = (ROBOT == 1 ? 35 : 50);
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
		delay(500);
		if (true){
			delay(500);
			LEDoff(LS_DEBUG);
			if (true){
				LEDoff(TSSP_DEBUG);
				delay(500);
				if(true){
					delay(500);
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

Move attack(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, double lineAngle){
	Move movement;
	if (outDir != -1) {
		if (ballDir > floatMod(lineAngle+90, 360) && ballDir < floatMod(lineAngle-90, 360)){
			if (tssps.ballVisible) {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition(bluetooth.thisData.role)), 360)) + 360;
				movement.speed = orbitData.speed;
			}
		} else{
			if (outAvoidance.botlocation > 0 && (lineAngle < 25 && lineAngle > 335) && (lineAngle < 115 && lineAngle > 55) && (lineAngle < 205 && lineAngle > 155) && (lineAngle < 295 && lineAngle > 245)){
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition(bluetooth.thisData.role)), 360)) + 360;
				movement.speed = orbitData.speed;
			} else{
				movement.dir = outDir;
				movement.speed = outSpd;
			}
			// LINE SLIDING
			// Serial.printf("AR1: %i, AR2: %i\n", abs(ballDir - lineAngle) > 10, abs(outDir - lineAngle) < 45);

			if(ballStr != 0){
				if(abs(ballDir - lineAngle) > 10 && abs(outDir - lineAngle) < 45) {
					if((ballDir <= 90 || ballDir >= 270) && ballVis) {
						movement.dir = 0;
						movement.speed = (50/90.f) * abs(ballDir - lineAngle);
					} else {
						movement.dir = 180;
						movement.speed = (50/90.f) * abs(ballDir - lineAngle);
					}
				} else if(abs(ballDir - lineAngle) <= 10 && abs(outDir - lineAngle) < 45) {
					movement.speed = 0;	
				}
			} else{
				movement.dir = outDir;
				movement.speed = outSpd;
			}
		}
	}
	else {
		if (tssps.ballVisible) {
			Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
			movement.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition(bluetooth.thisData.role)), 360)) + 360;
			movement.speed = orbitData.speed;
		}
		else {
			movement.dir = 0;
			movement.speed = 0;
		}
	}
	return movement;
}

Move defend(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, bool defendVis, int defendDist, double lineAngle){
	Move move;
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}
	bool surge = false;
	if (ballStr > DEFENSE_SURGE_STRENGTH){
		surge = true;
	}
	if (surge && defendDist > (ROBOT != 1 ? 45 : 30)){
		surge = false;
	}

	int fwdc = forwards_correct();
	int swdc = sideways_correct();

	if(defendDist < 40){
		if(outDir == -1){
			if(ballVis && defendVis){
				if (surge){
						move.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition(bluetooth.thisData.role)), 360)) + 360;
						move.speed = DEFENSE_SUGRE_SPEED;
						move.rot = defend_correct();
					}
				else {
					// if (ballDir < 165 && ballDir > 195){
					move.speed = sqrt(((fwdc)*(fwdc))+((swdc)*(swdc)));
					move.dir = floatMod(RAD_TO_DEG * atan2(swdc, fwdc), 360);
					move.rot = defend_correct();
					// } else{
					// 	move.speed = 0;
					// }
				}
			} else{
				if (defendVis){
					move.speed = sqrt(((fwdc)*(fwdc))+((swdc)*(swdc)));
					move.dir = floatMod(RAD_TO_DEG * atan2(swdc, fwdc), 360);
					move.rot = defend_correct();
				} 
			}
		} else{
			move.dir = outDir;
			move.speed = outSpd;

			//LINE SLIDING
			// Serial.printf("AR1: %i, AR2: %i\n", abs(ballDir - lineAngle) > 10, abs(outDir - lineAngle) < 45);
			// if(abs(ballDir - lineAngle) > 10 && abs(outDir - lineAngle) < 45) {
			// 	if((ballDir <= 90 || ballDir >= 270) && ballVis) {
			// 		move.dir = 0;
			// 		move.speed = (50/90.f) * abs(ballDir - lineAngle);
			// 	} else {
			// 		move.dir = 180;
			// 		move.speed = (50/90.f) * abs(ballDir - lineAngle);
			// 	}
			// } else if(abs(ballDir - lineAngle) <= 10 && abs(outDir - lineAngle) < 45) {
			// 	move.speed = 0;	
			// }
			move.rot = defend_correct();
		}
	}
	 else{
		move.speed = sqrt(((fwdc)*(fwdc))+((swdc)*(swdc)));
		move.dir = floatMod(RAD_TO_DEG * atan2(swdc, fwdc), 360);
		move.rot = defend_correct();
	}
	return move;
}

// bool stt;
// int loops;

// void onStage(float heading){
// 	unsigned long currentTime = millis();
// 	if (stt){
// 		if (currentTime - startTime >= 15000){
// 			startTime = millis();
// 			stt = false;
// 		}
// 		else if (currentTime - startTime > 12000 && currentTime - startTime < 15000){
// 			motors.move(20, 0, compass_correct(180, (float)event.orientation.x));
// 		} else {
// 			motors.move(0, 0, compass_correct(180, (float)event.orientation.x));
// 		}
// 	} else{
// 		if (spin){
// 			if (currentTime - startTime < 923){
// 				motors.move(0, 0, 10*pow(-1, loops%2));
// 			} else{
// 				spin = false;
// 				startTime = millis();
// 				loops += 1;
// 			}
// 		} else{
// 			if(currentTime - startTime < 923*1.5){
// 				motors.move(20, 90, compass_correct(180, (float)event.orientation.x));
// 			} else if (currentTime - startTime < 923 * 3){
// 				motors.move(20, 270, compass_correct(180, (float)event.orientation.x ));
// 			} else{
// 				spin = true;
// 				startTime = millis();
// 			}
// 		}
// 	}
// }

void setup() {
	LEDon(COMPASS_DEBUG);
	LEDon(LS_DEBUG);
	LEDon(BLUETOOTH_DEBUG);
	LEDon(TSSP_DEBUG);

	Serial.begin(9600);
	lightsensor.init();
	camera.init();
	kicker.init();
	bluetooth.init();
	pinMode(FLINA, OUTPUT);
	pinMode(FLINB, OUTPUT);
	pinMode(FLPWM, OUTPUT);
	pinMode(SUPERTEAMPIN, INPUT);

	debug();
	delay(500);
}

void loop() {
	// ----- ONSTAGE -----//
	// bnoCtr++;
	// 	if(bnoCtr % 5 == 0) {
	// 		bno.getEvent(&event);
	// }	
	// onStage((float)event.orientation.x);

	// Serial.println(compass_correct(0, (float)event.orientation.x));
	// motors.move(0, 0, compass_correct(0, event.orientation.x));

	// ----- TIMER -----//

	// if(micros() - start > 1000000) {
	// 	Serial.printf("Freq %iHz\n", counter);
	// 	Serial.printf("Period: %fms\n", 1000/(float)counter);
	// 	start = micros();
	// 	counter = 0;
	// }
	// counter++;
	// float outDir, outSpd, line;

	// ----- SENSOR INPUT -----//
	
	// tssps.update();
	// bnoCtr++;
	// if(bnoCtr % 5 == 0) {
	// 	bno.getEvent(&event);
	// }
	// float ol = lightsensor.update();
	// float orient = -1* ((float)event.orientation.x) +360;
	// if (orient > 180){
	// 	orient = orient -360;
	// }
  	// float lineAngle = (ol != -1 ? floatMod(ol+orient, 360) : -1.00);
  	// oAvoidance::Movement outavoidance = outAvoidance.moveDirection(lineAngle);
  	// outavoidance.direction = (outavoidance.direction != -1 ? -1* (floatMod(outavoidance.direction-orient, 360)) + 360 : -1.00);
	// camera.update(blueAttack == 1 ? true : false);
	// bluetooth.update(tssps.ballDir, tssps.ballStr);

	// // ----- MOTOR MOVING -----//
	// if (bluetooth.thisData.role == ATTACK_MODE){
	// 	// ----- ATTACK -----//
	// 	Move att = attack(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, lineAngle);
	// 	motors.move(att.speed, att.dir, compass_correct(0, event.orientation.x));
	// 	// Serial.println(att.dir);
	// }
	// else{
	// 	// ----- DEFEND -----//
	// 	Move def = defend(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, camera.defendVis, camera.defendDist, lineAngle);
	// 	motors.move(def.speed, def.dir, defend_correct());
	// 	// motors.move(forwards_correct(), 0, defend_correct());
	// 	// Serial.println(defend_correct());
	// 	// motors.move(sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct()))), floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360), (camera.defendDist < 55 ? defend_correct() : compass_correct(0, (float)event.orientation.x)));
	// } 

	// ----- TESTING -----//

	// OUTAVOIDANCE = OL : LINEANGLE : OUTAVOIDANCE DIRECTION
	// Serial.println(ol);
	// Serial.print("\t");
	// Serial.println(lineAngle);
	// Serial.print("\t");
	// Serial.println(outavoidance.direction);
	// lightsensor.test();

	// BALL DIRECTION = BALL DIR : BALL STR : ORBIT DIR
	// Serial.print(tssps.ballDir);
	// Serial.print("\t");
	// Serial.println(tssps.ballStr);
	// Serial.print("\t");
	// Serial.println(floatMod((-1*(tssps.ballDir+tssps.calculateAngleAddition()))+360,360));

	// CAMERA = ATTACK ANGLE : ATTACK DIST : DEFEND ANGLE : DEFEND DIST
	// Serial.println(camera.defendAngle);
	// Serial.print("\t");
	// Serial.println(camera.defendDist);
	// Serial.print("\t");

	// LIGHTGATE = LIGHT GATE SIGNAL
	// Serial.println(analogRead(LG_SIG));

	// BLUETOOTH
	// Serial.print(bluetooth.otherData.ballStr);
	// Serial.print("\t");
	// Serial.print(bluetooth.otherData.ballDir);
	// Serial.print("\t");
	// Serial.print(bluetooth.otherData.role);
	// Serial.print("\t");
	// Serial.print(bluetooth.thisData.ballDir);
	// Serial.print("\t");
	// Serial.print(bluetooth.thisData.ballStr);
	// Serial.print("\t");
	// Serial.print(bluetooth.thisData.role);
	// Serial.print("\t");
	// Serial.println(bluetooth.isConnected);

	// 

	digitalWrite(23, HIGH);
	digitalWrite(22, LOW);
	digitalWrite(4, HIGH);
	digitalWrite(3, LOW);


	digitalWrite(23, LOW);
	digitalWrite(22, HIGH);
	digitalWrite(23, LOW);
	digitalWrite(22, HIGH);
}
