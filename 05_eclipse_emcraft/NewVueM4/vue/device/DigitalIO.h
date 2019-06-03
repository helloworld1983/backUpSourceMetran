/*
 * DigitalIO.h
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */

#ifndef INC_DIGITALIO_H_
#define INC_DIGITALIO_H_
#include "stdint.h"
typedef enum
{
	eSolenoidValve1,
	eSolenoidValve2,
	eSolenoidValve3,
	eSolenoidValve4,
	eSolenoidValve5,
	eSolenoidValve6
}E_SOLENOIDVALVE;
typedef enum
{
	eOnSolenoidValve,
	eOffSolenoidValve
}E_VALVESTATE;

void DigitalIO_Init();
void DigitalIO_SetStateSolenoidValve(E_SOLENOIDVALVE valve,E_VALVESTATE state);


#endif /* INC_DIGITALIO_H_ */
