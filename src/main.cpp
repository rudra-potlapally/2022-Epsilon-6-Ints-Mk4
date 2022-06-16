#include <Arduino.h>
#include <math.h>
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
LightSensor lightsensor;
oAvoidance outAvoidance;
TSSPs tssps;
Orbit orbit;
Camera camera;
Vect vector;

int compass_correct(float targetHeading = 0) {
	sensors_event_t event;
	bno.getEvent(&event);
	float orient = (float)event.orientation.x;
	if (orient > 180){
		orient = orient -360;
	}
	if (targetHeading > 180) {
		targetHeading = targetHeading - 360;
	}
	if (targetHeading >= 25 && targetHeading <= 335){
		targetHeading = 0;
  }
	
	return pid.update(orient, 1.25*targetHeading);
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
		if (!lightsensor.broke){
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

	debug();
}

void loop() {
	// ----- SETUP -----//
	tssps.update();
  	float ol = lightsensor.update();
  	sensors_event_t event;
	bno.getEvent(&event);
	float orient = -1* ((float)event.orientation.x) +360;
	if (orient > 180){
		orient = orient -360;
	}
  	float lineAngle = (ol != -1 ? floatMod(ol+orient, 360) : -1.00);
  	oAvoidance::Movement outavoidance = outAvoidance.moveDirection(lineAngle);
  	outavoidance.direction = (outavoidance.direction != -1 ? -1* (floatMod(outavoidance.direction-orient, 360)) + 360 : -1.00);

  	// //----- MOVEMENT -----//
	if (outavoidance.direction != -1) {
		if (tssps.ballDir > floatMod(lineAngle+LINE_BUFFER, 360) && tssps.ballDir < floatMod(lineAngle-LINE_BUFFER, 360) && tssps.ballVisible){
			if (tssps.ballVisible) {
				Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
        		orbitData.angle = floatMod(-1* (tssps.ballDir + tssps.calculateAngleAddition())+360, 360);
				motors.move(orbitData.speed,orbitData.angle,compass_correct());
			}
		} else{
			motors.move(outavoidance.speed,outavoidance.direction,compass_correct());
		}
	} else {
		if (tssps.ballVisible) {
			Orbit::OrbitData orbitData = orbit.update(tssps.ballDir, tssps.ballStr);
      		orbitData.angle = floatMod(-1* (tssps.ballDir + tssps.calculateAngleAddition())+360, 360);
			motors.move(orbitData.speed,orbitData.angle,compass_correct());
		}
		else {
			motors.move(0,0,compass_correct());
		}
	}
}