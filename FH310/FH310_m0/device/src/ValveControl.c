/********************************************************************/
/*                                                                  */
/* File Name    : ValveControl.h                                	*/
/*                                                                  */
/* General       : This module contain function for init and		*/
/* 					control valve                                   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/06/02     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/* #0001	   2017/09/01	  Quyen Ngo				modify	        */
/*		-remove assert												*/
/********************************************************************/

#include "ValveControl.h"

static E_ValStatus s_val1Status = VAL_OFF;
static E_ValStatus s_val2Status = VAL_OFF;
static E_ValStatus s_val3Status = VAL_OFF;
static E_ValStatus s_val4Status = VAL_OFF;
static E_ValStatus s_val5Status = VAL_OFF;

//*****************************************************************************/
//    Operation Name: valve_Init()
//
//    Processing:  Config GPIO port for solenoid valve
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_Init(void) {
	// Set pin as GPIO output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL4_PORT_NUM, VAL4_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,VAL5_PORT_NUM, VAL5_BIT_NUM);		/* output */

	// Init value for out put pin
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL4_PORT_NUM, VAL4_BIT_NUM);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL5_PORT_NUM, VAL5_BIT_NUM);

	return;
}

//*****************************************************************************/
//    Operation Name: valve_getStatus()
//
//    Processing:  Get valve status
//
//    Input Parameters: E_ValveNum valNum
//
//    Output Parameters: NONE
//
//    Return Values:  ValStatus
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
E_ValStatus valve_getStatus(E_ValveNum valNum)
{
	switch (valNum) {
	case VAL1:
		return s_val1Status;
		break;
	case VAL2:
		return s_val2Status;
		break;
	case VAL3:
		return s_val3Status;
		break;
	case VAL4:
		return s_val4Status;
		break;
	case VAL5:
		return s_val5Status;
		break;
	default:
		DEBUGOUT("Invalid valve num\n");
		return ERR;
		break;
	}
}

//*****************************************************************************/
//    Operation Name: valve_openVal1()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_openVal1(void)
{
	s_val1Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_openVal2()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_openVal2(void)
{
	s_val2Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_openVal3()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_openVal3(void)
{
	s_val3Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_openVal4()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_openVal4(void)
{
	s_val4Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL4_PORT_NUM, VAL4_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_openVal5()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_openVal5(void)
{
	s_val5Status = VAL_ON;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,VAL5_PORT_NUM, VAL5_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_closeVal1()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_closeVal1(void)
{
	s_val1Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL1_PORT_NUM, VAL1_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_closeVal2()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_closeVal2(void)
{
	s_val2Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL2_PORT_NUM, VAL2_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_closeVal3()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_closeVal3(void)
{
	s_val3Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL3_PORT_NUM, VAL3_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_closeVal4()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_closeVal4(void)
{
	s_val4Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL4_PORT_NUM, VAL4_BIT_NUM);
	return;
}

//*****************************************************************************/
//    Operation Name: valve_closeVal5()
//
//    Processing:  Open valve 1
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void valve_closeVal5(void)
{
	s_val5Status = VAL_OFF;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,VAL5_PORT_NUM, VAL5_BIT_NUM);
	return;
}
