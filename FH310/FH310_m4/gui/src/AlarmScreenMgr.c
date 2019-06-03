/********************************************************************/
/*                                                                  */
/* File Name    : AlarmScreenMgr.c	                                */
/*                                                                  */
/* General      : Alarm screen manager 				                */
/* 								                                    */
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
/* #0001       2017/08/25		Viet Le				modify			*/
/*   handle alarm in standby mode (function : alarmScreenMgr_Show())*/
/* #0002       2017/08/25       Viet Le             modify          */
/*   change anypalMgr_GetAnypalError()->anypalDisplayHandler_GetAnypalError()*/
/* #0003       2017/08/30       Viet Le             modify          */
/*  compare alarm inactive with current alarm (alarmScreenMgr_Set())*/
/*  remove line s_currentAlarmId = eLastAlarmId (alarmScreenMgr_Show)*/
/* #0004       2017/09/05       Viet Le             modify          */
/*  remove #include "assert.h"                                      */
/* #0005       2017/09/16		Viet Le				modify			*/
/*			-change mode mgr design, add function to get high alarm */
/*			in standby mode, set alarm when recover mode and check  */
/*			whether theres is a alarm active when back to home screen*/
/*			-Check current mode and show alarm when receive alarm    */
/*			status change from Alarm task							*/
/* #0006       2017/09/19		Viet Le				modify			*/
/* 			-Handle nonvolatile memory alarm in alarmScreenMgr_Set()*/
/*				 function (bug 1879)								*/
/*			-Remove save eAlarmRecoverId to EEPROM in 				*/
/*				alarmScreenMgr_Show() function						*/
/*			-Check oxy cylinder status before goto mode	in 			*/
/*				alarmScreenMgr_Set()								*/
/*			-Send event show alarm screen to mainwindow when alarm  */
/*             inactive in alarmScreenMgr_Set() (bug 1877)			*/
/*         	-Remove handle case eNegativePressureId					*/
/* #0007       2017/10/14		Viet Le				modify			*/
/*			-Add function clear flag high alarm in standby(bug 1976)*/
/* #0008       2017/10/26		Viet Le				modify			*/
/*			-Start timer update animation on operation screen when  */
/*				alarm is inactive (bug 1998)						*/
/* #0009       2017/11/17		Viet Le				modify			*/
/*		-Handle case when alarm cannula active during alarm O2 		*/
/*	concentration lowering, stop LED(bug 2033) 						*/
/* #0010       2017/12/01		Viet Le				modify			*/
/*	-Handle flag blinking flow rate when alarm active(bug 2046)   	*/
/********************************************************************/

#include "string.h"
#include "WM.h"
#include "AlarmScreenMgr.h"
#include "DevelopmentSettingScreen.h"
#include "PasswordDialog.h"
#include "GuiHandler.h"
#include "HighAlarmScreen.h"
#include "MediumAlarmScreen.h"
#include "LowAlarmScreen.h"
#include "AlarmInfor.h"
#include "ModeMgr.h"
#include "MainScreen.h"
#include "setting.h"
#include "AudioMgr.h"
#include "OperationScreen.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "AnyPalDisplayHandler.h"

static E_AlarmId s_AlarmList[eLastAlarmId + 1];
static E_AlarmId s_currentAlarmId = eLastAlarmId;
static E_AlarmId s_preAlarmId = eLastAlarmId;
static E_AlarmId s_alarmActiveId = eLastAlarmId;
static bool s_isAlarmExternFlowPath = false;
static bool s_isAlarmScreen = false;
static E_AlarmId s_alarmInStandby = eLastAlarmId;

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Init 								       					*/
/*                                                                                      */
/* Details  -Init alarm screen manager													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_AlarmList : alarm list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_Init(void)
{
	for(int i = 0 ; i < eLastAlarmId; i++)
	{
		s_AlarmList[i] = eLastAlarmId;
	}

	highAlarmScreen_Init();
	mediumAlarmScreen_Init();
	lowAlarmScreen_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Show								       					*/
/*                                                                                      */
/* Details  -Show alarm screen depend on individual alarm								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentAlarmId : current alarm							    */
/*                     (O) s_AlarmList : alarm list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_Show(void)
{
	for(int i = 0; i < eLastAlarmId; i++)
	{
		if(s_AlarmList[i] != eLastAlarmId)
		{
			if(developmentSettingScreen_CheckEnable(s_AlarmList[i]) == true)
			{
				if(s_AlarmList[i] < s_currentAlarmId)
				{
					if(modeMgr_GetCurrentMode() == eStandyMode)
					{
						switch(s_AlarmList[i])
						{
						case eCompressorDCOverVoltageId:
						case eCompressorSoftwareOverCurrentId:
						case eCompressorHardwareOverCurrent:
						case eCompressorBoardTemperatureId:
						case eCompressorRPMId:
						case eFlowControllerCommunicationId:
						case eFlowControllerPressureSensorId:
						case eFlowControllerUltrasonicSensorId:
						case eFlowControllerTemperatureSensorId:
						case eFlowControllerCommandTimeOutId:
							break;
						default:
							s_currentAlarmId = s_AlarmList[i];
							break;
						}
					}
					else
					{
						s_currentAlarmId = s_AlarmList[i];
					}
				}
			}
		}
	}

	/****************************************************/
	if(s_currentAlarmId != s_preAlarmId)
	{
		guiTask_SetFlagEnableChangeFR(false);
		guiTask_SetFlagOnOff(true);

		passwordDialog_Hide();
		s_preAlarmId = s_currentAlarmId;
		guihandler_StopTimerAnimationAlarm();
		s_isAlarmScreen = true;
		guiTask_SetBlinkingFR(false);

		switch(s_currentAlarmId)
		{
		case eTemperatureId:
		case eHighPressureId:
		case eLowPressureId:
		case eCompressorBoardTemperatureId:
		case eCompressorRPMId:
		case eCoolingFanId:
		case eO2ConcentrationId:
		case eFlowRateId:
		case eCompressorOutletTemperatureId:
		case eFlowControllerCommunicationId:
		case eFlowControllerPressureSensorId:
		case eFlowControllerUltrasonicSensorId:
		case eFlowControllerTemperatureSensorId:
		case eFlowControllerCommandTimeOutId:
		case eNonvolatileMemoryId:
		case eCompressorDCOverVoltageId:
		case eCompressorSoftwareOverCurrentId:
		case eCompressorHardwareOverCurrent:
			lowAlarmScreen_Hide();
			mediumAlarmScreen_Hide();
			highAlarmScreen_Show(s_currentAlarmId);
			break;
		case eLoweringO2ConcentrationId:
		case eO2ConcentrationSensorId:
		case eRisingCompressorOutletTemperatureId:
			s_alarmActiveId = s_currentAlarmId;
			lowAlarmScreen_Hide();
			mediumAlarmScreen_Show(s_currentAlarmId);
			break;
		case eCannulaUnder27sId:
		case eCannulaMore27sId:
		case eFilterCleaningId:
		case eSpeakerId:
		case eExternalFlowPathpressureId:
		case eAnypalBatterId:
		case eAnypalRTCId:
		case eAnypalCommunicationId:
		case eOverCurrentId:
			s_alarmActiveId = s_currentAlarmId;

			//Handle when cannula alarm active
			if((s_currentAlarmId == eCannulaUnder27sId) ||
					(s_currentAlarmId == eCannulaMore27sId))
			{
				//Stop LED
				guiTask_SendPendingEvent(eStopLedAlarmChangeID,eOn);
			}

			lowAlarmScreen_Show(s_currentAlarmId);
			guiTask_SetFlagEnableChangeFR(true);
			break;
		case eLastAlarmId:
		{
			if(s_alarmActiveId != eLastAlarmId)
			{
				guiTask_SetFlagEnableChangeFR(true);
				guihandler_StopTimerAnimationAlarm();
				audioMgr_StopTimerAudio(s_alarmActiveId);

				//Start timer animation in opreation
				guihandler_StartTimerAnimation();

				alarmScreenMgr_SendToDevice(s_alarmActiveId,eInactive);
				guiTask_SendPendingEvent(eStopLedAlarmChangeID,eOn);

				if(guiTask_GetFlagServiceScreen() == false)
				{
					WM_SendMessageNoPara(g_mainWindow, WM_SHOW_HOMESCREEN);
				}

				s_alarmActiveId = eLastAlarmId;
				alarmInfor_SetInforAlarm(0);
			}
			break;
		}
		default:
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Hide 								       					*/
/*                                                                                      */
/* Details  -Hide alarm screen depend on individual alarm								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE															*/
/*                     (O) s_isAlarmScreen : flag check visible alarm screen            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_Hide(void)
{
	if(s_isAlarmScreen == true)
	{
		guihandler_StopTimerAnimationAlarm();
		s_isAlarmScreen = false;

		highAlarmScreen_Hide();
		mediumAlarmScreen_Hide();
		lowAlarmScreen_Hide();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_SendToDevice 								       		    */
/*                                                                                      */
/* Details  -Send alarm to device														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : -current alarm 						 			*/
/*				   -range : 0-31														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*				   E_AlarmStatus status	: -current status for alarm						*/
/*				   -range : 0-1															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void									                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_SendToDevice(E_AlarmId alarmId, E_AlarmStatus status)
{
	IPC_MSG_DATA_t data;
	data.data_id = eAlarmMGSId;
	data.data_value = 0;
	data.stAlarmStatPacket.ID = alarmId;
	data.stAlarmStatPacket.currentStatus = status;
	memset(data.version,0,sizeof(data.version));

	if (ipc_PushMsg(IPCEX_ID_DEVICE, data) == QUEUE_INSERT)
	{
		DEBUGOUT("\n send alarm to device = %d--%d", alarmId, status);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_GetFlagAlarmScreen 								       		*/
/*                                                                                      */
/* Details  -Get flag alarm screen														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if true is visible, else is invisible	                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isAlarmScreen : flag check alarm screen					*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool alarmScreenMgr_GetFlagAlarmScreen(void)
{
	return s_isAlarmScreen;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_SetFlagAlarmScreen 								       		*/
/*                                                                                      */
/* Details  -Set flag alarm screen														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag : - flag set visible alarm screen							*/
/*				   -range : 0-1															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isAlarmScreen : flag check visible alarm screen            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_SetFlagAlarmScreen(bool flag)
{
	s_isAlarmScreen = flag;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Set 								       					*/
/*                                                                                      */
/* Details  -Set alarm list when alarm activate or end alarm							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) ALARM_STAT_T alarmStat : current alarm stat					 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void									                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_AlarmList : alarm list                                     */
/*                     (O) s_isAlarmExternFlowPath : flag check alarm extern flow path	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_Set(ALARM_STAT_T alarmStat)
{
	if(alarmStat.currentStatus == eActive)
	{
		if(developmentSettingScreen_CheckEnable(alarmStat.ID) == true)
		{
			s_AlarmList[alarmStat.ID] = alarmStat.ID;

			if(alarmStat.ID == eExternalFlowPathpressureId)
			{
				s_isAlarmExternFlowPath = true;

				highAlarmScreen_HideO2Cylinder();
				operationScreen_HideO2Cylinder();
			}

			switch(alarmStat.ID)
			{
			case eNonvolatileMemoryId:
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				break;
			case eCompressorDCOverVoltageId:
			case eCompressorSoftwareOverCurrentId:
			case eCompressorHardwareOverCurrent:
			case eCompressorBoardTemperatureId:
			case eCompressorRPMId:
			case eFlowControllerCommunicationId:
			case eFlowControllerPressureSensorId:
			case eFlowControllerUltrasonicSensorId:
			case eFlowControllerTemperatureSensorId:
			case eFlowControllerCommandTimeOutId:
				if(modeMgr_GetCurrentMode() == ePatientMode ||
						modeMgr_GetCurrentMode() == eServiceMode)
				{
					if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
					{
						modeMgr_gotoAlarm();
					}
					else
					{
						modeMgr_GotoAlarmNotO2Cylinder();
					}
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				}
				else if(modeMgr_GetCurrentMode() == eStandyMode)
				{
					s_alarmInStandby = alarmStat.ID;
				}
				break;
			case eTemperatureId:
			case eHighPressureId:
			case eLowPressureId:
			case eCoolingFanId:
			case eO2ConcentrationId:
			case eFlowRateId:
			case eCompressorOutletTemperatureId:
				if(modeMgr_GetCurrentMode() == ePatientMode ||
						modeMgr_GetCurrentMode() == eServiceMode)
				{
					if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
					{
						modeMgr_gotoAlarm();
					}
					else
					{
						modeMgr_GotoAlarmNotO2Cylinder();
					}
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				}
				break;
			case eLoweringO2ConcentrationId:
			case eO2ConcentrationSensorId:
			case eRisingCompressorOutletTemperatureId:
				if(modeMgr_GetCurrentMode() == ePatientMode ||
						modeMgr_GetCurrentMode() == eServiceMode)
				{
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				}
				break;
			case eCannulaUnder27sId:
			case eCannulaMore27sId:
			case eFilterCleaningId:
			case eSpeakerId:
			case eExternalFlowPathpressureId:
				if(modeMgr_GetCurrentMode() == ePatientMode ||
						modeMgr_GetCurrentMode() == eServiceMode)
				{
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				}
				break;
			case eAnypalBatterId:
			case eAnypalRTCId:
			case eAnypalCommunicationId:
			case eOverCurrentId:
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
				break;
			default:
				break;
			}
		}
	}
	else
	{
		s_AlarmList[alarmStat.ID] = eLastAlarmId;

		if(alarmStat.ID == s_currentAlarmId)
		{
			s_currentAlarmId = eLastAlarmId;

			//Check remain alarm active
			if(alarmScreenMgr_RemainAlarm() == true)
			{
				switch(alarmStat.ID)
				{
				case eLoweringO2ConcentrationId:
				case eExternalFlowPathpressureId:
				case eRisingCompressorOutletTemperatureId:
				case eCannulaMore27sId:
					audioMgr_StopTimerAudio(alarmStat.ID);
					alarmScreenMgr_SendToDevice(alarmStat.ID,eInactive);
					guiTask_SendPendingEvent(eStopLedAlarmChangeID,eOn);
					break;
				default:
					break;
				}
			}
			//End check alarm active

			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Clear 	 								       				*/
/*                                                                                      */
/* Details  -Clear alarm list when turn off machine										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_AlarmList : alarm list                                     */
/*                     (O) s_isAlarmExternFlowPath : flag check alarm flow path			*/
/*                     (O) s_currentAlarmId : current alarm                             */
/*                     (O) s_preAlarmId : previous alarm                                */
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_Clear(void)
{
	for(int i = 0 ; i < eLastAlarmId; i++)
	{
		s_AlarmList[i] = eLastAlarmId;
	}

	s_alarmInStandby = eLastAlarmId;
	s_isAlarmExternFlowPath = false;
	s_currentAlarmId = eLastAlarmId;
	s_preAlarmId = eLastAlarmId;
	s_alarmActiveId = eLastAlarmId;

	if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
	{
		switch(anypalDisplayHandler_GetAnypalError())
		{
		case eAnyPalOverCurrentAbnormal:
			s_currentAlarmId = eOverCurrentId;
			break;
		case eAnyPalBatteryAbnormal:
			s_currentAlarmId = eAnypalBatterId;
			break;
		case eAnyPalRTCAbnormal:
			s_currentAlarmId = eAnypalRTCId;
			break;
		case eAnyPalCommAbnormal:
			s_currentAlarmId = eAnypalCommunicationId;
			break;
		default:
			break;
		}

		s_AlarmList[s_currentAlarmId] = s_currentAlarmId;

		s_preAlarmId = s_currentAlarmId;
		s_alarmActiveId = s_currentAlarmId;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_GetCurrentAlarmId 	 								       	*/
/*                                                                                      */
/* Details  -Get current alarm Id														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_AlarmId		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentAlarmId : current alarm							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmId alarmScreenMgr_GetCurrentAlarmId(void)
{
	return s_currentAlarmId;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_GetPrevAlarmId	 								       		*/
/*                                                                                      */
/* Details  -Get previous current alarm Id												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 												*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_AlarmId		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_preAlarmId : previous alarm							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmId alarmScreenMgr_GetPrevAlarmId(void)
{
	return s_preAlarmId;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_SetPrevALarmId								       		    */
/*                                                                                      */
/* Details  -Set previous alarm															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId prevAlarmId :  - alarm id							 		*/
/*				   -range : 0-31														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_preAlarmId : previous alarm                                */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_SetPrevALarmId(E_AlarmId prevAlarmId)
{
	s_preAlarmId = prevAlarmId;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_UpdateAnimation	 								       	    */
/*                                                                                      */
/* Details  -Update animation in alarm													*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE												 		   			*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentAlarmId : current alarm							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_UpdateAnimation(void)
{
	switch(s_currentAlarmId)
	{
	case eCannulaUnder27sId:
	case eCannulaMore27sId:
	{
		lowAlarmScreen_UpdateIconTube();
		lowAlarmScreen_UpdateCannulaAnimation();
		break;
	}
	case eLoweringO2ConcentrationId:
	case eO2ConcentrationSensorId:
	case eRisingCompressorOutletTemperatureId:
		mediumAlarmScreen_UpdateMediumAnimation();
		break;
	case eFilterCleaningId:
	case eSpeakerId:
	case eExternalFlowPathpressureId:
		lowAlarmScreen_UpdateLowAnimation();
		break;
	case eAnypalBatterId:
	case eAnypalRTCId:
	case eAnypalCommunicationId:
	case eOverCurrentId:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			lowAlarmScreen_UpdateLowAnimation();
		}
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_GetAlarmExternFlowPath	 								    */
/*                                                                                      */
/* Details  -Get flag alarm flow path pressure											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE												 		   			*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isAlarmExternFlowPath : flag check alarm flow path			*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool alarmScreenMgr_GetAlarmExternFlowPath(void)
{
	return s_isAlarmExternFlowPath;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_RemainAlarm	 								    			*/
/*                                                                                      */
/* Details  -Check remain alarm in list													*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE												 		   			*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if remain is true, else is false	        				    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_AlarmList : list alarm										*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool alarmScreenMgr_RemainAlarm(void)
{
	for(int i = 0 ; i < eLastAlarmId; i++)
	{
		if(s_AlarmList[i] != eLastAlarmId)
		{
			return true;
		}
	}

	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_GetHighAlarmInStandby	 								    */
/*                                                                                      */
/* Details  -Get high alarm	in standby mode												*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE												 		   			*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_AlarmId : Alarm id							        				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_alarmInStandby : flag high alarm in standby mode			*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmId alarmScreenMgr_GetHighAlarmInStandby(void)
{
	return s_alarmInStandby;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_ClearHighAlarmInStandby	 								    */
/*                                                                                      */
/* Details  -Clear high alarm in standby mode										    */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE												 		   			*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void											        				*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE												 			*/
/*                     (O) s_alarmInStandby : flag high alarm in standby mode           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_ClearHighAlarmInStandby(void)
{
	s_alarmInStandby = eLastAlarmId;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreenMgr_Set 								       					*/
/*                                                                                      */
/* Details  -Set alarm list when recover												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) ALARM_STAT_T alarmStat : current alarm stat					 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void									                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_AlarmList : alarm list                                     */
/*                     (O) NONE															*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void alarmScreenMgr_SetAlarmRecover(ALARM_STAT_T alarmStat)
{
	if(alarmStat.currentStatus == eActive)
	{
		s_AlarmList[alarmStat.ID] = alarmStat.ID;
	}

	return;
}

/* end of files */
