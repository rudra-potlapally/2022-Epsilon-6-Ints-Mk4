#ifndef MOTOR_H
#define MOTOR_H

#include "pins.h"
#include "defines.h"
#include "Arduino.h"

class Motor {
	public:
		Motor(){};
		Motor(int ina, int inb);
		void move(float speed);
		void brake();
	private:
		int inaPin, inbPin;
		bool reversed;	
};

#endif