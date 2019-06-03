/*
 * NCPAPController.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include <deviceTask/DeviceInterface.h>
#include "FlowController.h"
#include "NCPAPController.h"
#include "stdbool.h"

#define MIN_STEP_POSITION 0
#define VALVE_FTR_CONST     100
#define iDT                500           //200 for 5ms interval, 500 for 2ms interval
static bool gs_IsEnabled=false;
static int32_t gs_IntExValveFtr = 0;
static int32_t gs_TargetFlow = 0;
static const uint16_t gs_Scalen        =    8;
static desiredMotorPositionStruct gs_DesiredMotorPosition;
/**
 * Handle process of NCPAP controller
 * @param monitor
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct NCPAPController_Run(RealTimeM4ToA53* monitor)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;

	if(gs_IsEnabled==true)
	{
		NCPAPController_Calculate();
	}
	return gs_DesiredMotorPosition;
}
/**
 * This routine calculates the desired total flow rate
 * for the current execution cycle. The current sensor reading
 * and desired sensor readings are used to generate a desired flow
 * rate
 */
void NCPAPController_Calculate()
{
//	MotorController_MoveToStepPositionOld(eExhMotor,NCPAPController_ExValveFilter(MIN_STEP_POSITION));
	gs_DesiredMotorPosition.desiredExhPosition = NCPAPController_ExValveFilter(MIN_STEP_POSITION);
	FlowController_SetDesiredFlow(eAirFlowController,devIf_GetSetting()->desiredAirFlow);
	FlowController_SetDesiredFlow(eO2FlowController,devIf_GetSetting()->desiredO2Flow);
}
/**
 * Set target flow
 * @param value
 */
void NCPAPController_SetTargetFlow(int32_t value)
{
	gs_TargetFlow = value;
}
/**
 * Reset static variables
 */
void NCPAPController_ResetIMVStaticVariables()
{
	gs_IntExValveFtr=0;
}
/**
 * Filter Exhalation valve
 * @param u
 * @return int16_t Output Step position
 */
int16_t NCPAPController_ExValveFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntExValveFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntExValveFtr += (int32_t)Temp32*VALVE_FTR_CONST/iDT;

	return(Output);
}
/**
 * Enable NCPAP Controller
 */
void NCPAPController_Enable()
{
	gs_IsEnabled=true;
}
/**
 * Disable NCPAP Controller
 */
void NCPAPController_Disable()
{
	gs_IsEnabled=false;
}
/**
 * Check whether NCPAP controller is enable
 * @return bool gs_IsEnabled Return True if NCPAP is enable.
 * 							 Otherwise, return False.
 */
bool NCPAPController_IsEnable()
{
	return gs_IsEnabled;
}

