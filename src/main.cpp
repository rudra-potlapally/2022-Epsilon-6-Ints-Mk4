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
	// if (orient > 180){
	// 	orient = orient -360;
	// }
	
	return pid.update(orient, targetHeading);
}

int sideways_correct(float orient = tssps.ballDir, float targetHeading = 0) {
	if (orient > 180){
		orient -= 360;
	}
	return sidewayPid.update(orient, targetHeading);
}

// float centre_correct(float targetHeading = 180){
// 	float orient = camera.defendAngle;
// 	if (orient > 180){
// 		orient -= 360;
// 	}
// 	return sidewayPid.update(orient, targetHeading);
// }

int forwards_correct() {
	float targetDist = (ROBOT == 1 ? 53 : 28);
	int currentDist = camera.defendDist;
	if(ROBOT == 1 ? currentDist < 51 : currentDist < 26){
		currentDist = (ROBOT == 1 ? -10 : 10);
	} if (ROBOT == 1 ? currentDist > 55 : currentDist > 30){
		currentDist = (ROBOT == 1 ? 90 : 50);
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

Move attack(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, double lineAngle){
	Move movement;
	if (outDir != -1) {
		if (ballDir > floatMod(lineAngle+90, 360) && ballDir < floatMod(lineAngle-90, 360)){
			if (tssps.ballVisible) {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
				movement.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition()), 360)) + 360;
				movement.speed = orbitData.speed;
				// if (lineAngle < 180 && ballDir < 180){
				// 	if (camera.attackAngle < 180){
				// 		movement.dir = floatMod(lineAngle+90, 360);
				// 	}
				// 	if (camera.defendAngle < 180){
				// 		movement.dir = floatMod(lineAngle-90, 360);
				// 	}
				// } else if (lineAngle > 180 && ballDir > 180){
				// 	if (camera.attackAngle > 180){
				// 		movement.dir = floatMod(lineAngle-90, 360);
				// 	}
				// 	if (camera.defendAngle > 180){
				// 		movement.dir = floatMod(lineAngle+90, 360);
				// 	}
				// }
			}
		} else{
			movement.dir = outDir;
			movement.speed = outSpd;

			//LINE SLIDING
			// Serial.printf("AR1: %i, AR2: %i\n", abs(ballDir - lineAngle) > 10, abs(outDir - lineAngle) < 45);
		// 	if(abs(ballDir - lineAngle) > 10 && abs(outDir - lineAngle) < 45) {
		// 		if((ballDir <= 90 || ballDir >= 270) && ballVis) {
		// 			movement.dir = 0;
		// 			movement.speed = (50/90.f) * abs(ballDir - lineAngle);
		// 		} else {
		// 			movement.dir = 180;
		// 			movement.speed = (50/90.f) * abs(ballDir - lineAngle);
		// 		}
		// 	} else if(abs(ballDir - lineAngle) <= 10 && abs(outDir - lineAngle) < 45) {
		// 		movement.speed = 0;	
		// 	}
		}
	}
	else {
		if (tssps.ballVisible) {
			Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
			movement.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition()), 360)) + 360;
			movement.speed = orbitData.speed;
		}
		else {
			movement.dir = 0;
			movement.speed = 0;
		}
	}

	// if(kicker.kicked == true){
	// 	for (uint8_t i = 0; i < 20; i++){
	// 		movement.speed -= 10;
	// 	}
	// } else{
	// 	movement.speed = movement.speed;
	// }

	if (ballDir == -1){
		movement.dir = 1;
		movement.speed = 240;
	}
	return movement;
}

Move defend(double ballDir, double ballStr, bool ballVis, double outDir, double outSpd, bool defendVis, int defendDist, double lineAngle){
	Move move;
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}
	// float orient = -1* ((float)event.orientation.x) +360;
	if(outDir == -1){
		if(ballVis && defendVis){
			if (ballStr > DEFENSE_SURGE_STRENGTH){
					if (defendDist < (ROBOT == 1 ? 50 : 35)){
						move.dir = (-1* floatMod((tssps.ballDir+tssps.calculateAngleAddition()), 360)) + 360;;
						move.speed = DEFENSE_SUGRE_SPEED;
						move.rot = defend_correct();
					}
				} else {
					// if (abs(orient) < 75){
						move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct())));
						move.dir = floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360);
						move.rot = defend_correct();
					// }
				}
			} else{
				if (defendVis){
					move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct())));
					move.dir = floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360);
					move.rot = defend_correct();
				} else{
					// if (abs(orient) < 75){
						move.speed = sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct())));
						move.dir = floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360);
						move.rot = defend_correct();
					// }
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

		// move.rot = (defendDist < 40 ? defend_correct() : compass_correct(0, (float)event.orientation.x));k
	return move;
}

bool stt;
int loops;

void onStage(float heading){
	unsigned long currentTime = millis();
	if (stt){
		if (currentTime - startTime >= 15000){
			startTime = millis();
			stt = false;
		}
		else if (currentTime - startTime > 12000 && currentTime - startTime < 15000){
			motors.move(20, 0, compass_correct(180, (float)event.orientation.x));
		} else {
			motors.move(0, 0, compass_correct(180, (float)event.orientation.x));
		}
	} else{
		if (spin){
			if (currentTime - startTime < 923){
				motors.move(0, 0, 10*pow(-1, loops%2));
			} else{
				spin = false;
				startTime = millis();
				loops += 1;
			}
		} else{
			if(currentTime - startTime < 923*1.5){
				motors.move(20, 90, compass_correct(180, (float)event.orientation.x));
			} else if (currentTime - startTime < 923 * 3){
				motors.move(20, 270, compass_correct(180, (float)event.orientation.x ));
			} else{
				spin = true;
				startTime = millis();
			}
		}
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
	kicker.init();
	pinMode(FLINA, OUTPUT);
	pinMode(FLINB, OUTPUT);
	pinMode(FLPWM, OUTPUT);
	pinMode(SUPERTEAMPIN, INPUT);

	debug();
	stt = true;
	spin = false;
	startTime = millis();
	delay(8000);
}

void loop() {
	// ----- ONSTAGE -----//
	bnoCtr++;
		if(bnoCtr % 5 == 0) {
			bno.getEvent(&event);
	}	
	onStage((float)event.orientation.x);

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
	// if (SUPERTEAM != 1){
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
	// // } else{
	// // 	outDir = -1;
	// // 	outSpd = -1;
	// // 	line = -1;
	// // }
	// // // ----- MOTOR MOVING -----//
	// if (ROBOT == 2){
	// 	// kicker.update(camera.attackDist);
	// 	// ----- ATTACK -----//
	// 	Move att = attack(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, lineAngle);
	// 	if (SUPERTEAM == 1){
	// 		if (SUPERTEAMPIN == STSTOP){
	// 			motors.move(0, 0, compass_correct((float)event.orientation.x));
	// 			digitalWrite(13, HIGH);
	// 		} else{
	// 			motors.move(att.speed, att.dir, compass_correct(0, (float)event.orientation.x));
	// 			digitalWrite(13, LOW);
	// 		}
	// 	} else{
	// 		motors.move(att.speed, att.dir, compass_correct(0, (float)event.orientation.x));
	// 		digitalWrite(13, LOW);
	// 	}
	// 	// Serial.print(att.dir);
	// }
	// if (ROBOT == 2){
	// 	camera.update((blueAttack == 1) ? true : false);
	// 	// ----- DEFEND -----//
	// 	Move def = defend(tssps.ballDir, tssps.ballStr, tssps.ballVisible, outavoidance.direction, outavoidance.speed, camera.defendVis, camera.defendDist, lineAngle);
	// 	if (SUPERTEAM == 1){
	// 		if (SUPERTEAMPIN == STSTOP){
	// 			motors.move(0, 0, compass_correct((float)event.orientation.x));
	// 		} else{
	// 			motors.move(def.speed, def.dir, def.rot);	
	// 		}
	// 	} else{
	// 		motors.move(def.speed, def.dir, def.rot);
	// 	}
		// motors.move(forwards_correct(), 0, defend_correct());
		// motors.move(sqrt(((forwards_correct())*(forwards_correct()))+((sideways_correct())*(sideways_correct()))), floatMod(RAD_TO_DEG * atan2(sideways_correct(), forwards_correct()), 360), (camera.defendDist < 55 ? defend_correct() : compass_correct(0, (float)event.orientation.x)));
	// }

	// ----- TESTING -----//
	// OUTAVOIDANCE = OL : LINEANGLE : OUTAVOIDANCE DIRECTION
	// Serial.print(ol);
	// Serial.print("\t");
	// Serial.println(lineAngle);
	// Serial.print("\t");
	// Serial.println(outavoidance.direction);
	// lightsensor.test();

	// BALL DIRECTION = BALL DIR : BALL STR : ORBIT DIR
	// Serial.println(tssps.ballDir);
	// Serial.print("\t");
	// Serial.println(tssps.ballStr);
	// Serial.print("\t");
	// Serial.println(floatMod((-1*(tssps.ballDir+tssps.calculateAngleAddition()))+360,360));

	//CAMERA = ATTACK ANGLE : ATTACK DIST : DEFEND ANGLE : DEFEND DIST
	// Serial.print(camera.attackAngle);
	// Serial.print("\t");
	// Serial.print(camera.attackDist);
	// Serial.print("\t");
	// Serial.println(camera.defendAngle);
	// Serial.print("\t");
	// Serial.println(camera.defendDist);

	//LIGHTGATE = LIGHT GATE SIGNAL
	// Serial.println(analogRead(LG_SIG));
}
