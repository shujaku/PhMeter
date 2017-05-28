#include <Arduino.h>
#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 2
#define Offset 0.00            //deviation compensate
#define LED 13
#define samplingInterval 20000
#define printInterval 800
#define ArrayLenth  40    //times of collection
#define PumpInterval  40
#define PUMP 4

#include <Wire.h>
#include "src/PumpeStateMashine.h"
#include <TimerOne.h>

volatile int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
volatile int pHArrayIndex = 0;
volatile bool isNewData = false;

void setup(void) {
	pinMode(LED, OUTPUT);
	pinMode(PUMP, OUTPUT);
	Serial.begin(9600);
	Serial.println("pH meter experiment!");    //Test the serial monitor
	digitalWrite(PUMP, HIGH);
	Timer1.initialize(samplingInterval);
}

void printSerial(float voltage, float pHValue, int dosierStarts,
		PumpState pumpeMode) {
	Serial.print("Voltage:");
	Serial.print(voltage, 2);
	Serial.print("    pH value: ");
	Serial.println(pHValue, 2);
	Serial.print("       dosierStarts ");
	Serial.println(dosierStarts);
	Serial.print("       pumpeMode ");
	Serial.println(pumpeMode);
	digitalWrite(LED, digitalRead(LED) ^ 1);
}

double avergearray(int number, volatile int* arr) {
	int i;
	int max, min;
	double avg;
	long amount = 0;
	if (number <= 0) {
		Serial.println("Error number for the array to avraging!/n");
		return 0;
	}
	if (number < 5) {   //less than 5, calculated directly statistics
		for (i = 0; i < number; i++) {
			amount += arr[i];
		}
		avg = amount / number;
		return avg;
	} else {
		if (arr[0] < arr[1]) {
			min = arr[0];
			max = arr[1];
		} else {
			min = arr[1];
			max = arr[0];
		}
		for (i = 2; i < number; i++) {
			if (arr[i] < min) {
				amount += min;        //arr<min
				min = arr[i];
			} else {
				if (arr[i] > max) {
					amount += max;    //arr>max
					max = arr[i];
				} else {
					amount += arr[i]; //min<=arr<=max
				}
			} //if
		} //for
		avg = (double) amount / (number - 2);
	} //if
	return avg;
}

void samplingIsr(){
	pHArray[pHArrayIndex++] = analogRead(SensorPin);
	if (pHArrayIndex == ArrayLenth) {
		pHArrayIndex = 0;
	}
	isNewData = true;
}

void loop(void) {
	static unsigned long printTime = millis();
	static float pHValue, voltage;

	//prüfen ob die Pumpe eingeschalten oder ausgeschalten wurde//

	static PumpState pumpMode = IDLE; //static = merkt sich die Zahlen hier vom pumpeMode//
	static int dosierStarts = 0;
	//
	static unsigned long pumpeActiveTime = millis();
	static unsigned long pumpeDownTime = millis();

	//messen und pH Berechnung//
	if (isNewData) {
		voltage = avergearray(ArrayLenth, pHArray) * 5.0 / 1024;
		pHValue = 3.5 * voltage + Offset;
		isNewData = false;
	}

	//monitor Ausgabe//
	if (millis() - printTime > printInterval) //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
	{
		printSerial(voltage, pHValue, dosierStarts, pumpMode);
		digitalWrite(LED, digitalRead(LED) ^ 1);
		printTime = millis();
	}

	switch (pumpMode) {
	case IDLE:
		//ausgelesener wert aus static int pumpeMode//
		if (dosierStarts >= 10) {
			pumpMode = ERROR;
		} else if (pHValue > 6.3) {
			digitalWrite(PUMP, LOW);
			pumpMode = RUNNING;     //schreibt in static eine 1//
			pumpeActiveTime = millis();
			dosierStarts = dosierStarts + 1;     //setzt dosierStarts//
		}
		break;
	case RUNNING:
		//ausgelesener wert aus static int pumpeMode//
		if (pHValue < 5.7) {
			digitalWrite(PUMP, HIGH);
			pumpMode = IDLE;      //schreibt in static eine 0//
			dosierStarts = 0;      //rücksetzen der dosierStarts//
		} else if (millis() - pumpeActiveTime > 1000) {  //laufzeit pump //
			digitalWrite(PUMP, HIGH);
			pumpMode = WAIT;  //schreibt in static eine 2//
			pumpeDownTime = millis();
		}
		break;
	case WAIT:
		//2 Minuten Warten
		if (millis() - pumpeDownTime > 2 * 40 * 1000L) { //pausenzeit pump//
			pumpMode = IDLE; //schreibt in static eine 0//
		}
		break;
	case ERROR:
		if (pHValue < 5.7) {
			dosierStarts = 0;
			pumpMode = IDLE;
		}
		break;
	default:
		break;
	}

}

