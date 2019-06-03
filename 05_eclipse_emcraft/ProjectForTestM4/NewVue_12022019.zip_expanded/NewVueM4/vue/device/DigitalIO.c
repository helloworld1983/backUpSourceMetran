/*
 * DigitalIO.c
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */
#include <stdbool.h>
#include "fsl_gpio.h"
#include "DigitalIO.h"

#define SOLENOID_1_PORT             GPIO1             //PORT to control send data
#define SOLENOID_1_PIN              (27)               //PIN to control send data

#define SOLENOID_2_PORT             GPIO1            //PORT to control send data
#define SOLENOID_2_PIN              (26)               //PIN to control send data

#define SOLENOID_3_PORT             GPIO1             //PORT to control send data
#define SOLENOID_3_PIN              (24)               //PIN to control send data

#define SOLENOID_4_PORT             GPIO1            //PORT to control send data
#define SOLENOID_4_PIN              (25)               //PIN to control send data

#define SOLENOID_5_PORT            GPIO1            //PORT to control send data
#define SOLENOID_5_PIN              (17)               //PIN to control send data

#define SOLENOID_6_PORT             GPIO1             //PORT to control send data
#define SOLENOID_6_PIN              (16)               //PIN to control send data

#define PISTON_SW_PORT             GPIO3             //PORT to control send data
#define PISTON_SW_PIN              (04)               //PIN to control send data

#define AIR_SW_PORT             GPIO3             //PORT to control send data
#define AIR_SW_PIN              (12)               //PIN to control send data

//#define SOLENOID_5_PORT             GPIO3             //PORT to control send data
//#define SOLENOID_5_PIN              (9)               //PIN to control send data
//
//#define SOLENOID_6_PORT             GPIO3             //PORT to control send data
//#define SOLENOID_6_PIN              (20)               //PIN to control send data

#define O2_SW_PORT             GPIO3             //PORT to control send data
#define O2_SW_PIN              (16)               //PIN to control send data

#define MANUAL_BREATH_INPUT_SW_PORT             GPIO1             //PORT to control send data
#define MANUAL_BREATH_INPUT_SW_PIN              (20)               //PIN to control send data

/**
 * @brief Array of pin definitions passed to dio_Config() function
 */
typedef struct {
	GPIO_Type* gpioPort;			/* Pin group */
	uint8_t gpioPin;				/* Pin number */
	gpio_pin_config_t stPinconf ;	/* Pin mode and function for SCU */
} DIO_CONF_T;

static E_VALVESTATE gs_StateExhPressureZero = eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateExhPressureZero = eOffSolenoidValve;
static E_VALVESTATE gs_NormallyExhPressureZero= eOffSolenoidValve;

static E_VALVESTATE gs_StateInhPressureZero= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateInhPressureZero= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyInhPressureZero= eOffSolenoidValve;

static E_VALVESTATE gs_StateSafetyValve= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateSafetyValve= eOffSolenoidValve;
static E_VALVESTATE gs_NormallySafetyValve= eOffSolenoidValve;

static E_VALVESTATE gs_StateImpedanceLine= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateImpedanceLine= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyImpedanceLine= eOffSolenoidValve;

static E_VALVESTATE gs_StatePurgeFlow= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStatePurgeFlow= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyPurgeFlow= eOffSolenoidValve;

static E_VALVESTATE gs_StateOilPump= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateOilPump= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyOilPump= eOffSolenoidValve;

static E_VALVESTATE gs_StateRemoteAlarm= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateRemoteAlarm= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyRemoteAlarm= eOffSolenoidValve;

static E_VALVESTATE gs_StateEnableIO= eOffSolenoidValve;
static E_VALVESTATE gs_DesiredStateEnableIO= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyEnableIO= eOffSolenoidValve;

static E_VALVESTATE gs_StateProxyFlowResetLine= eOnSolenoidValve;
static E_VALVESTATE gs_DesiredStateProxyFlowResetLine= eOffSolenoidValve;
static E_VALVESTATE gs_NormallyProxyFlowResetLine= eOnSolenoidValve;

static DIO_CONF_T s_dioConfArr[] =
{
		{SOLENOID_1_PORT, SOLENOID_1_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_2_PORT, SOLENOID_2_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_3_PORT, SOLENOID_3_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_4_PORT, SOLENOID_4_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_5_PORT, SOLENOID_5_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_6_PORT, SOLENOID_6_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{PISTON_SW_PORT, PISTON_SW_PIN, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode}},
		{AIR_SW_PORT, AIR_SW_PIN, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode}},
		{O2_SW_PORT, O2_SW_PIN, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode}},
		{MANUAL_BREATH_INPUT_SW_PORT, MANUAL_BREATH_INPUT_SW_PIN, {kGPIO_DigitalInput, 0, kGPIO_NoIntmode}},
};

/**
 * Initialize GPIO port
 */
void DigitalIO_Init()
{
	for (int i=0; i < (sizeof(s_dioConfArr)/sizeof(DIO_CONF_T)) ; i++)
	{
		GPIO_PinInit(s_dioConfArr[i].gpioPort,s_dioConfArr[i].gpioPin, &(s_dioConfArr[i].stPinconf));
	}
}

/**
 * Get current switch state
 * @param sw Specify the switch that function would read state
 * @return E_State swState State of switch
 */
E_State DigitalIO_GetSwitchState(E_SWITCH sw)
{
	E_State swState = eOff;
	switch(sw)
	{
	case ePistonSW:
		if(GPIO_PinRead(PISTON_SW_PORT,PISTON_SW_PIN)==1U)
		{
			swState = eTrue;
		}
		break;
	case eAirSw:
		if(GPIO_PinRead(AIR_SW_PORT,AIR_SW_PIN)==1U)
		{
			swState = eTrue;
		}
		break;
	case eO2Sw:
		if(GPIO_PinRead(O2_SW_PORT,O2_SW_PIN)==1U)
		{
			swState = eTrue;
		}
		break;
	case eManualBreathInput:
		if(GPIO_PinRead(MANUAL_BREATH_INPUT_SW_PORT,MANUAL_BREATH_INPUT_SW_PIN)==1U)
		{
			swState = eTrue;
		}
		break;
	default:
		break;
	}
	return swState;
}

/**
 * Get Desired State of Valve
 * @param valve Specify the solenoid valve that function would read state
 * @return E_VALVESTATE state Desired state of solenoid valve
 */
E_VALVESTATE DigitalIO_GetDesiredState(E_SOLENOIDVALVE valve)
{
	E_SOLENOIDVALVE state=eOffSolenoidValve;
	switch(valve)
	{
	case eExhPressureZero:
		state = gs_DesiredStateExhPressureZero;
		break;
	case eInhPressureZero:
		state = gs_DesiredStateInhPressureZero;
		break;
	case eSafetyValve:
		state = gs_DesiredStateSafetyValve;
		break;
	case eImpedanceLine:
		state = gs_DesiredStateImpedanceLine;
		break;
	case ePurgeFlow:
		state = gs_DesiredStatePurgeFlow;
		break;
	case eOilPump:
		state = gs_DesiredStateOilPump;
		break;
	case eRemoteAlarm:
		state = gs_DesiredStateRemoteAlarm;
		break;
	case eEnableIO:
		state = gs_DesiredStateEnableIO;
		break;
	case eProxyFlowResetLine:
		state = gs_DesiredStateProxyFlowResetLine;
		break;
	default:
		break;
	}
	return state;
}

/**
 * Get State of solenoid valve
 * @param valve Specify the solenoid valve that function would read state
 * @return E_VALVESTATE state State of solenoid valve
 */
E_VALVESTATE DigitalIO_GetState(E_SOLENOIDVALVE valve)
{
	E_SOLENOIDVALVE state=eOffSolenoidValve;
	switch(valve)
	{
	case eExhPressureZero:
		state = gs_StateExhPressureZero;
		break;
	case eInhPressureZero:
		state = gs_StateInhPressureZero;
		break;
	case eSafetyValve:
		state = gs_StateSafetyValve;
		break;
	case eImpedanceLine:
		state = gs_StateImpedanceLine;
		break;
	case ePurgeFlow:
		state = gs_StatePurgeFlow;
		break;
	case eOilPump:
		state = gs_StateOilPump;
		break;
	case eRemoteAlarm:
		state = gs_StateRemoteAlarm;
		break;
	case eEnableIO:
		state = gs_StateEnableIO;
		break;
	case eProxyFlowResetLine:
		state = gs_StateProxyFlowResetLine;
		break;
	default:
		break;
	}
	return state;
}

/**
 * Set state of solenoid valve
 * @param valve Specify the solenoid valve that function would set state
 * @param state State of solenoid valve
 */
void DigitalIO_SetState(E_SOLENOIDVALVE valve,E_VALVESTATE state)
{
	switch(valve)
	{
	case eExhPressureZero:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyExhPressureZero==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallyExhPressureZero==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,0U);

			}
			else
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,1U);
			}
		}
		gs_StateExhPressureZero = state;
		break;
	case eInhPressureZero:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyInhPressureZero==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallyInhPressureZero==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,0U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,1U);
			}
		}
		gs_StateInhPressureZero = state;
		break;
	case eSafetyValve:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallySafetyValve==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallySafetyValve==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,0U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,1U);
			}
		}
		gs_StateSafetyValve = state;
		break;
	case eImpedanceLine:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyImpedanceLine==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallyImpedanceLine==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,0U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,1U);
			}
		}
		gs_StateImpedanceLine = state;
		break;
	case ePurgeFlow:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyPurgeFlow==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallyPurgeFlow==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,0U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,1U);
			}
		}
		gs_StatePurgeFlow = state;
		break;
	case eOilPump:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyOilPump==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,0U);
			}
		}
		else
		{
			if(gs_NormallyOilPump==eOffSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,0U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,1U);
			}
		}
		gs_StateOilPump = state;
		break;
	case eRemoteAlarm:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyRemoteAlarm==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		else
		{
			if(gs_NormallyRemoteAlarm==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		gs_StateRemoteAlarm = state;
		break;
	case eEnableIO:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyEnableIO==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		else
		{
			if(gs_NormallyEnableIO==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		gs_StateEnableIO = state;
		break;
	case eProxyFlowResetLine:
		if(state == eOnSolenoidValve)
		{
			if(gs_NormallyProxyFlowResetLine==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		else
		{
			if(gs_NormallyProxyFlowResetLine==eOffSolenoidValve)
			{
			}
			else
			{
			}
		}
		gs_StateProxyFlowResetLine = state;
		break;
	default:
		break;
	}
}

/**
 * Set direct state of solenoid valve
 * @param valve Specify the solenoid valve that function would set direct state
 * @param state State of solenoid valve
 */
void DigitalIO_SetStateDirect(E_SOLENOIDVALVE valve,E_VALVESTATE state)
{
	switch(valve)
	{
	case eExhPressureZero:
		if(gs_StateExhPressureZero != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,0U);
			}
			gs_StateExhPressureZero = state;
			gs_DesiredStateExhPressureZero = gs_StateExhPressureZero;
		}
		break;
	case eInhPressureZero:
		if(gs_StateInhPressureZero != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,0U);
			}
			gs_StateInhPressureZero = state;
			gs_DesiredStateInhPressureZero = gs_StateInhPressureZero;
		}
		break;
	case eSafetyValve:
		if(gs_StateSafetyValve != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,0U);
			}
			gs_StateSafetyValve = state;
			gs_DesiredStateSafetyValve = gs_StateSafetyValve;
		}
		break;
	case eImpedanceLine:
		if(gs_StateImpedanceLine != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,0U);
			}
			gs_StateImpedanceLine = state;
			gs_DesiredStateImpedanceLine = gs_StateImpedanceLine;
		}
		break;
	case ePurgeFlow:
		if(gs_StatePurgeFlow != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,0U);
			}
			gs_StatePurgeFlow = state;
			gs_DesiredStatePurgeFlow = gs_StatePurgeFlow;
		}
		break;
	case eOilPump:
		if(gs_StateOilPump != state)
		{
			if(state==eOnSolenoidValve)
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,1U);
			}
			else
			{
				GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,0U);
			}
			gs_StateOilPump = state;
			gs_DesiredStateOilPump = gs_StateOilPump;
		}
		break;
	case eRemoteAlarm:
		if(gs_StateRemoteAlarm != state)
		{
			if(state==eOnSolenoidValve)
			{
			}
			else
			{
			}
			gs_StateRemoteAlarm = state;
			gs_DesiredStateRemoteAlarm = gs_StateRemoteAlarm;
		}
		break;
	case eEnableIO:
		if(gs_StateEnableIO != state)
		{
			if(state==eOnSolenoidValve)
			{
			}
			else
			{
			}
			gs_StateEnableIO = state;
			gs_DesiredStateEnableIO = gs_StateEnableIO;
		}
		break;
	case eProxyFlowResetLine:
		if(gs_StateProxyFlowResetLine!= state)
		{
			if(state==eOnSolenoidValve)
			{
			}
			else
			{
			}
			gs_StateProxyFlowResetLine = state;
			gs_DesiredStateProxyFlowResetLine = gs_StateProxyFlowResetLine;
		}
		break;
	default:
		break;
	}
}

/**
 * Set desired state of solenoid valve
 * @param valve Specify the solenoid valve that function would set desired state
 * @param state State of solenoid valve
 */
void DigitalIO_SetDesiredState(E_SOLENOIDVALVE valve,E_VALVESTATE state)
{
	switch(valve)
	{
	case eExhPressureZero:
		gs_DesiredStateExhPressureZero = state;
		break;
	case eInhPressureZero:
		gs_DesiredStateInhPressureZero = state;
		break;
	case eSafetyValve:
		gs_DesiredStateSafetyValve = state;
		break;
	case eImpedanceLine:
		gs_DesiredStateImpedanceLine = state;
		break;
	case ePurgeFlow:
		gs_DesiredStatePurgeFlow = state;
		break;
	case eOilPump:
		gs_DesiredStateOilPump = state;
		break;
	case eRemoteAlarm:
		gs_DesiredStateRemoteAlarm = state;
		break;
	case eEnableIO:
		gs_DesiredStateEnableIO = state;
		break;
	case eProxyFlowResetLine:
		gs_DesiredStateProxyFlowResetLine= state;
		break;
	default:
		break;
	}
}

/**
 * Set specify solenoid valve to low state
 * @param valve Specify the solenoid valve that function would set to low
 */
void DigitalIO_SetLow(E_SOLENOIDVALVE valve)
{
	switch(valve)
	{
	case eExhPressureZero:
		GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,0U);
		break;
	case eInhPressureZero:
		GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,0U);
		break;
	case eSafetyValve:
		GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,0U);
		break;
	case eImpedanceLine:
		GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,0U);
		break;
	case ePurgeFlow:
		GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,0U);
		break;
	case eOilPump:
		GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,0U);
		break;
	case eRemoteAlarm:
		break;
	case eEnableIO:
		break;
	case eProxyFlowResetLine:
		break;
	default:
		break;
	}
}

/**
 * Set specify solenoid valve to high state
 * @param valve Specify the solenoid valve that function would set to high
 */
void DigitalIO_SetHigh(E_SOLENOIDVALVE valve)
{
	switch(valve)
	{
	case eExhPressureZero:
		GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN,1U);
		break;
	case eInhPressureZero:
		GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN,1U);
		break;
	case eSafetyValve:
		GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN,1U);
		break;
	case eImpedanceLine:
		GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN,1U);
		break;
	case ePurgeFlow:
		GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN,1U);
		break;
	case eOilPump:
		GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN,1U);
		break;
	case eRemoteAlarm:
		break;
	case eEnableIO:
		break;
	case eProxyFlowResetLine:
		break;
	default:
		break;
	}
}


