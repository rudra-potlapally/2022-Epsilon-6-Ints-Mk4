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
// PID sidewayPid = PID(SIDEWAY_P, SIDEWAY_I, SIDEWAY_D, 130);
// PID yupPid = PID(yup_P, yup_I, yup_D);
// PID defendPid = PID(defend_P, defend_I, defend_D);
// PID attackPid = PID(attack_P, attack_I, attack_D);
// LightSensor lightsensor;
// oAvoidance outAvoidance;
TSSPs tssps;
Orbit orbit;
// Camera camera;
// Kicker kicker;
// Bluetooth bluetooth;

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

void setup() {
	Serial.begin(9600);
	// lightsensor.init();
	// camera.init();
	// kicker.init();
	// bluetooth.init();

	if (!bno.begin(bno.OPERATION_MODE_IMUPLUS)){
		while(true){
			Serial.println("fuck");
		}
	}
}

void loop() {
	// ----- SENSOR INPUT -----//
	
	tssps.update();
	bnoCtr++;
	if(bnoCtr % 5 == 0) {
		bno.getEvent(&event);
	}

	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// float orient = -1* ((float)event.orientation.x) +360;
	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// if (orient > 180){
	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// 	orient = orient -360;
	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// }
  	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// float lineAngle = (ol != -1 ? floatMod(ol+orient, 360) : -1.00);
  	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// oAvoidance::Movement outavoidance = outAvoidance.moveDirection(lineAngle);
  	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// outavoidance.direction = (outavoidance.direction != -1 ? -1* (floatMod(outavoidance.direction-orient, 360)) + 360 : -1.00);
	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// camera.update(blueAttack == 1 ? true : false);
	// // /// // /// /// // / / / / / / / / / / / // / / / / / / / / / / / / // / / / / / / / / / / / / / / / / / / / / / / / / / / / / /// bluetooth.update(tssps.ballDir, tssps.ballStr);

	// // ----- MOTOR MOVING -----//

	analogWrite(BLINA, 150);
	digitalWrite(BLINB, LOW);
	analogWrite(BRINA, 150);
	digitalWrite(BRINB, LOW);
	analogWrite(FLINA, 150);
	digitalWrite(FLINB, LOW);
	analogWrite(FRINA, 150);
	digitalWrite(FRINB, LOW);
	// motors.move(100, 100, 100);
	// Serial.println(event.orientation.x);











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
