#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>
#include <math.h>
#include <EEPROM.h>



//--------- GENERAL ---------//

#define ROBOT 1

float floatMod(float x, float m);

float angleBetween(float angleCounterClockwise, float angleClockwise);

float smallestAngleBetween(float angleCounterClockwise, float angleClockwise);

int8_t findSign(float value);

float midAngleBetween(float angleCounterClockwise, float angleClockwise);

float smallestAngleBetween(float angleCounterClockwise, float angleClockwise);

int8_t findSign(float value);

bool angleIsInside(float angleBoundCounterClockwise, float angleBoundClockwise, float angleCheck);

void LEDon(int pin);
void LEDoff(int pin);

#define DEG2RAD (3.141592/180)
#define RAD2DEG (180/3.141592)

//--------- ORBIT ---------//
#define ORBIT_STRIKE_ANGLE 10
#define ORBIT_CLOSE_SPEED (ROBOT == 1 ? 30 : 90 )
#define ORBIT_FAR_SPEED (ROBOT == 1 ? 80 : 120 )
#define ORBIT_STRIKE_SPEED (ROBOT == 1 ? 100 : 140 )

//--------- COMPASS ---------//

#define COMPASS_P (ROBOT == 1 ? 0.62 : 0.62)
#define COMPASS_I 0.0
#define COMPASS_D (ROBOT == 1 ? 0.02 : 0.02)

#define SIDEWAY_P (ROBOT == 1 ? 1.0 : 0.5)
#define SIDEWAY_I 0.0
#define SIDEWAY_D (ROBOT == 1 ? 0.04 : 0.02)

#define yup_P (ROBOT == 1 ? 1.0 : 1.0)
#define yup_I 0.0
#define yup_D (ROBOT == 1 ? 0.08 : 0.08)

#define defend_P (ROBOT == 1 ? 0.5 : 0.5)
#define defend_I 0.0
#define defend_D (ROBOT == 1 ? 0.03 : 0.03)

#define attack_P (ROBOT == 1 ? 0.2 : 0.2)
#define attack_I 0.0
#define attack_D (ROBOT == 1 ? 0.02 : 0.02)

//----------LIGHT SENSORS ----------------//

#define LS_NUM 32
#define LS_NUM_IND 16
#define LINE_BUFFER (ROBOT == 1 ? 400 : 300)
#define LS_AVOID_MEDIUM (ROBOT == 1 ? 140  : 200 )
#define LS_AVOID_FAST (ROBOT == 1 ? 160  : 220)
#define LS_CALIBRATE_COUNT 10
#define LS_OFFSET 0
#define lineAngleOffset 90

#define LS_X_8   0
#define LS_X_7   0.195090322016
#define LS_X_6   0.382683432365
#define LS_X_5   0.55557023302
#define LS_X_4   0.707106781187
#define LS_X_3   0.831469612303
#define LS_X_2   0.923879532511
#define LS_X_1   0.980785280403
#define LS_X_0   1
#define LS_X_17  0.980785280403
#define LS_X_16  0.923879532511
#define LS_X_20  0.831469612303
#define LS_X_21  0.707106781187
#define LS_X_19  0.55557023302
#define LS_X_18  0.382683432365
#define LS_X_22  0.195090322016
#define LS_X_23  0
#define LS_X_31 -0.195090322016
#define LS_X_30 -0.382683432365
#define LS_X_29 -0.55557023302
#define LS_X_28 -0.707106781187
#define LS_X_24 -0.831469612303
#define LS_X_25 -0.923879532511
#define LS_X_26 -0.980785280403
#define LS_X_27 -1
#define LS_X_15 -0.980785280403
#define LS_X_14 -0.923879532511
#define LS_X_13 -0.831469612303
#define LS_X_12 -0.707106781187
#define LS_X_11 -0.55557023302
#define LS_X_10 -0.382683432365
#define LS_X_9  -0.195090322016

#define LS_Y_8   1
#define LS_Y_7   0.980785280403
#define LS_Y_6   0.923879532511
#define LS_Y_5   0.831469612303
#define LS_Y_4   0.707106781187
#define LS_Y_3   0.55557023302
#define LS_Y_2   0.382683432365
#define LS_Y_1   0.195090322016
#define LS_Y_0   0
#define LS_Y_17 -0.195090322016
#define LS_Y_16 -0.382683432365
#define LS_Y_20 -0.55557023302
#define LS_Y_21 -0.707106781187
#define LS_Y_19 -0.831469612303
#define LS_Y_18 -0.923879532511
#define LS_Y_22 -0.980785280403
#define LS_Y_23 -1
#define LS_Y_31 -0.980785280403
#define LS_Y_30 -0.923879532511
#define LS_Y_29 -0.831469612303
#define LS_Y_28 -0.707106781187
#define LS_Y_24 -0.55557023302
#define LS_Y_25 -0.382683432365
#define LS_Y_26 -0.195090322016
#define LS_Y_27  0
#define LS_Y_15  0.195090322016
#define LS_Y_14  0.382683432365
#define LS_Y_13  0.55557023302
#define LS_Y_12  0.707106781187
#define LS_Y_11  0.831469612303
#define LS_Y_10  0.923879532511
#define LS_Y_9   0.980785280403

// --- TSSPS --- //

#define TSSP_X_0  -1
#define TSSP_X_1  -0.923879532511
#define TSSP_X_2  -0.707106781187
#define TSSP_X_3  -0.382683432365
#define TSSP_X_4   1
#define TSSP_X_5   0.923879532511
#define TSSP_X_6   0.70710678118
#define TSSP_X_7   0.382683432365
#define TSSP_X_8   0
#define TSSP_X_9  -0.382683432365
#define TSSP_X_10 -0.707106781187
#define TSSP_X_11 -0.923879532511
#define TSSP_X_12  0
#define TSSP_X_13  0.382683432365
#define TSSP_X_14  0.707106781187
#define TSSP_X_15  0.923879532511

#define TSSP_Y_0   0
#define TSSP_Y_1  -0.382683432365
#define TSSP_Y_2  -0.707106781187
#define TSSP_Y_3  -0.923879532511
#define TSSP_Y_4   0
#define TSSP_Y_5   0.382683432365
#define TSSP_Y_6   0.707106781187
#define TSSP_Y_7   0.923879532511
#define TSSP_Y_8   1
#define TSSP_Y_9   0.923879532511
#define TSSP_Y_10  0.707106781187
#define TSSP_Y_11  0.382683432365
#define TSSP_Y_12 -1
#define TSSP_Y_13 -0.923879532511
#define TSSP_Y_14 -0.707106781187
#define TSSP_Y_15 -0.382683432365

#define TSSP_NUM 16
#define TSSP_READ_NUM 256
#define BALL_CLOSE_STRENGTH (ROBOT == 1 ? 160 : 155)
#define BALL_CLOSE_STRENGTH_LEFT (ROBOT == 1 ? 185 : 185)
#define SURGE_ANGLE 10
#define SURGE_SPEED 80
#define TSSP_OFFSET (ROBOT == 1 ? 13 : 10)

#define ARRAYSHIFTDOWN(a, lower, upper){          \
	if (upper == (sizeof(a)/sizeof(a[0])) - 1){   \
		for (int q = upper - 1; q >= lower; q--){ \
			*(a + q + 1) = *(a + q); }            \
	} else{                                       \
		for (int q = upper; q >= lower; q--){     \
			*(a + q + 1) = *(a + q); }}}

// --- DEBUG --- //

#define TSSP_DEBUG 17
#define BLUETOOTH_DEBUG 16
#define COMPASS_DEBUG 15
#define LS_DEBUG 13

// --- KICKER --- //

#define LDR_OFSET 80
#define KICKER_SIG 14
#define LG_SIG 49
#define KICK_DISCHARGE_TIME 150000 //ms
#define KICK_CHARGE_TIME 1250000*2
// --- CAMERA --- //

#define CAMERA_BAUD 15200
#define defendGoalFar 40
#define defendGoalClose 10
#define DEFENSE_SURGE_ANGLE 10
#define DEFENSE_SPEED 40
#define SWITCH_SPEED 40 
#define DEFENSE_SUGRE_SPEED (ROBOT == 1 ? 100  : 150)
#define DEFENSE_SURGE_STRENGTH (ROBOT == 1 ? 135 : 145)

// --- BLUETOOTH --- //

#define BT_BAUD 9600
#define BT_DISCONNECTED_TIME 1000000
#define BT_CONNECTED_TIMER 1000
#define BT_UPDATE_TIME 100000
#define BT_START_BYTE 255
#define BT_PACKET_SIZE 6
#define BT_SEND_TIMER 10
#define BT_SWITCH_TIMER 4000
#define SWITCH_STRENGTH_CONST 40
#define DEFENSE_MODE 0
#define ATTACK_MODE 1

#endif