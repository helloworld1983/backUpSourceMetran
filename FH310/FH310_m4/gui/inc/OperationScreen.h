#ifndef INC_OPERATIONSCREEN_H_
#define INC_OPERATIONSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : OperationScreen.h                                 */
/*                                                                  */
/* General      : OperationScreen display the value of flow oxygen,	*/
/*				  display the status of machine 					*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       2017/09/08       Viet Le				modify			*/
/*        Edit function operationScreen_ShowO2Cylinder()->          */
/*             operationScreen_CheckAndShowO2Cylinder()             */
/* #0002       2017/11/17       Viet Le				modify			*/
/*	Remove function operationScreen_SetExtFlowPath() (coverity)		*/
/********************************************************************/

#include "WM.h"
#include "stdint.h"
#include "stdbool.h"
#include "MainScreen.h"
#include "DualCoreCommon.h"

#define BORDER_FR_NAME_COLOR 			0xE6D2AA
#define HOUR_IN_ONE_MONTH 				720
#define SECONDS_IN_ONE_HOUR 			3600//1 hour
#define INCREASE_CUMULATIVE_HOUR 		1//1 second
#define COUNTER_HIDE_SHOW_CHILD_LOCK	10
#define SECONDS_IN_ONE_MIN				60

extern WM_HWIN g_operationScreen;

//Init operation screen
void operationScreen_Init(void);
//Show operation screen
void operationScreen_Show(void);
//Hide operation screen
void operationScreen_Hide(void);
//Enter Operation mode
void operationScreen_EnterOperationMode(void);
//Enter start operation
void operationScreen_EnterStartOperation(void);
//Change flow rate by encoder
void operationScreen_ChangeFlowRate(uint8_t FRPSAValue);
//Confirm flow rate in childlock mode
void operationScreen_ConfirmChildLock(uint8_t FRPSAValue);
//Restart update cumulative operation hour
void operationScreen_restartUpdateCumulativeHour(void);
//Show anypal in operation screen
void operationScreen_ShowAnypal(void);
//Show anypal charging in operation screen
void operationScreen_ShowAnypalCharging(void);
//Show o2 cylinder in operation screen
void operationScreen_CheckAndShowO2Cylinder(void);
//Clear patient cumulative hour
void operationScreen_ClearPatientCumulative(void);
//Clear unit cumulative hour
void operationScreen_ClearUnitCumulative(int hours);
//Clear unit cumulative hour when flow rate is 3L
void operationScreen_ClearUnitCumulativeThree(int hours);
//Clear unit cumulative hour when flow rate is 2.5L
void operationScreen_ClearUnitCumulativeTwoFive(int hours);
//Clear unit cumulative hour when flow rate is 2L
void operationScreen_ClearUnitCumulativeTwo(int hours);
//Clear unit cumulative hour when flow rate is 1.75L
void operationScreen_ClearUnitCumulativeOneSevenFive(int hours);
//Clear unit cumulative hour when flow rate is 1.5L
void operationScreen_ClearUnitCumulativeOneFive(int hours);
//Clear unit cumulative hour when flow rate is 1.25L
void operationScreen_ClearUnitCumulativeOneTwoFive(int hours);
//Clear unit cumulative hour when flow rate is 1L
void operationScreen_ClearUnitCumulativeOne(int hours);
//Clear unit cumulative hour when flow rate is 0.75L
void operationScreen_ClearUnitCumulativeZeroSevenFive(int hours);
//Clear unit cumulative hour when flow rate is 0.5L
void operationScreen_ClearUnitCumulativeZeroFive(int hours);
//Clear unit cumulative hour when flow rate is 0.25L
void operationScreen_ClearUnitCumulativeZeroTwoFive(int hours);
//Handle cumulative operation hour
void operationScreen_HandlerCumulativeHour(void);
//Update animation in operation screen
void operationScreen_updateAnimation(void);
//Send flow rate to device
void operationScreen_SendFRToDevice(void);
//Set cumulative operation hour
void operationScreen_SetCumulativeHour(void);
//Show hide flow rate in childlock mode
void operationScreen_ShowHideChildlock(void);
//Hide icon o2 cylinder
void operationScreen_HideO2Cylinder(void);

#endif /* INC_OPERATIONSCREEN_H_ */
/* end of file */

