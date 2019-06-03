#ifndef DEVICE_INC_FAN_H_
#define DEVICE_INC_FAN_H_
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
#include "board.h"

typedef enum
{
	OFF,
	OPERATE_5V,
	OPERATE_9V,
	OPERATE_12V
} E_FanVoltage;

/****************************************************************************************/
/*                                                                                      */
/* Function name:   fan_Init                                                            */
/*                                                                                      */
/* Details:       Initiallize GPIO for control FAN                                      */
/*                                                                                      */
/* Arguments:   NONE                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void fan_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   fan_SetVoltage                                                      */
/*                                                                                      */
/* Details:       Set Voltage supplied for Fan                                          */
/*                                                                                      */
/* Arguments:   (I) E_FanVoltage voltage                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void fan_SetVoltage(E_FanVoltage);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_GetFanVoltage							              			*/
/*                                                                                      */
/* Details:       get current voltage of FAN											*/
/*                                                                                      */
/* Arguments:	NONE																	*/
/*                                                                                      */
/* ReturnValue : E_FanVoltage			                                                */
/*                                                                                      */
/****************************************************************************************/
E_FanVoltage fan_GetFanVoltage();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   fan_Handle                                                          */
/*                                                                                      */
/* Details:     fan handle function is call for update Fan voltage base on current      */
/*              temperature every 1s                                                    */
/*                                                                                      */
/* Arguments:   (I) uint16_t temperature                                                */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void fan_Handle(uint16_t temperature);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   fan_ShutDown                                                        */
/*                                                                                      */
/* Details:        Shutdown Fan                                                         */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void fan_ShutDown(void);

#endif /* DEVICE_INC_FAN_H_ */
