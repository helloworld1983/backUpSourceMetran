/*
 * FlowController.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include <deviceTask/DeviceInterface.h>
#include "FlowController.h"
static const int16_t SERVO_RATE = 500;
static const uint32_t FLOW_SCALE = 3000;//3000
static const int16_t SERVO_SCALE = 8;
static int32_t gs_DesiredAirFlow = 0;
static int32_t gs_DesiredO2Flow = 0;
static bool gs_isEnabledAirFlow = false;
static bool gs_isEnabledO2Flow = false;
static int32_t gs_PreviousMesuredAirFlow = 0;
static int32_t gs_PreviousMesuredO2Flow = 0;
static int32_t gs_CurrentMesuredAirFlow = 0;
static int32_t gs_CurrentMesuredO2Flow = 0;
static int32_t gs_OutputValueAirFlow = 0;
static int32_t gs_OutputValueO2Flow = 0;
static int32_t gs_FirstIntegratorO2=0;
static int32_t gs_FirstIntegratorAir=0;
static int16_t gs_Speed =40;//40
static desiredMotorPositionStruct gs_DesiredMotorPosition;
static unsigned short gs_LiftOffAirMotor = 0;
static unsigned short gs_LiftOffO2Motor = 0;
/**
 * Set static argument
 * @param flowController
 * @param step
 */
void FlowController_SetStaticAgr(E_FlowControllerId flowController,int32_t step)
{
	int32_t step_t = step*FLOW_SCALE;
	switch(flowController)
	{
	case eAirFlowController:
		gs_FirstIntegratorAir = (int32_t)((int32_t)(step_t/gs_Speed))<<SERVO_SCALE;

		break;
	case eO2FlowController:
		gs_FirstIntegratorO2 = (int32_t)((int32_t)(step_t/gs_Speed))<<SERVO_SCALE;

		break;
	default:
		break;
	}
}
/**
 * This routine sets the desired rate of the
 * pressure being controlled.
 * @param flowController
 * @param rate
 */
void FlowController_SetDesiredFlow(E_FlowControllerId flowController, int32_t rate)
{
	switch(flowController)
	{
	case eAirFlowController:
		gs_DesiredAirFlow = rate;
		break;
	case eO2FlowController:
		gs_DesiredO2Flow = rate;
		break;
	default:
		break;
	}
}
/**
 * Check whether flow controller is enable
 * @param flowController
 * @return bool isEnable Return True if flow controller is enable,
 * 						 otherwise return false
 */
bool FlowController_IsEnable(E_FlowControllerId flowController)
{
	bool isEnable = false;
	switch(flowController)
		{
		case eAirFlowController:
			isEnable= gs_isEnabledAirFlow;
			break;
		case eO2FlowController:
			isEnable= gs_isEnabledO2Flow;
			break;
		default:
			break;
		}
	return isEnable;
}

/**
 * Set enable for specified flow controller
 * @param flowController
 * @param isEnabled
 */
void FlowController_SetEnable(E_FlowControllerId flowController,bool isEnabled)
{
	switch(flowController)
	{
	case eAirFlowController:
		gs_isEnabledAirFlow = isEnabled;
		if(isEnabled==false)
		{
			FlowController_Reset(eAirFlowController);
		}
		break;
	case eO2FlowController:
		gs_isEnabledO2Flow = isEnabled;
		if(isEnabled==false)
		{
			FlowController_Reset(eO2FlowController);
		}
		break;
	default:
		break;
	}
}
/**
 * This is the main processing loop for the Flow Controllers.
 * The routine obtains the current sensor reading, calculates the step
 * motor position, and commands the step motor.
 * @param flowController
 * @param monitor
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct FlowController_Run(E_FlowControllerId flowController,RealTimeM4ToA53* monitor)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;
	gs_LiftOffAirMotor = monitor->airValve.liftOff;
	gs_LiftOffO2Motor = monitor->o2Valve.liftOff;
	switch(flowController)
	{
	case eAirFlowController:
		if(gs_isEnabledAirFlow==true)
		{
			gs_PreviousMesuredAirFlow = gs_CurrentMesuredAirFlow;
			gs_CurrentMesuredAirFlow = monitor->airFlow.lastReading;
			FlowController_Calculate(eAirFlowController);
		}
		break;
	case eO2FlowController:
		if(gs_isEnabledO2Flow==true)
		{
			gs_PreviousMesuredO2Flow = gs_CurrentMesuredO2Flow;
			gs_CurrentMesuredO2Flow = monitor->o2Flow.lastReading;
			FlowController_Calculate(eO2FlowController);
		}
		break;
	default:
		break;
	}
	return gs_DesiredMotorPosition;
}
/**
 *This routine calculates the desired step motor position
 *for the current execution cycle. The current sensor reading
 *and desired sensor readings are used to generate a desired step
 *position.
 * @param flowController
 */
void FlowController_Calculate(E_FlowControllerId flowController)
{
	switch (flowController) {
	case eAirFlowController:
		gs_OutputValueAirFlow=FlowController_CompStepAirFlow();
		if(gs_OutputValueAirFlow<gs_LiftOffAirMotor)
		{
			gs_OutputValueAirFlow = gs_LiftOffAirMotor;
		}
//		MotorController_MoveToStepPositionOld(eAirMotor,gs_OutputValueAirFlow);
		gs_DesiredMotorPosition.desiredAirPosition = gs_OutputValueAirFlow;

		break;
	case eO2FlowController:
		gs_OutputValueO2Flow=FlowController_CompStepO2Flow();
		if(gs_OutputValueO2Flow<gs_LiftOffO2Motor)
		{
			gs_OutputValueO2Flow = gs_LiftOffO2Motor;
		}
//		MotorController_MoveToStepPositionOld(eO2Motor,gs_OutputValueO2Flow);
		gs_DesiredMotorPosition.desiredO2Position = gs_OutputValueO2Flow;
		break;
	default:
		break;
	}
}
/**
 * Computer step air flow
 * @return int32_t valStep step position
 */
int32_t FlowController_CompStepAirFlow()
{
	int32_t error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
	int32_t scaleUpInput;

	error = gs_DesiredAirFlow - gs_CurrentMesuredAirFlow;

	errorGain = error * 100;

	scaleUpInput =  errorGain / SERVO_RATE ;
	scaleUpInput = (int32_t) (scaleUpInput << SERVO_SCALE )*3;

	scaleDownIntegrator = (int32_t) (gs_FirstIntegratorAir >> SERVO_SCALE);

	preFilterOutput = scaleDownIntegrator * gs_Speed;

	preFilterOutput /= (int16_t)FLOW_SCALE;

	if ( preFilterOutput >= MAX_STEP_POSITION_OLD)
	{
		if ( errorGain <= 0 )
		{
			gs_FirstIntegratorAir += scaleUpInput ;
		}
		preFilterOutput = MAX_STEP_POSITION_OLD;
	}
	else if ( preFilterOutput <= 0 )
	{
		if ( errorGain >= 0 )
		{
			gs_FirstIntegratorAir += scaleUpInput ;
		}
		preFilterOutput = 0;
	}
	else
	{
		gs_FirstIntegratorAir += scaleUpInput ;
	}

	valStep = preFilterOutput;

	return valStep;
}
/**
 * This operation is invoked at the start of each inhalation to reset
 * the running sum and the cycle number.
 * @param flowController
 */
void FlowController_Reset(E_FlowControllerId flowController)
{
	switch (flowController) {
	case eAirFlowController:
		gs_FirstIntegratorAir=0;
		break;
	case eO2FlowController:
		gs_FirstIntegratorO2=0;
		break;
	default:
		break;
	}
}
/**
 * Compute step O2 flow
 * @return int32_t valStep step position
 */
int32_t FlowController_CompStepO2Flow()
{
	int32_t error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
	int32_t scaleUpInput;

	error = gs_DesiredO2Flow - gs_CurrentMesuredO2Flow;
	errorGain = error * 100;

	scaleUpInput =  errorGain / SERVO_RATE ;
	scaleUpInput = (int32_t) (scaleUpInput << SERVO_SCALE )*3;

	scaleDownIntegrator = (int32_t) (gs_FirstIntegratorO2 >> SERVO_SCALE);

	preFilterOutput = scaleDownIntegrator * gs_Speed;

	preFilterOutput /= (int16_t)FLOW_SCALE;

	if ( preFilterOutput >= MAX_STEP_POSITION_OLD)
	{
		if ( errorGain <= 0 )
		{
			gs_FirstIntegratorO2 += scaleUpInput ;
		}
		preFilterOutput = MAX_STEP_POSITION_OLD;
	}
	else if ( preFilterOutput <= 0 )
	{
		if ( errorGain >= 0 )
		{
			gs_FirstIntegratorO2 += scaleUpInput ;
		}
		preFilterOutput = 0;
	}
	else
	{
		gs_FirstIntegratorO2 += scaleUpInput ;
	}

	valStep = preFilterOutput;

	return valStep;
}

