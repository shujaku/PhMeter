#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-05-28 20:10:42

#include "Arduino.h"
#include <Arduino.h>
#define SensorPin A2
#define Offset 0.00
#define LED 13
#define samplingInterval 20000
#define printInterval 800
#define ArrayLenth  40
#define PumpInterval  40
#define PUMP 4
#include <Wire.h>
#include "src/PumpeStateMashine.h"
#include <TimerOne.h>
void setup(void) ;
void printSerial(float voltage, float pHValue, int dosierStarts, 		PumpState pumpeMode) ;
double avergearray(int number, volatile int* arr) ;
void samplingIsr();
void loop(void) ;

#include "PhMeter.ino"


#endif
