#ifndef DEVICE_INC_LEDBRIGHTNESSHANDLER_H_
#define DEVICE_INC_LEDBRIGHTNESSHANDLER_H_
/********************************************************************/
/*                                                                  */
/* File Name    : LedBrightnessHandler.h	                        */
/*                                                                  */
/* General      : brightness handle function						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/


/****************************************************************************************/
/*                                                                                      */
/* Function name:   LedBrightness_Handle                                                */
/*                                                                                      */
/* Details:        Brightness handle function is call for update bright value  	        */
/*                 base on illuminance sensor value every 200ms                         */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void LedBrightness_Handle();

#endif /* DEVICE_INC_LEDBRIGHTNESSHANDLER_H_ */
