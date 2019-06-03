/********************************************************************/
/*                                                                  */
/* File Name    : Fan.h                        				        */
/*                                                                  */
/* General      : Initiallize Fan									*/
/*                Contrl FAN voltage                                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       December 22, 2016     Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include "Fan.h"
#include "gpio.h"
#define MAX_NUMBER_OF_SAMPLES 30

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_SetVoltage						              				    */
/*                                                                                      */
/* Details:       Set Voltage supplied for Fan											*/
/*                                                                                      */
/* Arguments:	(I) E_FanVoltage voltage												*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void fan_SetVoltage(E_FanVoltage voltage)
{
	switch(voltage)
	{
	case OFF:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		break;
	case OPERATE_5V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		break;
	case OPERATE_9V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,true);
		break;
	case OPERATE_12V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		break;
	default:
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_Init							              				    */
/*                                                                                      */
/* Details:       Initiallize GPIO for control FAN										*/
/*                                                                                      */
/* Arguments:	NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void fan_Init()
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_EN_PORT_NUM, FAN_EN_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_9V_PORT_NUM, FAN_9V_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_12V_PORT_NUM, FAN_12V_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
	fan_SetVoltage(OFF);
}
