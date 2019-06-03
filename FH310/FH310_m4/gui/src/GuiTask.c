/********************************************************************/
/*                                                                  */
/* File Name    : GuiTask.c                              			*/
/*                                                                  */
/* General       : Create RTOS task for control and monitor 		*/
/* 				   device operation	                                */
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
/* #0002	   2017/08/30	   Quyen Ngo			modify			*/
/*	  - Add realTimeClock_ProcessSetTimeRequest to gui loop			*/
/* #0003	   2017/08/30	   Viet Le				modify			*/
/*	  - Add Send threshold of temp and O2 to M0(guiTask_InitFunc())	*/
/* #0004	   2017/09/05	   Viet Le				modify			*/
/*	  - Remove #include "assert.h"									*/
/* #0005	   2017/09/05	   Viet Le				modify			*/
/*	  - Add Send threshold of temp and O2 to M0 by  select port		*/
/*		(guiTask_CheckPSASelectPort())								*/
/*	  - Remove Send threshold of temp and O2 to M0					*/
/*      (guiTask_InitFunc())										*/
/*    - Edit operationScreen_ShowO2Cylinder()->						*/
/*      operationScreen_CheckAndShowO2Cylinder()					*/
/*    - Edit anypalDisplayHandler_SetAnypalError()                  */
/*      anypalDisplayHandler_CheckAndSetError()						*/
/*	  - Add flag accept record log of flow controller (bug 1817)	*/
/* #0006	   2017/09/08	   Viet Le				modify			*/
/*	  - Add flag s_latchValveCloseFinish and s_logPoweroutSaved		*/
/*	  - Add guiTask_CheckAndResetSystemInPowerLost() check latching */
/*      valve close finish and record log power after reset system  */
/*	  - Edit event eGotoBatteryAlarmMode->ePowerLostDetected in     */
/*      guiTask_HandleMonitorData()									*/
/*	  - Remove event eGotoBatteryStopMode in guiTask_HandleMonitorData()*/
/*	  - Edit handler event eResetWDTEventId in guiTask_ProcessEvent()*/
/*	  - Add check latching valve close finish in guiTask_HandleMonitorData()*/
/* #0007       2017/09/16		Viet Le				modify			*/
/*			-change mode mgr design, add call emwin event to show   */
/*			the matching screen when mode change					*/
/*  #0007      2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/* #0008       2017/09/19		Viet Le				modify			*/
/*  -Remove handle case eNegativePressureId							*/
/*  -Remove guiTask_GetFlagAlarmNVRam() (not use)					*/
/* #0009       2017/09/21		Viet Le				modify			*/
/*	-Check if service screen visible not send event 				*/
/*		show home to mainwindow	(bug 1881)							*/
/* #0010      2017/09/19       Linh Nguyen	  	    modify          */
/*  Use send monitor data function                         			*/
/* #0011       2017/09/22		Viet Le				modify			*/
/*  -Handle record log flag in case go to patient mode/go to 		*/
/*  standby mode by push turn on/off button on irda remote(bug 1890)*/
/* #0012	  2017/09/27       Quyen Ngo			modify			*/
/*	-remove wdt_SwitchToTaskObservation function					*/
/* 	-remove code not use											*/
/* #0013       2017/09/28		Viet Le				modify			*/
/*	- Add flag s_enableIrda and guihandler_StartTimerEnableIrda()   */
/*		function (bug 1918)											*/
/*	- Check flag s_enableIrda in guiTask_IrdaHandle() 				*/
/*		function (bug 1918)											*/
/* - Remove check alarm speaker in guiTask_IrdaHandle() 			*/
/*    and guiTask_HandleMonitorData() 								*/
/*  #0014	   2017/10/06      Viet Le  			modify	        */
/*		-Edit modeMgr_WatchDogReset()-> guiInterface_WatchDogReset()*/
/*			(bug 1957)												*/
/*  #0015	   2017/10/06      Viet Le  			modify	        */
/*		-Edit modeMgr_HandlePowerLost()-> guiInterface_HandlerPowerLost()*/
/*			(bug 1957)												*/
/*  #0016	   2017/10/06      Viet Le  			modify	        */
/*		-Remove flag record log flow controller error. Remove check */
/*			mode when record log flow controller error.	(bug 1958)	*/
/*  #0017	   2017/10/06      Viet Le  			modify	        */
/*		-Remove lcd_TouchHandle() function in guiTask_Func()		*/
/*	#0018	   2017/10/14	   Quyen Ngo			modify			*/
/*		-Add call lcd_TouchHandle() in guiTask_Func for back to 	*/
/* 		touch handle on V010144										*/
/*  #0019	   2017/10/14      Viet Le  			modify	        */
/*		-Record log before show alarm screen when alarm active in   */
/*			guiTask_HandleMonitorData() function (bug 1976)			*/
/*  #0020  	   2017/10/20	  Viet Le			    modify			*/
/*      -Change setting o2 cylinder to "exist" when                 */
/*			external flow path pressure is 90 kPa or more(bug 1990) */
/*  #0021       2017/10/27	   Viet Le				modify			*/
/*		-Add function : guiTaskEDIT_SetFont(),guiTaskTEXT_SetFont	*/
/*			guiTaskRADIO_SetFont(),guiTaskLISTVIEW_SetFont()		*/
/*			guiTaskMULTIPAGE_SetFont().(bug 1997)					*/
/*  #0022	   2017/10/28      Viet Le 	    	        modify	    */
/*	  Remove record log alarm cannula more 27seconds(bug 2005)		*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0023 Add the dump SPI flash memory functionality (bug #1992)	*/
/* #0024	  2017/11/07	  Viet Le				    modify		*/
/*	  Add case eCompressorDCOverVoltageId,							*/
/*	eCompressorSoftwareOverCurrentId,eCompressorHardwareOverCurrent */
/*  in guiTask_PlayFlowRate() function (bug 2014)    				*/
/* #0025	  2017/11/09	  Viet Le				    modify		*/
/*	Update oxy cylinder in high alarm screen when status change		*/
/*		(bug 2020)													*/
/* #0026       2017/11/10      Linh Nguyen	         modify			*/
/*	  Start timer s_RTCUpdateTimer 									*/
/* #0027       2017/11/16	   Viet Le				modify			*/
/*  - Remove case eCannulaUnder27sId when play flow rate (confirm). */
/*	Handle case eCannulaMore27sId when play flow rate(confirm)		*/
/*		(bug 1997 + 2024)											*/
/* #0028       2017/11/17       Viet Le 			modify			*/
/*	- Remove highAlarmScreen_SetExtFlowPath() (Coverity)			*/
/* #0029       2017/11/17       Viet Le				modify			*/
/*	- Remove function operationScreen_SetExtFlowPath() (coverity)	*/
/* #0030       2017/12/01	   Viet Le				modify			*/
/*	 -Edit fragile description on guiTask_ChangeFlowRate()(bug 2045)*/
/* #0031       2017/12/01	   Viet Le				modify			*/
/*	 -Add flag s_blinkingFR, function guiTask_SetBlinkingFR().		*/
/*	   Play flow rate when press preset 1,2 or 3 on remote.			*/
/*   Set s_blinkingFR = false when turn off system (bug 2046)		*/
/* #0032       2017/12/07	   Viet Le				modify			*/
/*	- Add check flag service screen when change flow rate during	*/
/*    alarm active (bug 2055)										*/
/* #0033	   2018/01/18      Viet Le 	    	        modify	    */
/*	 When user press confirm :										*/
/*	 -child lock is on play current flow rate and 					*/
/*			s_penddingFlowRate = s_flowrateSetting					*/
/*	 -child lock is off : when user change flow rate if user confirm*/
/* before 1 seconds play pendding flow rate, else user confirm after*/
/* 1 seconds play current flow rate(bug 2057)						*/
/* #0034	   2018/02/08      Viet Le 	    	        modify	    */
/*	 -Add flag s_isUpdatePsaName accept update psa select name. 	*/
/* After receive PSA request parameter at start from M0 set 		*/
/* flag s_isUpdatePsaName = true. Turn off system set 				*/
/*	s_isUpdatePsaName = false(bug 2062)								*/
/*	 -Check flag s_isUpdatePsaName before update psa Parameter and	*/
/*		 send to device(bug 2062)									*/
/* #0035	   2018/02/26      Viet Le 	    	        modify	    */
/*	 -Handler event : update animation,	send flow rate to device,	*/
/*	 update calendar and cumulative hour,hide icon charging	anypal	*/
/*  	(bug 2065)													*/
/* #0036	   2018/02/13      Viet Le 	    	        modify	    */
/*	 -Add function guiTask_ConfirmFlowRate() handler when confirm   */
/*		flow rate(bug 2068)											*/
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include <EEPROM.h>
#include <RealTimeClock.h>
#include <StringTools.h>
#include "GuiTask.h"
#include "MainScreen.h"
#include "EquipmentInforScreen.h"
#include "OperationScreen.h"
#include "FunctionTestPageOneScreen.h"
#include "FunctionTestPageThreeScreen.h"
#include "SettingButton.h"
#include "SettingOnOffButton.h"
#include "SettingOpenCloseButton.h"
#include "PSATableMgr.h"
#include "setting.h"
#include "DataDisplay.h"
#include "MonitorScreen.h"
#include "AnyPalDisplayHandler.h"
#include "AudioMgr.h"
#include <TaskCommon.h>
#include <UsbMgr.h>
#include "SettingExistNoButton.h"
#include "timers.h"
#include "ModeMgr.h"
#include "MonitorGraphScreen.h"
#include "SerialNumberSettingScreen.h"
#include "ConnectedEquipmentInfoScreen.h"
#include "EquipmentSettingScreen.h"
#include "RADIO.h"
#include "PasswordDialog.h"
#include "CompositionMgr.h"
#include "LogDataScreen.h"
#include "SoftwareUpgradeScreen.h"
#include "GuiHandler.h"
#include "DevelopmentSettingScreen.h"
#include "DevelopmentSettingScreenPageThree.h"
#include "DevelopmentSettingScreenPageFour.h"
#include "AlarmScreenMgr.h"
#include "LowAlarmScreen.h"
#include "HighAlarmScreen.h"
#include "StandbyScreen.h"
#include "BluetoothTask.h"
#include "DebugLog.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "../system/WdtHandle.h"
#include "BluetoothInterface.h"
#include "Lcd.h"
/** >>#0035
	include EquipmentCumulativeHourScreen.h
#0035 >>**/
#include "EquipmentCumulativeHourScreen.h"

QueueHandle_t g_guiQueue;
static E_O2ConcentratorType s_conditionO2 = eNormalO2Type;
static E_TemperatureType s_conditionTemp = eNormalTempType;
static E_PsaSelectName s_psaSelectName = LOW_TEMP_NORM_02;
static E_PsaPortSelect s_selectPort = EEPROM_SELECT;
static E_O2CylinStatus s_O2CylinStatus = O2_CYLIN_NOTATTACH;
static bool s_isServiceScreen = false;
static bool s_isEnableChangeFlowRate = false;
static bool s_isEnableOnOff = false;
static PSA_MONITOR_PACKET_t s_realMonitorData;
static uint8_t s_flowrateSetting = 0;
static uint8_t s_penddingFlowRate = 0;
static bool s_flagAlarmNVRam = false;
static bool s_latchValveCloseFinish = false;
static bool s_logPoweroutSaved = false;
static bool s_enableIrda = false;
static bool s_blinkingFR = false;
/** >>#0034
	Add flag accept update psa select name
#0034 >>**/
static bool s_isUpdatePsaName = false;

/** >>#0036
	Add function handler confirm flow rate
#0036 >>**/
/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_ConfirmFlowRate						                        	*/
/*                                                                                      */
/* Details  -Handler when user confirm flow rate      									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                    					*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_flowrateSetting : flow rate setting						*/
/*                     (O) s_penddingFlowRate : pendding flow rate setting              */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void guiTask_ConfirmFlowRate(void)
{
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		if(standbyScreen_GetStateMachine() == eOperating)
		{
			/** >>#0033
				When user change flow rate on child lock is off, if user confirm before 1 seconds
				then after 1 seconds not send flow rate actual to device.
				Check child lock is on then s_penddingFlowRate = s_flowrateSetting.
			#0033 >>**/
			if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
			{
				guiTask_PlayFlowRate(s_flowrateSetting);
				operationScreen_ConfirmChildLock(s_flowrateSetting);
				lowAlarmScreen_ConfirmChildLock(s_flowrateSetting);
				equipmentInforScreen_ConfirmChildLock(s_flowrateSetting);
				s_penddingFlowRate = s_flowrateSetting;
			}
			else//Child lock off
			{
				/** >>#0033
					Play flow rate previous when confirm before 1 seconds after change flow rate
				#0033 >>**/
				if(s_penddingFlowRate != s_flowrateSetting)
				{
					//Play flow rate previous
					guiTask_PlayFlowRate(s_penddingFlowRate);
				}
				else
				{
					//Play flow rate
					guiTask_PlayFlowRate(s_flowrateSetting);
				}
			}
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetAlarm						                        			*/
/*                                                                                      */
/* Details  -Set alarm Nonvolatile memory      											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId ID : - id of alarm											*/
/*                    - range :  0-31                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*                 E_AlarmStatus status : - status alarm	                            */
/*                    - range : 0-1                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetAlarm(E_AlarmId ID, E_AlarmStatus status)
{
	ALARM_STAT_T alarmStat;
	alarmStat.ID = ID;
	alarmStat.currentStatus = status;
	alarmStat.previousStatus = eInactive;

	alarmScreenMgr_Set(alarmStat);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_writeAlarmLog						                                */
/*                                                                                      */
/* Details  -Write alarm log	      												    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - id of alarm									*/
/*                    - range :  0-31                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_writeAlarmLog(E_AlarmId alarmId)
{
	switch(alarmId)
	{
	case eMCUClockId:
		break;
	case eNonvolatileMemoryId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalNonvolatileMemoryLogId);
		break;
	case ePowerOutageInInOperationId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalpoweroutageininoperationLogId);
		break;
	case ePressedOperationSWDuringUnplugId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalpressedoperationswduringunplugLogId);
		break;
	case eTemperatureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalTemperatureLogId);
		break;
	case eHighPressureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalHighPressureLogId);
		break;
	case eLowPressureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalLowPressureLogId);
		break;
	case eCompressorBoardTemperatureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCompressorBoardTemperatureLogId);
		break;
	case eCompressorRPMId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eCompressorRpmErrorLogId);
		break;
	case eFlowControllerCommunicationId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalFlowControllerCommunicationLogId);
		break;
	case eFlowControllerPressureSensorId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalFlowControllerPressureSensorLogId);
		break;
	case eFlowControllerUltrasonicSensorId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalUltrasonicControllerPressureSensorLogId);
		break;
	case eFlowControllerTemperatureSensorId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalTemperatureControllerPressureSensorLogId);
		break;
	case eCoolingFanId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCoolingFanLogId);
		break;
	case eO2ConcentrationId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalO2ConcentrationLogId);
		break;
	case eCompressorOutletTemperatureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCompressorOutletTemperatureLogId);
		break;
	case eFlowRateId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalFlowRateLogId);
		break;
	case eRisingCompressorOutletTemperatureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalRisingCompressorOutletTemperatureLogId);
		break;
	case eO2ConcentrationSensorId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalO2ConcentrationSensorLogId);
		break;
	case eCannulaUnder27sId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCannulaLogId);
		break;
	case eLoweringO2ConcentrationId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalLoweringO2ConcentrationLogId);
		break;
	case eExternalFlowPathpressureId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalExternalFlowPathPressureLogId);
		break;
	case eFilterCleaningId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalFilterCleaningModuleLogId);
		break;
	case eSpeakerId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalSpeakerLogId);
		break;
	case eAnypalBatterId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalAnypalBatteryLogId);
		break;
	case eAnypalRTCId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalAnypalRtcLogId);
		break;
	case eAnypalCommunicationId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalAnypalCommunicationLogId);
		break;
	case eOverCurrentId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOverCurrentIsDetectedLogId);
		break;
	case eFlowControllerCommandTimeOutId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerCommandTimeOutAbnormalId);
		break;
	case eCompressorDCOverVoltageId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCompressorDCOverVoltageLogId);
		break;
	case eCompressorSoftwareOverCurrentId:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCompressorSoftwareOverCurrentLogId);
		break;
	case eCompressorHardwareOverCurrent:
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAbnormalCompressorHardwareOverCurrentLogId);
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_ChangeFlowRate						                           	    */
/*                                                                                      */
/* Details  -Change flow rate by encoder or remote										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int step : - step change					 							*/
/*                    - range : 0-10                                                    */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isServiceScreen : flag check visible service screen		*/
/*                     (O) s_flowrateSetting : flow rate setting                        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_ChangeFlowRate(int step)
{
	if(step > 0)
	{
		if(s_flowrateSetting >= SETTING_FR_300L)
		{
			return;
		}

		for(int i = 0; i < step; i++)
		{
			if(s_flowrateSetting < SETTING_FR_200L)
			{
				s_flowrateSetting += SETTING_FR_025L;
			}
			else if (s_flowrateSetting < SETTING_FR_300L)
			{
				s_flowrateSetting += SETTING_FR_050L;
			}
		}
	}
	else
	{
		if(s_flowrateSetting <= SETTING_FR_025L)
		{
			return;
		}

		step = abs(step);

		for(int i = 0; i < step; i++)
		{
			if(s_flowrateSetting <= SETTING_FR_200L)
			{
				s_flowrateSetting = s_flowrateSetting - SETTING_FR_025L;
			}
			else if(s_flowrateSetting <= SETTING_FR_300L)
			{
				s_flowrateSetting = s_flowrateSetting - SETTING_FR_050L;
			}

			if(s_flowrateSetting < SETTING_FR_025L)
			{
				s_flowrateSetting = SETTING_FR_025L;
			}
		}
	}

	if(alarmScreenMgr_GetFlagAlarmScreen() == true)
	{
		if(s_isServiceScreen == false)//low alarm is show
		{
			lowAlarmScreen_ChangeFlowRate(s_flowrateSetting);
		}
		else//service screen show
		{
			equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
		}
	}
	else if(WM_IsVisible(g_operationScreen) == true)
	{
		operationScreen_ChangeFlowRate(s_flowrateSetting);
	}
	else
	{
		equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_PlayFlowRate                                    					*/
/*                                                                                      */
/* Details  -Play voice flow rate      													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowRate : - current flow rate								*/
/*                     - range : 5-60                                                   */
/*                     - unit : LPM                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_PlayFlowRate(uint8_t flowRate)
{
	uint8_t event = eNoneAudio;

	switch(flowRate)
	{
	case SETTING_FR_300L:
		event = eThreeAudio;
		break;
	case SETTING_FR_250L:
		event = eTwoFiveAudio;
		break;
	case SETTING_FR_200L:
		event = eTwoAudio;
		break;
	case SETTING_FR_175L:
		event = eOneSevenFiveAudio;
		break;
	case SETTING_FR_150L:
		event = eOneFiveAudio;
		break;
	case SETTING_FR_125L:
		event = eOneTwoFiveAudio;
		break;
	case SETTING_FR_100L:
		event = eOneAudio;
		break;
	case SETTING_FR_075L:
		event = eZeroSevenFiveAudio;
		break;
	case SETTING_FR_050L:
		event = eZeroFiveAudio;
		break;
	case SETTING_FR_025L:
		event = eZeroTwoFiveAudio;
		break;
	default:
		break;
	}

	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		switch(alarmScreenMgr_GetCurrentAlarmId())
		{
		case eLoweringO2ConcentrationId:
		case eO2ConcentrationSensorId:
		case eRisingCompressorOutletTemperatureId:
		case eTemperatureId:
		case eHighPressureId:
		case eLowPressureId:
		case eCompressorDCOverVoltageId:
		case eCompressorSoftwareOverCurrentId:
		case eCompressorHardwareOverCurrent:
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
		case eSpeakerId:
			return;
		case eCannulaMore27sId:
			if(s_flowrateSetting != s_penddingFlowRate)
			{
				audioMgr_Request(event, eNoAlarmAudio);
			}
			else
			{
				DEBUGOUT("\n not play flow rate when confirm");
			}
			break;
		default:
			audioMgr_Request(event, eNoAlarmAudio);
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_IrdaHandle                                    						*/
/*                                                                                      */
/* Details  -Handle Irda when press button on remote      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_IrdaEventId irdaEvent : - event of irda							*/
/*                 	   - range : 0-8                                                    */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isEnableOnOff : flag enable ON/OFF	FH310					*/
/*                     (I) s_isServiceScreen : flag check visible service screen		*/
/*                     (O) s_isEnableChangeFlowRate : flag enable change flow rate      */
/*                     (O) s_flowrateSetting : flow rate setting						*/
/*                     (O) s_penddingFlowRate : pendding flow rate setting				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_IrdaHandle(E_IrdaEventId irdaEvent)
{
	switch (irdaEvent)
	{
	case E_TURN_ON_ID:
	{
		if(alarmScreenMgr_GetCurrentAlarmId() == eNonvolatileMemoryId)
		{
			guiInterface_WatchDogReset();
		}
		else
		{
			if(modeMgr_GetCurrentMode() == eStandyMode)
			{
				if(s_isServiceScreen == false)
				{
					modeMgr_GotoPatient();
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);

					passwordDialog_Hide();
				}
			}
		}
		break;
	}
	case E_TURN_OFF_ID:
		if(modeMgr_GetCurrentMode() != eStopMode && modeMgr_GetCurrentMode() != eStandyMode)
		{
			if(standbyScreen_GetStateMachine() == eStartSequence ||
					standbyScreen_GetStateMachine() == eOperating ||
					standbyScreen_GetStateMachine() == eAlarmType)
			{
				if(s_enableIrda == true)
				{
					modeMgr_GotoStandby();
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);

					s_isEnableChangeFlowRate = false;
					passwordDialog_Hide();

					s_flowrateSetting = s_penddingFlowRate;
					s_blinkingFR = false;
					/** >>#0034
						Set flag accept update psa select name is false
					#0034 >>**/
					s_isUpdatePsaName = false;
				}
			}
		}
		break;
	case E_TURN_UP_ID:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				guiTask_ChangeFlowRate(1);
			}
		}
		break;
	case E_TURN_DOWN_ID:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				guiTask_ChangeFlowRate(-1);
			}
		}
		break;
	case E_CONFIRM_ID:
		/** >>#0036
			Move code to function guiTask_ConfirmFlowRate().
			Call guiTask_ConfirmFlowRate() handler when confirm flow rate by remote.
		#0036 >>**/
		guiTask_ConfirmFlowRate();
		break;
	case E_PRESET1_ID:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				if(s_blinkingFR == false)
				{
					if(s_flowrateSetting == guiInterface_GetSettingValue(eRestingFRSettingId))
					{
						guiTask_PlayFlowRate(s_flowrateSetting);
						return;
					}
				}

				s_flowrateSetting = guiInterface_GetSettingValue(eRestingFRSettingId);

				if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					if(s_isServiceScreen == false)//low alarm is show
					{
						lowAlarmScreen_ChangeFlowRate(s_flowrateSetting);
					}
					else//service screen show
					{
						equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
					}
				}
				else if(WM_IsVisible(g_operationScreen) == true)
				{
					operationScreen_ChangeFlowRate(s_flowrateSetting);
				}
				else
				{
					equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
				}
			}
		}
		break;
	case E_PRESET2_ID:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				if(s_blinkingFR == false)
				{
					if(s_flowrateSetting == guiInterface_GetSettingValue(eExertionFRSettingId))
					{
						guiTask_PlayFlowRate(s_flowrateSetting);
						return;
					}
				}

				s_flowrateSetting = guiInterface_GetSettingValue(eExertionFRSettingId);

				if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					if(s_isServiceScreen == false)//low alarm is show
					{
						lowAlarmScreen_ChangeFlowRate(s_flowrateSetting);
					}
					else//service screen show
					{
						equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
					}
				}
				else if(WM_IsVisible(g_operationScreen) == true)
				{
					operationScreen_ChangeFlowRate(s_flowrateSetting);
				}
				else
				{
					equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
				}
			}
		}
		break;
	case E_PRESET3_ID:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				if(s_blinkingFR == false)
				{
					if(s_flowrateSetting == guiInterface_GetSettingValue(eBedtimeFRSettingId))
					{
						guiTask_PlayFlowRate(s_flowrateSetting);
						return;
					}
				}

				s_flowrateSetting = guiInterface_GetSettingValue(eBedtimeFRSettingId);

				if(alarmScreenMgr_GetFlagAlarmScreen() == true)
				{
					if(s_isServiceScreen == false)//low alarm is show
					{
						lowAlarmScreen_ChangeFlowRate(s_flowrateSetting);
					}
					else//service screen show
					{
						equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
					}
				}
				else if(WM_IsVisible(g_operationScreen) == true)
				{
					operationScreen_ChangeFlowRate(s_flowrateSetting);
				}
				else
				{
					equipmentInforScreen_ChangeFlowrate(s_flowrateSetting);
				}
			}
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SaveAnypal                                    						*/
/*                                                                                      */
/* Details  -Save information anypal				      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) ATP_INFO infor : information of Anypal								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SaveAnypal(ATP_INFO infor)
{
	AnypalInfor anypalInfo;
	memset(&anypalInfo,0,sizeof(anypalInfo));
	anypalInfo = guiInterface_GetAnypalInfo();
	int result = strcmp((char*)anypalInfo.numberID,(char*)infor.patientID);

	if(result != 0 || strlen((char*)anypalInfo.equipmentName) == 0)
	{
		BTTask_SendEvent(eUpdateCompositionId);

		if(infor.type == ATP_2)
		{
			strcpy((char*)anypalInfo.equipmentName,ATP_2_EQ_NAME);
			strcpy((char*)anypalInfo.commodityCode, ATP_2_COMMODITY_CODE);
		}
		else
		{
			strcpy((char*)anypalInfo.equipmentName,ATP_3_EQ_NAME);
			strcpy((char*)anypalInfo.commodityCode, ATP_3_COMMODITY_CODE);
		}

		sprintf((char*)anypalInfo.version,"%.2d%.2d%.3d%.2d%.2d%.2d%.2d\0",infor.version[0],infor.version[1],infor.version[2],infor.version[3]
																																			,infor.version[4],infor.version[5],infor.version[6]);
		strncpy((char*)anypalInfo.numberID, (char*)infor.patientID,10);

		guiInterface_SetAnypalInfo(anypalInfo.equipmentName, anypalInfo.commodityCode, anypalInfo.version, anypalInfo.numberID);

		//clear Log anypal
		logInterface_DeleteLog(eDeleteAnypalLogId);
		guiInterface_SetNumberOfLogStored(0);
		setting_SaveAnypalInfo();
		setting_RecordChecksum();

		connectedEquipmentInforScreen_UpdateAnypal();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_InitFunc.	                                    					*/
/*                                                                                      */
/* Details  -Init all screen before start gui task      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE				            										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowrateSetting : flow rate setting                        */
/*                     (O) s_penddingFlowRate : pendding flow rate setting				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void guiTask_InitFunc(void)
{
	mainScreen_Init();

	uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);

	//Send brightness setting to M0, so that M0 can set LCD brightness
	if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
	{
		guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
	}
	s_flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);
	s_penddingFlowRate = guiInterface_GetSettingValue(eFlowRateSettingId);

	//Send close latching valve to M0
	if(setting_IsTagInitialized() == false)
	{
		guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveClose);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_CheckTime	                                    					*/
/*                                                                                      */
/* Details  -check time to log time change event     									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE				            										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void guiTask_CheckTime(void)
{
	static bool firstTime = true;
	static Real_Time_Clock prevClock;
	//check time to log time change event
	Real_Time_Clock currentClock = realtimeclock_GetLastTime();

	if(firstTime == true)
	{
		prevClock = currentClock;
		firstTime = false;
	}

	uint8_t previousHour = prevClock.hour;
	uint8_t previousDay = prevClock.mday;

	uint8_t currentDay = currentClock.mday;
	uint8_t currentHour = currentClock.hour;

	if (currentHour != previousHour)
	{
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangeHourLogId);
		prevClock = currentClock;
	}

	if (currentDay != previousDay)
	{
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangeDayLogId);
		prevClock = currentClock;
	}
	//end check time

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_CheckAndResetSystemInPowerLost	                                    */
/*                                                                                      */
/* Details  - Check and reset system when power lost     								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_logPoweroutSaved							                */
/*					   (I) s_latchValveCloseFinish										*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_CheckAndResetSystemInPowerLost(void)
{
	if((s_logPoweroutSaved == true) && (s_latchValveCloseFinish == true))
	{
		guiInterface_WatchDogReset();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_Func	                                    						*/
/*                                                                                      */
/* Details  -The function that implements the task.      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) void *pvParameters:  parameter of gui task				            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void guiTask_Func(void *pvParameters)
{
	guiTask_InitFunc();
	guihandler_StartRTCUpdateTimer();
	while(1)
	{
		lcd_TouchHandle();
		guiTask_ProcessEvent();
		realTimeClock_ProcessSetTimeRequest();
		guiTask_CheckTime();
		audioMgr_CheckAndPlayAudio();
		guiTask_CheckAndResetSystemInPowerLost();
		GUI_Exec();

		vTaskDelay(GUI_TASK_DELAY_MS);
	}

	return;
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_Create						                                   		*/
/*                                                                                      */
/* Details  -Create gui task with cyclic = 50ms      								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_Create(void)
{
	xTaskCreate(guiTask_Func, "GUI task",
			GUI_STACK_SIZE, (void *)1, TASK_PRIO_GUI_TASK,
			( TaskHandle_t * ) NULL );

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_Init						        								*/
/*                                                                                      */
/* Details  -Init gui task by register handle monitor data as callback function for     */
/*           IPC_receive_msg task								                        */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_guiQueue : gui Queue                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_Init(void)
{
	g_guiQueue = xQueueCreate(GUI_QUEUE_LENGTH, sizeof(GUI_EVENT_STRUCT));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_UpdateSelectPSA						                           	*/
/*                                                                                      */
/* Details  -Update psa select															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) PsaSelectName psaSelectName : - psa select 							*/
/*                     - range : 0-5                                                    */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_conditionTemp : condition temperature                      */
/*                     (O) s_conditionO2 : condition O2						            */
/*                     (O) s_psaSelectName : psa select name						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_UpdateSelectPSA(uint16_t psaSelectName)
{
	switch((E_PsaSelectName)psaSelectName)
	{
	case LOW_TEMP_NORM_02:
		s_conditionTemp = eLowTempType;
		s_conditionO2 = eNormalO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetLowNormalO2ConcentrationLogId);
		}
		break;
	case NORM_TEMP_NORM_02:
		s_conditionTemp = eNormalTempType;
		s_conditionO2 = eNormalO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetNormalNormalO2ConcentrationLogId);
		}
		break;
	case HIGH_TEMP_NORM_02:
		s_conditionTemp = eHighTempType;
		s_conditionO2 = eNormalO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetHighNormalO2ConcentrationLogId);
		}
		break;
	case LOW_TEMP_LOW_02:
		s_conditionTemp = eLowTempType;
		s_conditionO2 = eLowO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetLowLoweringO2ConcentrationLogId);
		}
		break;
	case NORM_TEMP_LOW_02:
		s_conditionTemp = eNormalTempType;
		s_conditionO2 = eLowO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetNormalLoweringO2ConcentrationLogId);
		}
		break;
	case HIGH_TEMP_LOW_02:
		s_conditionTemp = eHighTempType;
		s_conditionO2 = eLowO2Type;
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), ePsaParameterSetHighLoweringO2ConcentrationLogId);
		}
		break;
	default:
		break;
	}

	s_psaSelectName = (E_PsaSelectName)psaSelectName;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_DectectO2Cynlinder						                            */
/*                                                                                      */
/* Details  -Detect o2 cylinder															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_O2CylinStatus o2Cynlinder :  - o2 cynlinder						*/
/*                     - range : 0-2                                                    */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_O2CylinStatus : O2 cylinder status                         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_DectectO2Cynlinder(E_O2CylinStatus o2Cynlinder)
{
	if(o2Cynlinder != s_O2CylinStatus)
	{
		s_O2CylinStatus = o2Cynlinder;
		highAlarmScreen_UpdateO2Cylinder();
	}


	if(guiInterface_GetSettingValue(eO2CylinderSettingId) == eOff)
	{
		if(s_O2CylinStatus == O2_CYLIN_ATTACH)
		{
			guiInterface_SetSettingValue(eO2CylinderSettingId,eOn);
			setting_SaveAllSettingValue();
		}
	}

	operationScreen_CheckAndShowO2Cylinder();
	equipmentSettingScreen_UpdateSetting();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_CheckPSASelectPort						                            */
/*                                                                                      */
/* Details  -Check psa select port														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_PsaPortSelect psaPortSelect : - psa select port					*/
/*                      - range : 0-1                                                   */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_selectPort : psa select port                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_CheckPSASelectPort(E_PsaPortSelect psaPortSelect)
{
	s_selectPort = psaPortSelect;
	if(psaPortSelect == DEFAULT_SELECT)
	{
		//Send threshold of temp and o2
		guiTask_SendPendingEvent(eMaxTemperatureChangeId,SETTING_DEFAULT_HIGH_TEMP);
		guiTask_SendPendingEvent(eMinTemperatureChangeId,SETTING_DEFAULT_LOW_TEMP);
		guiTask_SendPendingEvent(eLimitOxygenConcentratorChangeId,SETTING_DEFAULT_O2_LIMIT);

		PSATableMgr_SetDefaultSelectPort();
	}
	else
	{
		//Send threshold of temp and o2
		guiTask_SendPendingEvent(eMaxTemperatureChangeId,guiInterface_GetSettingValue(eHightempSettingId));
		guiTask_SendPendingEvent(eMinTemperatureChangeId,guiInterface_GetSettingValue(eLowTempSettingId));
		guiTask_SendPendingEvent(eLimitOxygenConcentratorChangeId,guiInterface_GetSettingValue(eOxyLimitSettingId));

		PSATableMgr_SetEEPROMSelectPort();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_HandleMonitorData						                            */
/*                                                                                      */
/* Details  -Decode msg_data receive from device task and handle it						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) IPC_MSG_DATA_t data : message data									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_flagAlarmNVRam : flag check alarm NVRAM					*/
/*                     (O) s_penddingFlowRate : pendding flow rate setting              */
/*                     (O) s_flowrateSetting : flow rate setting						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_HandleMonitorData(IPC_MSG_DATA_t data)
{
	switch (data.data_id) {
	case eSendFirstMonitorInfoToGuiId:
		if(s_flagAlarmNVRam == true)
		{
			setting_InitWhenAlarm();

			guiTask_SetAlarm(eNonvolatileMemoryId,eActive);
			guiTask_writeAlarmLog(eNonvolatileMemoryId);
		}
		else
		{
			if (guiInterface_GetSettingValue(eResetWatchDogFlag) ==  RESET_WDT)
			{
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eResetWatchdogLogId);
			}
			else
			{
				guiInterface_SetSettingValue(eResetWatchDogFlag, RESET_WDT);
			}

			guiTask_CheckPSASelectPort(data.stPsaMonitorPacket.psaPortSelect);
			guiTask_DectectO2Cynlinder(data.stPsaMonitorPacket.oxyCylinderStatus);
			modeMgr_RecoverMode((E_ModeSetting)guiInterface_GetSettingValue(eModeSettingId));
			setting_SaveAllSettingValue();
		}
		break;
	case ePowerLostDetected:
		guiInterface_HandlerPowerLost();
		break;
	case eOperationswToggle:
		if(alarmScreenMgr_GetCurrentAlarmId() == eNonvolatileMemoryId)
		{
			guiInterface_WatchDogReset();
		}
		else
		{
			if(modeMgr_GetCurrentMode() == eStandyMode)
			{
				if(s_isServiceScreen == false)
				{
					modeMgr_GotoPatient();
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);

					passwordDialog_Hide();
				}
			}
		}
		break;
	case eRequestPsaParamAtStart:
		if(modeMgr_GetCurrentMode() == ePatientMode)
		{
			/** >>#0034
				Set flag accept update psa select name is true
			#0034 >>**/
			s_isUpdatePsaName = true;

			guiTask_UpdateSelectPSA(data.data_value);

			uint8_t flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);
			if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
			{
				guiTask_SendPSAParaEvent(eInitPacketPSAParaChangeId,flowrateSetting,
						guiTask_GetConditionO2(),guiTask_GetConditionTemp());
			}
		}
		break;
	case eOperationswToggleShort:
		/** >>#0036
			Move code to function guiTask_ConfirmFlowRate().
			Call guiTask_ConfirmFlowRate() handler when confirm flow rate by operation switch.
		#0036 >>**/
		guiTask_ConfirmFlowRate();
		break;
	case eOperationswToggle2S:
		if(modeMgr_GetCurrentMode() != eStopMode && modeMgr_GetCurrentMode() != eStandyMode)
		{
			if(standbyScreen_GetStateMachine() == eStartSequence ||
					standbyScreen_GetStateMachine() == eOperating ||
					standbyScreen_GetStateMachine() == eAlarmType)
			{
				if(s_isEnableOnOff == true)
				{
					modeMgr_GotoStandby();
					WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);

					s_isEnableChangeFlowRate = false;
					passwordDialog_Hide();

					s_flowrateSetting = s_penddingFlowRate;
					s_blinkingFR = false;
					/** >>#0034
						Set flag accept update psa select name is false
					#0034 >>**/
					s_isUpdatePsaName = false;
				}
			}
		}
		break;
	case ePsaMonitorInfo:
	{
		if(data.stPsaMonitorPacket.psaParamSelect != s_psaSelectName)
		{
			/** >>#0034
				Check flag accept update psa select name before update and send to device
			#0034 >>**/
			if(s_isUpdatePsaName == true)
			{
				guiTask_UpdateSelectPSA(data.stPsaMonitorPacket.psaParamSelect);

				if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
				{
					uint8_t flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);
					if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
					{
						guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
								guiTask_GetConditionO2(),guiTask_GetConditionTemp());
					}
				}
			}
		}
		guiTask_DectectO2Cynlinder(data.stPsaMonitorPacket.oxyCylinderStatus);

		s_realMonitorData = data.stPsaMonitorPacket;
		standbyScreen_SetPsaSequence(data.stPsaMonitorPacket.psaCurSeq);
		monitorScreen_UpdatePSA(data);
		monitorGraphScreen_UpdatePSA(data);
		equipmentInforScreen_UpdatePSA(data);
		functionTestPageOneScreen_UpdateMonitor(data);
		functionTestPageThreeScreen_UpdateMonitor(data);
		break;
	}
	case eCompVersion:
		functionTestPageOneScreen_DisplayVersionComp((int8_t*)data.version);
		break;
	case eFLowVersion:
		functionTestPageOneScreen_DisplayVersionFlow((int8_t*)data.version);
		break;
	case eEncoderChangeStep:
		if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
		{
			if(s_isEnableChangeFlowRate == true)
			{
				int step = (int)data.data_value - SCALE_SETTING_100;
				guiTask_ChangeFlowRate(step);
			}
		}
		break;
	case eAlarmMGSId:
		if((data.stAlarmStatPacket.ID != ePowerOutageInInOperationId) &&
				(data.stAlarmStatPacket.ID != ePressedOperationSWDuringUnplugId))
		{
			switch(data.stAlarmStatPacket.ID)
			{
			case eAnypalBatterId:
			case eAnypalRTCId:
			case eAnypalCommunicationId:
			case eOverCurrentId:
				break;
			default:
				if(data.stAlarmStatPacket.currentStatus == eActive)
				{
					guiTask_writeAlarmLog(data.stAlarmStatPacket.ID);
				}

				alarmScreenMgr_Set(data.stAlarmStatPacket);
				break;
			}
		}
		break;
	case eFcRequestRecordLog:
	{
		switch (data.data_value) {
		case eChecksumErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerCommandChecksumErrorLogId);
			break;
		case eCommandErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerCommandCodeErrorLogId);
			break;
		case eParamErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerCommandParameterErrorLogId);
			break;
		case eNoResponseErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerNoResponseLogId);
			break;
		case eResponseLengthErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerResponseLengthErrorLogId);
			break;
		case eResponseCodeErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerResponseCodeErrorLogId);
			break;
		case eResponseCheckSumErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerResponseChecksumErrorLogId);
			break;
		case eResponseParamErr:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerResponseParameterErrorLogId);
			break;
		case eFlowReset:
			logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eFlowControllerResetNotificationId);
			break;
		default:
			break;
		}
		break;
	}
	case eLatchValveCloseFinish:
		//Reset if tag is not initialize
		if(setting_IsTagInitialized() == false)
		{
			DEBUGOUT("\n eLatchValveCloseFinish");
			guiInterface_WatchDogReset();
		}

		//Check latching valve finish
		if(modeMgr_GetCurrentMode() != eStopMode &&
				modeMgr_GetCurrentMode() != eStandyMode)
		{
			s_latchValveCloseFinish = true;
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SendPendingEvent						                			*/
/*                                                                                      */
/* Details  -This function must be calling form IPC send msg task for 					*/
/*           send the device message to device task on M0 core							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) MsgDataId dataId : - id of data 		    							*/
/*                     - range : 0-100                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint32_t value : data value                                          */
/*                     - range : 0-4294967295                                           */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SendPendingEvent(E_MsgDataId dataId, uint32_t value)
{
	IPC_MSG_DATA_t msgPara;
	msgPara.data_id = dataId;
	msgPara.data_value = value;
	memset(msgPara.version,0,sizeof(msgPara.version));
	if (ipc_PushMsg(IPCEX_ID_DEVICE, msgPara) == QUEUE_INSERT)
	{
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SendPSAParaEvent						                			*/
/*                                                                                      */
/* Details  -This function must be calling form IPC send msg task for 					*/
/*           send the device message to device task on M0 core							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) MsgDataId dataId : - id of data			 			    			*/
/*                  	- range : 0-100                                                 */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*                 E_O2ConcentratorType conditionO2 : - condition O2   					*/
/*                  	- range : 0-2                                                 	*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*                 E_TemperatureType conditionTemp : - condition temperature            */
/*                  	- range : 0-3                                                 	*/
/*                      - unit : NONE                                                   */
/*																						*/
/*                 uint8_t flowRateSetting : - flow rate setting                        */
/*                  	- range : 5-60                                                 	*/
/*                      - unit : LPM	                                                */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SendPSAParaEvent(E_MsgDataId dataId, uint8_t flowRateSetting,E_O2ConcentratorType conditionO2,E_TemperatureType conditionTemp)
{
	IPC_MSG_DATA_t msgPara;

	msgPara.data_id = dataId;
	msgPara.data_value = 0;
	msgPara.stPsaParamPacket.psaPressure = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,ePSAPress);
	msgPara.stPsaParamPacket.solenoid1OnOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid1OnOffsetTime);
	msgPara.stPsaParamPacket.solenoid1OffOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid1OffOffsetTime);
	msgPara.stPsaParamPacket.solenoid2OnOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid2OnOffsetTime);
	msgPara.stPsaParamPacket.solenoid2OffOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid2OffOffsetTime);
	msgPara.stPsaParamPacket.solenoid3OnOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid3OnOffsetTime);
	msgPara.stPsaParamPacket.solenoid3OffOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid3OffOffsetTime);
	msgPara.stPsaParamPacket.solenoid4OnOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid4OnOffsetTime);
	msgPara.stPsaParamPacket.solenoid4OffOffsetTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid4OffOffsetTime);
	msgPara.stPsaParamPacket.solenoid5OnOffsetTimeA = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid5OnOffsetTimeA);
	msgPara.stPsaParamPacket.solenoid5OnOffsetTimeB = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid5OnOffsetTimeB);
	msgPara.stPsaParamPacket.solenoid5OpenTimeA = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid5OpenTimeA);
	msgPara.stPsaParamPacket.solenoid5OpenTimeB = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoid5OpenTimeB);
	msgPara.stPsaParamPacket.coolingFan = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eCoolingFanDutyCycle);

	msgPara.stPsaParamPacket.compressor = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eCompressorRPM);
	uint16_t FCValue = (uint16_t)(PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eMFCOutput)*FR_FC_SCALE);
	uint16_t flowRateValue = (uint16_t)(flowRateSetting * FR_FC_SCALE);

	msgPara.stPsaParamPacket.flowRateSetting = flowRateValue;
	msgPara.stPsaParamPacket.FCoutput = FCValue;
	msgPara.stPsaParamPacket.MaxPsa = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eMaxPSATime);

	msgPara.stPsaParamPacket.solenoidEndOpenTime = PSATableMgr_GetPSAItem(flowRateSetting,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd);

	memset(msgPara.version,0,sizeof(msgPara.version));
	if (ipc_PushMsg(IPCEX_ID_DEVICE, msgPara) == QUEUE_INSERT) {
		DEBUGOUT("M4 Push data: %d\n",msgPara.data_id);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_IMAGESetBitmap                                   				    */
/*                                                                                      */
/* Details  -Set bit map for image					      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) IMAGE_Handle hWin  : image for emwin									*/
/* 				   const GUI_BITMAP * pBitmap : new bit map								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_IMAGESetBitmap(IMAGE_Handle hWin, const GUI_BITMAP * pBitmap)
{
	IMAGE_SetBitmap(hWin,pBitmap);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetFlowrate                             						    */
/*                                                                                      */
/* Details  -Get flow rate setting					      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : current flow rate setting		                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_flowrateSetting : flow rate setting						*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t guiTask_GetFlowrate(void)
{
	return s_flowrateSetting;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFowRate                             						    */
/*                                                                                      */
/* Details  -Set flow rate setting					      								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowrate	: - flow rate setting								*/
/*                      - range : 5-60                                                  */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowrateSetting : flow rate setting                        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFowRate(uint8_t flowrate)
{
	s_flowrateSetting = flowrate;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetPenddingFlowRate                             				    */
/*                                                                                      */
/* Details  -Get pendding flow rate setting					      					    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : pendding flow rate setting	                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_penddingFlowRate : pendding flow rate setting				*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t guiTask_GetPenddingFlowRate(void)
{
	return s_penddingFlowRate;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetPenddingFowRate                             				    */
/*                                                                                      */
/* Details  -Set pendding flow rate setting					      						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowrate	: - flow rate setting								*/
/*                     - range : 5-60                                                   */
/*                     - unit : LPM                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_penddingFlowRate : pendding flow rate setting              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetPenddingFowRate(uint8_t flowrate)
{
	s_penddingFlowRate = flowrate;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFlagNVRam                             				    		*/
/*                                                                                      */
/* Details  -Set flag alarm NVRAM									      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isAlarm : - flag alarm NVRAM									*/
/*                      - range : 0-1                                                   */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flagAlarmNVRam : flag check alarm NVRAM                    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFlagAlarmNVRam(bool isAlarm)
{
	s_flagAlarmNVRam = isAlarm;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetConditionO2                             				    	*/
/*                                                                                      */
/* Details  -Get condition O2										      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_O2ConcentratorType : condition O2			                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_conditionO2 : condition O2							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_O2ConcentratorType guiTask_GetConditionO2(void)
{
	return s_conditionO2;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetConditionTemp                             				    	*/
/*                                                                                      */
/* Details  -Get condition temp										      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_TemperatureType : condition temperature		                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_conditionTemp : condition temperature                      */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
E_TemperatureType guiTask_GetConditionTemp(void)
{
	return s_conditionTemp;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetRealMonitorData                        				    		*/
/*                                                                                      */
/* Details  -Get Real Monitor Data									      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : PSA_MONITOR_PACKET_t : real monitor data		                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_realMonitorData : real monitor data						*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
PSA_MONITOR_PACKET_t guiTask_GetRealMonitorData(void)
{
	return s_realMonitorData;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetFlagServiceScreen                        				    	*/
/*                                                                                      */
/* Details  -Get flag srevice screen								      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												    				*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : true if service screen is visible			                    */
/*        		        false if service screen is not visible			                */
/*                                                                                      */
/* Using Global Data : (I) s_isServiceScreen : flag check visible service screen		*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool guiTask_GetFlagServiceScreen(void)
{
	return s_isServiceScreen;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFlagServiceScreen                        				    	*/
/*                                                                                      */
/* Details  -Set flag srevice screen								      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag : - flag set visible service screen						*/
/*                     - range : 0-1                                                    */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isServiceScreen : flag check visible service screen        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFlagServiceScreen(bool flag)
{
	s_isServiceScreen = flag;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetFlagEnableChangeFR                        				    	*/
/*                                                                                      */
/* Details  -Get flag enable change flow rate setting								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												    				*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isEnableChangeFlowRate : flag enable change flow rate		*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool guiTask_GetFlagEnableChangeFR(void)
{
	return s_isEnableChangeFlowRate;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFlagEnableChangeFR                        				    	*/
/*                                                                                      */
/* Details  -Set flag enable change flow rate setting								    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag : - flag enable change flow rate							*/
/*                    - range : 0-1                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isEnableChangeFlowRate : flag enable change flow rate      */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFlagEnableChangeFR(bool flag)
{
	s_isEnableChangeFlowRate = flag;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetSelectPort                  		      				    	*/
/*                                                                                      */
/* Details  -Get PSA select port 														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												    	   			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_PsaPortSelect : psa select port		                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_selectPort : psa select port                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_PsaPortSelect guiTask_GetSelectPort(void)
{
	return s_selectPort;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetO2CylinStatus                  		      				    	*/
/*                                                                                      */
/* Details  -Get O2 cylinder status	 													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												    	   			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_O2CylinStatus : O2 cylinder status			                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_O2CylinStatus : O2 cylinder status							*/
/*                     (O) NONE								                       		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
E_O2CylinStatus guiTask_GetO2CylinStatus(void)
{
	return s_O2CylinStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_GetFlagOnOff                  		      				    		*/
/*                                                                                      */
/* Details  -Get flag on/off system	 													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												    	   			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : true if enable on/off FH310			                            */
/*        		 		false if disable on/off FH310	                                */
/*                                                                                      */
/* Using Global Data : (I) s_isEnableOnOff : flag enable ON/OFF FH310              	    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool guiTask_GetFlagOnOff(void)
{
	return s_isEnableOnOff;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFlagOnOff                        				    			*/
/*                                                                                      */
/* Details  -Set flag enable on/off system											    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag : - flag enable on/off FH310								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isEnableOnOff : flag enable on/off FH310                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFlagOnOff(bool flag)
{
	s_isEnableOnOff = flag;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_ProcessEvent                        				    			*/
/*                                                                                      */
/* Details  -Process event data	in gui queue											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_ProcessEvent(void)
{
	GUI_EVENT_STRUCT event;
	event.id = eNoneGuiEventId;

	if(xQueueReceive(g_guiQueue,&event,QUEUE_NO_WAIT) == pdTRUE)
	{
		switch(event.id)
		{
		case eAnypalConnectEventId:
			anypalDisplayHandler_SetAnypalConnectStatus((bool)event.data.charData);
			break;
		case eAnypalInforEventId:
			//ATP infor
			guiTask_SaveAnypal(event.data.atpInfor);
			break;
		case eAnypalStatusEventId:
			anypalDisplayHandler_SetChargingStatus(event.data.atpStatus.isChaging);
			anypalDisplayHandler_CheckAndSetError(event.data.atpStatus);
			break;
		case eBluetoothFwVersionEventId:
			functionTestPageOneScreen_DisplayBT(event.data.btFwVersion);
			break;
		case eBluetoothAddrId:
			connectedEquipmentInforScreen_DisplayBTAddress(event.data.btAddress);
			break;
		case eBarcodeSerialEventId:
			serialNumberSettingScreen_DisplaySerialNumber(event.data.barcode.length,
					(int8_t*)event.data.barcode.barcodeString);
			break;
		case eOverCurrentEventId:
			anypalDisplayHandler_SetAnypalOverCurrent((E_AlarmStatus)event.data.charData);
			break;
		case eResultSaveLogEventId:
			logDataScreen_ClosePopUp(event.data.charData);
			break;
		case eCheckFWFileResultEventId:
			softwareUpgradeScreen_UpgradeFW((CheckFWFileResult)event.data.charData);
			break;
		case eCheckPWFileResultEventId:
			passwordDialog_CheckFile(event.data.charData);
			break;
		case eDumpFileEEPROMResultEventId:
		case eDumpFileCBXLogResultEventId:
		case eDumpFileAnypalLogResultEventId:
		case eDumpFileSpO2PulseLogResultEventId:
		case eDumpFileSRAMLogResultEventId:
		case eDumpFileSDRAMLogResultEventId:
		case eDumpFileFlashALogResultEventId:
		case eDumpFileFlashBLogResultEventId:
		case eExportFileDebugLogEventId:
		case eDumpFileFlashSPILogResultEventId:
			developmentSettingScreenPageThree_ClosePopUp(event.data.charData);
			break;
		case eUploadEEPROMResultEventId:
		case eUploadCBXResultEventId:
		case eUploadAnypalResultEventId:
		case eUploadSPO2PulseResultEventId:
			developmentSettingScreenPageFour_ClosePopUp(event.data.charData);
			break;
		case eResetWDTEventId:
			s_logPoweroutSaved = true;
			break;
		case eIrdaOnEventId:
		case eIrdaOffEventId:
		case eIrdaUpEventId:
		case eIrdaDownEventId:
		case eIrdaPreset1EventId:
		case eIrdaPreset2EventId:
		case eIrdaPreset3EventId:
		case eIrdaConfirmEventId:
			guiTask_IrdaHandle((E_IrdaEventId)event.data.charData);
			break;
		case eIpcDataEventId:
			guiTask_HandleMonitorData(event.data.ipcData);
			break;
		case eCBXGetRealDateid:
		{
			MONITOR_DATA_t monitorData;
			monitorData.PASMonitorData = s_realMonitorData;
			monitorData.currentAlarm = alarmScreenMgr_GetCurrentAlarmId();
			monitorData.currentMode =  modeMgr_GetCurrentMode();
			monitorData.flowRateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);
			BTInterface_sendRealdata(&monitorData);
			break;
		}
		/** >>#0035
				Handler event update animation in operation screen
			#0035 >>**/
		case eUpdateAnimationOperationEventId:
			guihandler_UpdateAnimationOperation();
			break;
			/** >>#0035
				Handler event update animation in alarm screen
			#0035 >>**/
		case eUpdateAnimationAlarmEventId:
			alarmScreenMgr_UpdateAnimation();
			break;
			/** >>#0035
				Handler event update calendar and cumulative hour
			#0035 >>**/
		case eUpdateCalenderCumulativeHoursEventId:
			equipmentInforScreen_UpdateCalendarCumulative();
			equipmentCumulativeHourScreen_DisplayCumulative();
			break;
			/** >>#0035
				Handler event hide icon charging anypal
			#0035 >>**/
		case eHideIconChargingAnypalEventId:
			anypalDisplayHandler_ChargedFull();
			break;
			/** >>#0035
				Handler event send flow rate to device
			#0035 >>**/
		case eSendFlowRateToDeviceEventId:
			operationScreen_SendFRToDevice();
			lowAlarmScreen_SendFRToDevice();
			break;
		default:
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_ClearLatchValveProcessingRequestStatus                        		*/
/*                                                                                      */
/* Details  -Clear latchve close finish flag											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) bool s_latchValveCloseFinish				                   	*/
/*																						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_ClearLatchValveProcessingRequestStatus(void)
{
	s_latchValveCloseFinish = false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_EnableIrda                        									*/
/*                                                                                      */
/* Details  -Enable or disable Irda 													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool flag : - flag enable/disable irda								*/
/*						 - range : 0-1													*/
/*                       - unit : NONE                                                  */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*																						*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_EnableIrda(bool flag)
{
	s_enableIrda = flag;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetFont	                                    					*/
/*                                                                                      */
/* Details  -Set font for FH310								      						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetFont(const GUI_FONT * pNewFont)
{
	vTaskSuspendAll();
	GUI_SetFont(pNewFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTaskEDIT_SetFont	                                    				*/
/*                                                                                      */
/* Details  -Set font for EDIT(emwin)								      			    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                 EDIT_Handle hObj : edit object                                       */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTaskEDIT_SetFont(EDIT_Handle hObj, const GUI_FONT * pFont)
{
	vTaskSuspendAll();
	EDIT_SetFont(hObj,pFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTaskTEXT_SetFont	                                    				*/
/*                                                                                      */
/* Details  -Set font for TEXT(emwin)								      			    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                 TEXT_Handle hObj : text object                                       */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTaskTEXT_SetFont(TEXT_Handle hObj, const GUI_FONT * pFont)
{
	vTaskSuspendAll();
	TEXT_SetFont(hObj,pFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTaskRADIO_SetFont	                                    				*/
/*                                                                                      */
/* Details  -Set font for RADIO(emwin)								      			    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                 RADIO_Handle hObj : radio button object                              */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTaskRADIO_SetFont(RADIO_Handle hObj, const GUI_FONT * pFont)
{
	vTaskSuspendAll();
	RADIO_SetFont(hObj,pFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTaskLISTVIEW_SetFont	                                    			*/
/*                                                                                      */
/* Details  -Set font for LISTVIEW(emwin)								      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                 LISTVIEW_Handle hObj : list view object                              */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTaskLISTVIEW_SetFont(LISTVIEW_Handle hObj, const GUI_FONT * pFont)
{
	vTaskSuspendAll();
	LISTVIEW_SetFont(hObj,pFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTaskMULTIPAGE_SetFont	                                    			*/
/*                                                                                      */
/* Details  -Set font for MULTIPAGE(emwin)								      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) GUI_FONT * pNewFont : new font										*/
/*                 MULTIPAGE_Handle hObj : multipage object                             */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTaskMULTIPAGE_SetFont(MULTIPAGE_Handle hObj, const GUI_FONT * pFont)
{
	vTaskSuspendAll();
	MULTIPAGE_SetFont(hObj,pFont);
	xTaskResumeAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guiTask_SetBlinkingFR						                        		*/
/*                                                                                      */
/* Details  -Set flag blinking flow rate      											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool blinking : - status flag blinking								*/
/*                    - range :  0-1                                                    */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE													        */
/*                     (O) s_blinkingFR : flag blinking flow rate                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guiTask_SetBlinkingFR(bool blinking)
{
	s_blinkingFR = blinking;

	return;
}

/* end of files */

