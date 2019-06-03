/*
 * ExhController.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include "ExhController.h"
#define SERVO_RATE 500
#define SERVO_SCALE 8
#define FLOW_SCALER 3000
static bool gs_IsEnabled=false;
static int32_t gs_FirstIntegrator=0;
static int32_t gs_PreFilterIntegrator=0;
static int32_t gs_PreFilterPress=0;
static int32_t gs_CurrentMeasured=0;
static int32_t gs_OutputValue=0;
static int32_t gs_Desired = 0;
static desiredMotorPositionStruct gs_DesiredMotorPosition;
/**
 * This routine calculates the next step position to command the
 * motor in order to achieve a smooth transition to the desired pressure
 * level.
 */
static void ExhController_Calculate()
{
	int32_t error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
	int32_t scaleUpInput;

	error = gs_Desired - gs_CurrentMeasured;

	errorGain = error * 100;

	scaleUpInput =  errorGain / SERVO_RATE ;
	scaleUpInput = (int32_t) (scaleUpInput << SERVO_SCALE);

	scaleDownIntegrator = (int32_t) (gs_FirstIntegrator >> SERVO_SCALE);

	preFilterOutput = scaleDownIntegrator * 20;

	preFilterOutput /= (int16_t)FLOW_SCALER;


	if ( preFilterOutput >= 2000)
	{
		if ( errorGain <= 0 )
		{
			gs_FirstIntegrator += scaleUpInput ;
		}
		preFilterOutput = 2000;
	}
	else if ( preFilterOutput <= 0 )
	{
		if ( errorGain >= 0 )
		{
			gs_FirstIntegrator += scaleUpInput ;
		}
		preFilterOutput = 0;
	}
	else
	{
		gs_FirstIntegrator += scaleUpInput ;
	}

	valStep = preFilterOutput;

	gs_OutputValue = valStep;
}
/**
 * Disable Exhalation controller
 */
void ExhController_Disable()
{
	gs_IsEnabled=false;
}
/**
 * Enable Exhalation controller
 */
void ExhController_Enable()
{
	gs_IsEnabled=true;
}
/**
 * Check whether Exhalation controller is enable
 * @return bool gs_IsEnabled Return True if Exhalation controller is enable,
 * 							 otherwise return False
 */
bool ExhController_IsEnable()
{
	return gs_IsEnabled;
}
/**
 * The Reset() operation sets the initial value of the
 * accumulated integral error and sets the Exhalation valve to
 * almost open.
 */
void ExhController_Reset()
{
	gs_FirstIntegrator=0;
	gs_PreFilterIntegrator=0;
	gs_PreFilterPress=0;
}
/**
 * Get pressure from monitor struct and update gs_CurrentMeasured variable
 * @param monitor
 */
void ExhController_UpdateMonitorStruct(RealTimeM4ToA53* monitor)
{
	gs_CurrentMeasured = monitor->activePress.lastReading;
}
/**
 * This is the main processing loop for the
 * ExhPressure Controller.
 * The routine obtains the current sensor reading, calculates the step
 * motor position, and commands the step motor
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct ExhController_Run()
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;

	if(gs_IsEnabled==false)
	{
		return gs_DesiredMotorPosition;
	}
	ExhController_Calculate();
//	MotorController_MoveToStepPositionOld(eExhMotor,gs_OutputValue);
	gs_DesiredMotorPosition.desiredExhPosition=gs_OutputValue;

}
/**
 * Set desired pressure
 * @param rate
 */
void ExhController_SetDesired(int32_t rate)
{
	gs_Desired = rate;
}

