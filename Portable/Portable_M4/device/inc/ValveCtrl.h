#ifndef DEVICE_INC_VALVECTRL_H_
#define DEVICE_INC_VALVECTRL_H_
/********************************************************************/
/*                                                                  */
/* File Name    : ValveCtrl.h                        	   		    */
/*                                                                  */
/* General      : contain function to control valves                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       April 18, 2017     Thao Ha(MV)  		new file        	*/
/*                                                                  */
/********************************************************************/
#include <DualCoreCommon.h>

typedef enum
{
	VAL1,
	VAL2,
	VAL3,
} E_ValveNum;

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_Init                                                          */
/*                                                                                      */
/* Details:        Init GPIO for valve control and set default value for vale           */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_getStatus                                                     */
/*                                                                                      */
/* Details:    Get current Valve status                                                 */
/*                                                                                      */
/* Arguments:   E_ValveNum valNum                                                       */
/*                                                                                      */
/* ReturnValue : E_ValStatus                                                            */
/*                                                                                      */
/****************************************************************************************/
E_ValStatus valve_getStatus(E_ValveNum valNum);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_openVal1                                                      */
/*                                                                                      */
/* Details:    Open Valve 1                                                             */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal1();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_openVal2                                                      */
/*                                                                                      */
/* Details:    Open Valve 2                                                             */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal2();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_openVal3                                                      */
/*                                                                                      */
/* Details:    Open Valve 3                                                             */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_openVal3();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_closeVal1                                                     */
/*                                                                                      */
/* Details:    Close Valve 1                                                            */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal1();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_closeVal2                                                     */
/*                                                                                      */
/* Details:    Close Valve 2                                                            */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal2();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   valve_closeVal3                                                     */
/*                                                                                      */
/* Details:    Close Valve 3                                                            */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void valve_closeVal3();

#endif /* DEVICE_INC_VALVECTRL_H_ */
