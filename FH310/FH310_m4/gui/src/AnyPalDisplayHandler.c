/********************************************************************/
/*                                                                  */
/* File Name    : AnyPalDisplayHandler.c                            */
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
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       2017/09/07       Viet Le             modify			*/
/*       Edit function anypalDisplayHandler_SetAnypalError()->      */
/*			anypalDisplayHandler_CheckAndSetError() 				*/
/********************************************************************/

#include "AnyPalDisplayHandler.h"
#include "StandbyScreen.h"
#include "OperationScreen.h"
#include "LowAlarmScreen.h"
#include "GuiHandler.h"
#include "DevelopmentSettingScreen.h"
#include "GuiTask.h"
#include "AudioMgr.h"
#include "setting.h"
#include "ModeMgr.h"
#include "GuiInterface.h"

static bool s_isConnectAnypal;
static E_AnyPalChargeStatus s_chargingStatus;
static E_AnyPalErrorStatus s_errorStatus;
static int s_counter50Min = 0;

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_Init													*/
/*                                                                                      */
/* Details  -Initialize anypal display handler		 			    				    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isConnectAnypal : flag check connect AnyPal                */
/*                     (O) s_chargingStatus : flag check charging Anypal				*/
/*                     (O) s_errorStatus : flag check error Anypal						*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_Init(void)
{
	s_isConnectAnypal = false;
	s_chargingStatus = eUnknownChargeStatus;
	s_errorStatus = eNoneApyPalError;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_GetAnypalConnectStatus								*/
/*                                                                                      */
/* Details  -Get status connect anypal		 			    				        	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isConnectAnypal : flag check connect Anypal				*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool anypalDisplayHandler_GetAnypalConnectStatus(void)
{
	return s_isConnectAnypal;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_SetAnypalConnectStatus								*/
/*                                                                                      */
/* Details  -Show icon and play buzzer anypal when anypal is connect.		 			*/
/*				Hide icon anypal when anypal is disconnect								*/
/*                                                                                      */
/* Arguments : (I) bool status: - status connect Anypal									*/
/*				   -range : 0-1															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isConnectAnypal : flag check connect Anypal                */
/*                     (O) s_errorStatus : flag check error Anypal						*/
/*                     (O) s_counter50Min : flag check charging full Anypal				*/
/*                     (O) s_chargingStatus : flag check charging Anypal				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_SetAnypalConnectStatus(bool status)
{
	s_isConnectAnypal = status;

	if(status == false)
	{
		s_errorStatus = eNoneApyPalError;

		guiTask_SetAlarm(eAnypalCommunicationId, eInactive);
		guiTask_SetAlarm(eAnypalRTCId, eInactive);
		guiTask_SetAlarm(eAnypalBatterId, eInactive);

		s_counter50Min = 0;
		s_chargingStatus = eUnknownChargeStatus;
		guihandler_StopTimerChargingAnypal();
	}
	else
	{
		//Check and play audio
		if(guiInterface_GetSettingValue(eAlarmRecoverId) == eLastAlarmId)
		{
			audioMgr_Request(eAnyPalAudio, eNoAlarmAudio);
		}
	}

	operationScreen_ShowAnypal();
	standbyScreen_ShowAnypal();
	lowAlarmScreen_ShowAnypal();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_GetChargingStatus										*/
/*                                                                                      */
/* Details  -Get status charging anypal		 			    				        	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_AnyPalChargeStatus		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_chargingStatus : flag check status charging Anypal			*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
E_AnyPalChargeStatus anypalDisplayHandler_GetChargingStatus(void)
{
	return s_chargingStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_SetChargingStatus										*/
/*                                                                                      */
/* Details  -Show icon charging anypal		 			    				        	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool status : - status charing Anypal								*/
/*				   -range : 0-1															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                            				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isConnectAnypal : flag connect Anypal						*/
/*                     (O) s_chargingStatus : flag charging Anypal                      */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_SetChargingStatus(bool status)
{
	if(s_isConnectAnypal == true)
	{
		if(status == true)
		{
			s_chargingStatus = eAnyPalCharging;
		}
		else
		{
			if(s_chargingStatus == eAnyPalCharging)
			{
				s_chargingStatus = eAnyPalStopCharging;
				guihandler_StartTimerChargingAnypal();
			}
		}

		standbyScreen_ShowAnypalCharging();
		operationScreen_ShowAnypalCharging();
		lowAlarmScreen_ShowAnypalCharging();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_CheckAndSetError										*/
/*                                                                                      */
/* Details  -Show alarm anypal 				 			    				        	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) ATP_STATUS atpStatus : anypal status									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                            				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_errorStatus : error status Anypal                          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_CheckAndSetError(ATP_STATUS atpStatus)
{
	if(atpStatus.isCommunication == true)
	{
		if(developmentSettingScreen_CheckEnable(eAnypalCommunicationId) == true)
		{
			s_errorStatus = eAnyPalCommAbnormal;
			guiTask_SetAlarm(eAnypalCommunicationId, eActive);
		}

		guiTask_writeAlarmLog(eAnypalCommunicationId);

	}
	else if(atpStatus.isRTCAbnormal == true)
	{
		if(developmentSettingScreen_CheckEnable(eAnypalRTCId) == true)
		{
			s_errorStatus = eAnyPalRTCAbnormal;
			guiTask_SetAlarm(eAnypalRTCId, eActive);
		}
		guiTask_writeAlarmLog(eAnypalRTCId);
	}
	else if(atpStatus.isBatteryAbnomal == true)
	{
		if(developmentSettingScreen_CheckEnable(eAnypalBatterId) == true)
		{
			s_errorStatus = eAnyPalBatteryAbnormal;
			guiTask_SetAlarm(eAnypalBatterId, eActive);
		}
		guiTask_writeAlarmLog(eAnypalBatterId);
	}

	if(s_errorStatus != eNoneApyPalError)
	{
		lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_SetAnypalOverCurrent									*/
/*                                                                                      */
/* Details  -Show alarm anypal over current		 			    				        */
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmStatus status	: - error overcurrent Anypal					*/
/*				   -range : 0-1															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                            				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_errorStatus : error Anypal                                 */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_SetAnypalOverCurrent(E_AlarmStatus status)
{
	if(developmentSettingScreen_CheckEnable(eOverCurrentId) == true)
	{
		if(status == eActive)
		{
			s_errorStatus = eAnyPalOverCurrentAbnormal;
			guiTask_SetAlarm(eOverCurrentId, eActive);

			//If active show alarm and write log
			lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm();
			guiTask_writeAlarmLog(eOverCurrentId);
		}
		else if (status == eInactive)
		{
			s_errorStatus = eNoneApyPalError;
			guiTask_SetAlarm(eOverCurrentId, eInactive);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_GetAnypalError										*/
/*                                                                                      */
/* Details  -Get status error anypal		 			    				        	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_AnyPalErrorStatus : error status Anypal	                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_errorStatus : error status Anypal							*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
E_AnyPalErrorStatus anypalDisplayHandler_GetAnypalError(void)
{
	return s_errorStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-anypalDisplayHandler_ChargedFull										    */
/*                                                                                      */
/* Details  -Hide icon charging when charged full anypal		 			    		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                            				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_chargingStatus : charging status                           */
/*                     (O) s_counter50Min : charging full status						*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void anypalDisplayHandler_ChargedFull(void)
{
	if(s_isConnectAnypal == true)
	{
		if(s_chargingStatus == eAnyPalStopCharging)
		{
			s_counter50Min++;

			if(s_counter50Min >= TIMER_FULL_CHARGING)
			{
				guihandler_StopTimerChargingAnypal();
				s_counter50Min = 0;
				s_chargingStatus = eAnyPalChargedFull;

				standbyScreen_ShowAnypalCharging();
				operationScreen_ShowAnypalCharging();
				lowAlarmScreen_ShowAnypalCharging();
			}
		}
	}

	return;
}

/* end of files */

