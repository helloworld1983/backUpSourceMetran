/********************************************************************/
/*                                                                  */
/* File Name    : PwmController.c                                  */
/*                                                                  */
/* General       : This module contain functions for control SCT PWM
 * 					out put chanel with same frequency of FH-310	*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jul 31, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include "PwmController.h"

#define FAN_PORT_MUX	0x4
#define FAN_PIN_MUX		4

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	pwm_Init													*/
/*                                                                                      */
/* Details： 	Init pwm port			        								   		    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 									                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void pwm_Init(void) {
	Chip_SCU_PinMuxSet(FAN_PORT_MUX, FAN_PIN_MUX, (SCU_MODE_FUNC1 | SCU_MODE_PULLUP | SCU_MODE_ZIF_DIS));//FAN.PWM
	// Enable clock use for PWM
	Chip_SCTPWM_Init(SCT_PWM);
	// set frequency
	Chip_SCTPWM_SetRate(SCT_PWM, SCT_PWM_FREQ);

	// Config Cooling fan
	/* Use SCT2_OUT2 pin */
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_COOLINGFAN, SCT_PWM_PIN_COOLINGFAN);
	////////////////////////////////////////////////////////////////////////////////
	/* Use SCT3_OUT1 pin */
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_BACKLIGHT, SCT_PWM_PIN_BACKLIGHT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	pwm_Start															*/
/*                                                                                      */
/* Details： 	 Start PWM output on cooling fan chanel with config duty cycle by call LPC
 * 			 Open lib function
 *		     This function wil start all out put chanel			        				*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 									                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void pwm_Start(void) {
	Chip_SCTPWM_Start(SCT_PWM);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	pwm_Stop															*/
/*                                                                                      */
/* Details： 	 Start PWM output on cooling fan chanel by call LPC Open lib function
 *		     This function wil stop all out put chanel			        				*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 									                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void pwm_Stop(void) {
	Chip_SCTPWM_Stop(SCT_PWM);
	return;
}
