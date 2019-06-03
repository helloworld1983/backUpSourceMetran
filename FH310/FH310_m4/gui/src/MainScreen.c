/********************************************************************/
/*                                                                  */
/* File Name    : MainScreen.c                       			    */
/*                                                                  */
/* General       : Manager handle event from all screen				*/
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
/* #0002       2017/09/16		Viet Le				modify			*/
/*			-change mode mgr design, remove goto mode when show 	*/
/*				screen												*/
/* #0003       2017/09/19		Viet Le				modify			*/
/*			- Check oxy cylinder status before go to mode in 		*/
/*				mainScreen_ShowHomeScreen() function				*/
/*			- Remove check medium alarm when recover mode in 		*/
/*				mainScreen_ShowHomeScreen() function (bug 1857)		*/
/*			-Check curent alarm Id instead of check remain alarm in	*/
/*			alarm stat list when back to patient mode from service  */
/* #0004       2017/09/28		Viet Le				modify			*/
/*			- Add check alarm speaker before play buzzer start      */
/* #0005	   2017/10/09		Quyen Ngo			modify			*/
/*			-Record log of alarm speaker before call alarmMgrScree_Set*/
/*			function for correct the order of log					*/
/* #0006	   2017/10/14		Viet Le				modify			*/
/*			-Change wake up screen show time from 1s to 3s(new spec)*/
/* #0007	   2017/10/20		Viet Le				modify			*/
/*			-Show alarm anypal when user turn on system with		*/
/*		alarm anypal and high alarm active in standby(bug 1994)		*/
/* #0008	   2017/10/20		Viet Le				modify			*/
/*			-Create flag playStartAudio is flase, turn on system	*/
/*			not alarm anypal set flag is true, turn on system		*/
/*			with alarm anypal set flag is true.						*/
/*			Play start audio set flag is true(bug 1995)				*/
/* #0009	   2017/10/28		Viet Le				modify			*/
/*			-Handle case alarm NVRAM when back to home to service	*/
/*		screen while alarm NVRAM is active(bug 2006)				*/
/* #0010	   2017/12/01		Viet Le				modify			*/
/*    Handle flag blinking flow rate when back to home and 			*/
/*     enter service screen(bug 2046)								*/
/********************************************************************/
#include "WM.h"
#include "MainScreen.h"
#include "board.h"
#include "TaskCommon.h"
#include "setting.h"
#include "GuiTask.h"
#include "OperationScreen.h"
#include "ModeMgr.h"
#include "PasswordDialog.h"
#include "GuiHandler.h"
#include "AlarmInfor.h"
#include "AlarmScreenMgr.h"
#include "StandbyScreen.h"
#include "ServiceScreen.h"
#include "OptionScreen.h"
#include "Bitmap.h"
#include "AnyPalDisplayHandler.h"
#include "GuiInterface.h"
#include "DevelopmentSettingScreen.h"
#include "AudioMgr.h"

//main window
WM_HWIN g_mainWindow;

static GUI_BITMAP s_bitmapbmwakeup;

/****************************************************************************************/
/*                                                                                      */
/* General 	-mainScreen_ShowHomeScreen                                            	    */
/*                                                                                      */
/* Details  -Show home screen (standby screen or operation screen)						*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE 					  											*/
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
void mainScreen_ShowHomeScreen(void)
{
	if(guiTask_GetFlagServiceScreen() == true)
	{
		if(modeMgr_GetCurrentMode() == eStandyMode)
		{
			standbyScreen_RestartTimerBlackScreen();
		}

		guiTask_SetFlagServiceScreen(false);

		serviceScreen_Hide();
	}

	alarmInfor_HideAlarmInfor();
	alarmScreenMgr_Hide();
	guiTask_SetBlinkingFR(false);

	if (modeMgr_GetCurrentMode() == eServiceMode)
	{
		//Enter patinent mode from service mode
		modeMgr_GotoPatient();

		if(alarmScreenMgr_GetCurrentAlarmId() == eLastAlarmId)
		{
			if(standbyScreen_GetStateMachine() == eOperating)
			{
				guihandler_StartTimerAnimation();
				operationScreen_EnterOperationMode();
			}
			else if(standbyScreen_GetStateMachine() == eStartSequence)
			{
				operationScreen_EnterStartOperation();
			}
		}
		else
		{
			alarmScreenMgr_SetPrevALarmId(eLastAlarmId);
			alarmScreenMgr_Show();
		}
	}
	else if (modeMgr_GetCurrentMode() == ePatientMode)
	{
		//Flag play start audio
		static bool playStartAudio = false;

		if(standbyScreen_GetStateMachine() == eOperating)
		{
			operationScreen_EnterOperationMode();
		}
		else
		{
			operationScreen_EnterStartOperation();

			//Set flag play start
			playStartAudio = true;
		}

		//Show alarm anypal
		if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
		{
			alarmScreenMgr_SetPrevALarmId(eLastAlarmId);
			alarmScreenMgr_Show();

			//Set flag play start
			playStartAudio = true;
		}

		//Play start audio
		if(playStartAudio == true)
		{
			playStartAudio = false;

			if(audioMgr_CheckAlarmSpeaker() == true)
			{
				guiTask_writeAlarmLog(eSpeakerId);
				guiTask_SetAlarm(eSpeakerId,eActive);
			}
			else
			{
				audioMgr_Request(eBuzzer100msAudio, eNoAlarmAudio);
			}
		}

		if(alarmScreenMgr_GetHighAlarmInStandby() != eLastAlarmId)
		{
			if(developmentSettingScreen_CheckEnable(alarmScreenMgr_GetHighAlarmInStandby()) == true)
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
		}
	}
	else if (modeMgr_GetCurrentMode() == eStandyMode)
	{
		if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
		{
			alarmScreenMgr_SetPrevALarmId(eLastAlarmId);
			alarmScreenMgr_Show();
		}
		else
		{
			alarmInfor_SetInforAlarm(0);
			standbyScreen_Show();
		}
	}
	else if (modeMgr_GetCurrentMode() == eAlarmMode ||
			modeMgr_GetCurrentMode() == eAlarmExternalFlowPathMode ||
			modeMgr_GetCurrentMode() == eAlarmNoOxyCylinderMode ||
			alarmScreenMgr_GetCurrentAlarmId() == eNonvolatileMemoryId)
	{
		alarmScreenMgr_SetPrevALarmId(eLastAlarmId);
		alarmScreenMgr_Show();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mainScreen_Callback                                            		    */
/*                                                                                      */
/* Details  -call back function for main screen, handle event from window		        */
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger	from window 					  		*/
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
void mainScreen_Callback(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_Clear();
		break;
	case WM_SHOW_HOMESCREEN:
		mainScreen_ShowHomeScreen();
		break;
	case WM_SHOW_SERVICE_TAB_SCREEN:
		guiTask_SetFlagServiceScreen(true);
		guiTask_SetBlinkingFR(false);
		modeMgr_GotoService();
		serviceScreen_Show();
		alarmInfor_HideAlarmInfor();
		guihandler_StopTimerAnimation();
		break;
	case WM_SHOW_ALARMSCREEN:
		alarmInfor_HideAlarmInfor();
		alarmScreenMgr_Show();
		if(alarmScreenMgr_GetFlagAlarmScreen() == true)
		{
			operationScreen_Hide();
		}
		break;
	case WM_SHOW_PASSWORD_DIALOG:
		alarmInfor_HideAlarmInfor();
		passwordDialog_Show();
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mainScreen_Init		 							                		*/
/*                                                                                      */
/* Details  -Initialize main screen 			            	                		*/
/* 																						*/
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
/*                     (O) g_mainWindow : main window                                   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mainScreen_Init(void)
{
	GUI_UC_SetEncodeUTF8(); // Enable UTF8 decoding

	//create a window as basic
	g_mainWindow = WM_CreateWindow(MAIN_WINDOW_X, MAIN_WINDOW_Y, MAIN_WINDOW_SIZE_X, MAIN_WINDOW_SIZE_Y, WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP, mainScreen_Callback, 0);

	anypalDisplayHandler_Init();
	passwordDialog_Init();
	standbyScreen_Init();
	operationScreen_Init();
	alarmScreenMgr_Init();
	serviceScreen_Init();
	alarmInfor_Init();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mainScreen_ShowWakeup			 							        		*/
/*                                                                                      */
/* Details  -Show wake up screen 	            	                					*/
/* 																						*/
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
/*                     (O) s_bitmapbmwakeup : wake up image                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mainScreen_ShowWakeup(void)
{
	s_bitmapbmwakeup = bmwakeup;
	GUI_DrawBitmap(&s_bitmapbmwakeup, 0,0);
	GUI_X_Delay(WAKEUP_SHOW_TIME);

	return;
}

/* end of files */

