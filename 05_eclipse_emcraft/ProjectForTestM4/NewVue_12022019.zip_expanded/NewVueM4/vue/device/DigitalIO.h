/*
 * DigitalIO.h
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */

#ifndef INC_DIGITALIO_H_
#define INC_DIGITALIO_H_
#include <IpcCommonTypes.h>
#include "stdint.h"
typedef enum
{
	eExhPressureZero,//Exh Pressure Zero 1
	eInhPressureZero,//Inh Pressure Zero 2
	eSafetyValve,//Safety valve 3
	eImpedanceLine,//Impedance valve 4
	ePurgeFlow,//Purge Flow 5
	eOilPump,//Oil Pump 6
	eRemoteAlarm,
	eEnableIO,
	eProxyFlowResetLine
}E_SOLENOIDVALVE;

typedef enum
{
	eOnSolenoidValve,
	eOffSolenoidValve
}E_VALVESTATE;

typedef enum
{
	ePistonSW,
	eAirSw,
	eO2Sw,
	eManualBreathInput
}E_SWITCH;

void DigitalIO_Init();
E_State DigitalIO_GetSwitchState(E_SWITCH sw);
E_VALVESTATE DigitalIO_GetDesiredState(E_SOLENOIDVALVE valve);
E_VALVESTATE DigitalIO_GetState(E_SOLENOIDVALVE valve);
void DigitalIO_SetState(E_SOLENOIDVALVE valve,E_VALVESTATE state);
void DigitalIO_SetStateDirect(E_SOLENOIDVALVE valve,E_VALVESTATE state);
void DigitalIO_SetDesiredState(E_SOLENOIDVALVE valve,E_VALVESTATE state);
void DigitalIO_SetLow(E_SOLENOIDVALVE valve);
void DigitalIO_SetHigh(E_SOLENOIDVALVE valve);
#endif /* INC_DIGITALIO_H_ */
