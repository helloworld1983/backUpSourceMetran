/********************************************************************/
/*                                                                  */
/* File Name     : CoolingFan.h		                                */
/*                                                                  */
/* General       : This file contains the functions for init,	    */
/*				  control cooling fan and detect error              */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/07      Quyen Ngo  	new file       		    */
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/*		-add define FAN_MAX_DUTY									*/
/*		-fix header file and function								*/
/*                                                                  */
/********************************************************************/

#include "coolingfan.h"
#include "gpio.h"

#ifdef DEBUG
#define DEBUG_COOLFAN_EN
#endif

#ifdef DEBUG_COOLFAN_EN
#define DEBUG_COOLFAN(...) printf(__VA_ARGS__)
#else
#define DEBUG_COOLFAN(...)
#endif

#define FAN_MAX_DUTY			100

static uint8_t s_currentFanDuty = 0;

/****************************************************************************************/
/*                                                                                      */
/* Function name:  coolingFan_Init														*/
/*                                                                                      */
/* Details:  Config hardware interface for cooling fan									*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		SCT_PWM																*/
/*					SCT_PWM_COOLINGFAN													*/
/*					LPC_GPIO_PORT														*/
/*					FAN_ERR_PORT_NUM													*/
/*					FAN_ERR_BIT_NUM														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void coolingFan_Init(void)
{
	// pwm cooling fan config and Set duty cycle = 0
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_COOLINGFAN, Chip_SCTPWM_GetTicksPerCycle(SCT_PWM)/2);
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_COOLINGFAN, Chip_SCTPWM_PercentageToTicks(SCT_PWM, 100));

	//Fan error GPIO config
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, FAN_ERR_PORT_NUM, FAN_ERR_BIT_NUM);		/* input */
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  coolingFan_Setduty													*/
/*                                                                                      */
/* Details:  Set duty cycle of cooling fan by call LPC open chip lib, if application want*/
/*				to stop cooling fan just set dutycycle = 0								*/
/*                                                                                      */
/* Arguments :	(I) uint8_t dutyCycle: Dutcy cycle in %									*/
/*				 Unit: %																*/
/*				 Range: 0~100												 			*/
/*                                                                                      */
/* Macros:	   		SCT_PWM																*/
/*					SCT_PWM_COOLINGFAN													*/
/*					FAN_MAX_DUTY														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void coolingFan_Setduty(uint8_t dutyCycle)
{
	s_currentFanDuty = dutyCycle;
	if(dutyCycle > FAN_MAX_DUTY)
	{
		DEBUGOUT("cooling Fan duty out of range \n");
		return;
	}
	uint8_t invDuty;
	invDuty = FAN_MAX_DUTY - dutyCycle;
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_COOLINGFAN, Chip_SCTPWM_PercentageToTicks(SCT_PWM, invDuty));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  coolingFan_getDuty													*/
/*                                                                                      */
/* Details:  Get current duty cycle of cooling Fan										*/
/*                                                                                      */
/* Arguments :																			*/
/*                                                                                      */
/* Macros:	   		SCT_PWM																*/
/*					SCT_PWM_COOLINGFAN													*/
/*					FAN_MAX_DUTY														*/
/*																			 			*/
/* ReturnValue : 	uint8_t															   	*/
/*				 	Unit: %																*/
/*				 	Range: 0~100												 		*/
/*                                                                                      */
/* Using Global Data : 	(I) uint8_t s_currentFanDuty: Dutcy cycle in %					*/
/*				 		Unit: %															*/
/*				 		Range: 0~100												 	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
uint8_t coolingFan_getDuty(void)
{
	return s_currentFanDuty;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  coolingFan_GerErr													*/
/*                                                                                      */
/* Details:  Get err state of cooling Fan												*/
/*                                                                                      */
/* Arguments :																			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					FAN_ERR_PORT_NUM													*/
/*					FAN_ERR_BIT_NUM														*/
/*																			 			*/
/* ReturnValue : 	bool    true: Fan Err/ not rotation									*/
/*					         false: Fan rotation										*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
bool coolingFan_GerErr(void)
{
	bool isFanNotRotation;
	isFanNotRotation = Chip_GPIO_GetPinState(LPC_GPIO_PORT, FAN_ERR_PORT_NUM, FAN_ERR_BIT_NUM); //true fan not rotation
	return isFanNotRotation;
}
