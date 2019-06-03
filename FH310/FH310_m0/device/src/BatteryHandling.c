/********************************************************************/
/*                                                                  */
/* File Name     : battery_handling.c                               */
/*                                                                  */
/* General       : NiMH Battery handling sequence                   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                                                 */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/03      Quyen Ngo  	new file       		    */
/*																	*/
/*	#0001	   2017/09/02	   Quyen Ngo	modify					*/
/*		-change T_BAT_SCHG_TICK from 42 to 56(new spec 0901)		*/
/*                                                                  */
/* 	#0002	   2017/09/08	   Quyen Ngo	modify					*/
/*		-remove battery shutdown									*/
/********************************************************************/

#include <BatteryHandling.h>
#include "gpio.h"
#include "TaskCommon.h"

#define H_TO_MS(x)              3600*1000*x			//hour to ms
#define T_BAT_SCHG_TICK			H_TO_MS(56)/DEVICE_TASK_DELAY_TIME

/****************************************************************************************/
/*                                                                                      */
/* Function name:  battery_Init															*/
/*                                                                                      */
/* Details:  config GPIO pin use for batttery handling sequence							*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					POWER_BACKUP_EN_PORT_NUM											*/
/*					POWER_BACKUP_EN_BIT_NUM												*/
/*					NIMH_DETECT_LOW_PORT_NUM											*/
/*					NIMH_DETECT_LOW_BIT_NUM												*/
/*					BATTERY_CHARGE_PORT													*/
/*					BATTERY_CHARGE_BIT													*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void battery_Init(void)
{
	// Set pin as GPIO output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,POWER_BACKUP_EN_PORT_NUM, POWER_BACKUP_EN_BIT_NUM);		/* Power backup enable */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT,NIMH_DETECT_LOW_PORT_NUM, NIMH_DETECT_LOW_BIT_NUM);		/* Low battery detect */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,BATTERY_CHARGE_PORT, BATTERY_CHARGE_BIT);				/* Battery charge */

	// Enable charge
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,BATTERY_CHARGE_PORT, BATTERY_CHARGE_BIT);
	// Enable battery power
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,POWER_BACKUP_EN_PORT_NUM, POWER_BACKUP_EN_BIT_NUM);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: battery_Handle
//
//    Detail: Handle batter charge port. Battery charge port will be enable for T_bat_schg (h) from AC100V plug in.
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/

/****************************************************************************************/
/*                                                                                      */
/* Function name:  battery_Handle														*/
/*                                                                                      */
/* Details:  Handle batter charge port. Battery charge port will be enable for 			*/
/* 			 T_bat_schg (h) from AC100V plug in.										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		T_BAT_SCHG_TICK														*/
/*					LPC_GPIO_PORT														*/
/*					BATTERY_CHARGE_PORT													*/
/*					BATTERY_CHARGE_BIT													*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void battery_Handle(void)
{
	static uint32_t s_timeCnt = 0;
	if(s_timeCnt < T_BAT_SCHG_TICK)
	{
		s_timeCnt++;
	}
	else if (s_timeCnt == T_BAT_SCHG_TICK)
	{
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,BATTERY_CHARGE_PORT, BATTERY_CHARGE_BIT);
		s_timeCnt = T_BAT_SCHG_TICK + 1;//Exit time count for T_BAT_SCHG condition
	}
	return;
}
/* end of file */
