/********************************************************************/
/*                                                                  */
/* File Name    : MediumAlarmScreen.c                               */
/*                                                                  */
/* General      : Display medium alarm screen			            */
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
/* #0001       2017/09/05       Viet Le				modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/09/19       Viet Le				modify			*/
/*	   Remove save alarm medium to EEPROM in 						*/
/*        mediumAlarmScreen_Show() function (bug 1857)				*/
/* #0003       2017/09/27       Viet Le				modify			*/
/*	   Return original code											*/
/* #0004       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)*/
/********************************************************************/
#include "MediumAlarmScreen.h"
#include "MainScreen.h"
#include "Font.h"
#include "setting.h"
#include "stdio.h"
#include "GuiTask.h"
#include "string.h"
#include "TEXT.h"
#include "OperationScreen.h"
#include "AudioMgr.h"
#include "GuiHandler.h"
#include "stdlib.h"
#include "AlarmInfor.h"
#include "AlarmScreenMgr.h"
#include "StandbyScreen.h"
#include "Bitmap.h"
#include "PasswordDialog.h"
#include "BUTTON.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBottomTextMediumAlarmId,
	eImageMediumAlarmContactId,
	eRoundTextMediumAlarmId,
	eContactNumberMediumAlarmId,
	eUnitFRMediumAlarmId,
	eAnimOneMediumAlarmId,
	eAnimTwoMediumAlarmId,
	eAnimThreeMediumAlarmId,
	eAnimFourMediumAlarmId,
	eAnimFiveMediumAlarmId,
	eAnimSixMediumAlarmId,
	eAnimSevenMediumAlarmId,
	eSilenceBtnMediumAlarmId,
	eImageSilenceIconMediumAlarmId,
	eTimerRestartAudioMediumAlarmId
};

WM_HWIN g_mediumAlarmScreen;

static GUI_BITMAP s_bitmapHighContact;
static GUI_BITMAP s_bitmapanimationAlarm1;
static GUI_BITMAP s_bitmapanimationAlarm2;
static GUI_BITMAP s_bitmapSilenceIcon;
static IMAGE_Handle s_highContact;
static TEXT_Handle s_bottomText;
static TEXT_Handle s_roundText;
static TEXT_Handle s_contactNumberText;
static TEXT_Handle s_unitFlowRate;
static IMAGE_Handle s_animationAlarmOne;
static IMAGE_Handle s_animationAlarmTwo;
static IMAGE_Handle s_animationAlarmThree;
static IMAGE_Handle s_animationAlarmFour;
static IMAGE_Handle s_animationAlarmFive;
static IMAGE_Handle s_animationAlarmSix;
static IMAGE_Handle s_animationAlarmSeven;
static IMAGE_Handle s_silenceIcon;
static BUTTON_Handle s_silenceBtn;
static E_Touch s_touchAlarmPos = eTouchScrOne;
static WM_HTIMER s_timerStartAudio;

/****************************************************************************************/
/*                                                                                      */
/* General 	-operationScreen_HandlerTouchPass 								    		*/
/*                                                                                      */
/* Details  -Handle position when user touch 											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int posx :  - position x									 			*/
/*                     - range : 0-800                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*				   int posy : - position y												*/
/*                     - range : 0-480                                                  */
/*                     - unit : NONE                                                    */
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
void mediumAlarmScreen_HandlerTouchPass(int posx, int posy)
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
/* General 	-mediumAlarmScreen_UpdateMediumAnimation 							        */
/*                                                                                      */
/* Details  -update medium animation when alarm activate             		            */
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE                                      					 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_animationAlarmOne : animation image               		   	*/
/*                     (O) s_animationAlarmTwo : animation image						*/
/*                     (O) s_animationAlarmThree : animation image						*/
/*                     (O) s_animationAlarmFour : animation image						*/
/*                     (O) s_animationAlarmFive : animation image						*/
/*                     (O) s_animationAlarmSix : animation image						*/
/*                     (O) s_animationAlarmSeven : animation image						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_UpdateMediumAnimation(void)
{
	static int counter = 0;
	switch(counter)
	{
	case 0:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_ONE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_TWO:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_THREE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_FOUR:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_FIVE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm2);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm1);
		break;
	case IMAGE_ANIM_SIX:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm2);
		break;
	default:
		break;
	}

	counter++;
	if(counter > MAX_IMAGE_ANIMATION)
	{
		counter = 0;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mediumAlarmScreen_Callback 								                */
/*                                                                                      */
/* Details  -Call back function for medium alarm screen, handle event from 				*/
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
/*                     (O) s_silenceIcon : silence image                                */
/*                     (O) s_silenceBtn : silence button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_Callback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
	{
		GUI_SetBkColor(BACKGROUND_MEDIUM_ALARM);
		GUI_Clear();

		passwordDialog_SetBackgroundColor(BACKGROUND_MEDIUM_ALARM);

		GUI_SetColor(GUI_WHITE);
		GUI_AA_FillRoundedRect(20,20,780,460,10);

		GUI_SetColor(BORDER_MEDIUM_ALARM_COLOR);
		GUI_FillRect(20,300,780,303);//Bottom line

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

		GUI_SetBkColor(GUI_WHITE);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringAt("In operation ...",330,220);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringAt("運転中です. . .",330,220);
		}

		int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
		double floatValue = (double)guiInterface_GetSettingValue(eFlowRateSettingId)/FLOWRATE_SCALE;
		sprintf((char*)buffer,"%.2f",floatValue);

		guiTask_SetFont(&GUI_FontEng3L40B2PP);
		GUI_SetColor(PATIENT_BACKGROUND_COLOR);
		GUI_DispStringAt((char*)buffer,280,55);
		break;
	}
	case WM_TOUCH:
	case WM_TOUCH_CHILD:
	{
		static GUI_PID_STATE StatePID;
		GUI_PID_GetState(&StatePID);

		if(StatePID.Pressed == 1)
		{
			mediumAlarmScreen_HandlerTouchPass(StatePID.x,StatePID.y);
		}
		break;
	}
	case WM_TIMER:
	{
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eTimerRestartAudioMediumAlarmId:
			if(WM_IsVisible(g_mediumAlarmScreen))
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
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mediumAlarmScreen_CallbackSilenceBtn 								        */
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
/*                     (O) s_silenceIcon : silence image                                */
/*                     (O) s_timerStartAudio : timer 2 minutes play alarm               */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_CallbackSilenceBtn(WM_MESSAGE *pMsg)
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
/* General 	-mediumAlarmScreen_CallbackSilenceIcon 								        */
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
/*                     (O) s_silenceIcon : silence image                                */
/*                     (O) s_silenceBtn : silence button						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_CallbackSilenceIcon(WM_MESSAGE *pMsg)
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
/* General 	-mediumAlarmScreen_Init 								       				*/
/*                                                                                      */
/* Details  -Initialize medium alarm screen											    */
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
/*                     (O) g_mediumAlarmScreen : medium alarm screen                    */
/*                     (O) s_bitmapHighContact : contact bitmap					       	*/
/*                     (O) s_bitmapanimationAlarm1 : animation bitmap                   */
/*                     (O) s_bitmapanimationAlarm2 : animation bitmap        			*/
/*                     (O) s_bitmapSilenceIcon : silence bitmap				           	*/
/*                     (O) s_unitFlowRate : display unit flow rate		             	*/
/*                     (O) s_animationAlarmOne : animation image			          	*/
/*                     (O) s_animationAlarmTwo : animation image          				*/
/*                     (O) s_animationAlarmThree : animation image					   	*/
/*                     (O) s_animationAlarmFour : animation image						*/
/*                     (O) s_animationAlarmFive : animation image						*/
/*                     (O) s_animationAlarmSix : animation image						*/
/*                     (O) s_animationAlarmSeven : animation image						*/
/*                     (O) s_highContact : contact image						        */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_roundText : round text						                */
/*                     (O) s_bottomText : bottom text						            */
/*                     (O) s_contactNumberText : contact number text			        */
/*                     (O) s_timerStartAudio : timer 2 minutes play alarm			    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_Init(void)
{
	g_mediumAlarmScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_mainWindow,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,mediumAlarmScreen_Callback,0);

	s_bitmapHighContact = bmHighContact;
	s_bitmapanimationAlarm1 = bmanimationAlarm1;
	s_bitmapanimationAlarm2 = bmanimationAlarm2;
	s_bitmapSilenceIcon = bmsilenceIocnHigh;

	s_unitFlowRate = TEXT_CreateEx(390, 59, 200, 80,g_mediumAlarmScreen,WM_CF_SHOW,TEXT_CF_HCENTER,eUnitFRMediumAlarmId,"LPM");
	guiTaskTEXT_SetFont(s_unitFlowRate,&GUI_FontEng3L36B2PP);
	TEXT_SetTextColor(s_unitFlowRate,PATIENT_BACKGROUND_COLOR);

	s_animationAlarmOne = IMAGE_CreateEx(330,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimOneMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmOne,&s_bitmapanimationAlarm1);

	s_animationAlarmTwo = IMAGE_CreateEx(351,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimTwoMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmTwo,&s_bitmapanimationAlarm2);

	s_animationAlarmThree = IMAGE_CreateEx(372,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimThreeMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmThree,&s_bitmapanimationAlarm2);

	s_animationAlarmFour = IMAGE_CreateEx(393,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimFourMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmFour,&s_bitmapanimationAlarm2);

	s_animationAlarmFive = IMAGE_CreateEx(414,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_MEMDEV,eAnimFiveMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmFive,&s_bitmapanimationAlarm2);

	s_animationAlarmSix = IMAGE_CreateEx(435,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimSixMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmSix,&s_bitmapanimationAlarm2);

	s_animationAlarmSeven = IMAGE_CreateEx(456,140,14,63,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimSevenMediumAlarmId);
	IMAGE_SetBitmap(s_animationAlarmSeven,&s_bitmapanimationAlarm2);

	s_highContact = IMAGE_CreateEx(50,325,118,118,g_mediumAlarmScreen,WM_CF_SHOW,IMAGE_CF_MEMDEV,eImageMediumAlarmContactId);
	IMAGE_SetBitmap(s_highContact,&s_bitmapHighContact);

	s_silenceBtn = BUTTON_CreateEx(660,330,110,110, g_mediumAlarmScreen, WM_CF_SHOW, 0, eSilenceBtnMediumAlarmId);
	WM_SetCallback(s_silenceBtn,mediumAlarmScreen_CallbackSilenceBtn);

	s_silenceIcon = IMAGE_CreateEx(660,330,110,110,g_mediumAlarmScreen,WM_CF_HIDE,IMAGE_CF_MEMDEV,eImageSilenceIconMediumAlarmId);
	WM_SetCallback(s_silenceIcon, mediumAlarmScreen_CallbackSilenceIcon);

	s_roundText = TEXT_CreateEx(702, 335, 50, 40,g_mediumAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eRoundTextMediumAlarmId,"");

	s_bottomText = TEXT_CreateEx(170, 320, 550, 60,g_mediumAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eBottomTextMediumAlarmId,"");

	s_contactNumberText = TEXT_CreateEx(170, 380, 550, 60,g_mediumAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eContactNumberMediumAlarmId,"");

	s_timerStartAudio = WM_CreateTimer(g_mediumAlarmScreen, eTimerRestartAudioMediumAlarmId, TIMER_100_MS, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mediumAlarmScreen_SetNumberAlarm 								       		*/
/*                                                                                      */
/* Details  -Set number error for alarm													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 				    */
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
void mediumAlarmScreen_SetNumberAlarm(E_AlarmId alarmId)
{
	int8_t buffer[SIZE_NUMBER_ALARM_BUFFER] = {'\0'};

	uint8_t number = 0;

	switch(alarmId)
	{
	case eLoweringO2ConcentrationId:
		number = ERROR_CODE_LOW_O2;
		break;
	case eO2ConcentrationSensorId:
		number = ERROR_CODE_O2_SENSOR;
		break;
	case eRisingCompressorOutletTemperatureId:
		number = ERROR_CODE_RISING_COMP_OUTLET;
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
	TEXT_SetText(s_roundText,(char*)buffer);

	alarmInfor_SetInforAlarm(number);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mediumAlarmScreen_Show		 								       			*/
/*                                                                                      */
/* Details  -Show medium alarm screen												    */
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
/*                     (O) s_touchAlarmPos : touch position                             */
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_silenceBtn : silence button                                */
/*                     (O) s_unitFlowRate : display unit flow rate	                  	*/
/*                     (O) s_roundText : round text                                     */
/*                     (O) s_contactNumberText : contact number text				    */
/*                     (O) s_bottomText : bottom text                                   */
/*                     (O) g_mediumAlarmScreen : medium alarm screen					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_Show(E_AlarmId alarmId)
{
	guihandler_StopTimerSendFRToDevice();
	s_touchAlarmPos = eTouchScrOne;
	passwordDialog_SetBackgroundColor(BACKGROUND_MEDIUM_ALARM);
	WM_HideWindow(s_silenceIcon);
	WM_ShowWindow(s_silenceBtn);

	if(standbyScreen_GetStateMachine() == eStartSequence)
	{
		standbyScreen_SetStateMachine(eOperating);
	}

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_unitFlowRate,&GUI_FontEng3L36B2PP);
		TEXT_SetText(s_unitFlowRate,"LPM");
	}
	else
	{
		guiTaskTEXT_SetFont(s_unitFlowRate,&GUI_FontJapan3L36B2PP);
		TEXT_SetText(s_unitFlowRate,"L/分");
	}

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

		guiTaskTEXT_SetFont(s_bottomText,&GUI_FontJapan3L33R2PP);
		TEXT_SetText(s_bottomText,"連絡してください");
	}

	alarmScreenMgr_SendToDevice(alarmId, eActive);
	guiTask_SendPendingEvent(ePlayLedHighAlarmChangeId,eOn);

	audioMgr_Request(eBuzzer200msAudio, ehighAlarmAudio);
	mediumAlarmScreen_SetNumberAlarm(alarmId);

	guiInterface_SetSettingValue(eAlarmRecoverId, alarmId);
	setting_SaveAllSettingValue();

	if(WM_IsVisible(g_mediumAlarmScreen))
	{
		WM_HideWindow(g_mediumAlarmScreen);
	}

	WM_ShowWindow(g_mediumAlarmScreen);
	WM_SetFocus(g_mediumAlarmScreen);

	guihandler_StartTimerAnimationAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-mediumAlarmScreen_Hide		 								       			*/
/*                                                                                      */
/* Details  -Hide medium alarm screen												    */
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
/*                     (O) g_mediumAlarmScreen : medium alarm screen           		   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void mediumAlarmScreen_Hide(void)
{
	WM_HideWindow(g_mediumAlarmScreen);

	return;
}

/* end of files */

