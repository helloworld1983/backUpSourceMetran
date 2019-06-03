/********************************************************************/
/*                                                                  */
/* File Name    : HighAlarmScreen.c                                 */
/*                                                                  */
/* General      : Display high alarm screen			                */
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
/* #0001       2017/09/05		Viet Le				modify			*/
/*      remove #include "assert.h"                                  */
/*      remove check flow path pressure (bug 1823)					*/
/* #0002       2017/09/16		Viet Le				modify			*/
/*			-change mode mgr design, remove goto mode when show 	*/
/*			screen													*/
/* #0003       2017/09/19		Viet Le				modify			*/
/*			-Remove handle case eNegativePressureId					*/
/* #0004       2017/10/09		Viet Le				modify			*/
/*          - Record log alarm compressor and flow controller		*/
/*		(detect in standby mode) when go to patient to fix bug 1959 */
/* #0005       2017/10/14		Viet Le				modify			*/
/*	-Check flag high alarm in standby before record log and clear   */
/*    flag high alarm in standby in highAlarmScreen_Show() function	*/
/*		(bug 1976)													*/
/* #0006       2017/10/27	   Viet Le				modify			*/
/* -Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont().(bug 1997)*/
/* #0007       2017/10/28	   Viet Le				modify			*/
/*		-Handle case detect Oxy cylinder in alarm NVRAM. 			*/
/*	Add flag s_isPressButtonO2Cylinder initialize is false. when    */
/*  NVRAM is active with O2 cylinder is ATTACH, press button O2 	*/
/*  cylinder set s_isPressButtonO2Cylinder is true (bug 2004)		*/
/* #0008       2017/11/02	   Viet Le				modify			*/
/*     -Send open latch valve to M0, when user push changing flow   */
/*			path button during alarm NVRAM (bug 2004)				*/
/* #0009	   2017/11/03      Viet Le 	    	        modify	    */
/*	  Add case eCompressorDCOverVoltageId,							*/
/*	eCompressorSoftwareOverCurrentId,eCompressorHardwareOverCurrent */
/*  in highAlarmScreen_SetNumberAlarm() function (bug 2014)   		*/
/* #0010       2017/11/17       Viet Le 			modify			*/
/*	 - Add s_toggle to blinking icon after 2 seconds. Change timer 1*/
/*		seconds to 2 seconds(bug 2020)								*/
/* #0011       2017/11/17       Viet Le 			modify			*/
/*	- Remove highAlarmScreen_SetExtFlowPath() (Coverity)			*/
/* #0012       2017/11/17       Viet Le 			modify			*/
/*	- Remove s_extFlowPathPressure (Coverity)						*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include "HighAlarmScreen.h"
#include "MainScreen.h"
#include "IMAGE.h"
#include "GuiTask.h"
#include "ModeMgr.h"
#include "setting.h"
#include "OperationScreen.h"
#include "BUTTON.h"
#include "GuiHandler.h"
#include "TEXT.h"
#include "Font.h"
#include "AudioMgr.h"
#include "AlarmScreenMgr.h"
#include "AlarmInfor.h"
#include "StandbyScreen.h"
#include "Bitmap.h"
#include "PasswordDialog.h"
#include "GuiInterface.h"

//Define item id
enum{
	eTopTextHighAlarmId,
	eCenterTextHighAlarmId,
	eBottomTextHighAlarmId,
	eImageHighAlarmContactId,
	eO2CylinderBtnHighAlarmId,
	eO2CylinderBtnInALarmNVId,
	eRoundTextHighAlarmId,
	eContactNumberHighAlarmId,
	eTimerChangeIconOxyHighAlarmId,
	eToplineHighAlarmId,
	eBottomlineHighAlarmId,
	eSilenceBtnHighAlarmId,
	eImageSilenceIconId,
	eTimerRestartAudioId
};

WM_HWIN g_highAlarmScreen;

static GUI_BITMAP s_bitmapHighContact;
static GUI_BITMAP s_bitmapIconOxyOne;
static GUI_BITMAP s_bitmapIconOxyTwo;
static GUI_BITMAP s_bitmapSilenceIcon;
static IMAGE_Handle s_highContact;
static IMAGE_Handle s_silenceIcon;
static BUTTON_Handle s_O2CylinderBtn;
static TEXT_Handle s_centerText;
static TEXT_Handle s_bottomText;
static TEXT_Handle s_roundText;
static TEXT_Handle s_contactNumberText;
static TEXT_Handle s_bottomLine;
static TEXT_Handle s_topLine;
static BUTTON_Handle s_silenceBtn;
static WM_HTIMER s_timerChangeIconOxy;
static E_Touch s_touchAlarmPos = eTouchScrOne;
static WM_HTIMER s_timerStartAudio;
static bool s_isPressButtonO2Cylinder = false;
static bool s_toggle = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_HandlerTouchPass 								    		*/
/*                                                                                      */
/* Details  -Handle position when user touch 											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int posx : - axis x									 				*/
/*                    - range : 0-800                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*				   int posy : - axis y													*/
/*                    - range : 0-480                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) s_touchAlarmPos : touch position                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_HandlerTouchPass(int posx, int posy)
{
	switch(s_touchAlarmPos)
	{
	case eTouchScrOne:
		if((posx >= 400) &&
				(posy < 240))
		{
			s_touchAlarmPos = eTouchScrTwo;
		}
		break;
	case eTouchScrTwo:
		if((posx > 400) &&
				(posy >= 240))
		{
			s_touchAlarmPos = eTouchScrThree;
		}
		break;
	case eTouchScrThree:
		if((posx < 400) &&
				(posy >= 240))
		{
			s_touchAlarmPos = eTouchScrFour;
		}
		break;
	case eTouchScrFour:
		if((posx < 400) &&
				(posy< 240))
		{
			s_touchAlarmPos = eTouchScrOne;
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_PASSWORD_DIALOG);
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_Callback 								                    */
/*                                                                                      */
/* Details  -Call back function for high alarm screen, handle event from 				*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_highAlarmScreen : high alarm screen						*/
/*                     (O) s_O2CylinderBtn : O2 cylinder button                         */
/*                     (O) s_timerChangeIconOxy : timer change icon O2 cylinder	        */
/*                     (O) s_silenceIcon : silence icon	        						*/
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_topLine : top line									        */
/*                     (O) s_centerText : center text									*/
/*                     (O) s_topLine : top line									        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_Callback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(BACKGROUND_HIGH_ALARM);
		GUI_Clear();

		passwordDialog_SetBackgroundColor(BACKGROUND_HIGH_ALARM);

		GUI_SetColor(GUI_WHITE);
		GUI_AA_FillRoundedRect(20,20,780,460,10);

		GUI_SetColor(PATIENT_BACKGROUND_COLOR);
		GUI_SetPenSize(3);

		if(strlen((char*)setting_GetContact()) > 0)
		{
			GUI_DrawEllipse(595,352,32,32);
		}
		else
		{
			GUI_DrawEllipse(595,382,32,32);
		}
		break;
	case WM_TIMER:
	{
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eTimerChangeIconOxyHighAlarmId:
			if(WM_IsVisible(g_highAlarmScreen) && WM_IsVisible(s_O2CylinderBtn))
			{
				if(s_toggle == false)
				{
					BUTTON_SetBitmap(s_O2CylinderBtn,BUTTON_BI_UNPRESSED,&s_bitmapIconOxyTwo);
				}
				else
				{
					BUTTON_SetBitmap(s_O2CylinderBtn,BUTTON_BI_UNPRESSED,&s_bitmapIconOxyOne);
				}

				s_toggle = !s_toggle;

				WM_RestartTimer(s_timerChangeIconOxy,TIMER_BLINKING);
			}
			break;
		case eTimerRestartAudioId:
			if(WM_IsVisible(g_highAlarmScreen))
			{
				if(WM_IsVisible(s_silenceIcon))
				{
					WM_HideWindow(s_silenceIcon);
					WM_ShowWindow(s_silenceBtn);
					guihandler_StartTimerPlayHighAlarm();
				}
			}
			break;
		default:
			break;
		}
		break;
	}
	case WM_NOTIFY_PARENT:
	{
		switch(WM_GetId(pMsg->hWinSrc))
		{
		case eO2CylinderBtnHighAlarmId:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				//mode cylinder attach
				if(alarmScreenMgr_GetCurrentAlarmId() != eNonvolatileMemoryId)
				{
					modeMgr_GotoAlarmExternalFlowPath();
				}
				else
				{
					s_isPressButtonO2Cylinder = true;
					guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveOpen);
					guiInterface_SetSettingValue(eModeSettingId, eStopMode);
					setting_SaveAllSettingValue();
				}

				WM_HideWindow(s_O2CylinderBtn);
				WM_HideWindow(s_topLine);

				if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
				{
					guiTaskTEXT_SetFont(s_centerText,&GUI_FontEng3L28B2PP);
					TEXT_SetText(s_centerText, "Changed to the oxygen tank");
				}
				else
				{
					guiTaskTEXT_SetFont(s_centerText,&GUI_FontJapan3L40R2PP);
					TEXT_SetText(s_centerText, "酸素ボンベに切り替えました");
				}
			}
			break;
		default:
			break;
		}
		break;
	}
	case WM_TOUCH:
	case WM_TOUCH_CHILD:
	{
		static GUI_PID_STATE StatePID;
		GUI_PID_GetState(&StatePID);

		if(StatePID.Pressed == 1)
		{
			highAlarmScreen_HandlerTouchPass(StatePID.x,StatePID.y);
		}
		break;
	}
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_CallbackBottomLine 								        */
/*                                                                                      */
/* Details  -Call back function for bottom line, handle event from 						*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
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
void highAlarmScreen_CallbackBottomLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BORDER_HIGH_ALARM_COLOR);
		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_CallbackTopLine 								            */
/*                                                                                      */
/* Details  -Call back function for top line, handle event from 						*/
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
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
void highAlarmScreen_CallbackTopLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BORDER_HIGH_ALARM_COLOR);
		GUI_FillRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1);
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_CallbackSilenceBtn 								        */
/*                                                                                      */
/* Details  -Call back function for silence button, handle event from 				    */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_silenceIcon : silence icon                                 */
/*                     (O) s_timerStartAudio : timer 2 minutes play high alarm			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_CallbackSilenceBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(PATIENT_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BACKGROUND_SILENCE_BTN_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0+8,Rect.y0+8,Rect.x1-8,Rect.y1-8,8);

		GUI_SetBkColor(BACKGROUND_SILENCE_BTN_COLOR);
		GUI_SetColor(PATIENT_BACKGROUND_COLOR);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L14B2PP);
			GUI_DispStringAt("Silence",Rect.x0 + 15,Rect.y1/2 - 10);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L25R2PP);
			GUI_DispStringAt("消音",Rect.x0 + 23,Rect.y1/2 - 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(pMsg->hWin);
			WM_ShowWindow(s_silenceIcon);
			audioMgr_StopHighAlarm();
			WM_RestartTimer(s_timerStartAudio,TIMER_RESTART_AUDIO);
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_CallbackSilenceIcon 								        */
/*                                                                                      */
/* Details  -Call back function for silence icon, handle event from 				    */
/* 				the window																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from the window					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_silenceIcon : silence icon                                 */
/*                     (O) s_silenceBtn : silence button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_CallbackSilenceIcon(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(GUI_WHITE);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(PATIENT_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BACKGROUND_SILENCE_ICON_BTN_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0+8,Rect.y0+8,Rect.x1-8,Rect.y1-8,8);

		GUI_DrawBitmap(&s_bitmapSilenceIcon,Rect.x0+15,Rect.y0+12);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_silenceIcon);
			WM_ShowWindow(s_silenceBtn);
			guihandler_StartTimerPlayHighAlarm();
		}
		break;
	default:
		IMAGE_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_Init								       		    		*/
/*                                                                                      */
/* Details  -Initialize high alarm screen											    */
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
/*                     (O) g_highAlarmScreen : high alarm screen                        */
/*                     (O) s_bitmapHighContact : contact bitmap				         	*/
/*                     (O) s_bitmapIconOxyOne : oxy cylinder icon						*/
/*                     (O) s_bitmapIconOxyTwo : oxy cylinder icon			        	*/
/*                     (O) s_bitmapSilenceIcon : silence icon						    */
/*                     (O) s_centerText : center text						            */
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_bottomLine : bottom line						            */
/*                     (O) s_O2CylinderBtn : o2 cylinder button						    */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_highContact : contact image						        */
/*                     (O) s_silenceIcon : silence icon						            */
/*                     (O) s_roundText : round text						                */
/*                     (O) s_bottomText : bottom text						            */
/*                     (O) s_contactNumberText : contact number text					*/
/*                     (O) s_timerChangeIconOxy : timer change icon o2 cylinder			*/
/*                     (O) s_timerStartAudio : timer 2 minutes play high alarm          */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_Init(void)
{
	g_highAlarmScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_mainWindow,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,highAlarmScreen_Callback,0);

	s_bitmapHighContact = bmHighContact;
	s_bitmapIconOxyOne = bmiconOxyOne;
	s_bitmapIconOxyTwo = bmiconOxyTwo;
	s_bitmapSilenceIcon = bmsilenceIocnHigh;

	s_centerText = TEXT_CreateEx(55, 210, 690, 60,g_highAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eCenterTextHighAlarmId,"");
	TEXT_SetTextColor(s_centerText,CENTER_TEXT_COLOR);

	s_topLine = TEXT_CreateEx(20,160,761,3,g_highAlarmScreen,WM_CF_HIDE,TEXT_CF_LEFT,eToplineHighAlarmId,"");
	WM_SetCallback(s_topLine,highAlarmScreen_CallbackTopLine);

	s_bottomLine = TEXT_CreateEx(20,300,761,3,g_highAlarmScreen,WM_CF_SHOW,TEXT_CF_LEFT,eBottomlineHighAlarmId,"");
	WM_SetCallback(s_bottomLine,highAlarmScreen_CallbackBottomLine);

	s_O2CylinderBtn = BUTTON_CreateEx(110,32,580,117, g_highAlarmScreen, WM_CF_HIDE, 0, eO2CylinderBtnHighAlarmId);

	s_silenceBtn = BUTTON_CreateEx(660,330,110,110, g_highAlarmScreen, WM_CF_SHOW, 0, eSilenceBtnHighAlarmId);
	WM_SetCallback(s_silenceBtn,highAlarmScreen_CallbackSilenceBtn);

	s_highContact = IMAGE_CreateEx(50,325,118,118,g_highAlarmScreen,WM_CF_SHOW,IMAGE_CF_MEMDEV,eImageHighAlarmContactId);
	IMAGE_SetBitmap(s_highContact,&s_bitmapHighContact);

	s_silenceIcon = IMAGE_CreateEx(660,330,110,110,g_highAlarmScreen,WM_CF_HIDE,IMAGE_CF_MEMDEV,eImageSilenceIconId);
	WM_SetCallback(s_silenceIcon, highAlarmScreen_CallbackSilenceIcon);

	s_roundText = TEXT_CreateEx(702, 335, 50, 40,g_highAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eRoundTextHighAlarmId,"");

	s_bottomText = TEXT_CreateEx(170, 320, 550, 60,g_highAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eBottomTextHighAlarmId,"");

	s_contactNumberText = TEXT_CreateEx(170, 380, 550, 60,g_highAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eContactNumberHighAlarmId,"");

	s_timerChangeIconOxy = WM_CreateTimer(g_highAlarmScreen, eTimerChangeIconOxyHighAlarmId,TIMER_BLINKING,0);
	s_timerStartAudio = WM_CreateTimer(g_highAlarmScreen, eTimerRestartAudioId, TIMER_100_MS, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_SetNumberAlarm 								       		*/
/*                                                                                      */
/* Details  -Set error number for alarm													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 					*/
/*					 - range : 0-31														*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_roundText : round text                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_SetNumberAlarm(E_AlarmId alarmId)
{
	int8_t buffer[SIZE_BUFFER_NUMBER_ERROR] = {'\0'};

	uint8_t number = 0;

	switch(alarmId)
	{
	case eTemperatureId:
		number = ERROR_CODE_TEMPERATURE;
		break;
	case eHighPressureId:
		number = ERROR_CODE_HIGH_PRESSURE;
		break;
	case eLowPressureId:
		number = ERROR_CODE_LOW_PRESSURE;
		break;
	case eCompressorDCOverVoltageId:
	case eCompressorSoftwareOverCurrentId:
	case eCompressorBoardTemperatureId:
	case eCompressorRPMId:
	case eCompressorHardwareOverCurrent:
		number = ERROR_CODE_COMPRESSOR;
		break;
	case eCoolingFanId:
		number = ERROR_CODE_COOLING_FAN;
		break;
	case eO2ConcentrationId:
		number = ERROR_CODE_O2_CONCENTRATION;
		break;
	case eFlowRateId:
		number = ERROR_CODE_FLOWRATE;
		break;
	case eCompressorOutletTemperatureId:
		number = ERROR_CODE_COMP_OUTLET_TEMP;
		break;
	case eFlowControllerCommunicationId:
	case eFlowControllerPressureSensorId:
	case eFlowControllerUltrasonicSensorId:
	case eFlowControllerTemperatureSensorId:
	case eFlowControllerCommandTimeOutId:
		number = ERROR_CODE_FLOW_CONTROLLER;
		break;
	case eNonvolatileMemoryId:
		number = ERROR_CODE_NVRAM;
		break;
	default:
		break;
	}

	itoa(number, (char*)&buffer[0], 10);

	if(strlen((char*)setting_GetContact()) > 0)
	{
		WM_MoveChildTo(s_roundText,572, 335);
	}
	else
	{
		WM_MoveChildTo(s_roundText,572, 365);
	}

	TEXT_SetTextAlign(s_roundText, GUI_TA_CENTER | GUI_TA_VCENTER);
	guiTaskTEXT_SetFont(s_roundText,&GUI_FontJapan3L30R2PP);
	TEXT_SetText(s_roundText,(char*)&buffer[0]);

	alarmInfor_SetInforAlarm(number);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_Show		 								       		    */
/*                                                                                      */
/* Details  -Show high alarm screen													    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id										*/
/*					 - range : 0-31														*/
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE															*/
/*                     (O) s_touchAlarmPos : touch position                             */
/*                     (O) s_O2CylinderBtn : O2 cylinder button						    */
/*                     (O) s_silenceIcon : silence icon								    */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_O2CylinderBtn : O2 cylinder button						    */
/*                     (O) s_topLine : top line									        */
/*                     (O) s_centerText : center text									*/
/*                     (O) s_roundText : round text									    */
/*                     (O) s_contactNumberText :contact number text						*/
/*                     (O) s_bottomText : bottom text									*/
/*                     (O) s_highContact : contact image								*/
/*                     (O) g_highAlarmScreen : high alarm screen						*/
/*                     (O) s_timerChangeIconOxy : timer change icon O2 cylinder			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_Show(E_AlarmId alarmId)
{
	s_touchAlarmPos = eTouchScrOne;
	passwordDialog_SetBackgroundColor(BACKGROUND_HIGH_ALARM);
	standbyScreen_SetStateMachine(eAlarmType);

	//Record log alarm compressor and flow controller
	if(alarmScreenMgr_GetHighAlarmInStandby() != eLastAlarmId)
	{
		guiTask_writeAlarmLog(alarmScreenMgr_GetHighAlarmInStandby());
		alarmScreenMgr_ClearHighAlarmInStandby();
	}

	guihandler_StopTimerUpdateCumulative();
	guihandler_StopTimerAnimation();
	BUTTON_SetBitmap(s_O2CylinderBtn,BUTTON_BI_UNPRESSED,&s_bitmapIconOxyOne);
	WM_HideWindow(s_silenceIcon);
	WM_ShowWindow(s_silenceBtn);

	if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
	{
		if(modeMgr_GetCurrentMode() == eAlarmMode ||
				alarmScreenMgr_GetCurrentAlarmId() == eNonvolatileMemoryId)
		{
			if(alarmScreenMgr_GetAlarmExternFlowPath() == false)
			{
				WM_ShowWindow(s_O2CylinderBtn);
				WM_ShowWindow(s_topLine);
			}
			else
			{
				WM_HideWindow(s_O2CylinderBtn);
				WM_HideWindow(s_topLine);
			}
		}
	}
	else if (guiTask_GetO2CylinStatus() == O2_CYLIN_NOTATTACH ||
			guiTask_GetO2CylinStatus() == O2_CYLIN_REMOVE)
	{
		WM_HideWindow(s_O2CylinderBtn);
		WM_HideWindow(s_topLine);
	}

	if(modeMgr_GetCurrentMode() == eAlarmExternalFlowPathMode ||
			s_isPressButtonO2Cylinder == true)
	{
		WM_HideWindow(s_O2CylinderBtn);
		WM_HideWindow(s_topLine);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_centerText,&GUI_FontEng3L28B2PP);
			TEXT_SetText(s_centerText, "Changed to the oxygen tank");
		}
		else
		{
			guiTaskTEXT_SetFont(s_centerText,&GUI_FontJapan3L40R2PP);
			TEXT_SetText(s_centerText, "酸素ボンベに切り替えました");
		}
	}

	TEXT_SetTextAlign(s_centerText, GUI_TA_CENTER);
	TEXT_SetTextColor(s_roundText, PATIENT_BACKGROUND_COLOR);

	WM_SetSize(s_contactNumberText,450, 60);
	TEXT_SetTextColor(s_contactNumberText, PATIENT_BACKGROUND_COLOR);
	TEXT_SetTextAlign(s_contactNumberText, GUI_TA_LEFT);
	guiTaskTEXT_SetFont(s_contactNumberText,&GUI_FontJapan3L40R2PP);
	TEXT_SetText(s_contactNumberText,(char*)setting_GetContact());

	WM_SetSize(s_bottomText,450,60);
	TEXT_SetTextColor(s_bottomText, PATIENT_BACKGROUND_COLOR);
	TEXT_SetTextAlign(s_bottomText, GUI_TA_LEFT);

	WM_MoveChildTo(s_contactNumberText,200,400);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		if(strlen((char*)setting_GetContact()) > 0)
		{
			WM_MoveChildTo(s_bottomText,200, 330);
		}
		else
		{
			WM_MoveChildTo(s_bottomText,200, 360);
		}
		guiTaskTEXT_SetFont(s_centerText,&GUI_FontEng3L28B2PP);
		if(modeMgr_GetCurrentMode() != eAlarmExternalFlowPathMode &&
				s_isPressButtonO2Cylinder == false)
		{
			TEXT_SetText(s_centerText,"Machine stop");
		}
		guiTaskTEXT_SetFont(s_bottomText,&GUI_FontEng3L19B2PP);
		TEXT_SetText(s_bottomText,"Please call the emergency");
	}
	else
	{
		if(strlen((char*)setting_GetContact()) > 0)
		{
			WM_MoveChildTo(s_bottomText,200, 325);
		}
		else
		{
			WM_MoveChildTo(s_bottomText,200, 355);
		}

		guiTaskTEXT_SetFont(s_centerText,&GUI_FontJapan3L40R2PP);
		if(modeMgr_GetCurrentMode() != eAlarmExternalFlowPathMode &&
				s_isPressButtonO2Cylinder == false)
		{
			TEXT_SetText(s_centerText,"停止しました");
		}
		guiTaskTEXT_SetFont(s_bottomText,&GUI_FontJapan3L33R2PP);
		TEXT_SetText(s_bottomText,"連絡してください");
	}

	alarmScreenMgr_SendToDevice(alarmId, eActive);
	guiTask_SendPendingEvent(ePlayLedHighAlarmChangeId,eOn);

	audioMgr_Request(eBuzzer200msAudio, ehighAlarmAudio);

	highAlarmScreen_SetNumberAlarm(alarmId);

	if(alarmId == eNonvolatileMemoryId)
	{
		guiInterface_SetSettingValue(eModeSettingId, eStopMode);
		setting_SaveAllSettingValue();
	}
	else
	{
		guiInterface_SetSettingValue(eAlarmRecoverId, alarmId);
		setting_SaveAllSettingValue();
	}

	WM_ShowWindow(s_contactNumberText);
	WM_ShowWindow(s_bottomText);
	WM_ShowWindow(s_centerText);
	WM_ShowWindow(s_highContact);
	WM_ShowWindow(s_roundText);

	if(WM_IsVisible(g_highAlarmScreen))
	{
		WM_HideWindow(g_highAlarmScreen);
	}

	WM_ShowWindow(g_highAlarmScreen);
	WM_SetFocus(g_highAlarmScreen);

	if(WM_IsVisible(s_O2CylinderBtn))
	{
		s_toggle = false;
		WM_RestartTimer(s_timerChangeIconOxy,TIMER_BLINKING);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_Hide								       		    		*/
/*                                                                                      */
/* Details  -Hide high alarm screen											    		*/
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
/*                     (O) s_touchAlarmPos : touch position                             */
/*                     (O) g_highAlarmScreen : high alarm screen						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_Hide(void)
{
	s_touchAlarmPos = eTouchScrOne;
	WM_HideWindow(g_highAlarmScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_UpdateO2Cylinder	 								       	*/
/*                                                                                      */
/* Details  -Update O2 cylinder															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE								 		   							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_highAlarmScreen : high alarm screen						*/
/*                     (O) s_O2CylinderBtn : O2 cylinder button                         */
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_timerChangeIconOxy : timer change icon o2 cylinder			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_UpdateO2Cylinder(void)
{
	if(WM_IsVisible(g_highAlarmScreen))
	{
		if(modeMgr_GetCurrentMode() == eAlarmMode || modeMgr_GetCurrentMode() == eAlarmNoOxyCylinderMode)
		{
			if(guiTask_GetO2CylinStatus() == O2_CYLIN_NOTATTACH ||
					guiTask_GetO2CylinStatus() == O2_CYLIN_REMOVE)
			{
				WM_HideWindow(s_O2CylinderBtn);
				WM_HideWindow(s_topLine);

				if(modeMgr_GetCurrentMode() == eAlarmMode)
				{
					modeMgr_GotoAlarmNotO2Cylinder();
				}
			}
			else if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
			{
				BUTTON_SetBitmap(s_O2CylinderBtn,BUTTON_BI_UNPRESSED,&s_bitmapIconOxyOne);
				if(alarmScreenMgr_GetAlarmExternFlowPath() == false)
				{
					WM_ShowWindow(s_O2CylinderBtn);
					WM_ShowWindow(s_topLine);
				}
				else
				{
					WM_HideWindow(s_O2CylinderBtn);
					WM_HideWindow(s_topLine);
				}

				if(modeMgr_GetCurrentMode() == eAlarmNoOxyCylinderMode)
				{
					modeMgr_gotoAlarm();
				}

				s_toggle = false;
				WM_RestartTimer(s_timerChangeIconOxy,TIMER_BLINKING);
			}
		}
		else
		{
			//Update O2 cylinder in alarm NVRAM
			if(alarmScreenMgr_GetCurrentAlarmId() == eNonvolatileMemoryId)
			{
				if(guiTask_GetO2CylinStatus() == O2_CYLIN_NOTATTACH ||
						guiTask_GetO2CylinStatus() == O2_CYLIN_REMOVE)
				{
					WM_HideWindow(s_O2CylinderBtn);
					WM_HideWindow(s_topLine);
				}
				else if(guiTask_GetO2CylinStatus() == O2_CYLIN_ATTACH)
				{
					BUTTON_SetBitmap(s_O2CylinderBtn,BUTTON_BI_UNPRESSED,&s_bitmapIconOxyOne);
					if(alarmScreenMgr_GetAlarmExternFlowPath() == false)
					{
						if(s_isPressButtonO2Cylinder == false)
						{
							WM_ShowWindow(s_O2CylinderBtn);
							WM_ShowWindow(s_topLine);
						}
					}
					else
					{
						WM_HideWindow(s_O2CylinderBtn);
						WM_HideWindow(s_topLine);
					}

					s_toggle = false;
					WM_RestartTimer(s_timerChangeIconOxy,TIMER_BLINKING);
				}
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-highAlarmScreen_HideO2Cylinder 								       	    */
/*                                                                                      */
/* Details  -Hide O2 cylinder															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE								 		   							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_O2CylinderBtn : O2 cylinder button                         */
/*                     (O) s_topLine : top line						                    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void highAlarmScreen_HideO2Cylinder(void)
{
	WM_HideWindow(s_O2CylinderBtn);
	WM_HideWindow(s_topLine);

	return;
}

/* end of files */

