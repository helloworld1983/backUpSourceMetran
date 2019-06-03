#ifndef INC_ANYPALDISPLAYHANDLER_H_
#define INC_ANYPALDISPLAYHANDLER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : AnyPalDisplayHandler.h                            */
/*                                                                  */
/* General      : AnyPalDisplayHandler used to show/hide icon anypal*/
/* 						play buzzer anypal		                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/25       Viet Le		  	    new file        */
/* #0001       change AnypalMgr file to AnypalDisplayHandler		*/
/* #0002       2017/09/08       Viet Le             modify			*/
/*             Edit function anypalDisplayHandler_SetAnypalError()->*/
/*              anypalDisplayHandler_CheckAndSetError()				*/
/********************************************************************/
#include "stdbool.h"
#include "AnypalHandler.h"
#include "DualCoreCommon.h"

#define TIMER_FULL_CHARGING 3000 //3000 seconds

//Define charge status for anypal
typedef enum
{
    eUnknownChargeStatus,
    eAnyPalCharging,
    eAnyPalStopCharging,
    eAnyPalChargedFull
}E_AnyPalChargeStatus;

//Define error status of anypal
typedef enum
{
    eNoneApyPalError,
    eAnyPalOverCurrentAbnormal,
    eAnyPalBatteryAbnormal,
    eAnyPalRTCAbnormal,
    eAnyPalCommAbnormal
}E_AnyPalErrorStatus;

//Init anypal manager
void anypalDisplayHandler_Init(void);
//Get connect status of AnyPal
bool anypalDisplayHandler_GetAnypalConnectStatus(void);
//Set connect status of Anypal
void anypalDisplayHandler_SetAnypalConnectStatus(bool status);
//Get charging status of AnyPal
E_AnyPalChargeStatus anypalDisplayHandler_GetChargingStatus(void);
//Set charging status of AnyPal
void anypalDisplayHandler_SetChargingStatus(bool status);
//Set error status of Anypal
void anypalDisplayHandler_CheckAndSetError(ATP_STATUS atpStatus);
//Set over current alarm
void anypalDisplayHandler_SetAnypalOverCurrent(E_AlarmStatus status);
//Get error status of Anypal
E_AnyPalErrorStatus anypalDisplayHandler_GetAnypalError(void);
//Handle gui when anypal charged full
void anypalDisplayHandler_ChargedFull(void);

#endif /* INC_ANYPALDISPLAYHANDLER_H_ */
/* end of file */
