//Copyright (C) 1997-2011 Julián U. da Silva Gillig - 
//This file is  is distributed under the RobotGroup-Multiplo Pacifist License (RMPL), either version 1.0 of the licence, 
//or (at your option) any later version. 

//##Aux functions for the code generated with Minibloq:

#include "DCMotor.h"
#include "IRRanger.h"
//#include "Ping.h"
//#include "pitches.h"
//#include "Servo.h"

//##Maple compatibility:
#define uint8_t uint8
#define serial0 SerialUSB
#define serial1 Serial1
#define serial2 Serial2
#define serial3 Serial3

//##Apparently not supported yet by Maple (see this: http://forums.leaflabs.com/topic.php?id=715#post-4122):
//#define BuzzerPin 8

//Always cast (at least by now):
#define MOD(term1, term2) ( ((int)term1) % ((int)term2) )

#define timeStamp() millis()

#define M_E		2.7182818284590452354
#define M_PI	3.14159265358979323846

//IRrecv irReceiver(2);

DCMotor motor0(3, 4, 5);
DCMotor motor1(11, 10, 9);
//PingSensor ping(0);
IRRanger irRanger20to150(1, IRRanger::range20to150cm);
IRRanger irRanger10to80(1, IRRanger::range10to80cm);


//Servos:
//Servo servo0;
//Servo servo1;
//Servo servo2;

//Defined this way to maintain pin-out compatibility with other Minibloq targets, such as all the boards 
//with standard Arduino-shield expansion footprints:
#define sensor0  10
#define sensor1  11
#define sensor2  12
#define sensor3  13
#define sensor4  14
#define sensor5  15


//##PWM pins:
#define PWM0   0
#define PWM1   1
#define PWM2   2
#define PWM3   3
#define PWM5   5
#define PWM6   6
#define PWM7   7
#define PWM8   8
#define PWM9   9
#define PWM11  11
#define PWM12  12
#define PWM14  14
#define PWM24  24
#define PWM27  27
#define PWM28  28

//##Digital pins:
#define D0  0
#define D1  1
#define D2  2
#define D3  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7
#define D8  8
#define D9  9
#define D10 10
#define D11 11
#define D12 12
#define D13_LED 13
#define D14 14
#define D15 15
#define D16 16
#define D17 17
#define D18 18
#define D19 19
#define D21 21
#define D22 22
#define D23 23
#define D24 24
#define D25 25
#define D26 26
#define D27 27
#define D28 28
#define D29 29
#define D30 30
#define D31 31
#define D32 32
#define D33 33
#define D34 34
#define D35 35
#define D36 36
#define D37 37
#define D38 38
#define D39 39
#define D40 40
#define D41 41
#define D42 42


inline uint8_t boolToPinLevel(bool value)
{
    if (value)
        return HIGH;
    return LOW;
}


inline bool pinLevelToBool(uint8_t value)
{
    //Only returns true if value is HIGH, otherwise, returns false:
    return (value == HIGH);
}


inline bool XOR(bool term1, bool term2) { return term1 != term2; }


//##The result will be a float between 0 and 100, but with higher resolution than the resolution reached 
//using random(min, max):
inline float Random()
{
    return (random(0, 10000)*0.01);
}


//##
// inline void toneWithDelay(uint8_t _pin, unsigned int frequency, unsigned long duration)
// {
    // tone(_pin, frequency, duration);
    // delay(duration * 1.30);
// }


inline uint8_t DigitalRead(uint8_t _pin)
{
    pinMode(_pin, INPUT);
    return pinLevelToBool(digitalRead(_pin));
}


inline void DigitalWrite(uint8_t _pin, bool value)
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, boolToPinLevel(value));
}

inline float AnalogRead(uint8_t _pin)
{
    //##Optimization: This can be precomputed:
    return (((float)analogRead(_pin))*100.0)/1023.0;
}


// inline float PulseIn(uint8_t _pin, bool value, float timeOut)
// {
	// pinMode(_pin, INPUT);
	// return pulseIn(_pin, boolToPinLevel(value), (unsigned long) timeOut);
// }


inline void AnalogWrite(uint8_t _pin, float value)
{
    float tempValue = constrain(value, 0, 100);
    tempValue = tempValue*2.55; // =tempValue*255/100;
    pinMode(_pin, PWM);
    analogWrite(_pin, (int)tempValue);
}


void initBoard()
{
    pinMode(14, INPUT_ANALOG);
    pinMode(15, INPUT_ANALOG);
    pinMode(16, INPUT_ANALOG);
    pinMode(17, INPUT_ANALOG);
    pinMode(18, INPUT_ANALOG);
    pinMode(19, INPUT_ANALOG);

    // ##Uses the analog input 0, that may have the same value in some designs, but it's not so prabably:
    randomSeed(analogRead(0));

   // irReceiver.enableIRIn();
   
	//servo0.setPin(6); //This does not attach the servo (so the pin can be used by other library if the servo is not used).
	//servo1.setPin(7);
	//servo2.setPin(8);   
}
