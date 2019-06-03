/********************************************************************/
/*                                                                  */
/* File Name    : ModeMgr.c                              			*/
/*                                                                  */
/* General       : This file contain functions for handle 			*/
/* 				   	    mode operation of FH310                     */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       2017/08/25	   Viet Le				modify 			*/
/*       Change AnypalMgr to AnypalDisplayHandler                   */
/* #0002       2017/08/25	   Viet Le				modify			*/
/*    Handler alarm when alarm active in standby mode(modeMgr_GotoPatient())*/
/* #0003       2017/09/05	   Viet Le				modify			*/
/*    remove #include <assert.h>									*/
/* #0004       2017/09/08	   Viet Le				modify			*/
/*	  - Add send open latching valve to M0 in modeMgr_GotoBatteryAlarmMode()*/
/*    - Add modeMgr_HandlerPowerLost() handler power lost		    */
/* #0005      2017/09/12       Linh Nguyen	  	    modify          */
/*  	Use new save log function                            		*/
/* #0006      2017/09/19       Viet Le		  	    modify          */
/*      - Remove handle case eAlarmExternalFlowPathMode in 			*/
/*			modeMgr_gotoAlarm() function							*/
/*		- Add handle case eAlarmExternalFlowPathMode 				*/
/*			modeMgr_GotoAlarmNotO2Cylinder() function				*/
/*		- Check oxy cylinder status before go to mode in 			*/
/*			modeMgr_RecoverMode() function							*/
/*		- Remove check service screen visible in modeMgr_GotoPatient()*/
/* #0007      2017/09/28       Viet Le		  	    modify          */
/*		- Remove play buzzer start in goto patient mode				*/
/* #0008      2017/10/03       Viet Le		  	    modify          */
/*		- Handle case eEndOfBatteryActionMode : go to standby mode  */
/*			when recover (bug 1946)									*/
/* #0009      2017/10/03       Viet Le		  	    modify          */
/*		- Handle case eEndOfBatteryActionMode in go to standby mode */
/*		  (bug 1946)												*/
/* #0010      2017/10/03       Viet Le		  	    modify          */
/*		- Handle case ePowerErrorMode: check previous mode. 		*/
/*			And return to previous mode when recover (bug 1946)		*/
/* #0011       2017/10/27	   Viet Le				modify			*/
/*	-Handle case push operation SW without 24V, 					*/
/*		system start operation when plug 24V (bug 1999)				*/
/* #0012       2017/11/20	   Viet Le				modify			*/
/* -Edit enumerated type mixed with another type(Coverity)			*/
/* #0013       2018/02/07	   Viet Le				modify			*/
/* -Enable turn off irda when recover mode from alarm mode(bug 2061)*/
/********************************************************************/

#include <ModeMgr.h>
#include "setting.h"
#include "MainScreen.h"
#include "AudioMgr.h"
#include "GuiTask.h"
#include "OperationScreen.h"
#include "OperationScreen.h"
#include "DevelopmentSettingScreen.h"
#include "GuiHandler.h"
#include "EquipmentInforScreen.h"
#include "EquipmentSettingScreen.h"
#include "ConnectedEquipmentInfoScreen.h"
#include "EquipmentCumulativeHourScreen.h"
#include "MonitorScreen.h"
#include "MonitorGraphScreen.h"
#include "CalendarSettingScreen.h"
#include "SoftwareUpgradeScreen.h"
#include "SerialNumberSettingScreen.h"
#include "AlarmInfor.h"
#include "AlarmScreenMgr.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "DebugLog.h"
#include "AnyPalDisplayHandler.h"
#include "Delay.h"

static E_ModeSetting s_currentMode = eStopMode;
static E_ModeSetting s_previousMode = eStopMode;

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoStandby 								    					*/
/*                                                                                      */
/* Details  -Processing device operation when go to stand by mode fromMode?				*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoStandby(void)
{
	uint8_t event = eFinishAudio;
	switch (s_previousMode)
	{
	case eAlarmMode:
	case eAlarmExternalFlowPathMode:
	case eAlarmNoOxyCylinderMode:
	case ePatientMode:
	case eServiceMode:
	case eStopMode:
	case eStandyMode:
	case eBootMode:
	case eEndOfBatteryActionMode:
		s_currentMode = eStandyMode;

		if((s_previousMode != eStopMode) && (s_previousMode != eStandyMode)
				&& (s_previousMode != eBootMode) && s_previousMode != eEndOfBatteryActionMode)
		{
			audioMgr_Request(event, eNoAlarmAudio);
		}

		alarmScreenMgr_Clear();
		guihandler_StopTimerUpdateCumulative();
		guihandler_StopTimerAnimation();
		guihandler_StopTimerSendFRToDevice();

		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		guiInterface_SetSettingValue(eAlarmRecoverId, eLastAlarmId);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterStandbyModeChangeId, s_previousMode);

		break;
	default:
		break;
	}
	s_previousMode = s_currentMode;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoPatient 								    					*/
/*                                                                                      */
/* Details  -Processing device operation when go to patient mode fromMode?				*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoPatient(void)
{
	switch (s_previousMode) {
	case eStandyMode:
	case eServiceMode:
	case ePatientMode:
	case ePowerErrorMode:
		if(s_previousMode != eServiceMode)
		{
			guihandler_StopTimerSendFRToDevice();
		}

		s_currentMode = ePatientMode;

		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterPatientModeChangeId,s_previousMode);

		if(s_previousMode != eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationStartLogId);

			//Update cumulative hour
			operationScreen_restartUpdateCumulativeHour();
			debugLog_StartTimerCheckDebugLog();
		}

		s_previousMode = s_currentMode;
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoService 								    					*/
/*                                                                                      */
/* Details  -Processing device operation when go to service mode fromMode?				*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE													        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoService(void)
{
	switch (s_previousMode)
	{
	case ePatientMode:
		guihandler_StopTimerSendFRToDevice();
		s_previousMode = s_currentMode;
		s_currentMode = eServiceMode;

		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterServiceModeChangeId,s_previousMode);
		s_previousMode = s_currentMode;
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_gotoAlarm 								    						*/
/*                                                                                      */
/* Details  -Processing device operation when go to alarm mode fromMode?				*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_gotoAlarm(void)
{
	switch (s_previousMode) {
	case ePatientMode:
	case eServiceMode:
	case eAlarmNoOxyCylinderMode:
	case eAlarmMode:
		s_currentMode = eAlarmMode;
		guihandler_StopTimerSendFRToDevice();
		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterAlarmModeChangeId,s_previousMode);

		break;
	default:
		break;
	}
	s_previousMode = s_currentMode;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoAlarmExternalFlowPath 								    		*/
/*                                                                                      */
/* Details  -Processing device operation when go to extenal flow path mode fromMode?	*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoAlarmExternalFlowPath(void)
{
	switch (s_previousMode) {
	case eAlarmMode:
	case eAlarmExternalFlowPathMode:
		s_currentMode = eAlarmExternalFlowPathMode;
		guihandler_StopTimerSendFRToDevice();
		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterAlarmExternalFlowPathModeChangeId,s_previousMode);

		break;
	default:
		break;
	}
	s_previousMode = s_currentMode;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoAlarmNotO2Cylinder 								    		*/
/*                                                                                      */
/* Details  -Processing device operation when go to not O2 cylinder mode fromMode?		*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_previousMode : previous mode                               */
/*                     (O) s_currentMode : current mode						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoAlarmNotO2Cylinder(void)
{
	switch (s_previousMode) {
	case ePatientMode:
	case eServiceMode:
	case eAlarmMode:
	case eAlarmExternalFlowPathMode:
	case eAlarmNoOxyCylinderMode:
		s_currentMode = eAlarmNoOxyCylinderMode;
		guihandler_StopTimerSendFRToDevice();
		guiInterface_SetSettingValue(eModeSettingId, s_currentMode);
		guiInterface_SetSettingValue(ePrevModeSettingId,s_currentMode);
		setting_SaveAll();

		guiTask_SendPendingEvent(eEnterAlarmNotO2CylinderModeChangeId,s_previousMode);
		break;
	default:
		break;
	}
	s_previousMode = s_currentMode;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GetCurrentMode 								    				*/
/*                                                                                      */
/* Details  -Get current mode															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_ModeSetting : current mode	                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentMode : current mode							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_ModeSetting  modeMgr_GetCurrentMode(void)
{
	return s_currentMode;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GetPreviousMode 								    				*/
/*                                                                                      */
/* Details  -Get previous mode															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_ModeSetting	: previous mode	                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_previousMode : previous mode							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_ModeSetting  modeMgr_GetPreviousMode(void)
{
	return s_previousMode;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_RecoverMode 								    					*/
/*                                                                                      */
/* Details  -Recover Previous Mode and Send to GUI									    */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) E_ModeSetting eepromMode : - get mode from eeprom					*/
/*						- range : 0-11													*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_RecoverMode(E_ModeSetting eepromMode)
{
	DEBUGOUT("\n eepromMode = %d",eepromMode);
	s_previousMode = eepromMode;

	switch (eepromMode) {
	case eStopMode:
	case eStandyMode:
	case eBootMode:
		modeMgr_GotoStandby();
		WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		break;
	case eServiceMode:
		s_previousMode = ePatientMode;
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationRestartLogId);
		modeMgr_GotoPatient();
		WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		break;
	case ePatientMode:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationRestartLogId);
		modeMgr_GotoPatient();
		WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		break;
	case eAlarmMode:
		/** >>#0013
			Enable turn off irda when recover mode from alarm mode
		#0013 >>**/
		guiTask_EnableIrda(true);

		if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
		{
			modeMgr_gotoAlarm();
		}
		else
		{
			modeMgr_GotoAlarmNotO2Cylinder();
		}

		if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
		{
			ALARM_STAT_T alarmStat;
			alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
			alarmStat.currentStatus = eActive;

			alarmScreenMgr_SetAlarmRecover(alarmStat);

			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
		}
		break;
	case eAlarmExternalFlowPathMode:
		/** >>#0013
			Enable turn off irda when recover mode from alarm external flow path mode
		#0013 >>**/
		guiTask_EnableIrda(true);

		if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
		{
			modeMgr_GotoAlarmExternalFlowPath();
		}
		else
		{
			modeMgr_GotoAlarmNotO2Cylinder();
		}

		if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
		{
			ALARM_STAT_T alarmStat;
			alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
			alarmStat.currentStatus = eActive;

			alarmScreenMgr_SetAlarmRecover(alarmStat);

			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
		}
		break;
	case eAlarmNoOxyCylinderMode:
		/** >>#0013
			Enable turn off irda when recover mode from alarm no oxy cylinder mode
		#0013 >>**/
		guiTask_EnableIrda(true);

		if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
		{
			modeMgr_gotoAlarm();
		}
		else
		{
			modeMgr_GotoAlarmNotO2Cylinder();
		}

		if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
		{
			ALARM_STAT_T alarmStat;
			alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
			alarmStat.currentStatus = eActive;

			alarmScreenMgr_SetAlarmRecover(alarmStat);

			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
		}
		break;
	case eEndOfBatteryActionMode:
		modeMgr_GotoStandby();
		WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
		break;
	case ePowerErrorMode:
	case eBatteryMode:
	{
		s_previousMode = guiInterface_GetSettingValue(ePrevModeSettingId);
		switch(s_previousMode)
		{
		case ePatientMode:
		case eServiceMode:
			s_previousMode = ePatientMode;
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationRestartLogId);
			modeMgr_GotoPatient();
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
			break;
		case eStopMode:
			modeMgr_GotoStandby();
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
			break;
		case eStandyMode:
			if(eepromMode == ePowerErrorMode)
			{
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationRestartLogId);
				modeMgr_GotoPatient();
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
			}
			else
			{
				modeMgr_GotoStandby();
				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
			}
			break;
		case eAlarmMode:
			/** >>#0013
				Enable turn off irda when recover mode from alarm mode
			#0013 >>**/
			guiTask_EnableIrda(true);

			if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
			{
				modeMgr_gotoAlarm();
			}
			else
			{
				modeMgr_GotoAlarmNotO2Cylinder();
			}

			if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
			{
				ALARM_STAT_T alarmStat;
				alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
				alarmStat.currentStatus = eActive;

				alarmScreenMgr_SetAlarmRecover(alarmStat);

				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
			}
			break;
		case eAlarmExternalFlowPathMode:
			/** >>#0013
				Enable turn off irda when recover mode from alarm external flow path mode
			#0013 >>**/
			guiTask_EnableIrda(true);

			if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
			{
				modeMgr_GotoAlarmExternalFlowPath();
			}
			else
			{
				modeMgr_GotoAlarmNotO2Cylinder();
			}

			if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
			{
				ALARM_STAT_T alarmStat;
				alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
				alarmStat.currentStatus = eActive;

				alarmScreenMgr_SetAlarmRecover(alarmStat);

				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
			}
			break;
		case eAlarmNoOxyCylinderMode:
			/** >>#0013
				Enable turn off irda when recover mode from alarm no oxy cylinder mode
			#0013 >>**/
			guiTask_EnableIrda(true);

			if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
			{
				modeMgr_gotoAlarm();
			}
			else
			{
				modeMgr_GotoAlarmNotO2Cylinder();
			}

			if(guiInterface_GetSettingValue(eAlarmRecoverId) != eLastAlarmId)
			{
				ALARM_STAT_T alarmStat;
				alarmStat.ID = (E_AlarmId)guiInterface_GetSettingValue(eAlarmRecoverId);
				alarmStat.currentStatus = eActive;

				alarmScreenMgr_SetAlarmRecover(alarmStat);

				WM_SendMessageNoPara(g_mainWindow,WM_SHOW_ALARMSCREEN);
			}
			break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoBatteryAlarmMode 								    			*/
/*                                                                                      */
/* Details  -Go to batter alarm mode by reset system for go to boot loader				*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoBatteryAlarmMode(void)
{
	guiTask_ClearLatchValveProcessingRequestStatus();
	//Send open latching valve to M0
	guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveOpen);

	//Save setting when dectec 24V
	guihandler_StopTimerSendFRToDevice();
	logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalpoweroutageininoperationLogId);
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetPenddingFlowRate());
	}
	setting_SaveAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_GotoStopMode 								    					*/
/*                                                                                      */
/* Details  -Go to stop mode															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_GotoStopMode(void)
{
	guihandler_StopTimerSendFRToDevice();
	modeMgr_WatchDogReset();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_WatchDogReset 								    					*/
/*                                                                                      */
/* Details  -Watch dog reset															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_WatchDogReset(void)
{
	guiInterface_SetSettingValue(eResetWatchDogFlag, NOT_RESET_WDT);
	setting_SaveAll();
	delay_MS(200);
	/* Initialize WWDT and event router */
	Chip_WWDT_Init(LPC_WWDT);
	Chip_WWDT_SetTimeOut(LPC_WWDT, 0xFF);
	/* Configure WWDT to reset on timeout */
	/* Start watchdog */
	Chip_WWDT_Start(LPC_WWDT);
	/* Enable watchdog interrupt */
	NVIC_EnableIRQ(WWDT_IRQn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-modeMgr_HandlerPowerLost 								    				*/
/*                                                                                      */
/* Details  -Handler power lost															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void modeMgr_HandlerPowerLost(void)
{
	switch(s_currentMode)
	{
	case ePatientMode:
	case eServiceMode:
	case eAlarmMode:
	case eAlarmExternalFlowPathMode:
	case eAlarmNoOxyCylinderMode:
		modeMgr_GotoBatteryAlarmMode();
		break;
	case eStandyMode:
	case eStopMode:
		modeMgr_GotoStopMode();
		break;
	default:
		break;
	}

	return;
}

/* end of files */
