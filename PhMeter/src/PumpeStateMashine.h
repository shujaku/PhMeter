/*
 * StateMashine.h
 *
 *  Created on: 23.05.2017
 *      Author: shukjaku
 */

#ifndef SRC_PUMPESTATEMASHINE_H_
#define SRC_PUMPESTATEMASHINE_H_

enum PumpState{
	IDLE,RUNNING,WAIT,ERROR
};

class PumpeStateMashine {
public:
	PumpeStateMashine();
	virtual ~PumpeStateMashine();
};

#endif /* SRC_PUMPESTATEMASHINE_H_ */
