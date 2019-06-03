/********************************************************************/
/*                                                                  */
/* File Name    : Key.c	  		                                    */
/*                                                                  */
/* General       : This module contain function for checking current*/
/*                 state of the button						        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include "Key.h"
#include "adc.h"
#define POWER_BUTTON_PRESSED_VOLTAGE 0.5
static bool gs_isPowerOn = false;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	Key_isPowerButtonPressed				              				*/
/*                                                                                      */
/* Details:    Check whether power button is pressing									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool Key_isPowerButtonPressed()
{
	float voltVal_Key0 = adc_Read(KEY0_ADC_CH);
	if(voltVal_Key0 <= POWER_BUTTON_PRESSED_VOLTAGE)
	{
		return true;
	}
	else
		return false;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	Key_setPowerOn							              				*/
/*                                                                                      */
/* Details:    set Power On action has been done										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void Key_setPowerOn()
{
	gs_isPowerOn = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	Key_isPowerOn							              				*/
/*                                                                                      */
/* Details:    Check whether Power On action has been done or not						*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool Key_isPowerOn()
{
	return gs_isPowerOn;
}
