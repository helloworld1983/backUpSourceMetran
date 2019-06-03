/********************************************************************/
/*                                                                  */
/* File Name    : LowAlarmScreen.c                                  */
/*                                                                  */
/* General      : Display low alarm screen			                */
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
/* #0001       2017/08/25	   Viet Le				modify 			*/
/*       Change AnypalMgr to AnypalDisplayHandler                   */
/* #0002       2017/09/05	   Viet Le				modify 			*/
/*       remove #include "assert.h"									*/
/* #0003       2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/* #0004       2017/10/03	   Viet Le				modify 			*/
/*	    - Add flag s_changedFR. When change flow rate by 			*/
/*         encode or remote s_changedFR is true, after 5s in        */
/*         child lock mode s_changedFR is false.If user confirm by  */
/*         encoder or remote s_changedFR is false. Check flag in 	*/
/*		   lowAlarmScreen_Callback() to blinking flow rate			*/
/*          when change flow rate in child lock mode(bug 1947)		*/
/* #0005       2017/10/19	   Viet Le				modify 			*/
/*      - Add passwordDialog_DisplayFlowRate() to display flow rate */
/*			when change flow rate in low alarm screen(bug 1986)		*/
/* #0006       2017/10/27	   Viet Le				modify			*/
/* -Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont().(bug 1997)*/
/* #0007       2017/11/07	   Viet Le				modify			*/
/*   When flow rate change (child ON) during alarm ,flow rate 		*/
/*	blinking, set s_changedFR = false when alarm other active, 		*/
/*   cancel blinking	(bug 2017)									*/
/* #0008       2017/11/07	   Viet Le				modify			*/
/*    When flow rate change in operation screen(child lock ON), 	*/
/*    low alarm active set flow rate on RAM = setting 				*/
/*		flow rate on EEPROM (bug 2017)								*/
/* #0009       2017/11/07	   Viet Le				modify			*/
/*	  Display flow rate when change flow rate (bug 2017)			*/
/* #0010       2017/12/01	   Viet Le				modify			*/
/*    Handle flag blinking flow rate when change flow rate in 		*/
/*     low alarm screen with child lock mode(bug 2046)			    */
/* #0011	   2018/01/18      Viet Le 	    	        modify	    */
/*	 When the flow rate changes, wait 1 second to play the flow rate*/
/*		(bug 2057)													*/
/* #0012	   2018/02/01      Viet Le 	    	        modify	    */
/*   Not check current flow rate and pendding flow rate when play 	*/
/*		flow rate after 1 seconds(bug 2059)							*/
/* #0013	   2018/02/06      Viet Le 	    	        modify	    */
/*	 Add comment(bug 2064) 											*/
/********************************************************************/
#include "LowAlarmScreen.h"
#include "MainScreen.h"
#include "IMAGE.h"
#include "TEXT.h"
#include "BUTTON.h"
#include "GuiTask.h"
#include "Font.h"
#include "OperationScreen.h"
#include "setting.h"
#include "AudioMgr.h"
#include "AlarmScreenMgr.h"
#include "stdlib.h"
#include "string.h"
#include "GuiHandler.h"
#include "AlarmInfor.h"
#include "ModeMgr.h"
#include "StandbyScreen.h"
#include "Bitmap.h"
#include "PasswordDialog.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "AnyPalDisplayHandler.h"

//Define item id
enum{
	eIconContactLowAlarmId,
	eBottomTextLowAlarmId,
	eRoundTextLowAlarmId,
	eAnypalConnectLowAlarmId,
	eAnypalRoundChargingLowAlarmId,
	eAnypalChargingLowAlarmId,
	eFRLowAlarmId,
	eUnitFRLowAlarmId,
	eAnimOneLowAlarmId,
	eAnimTwoLowAlarmId,
	eAnimThreeLowAlarmId,
	eAnimFourLowAlarmId,
	eAnimFiveLowAlarmId,
	eAnimSixLowAlarmId,
	eAnimSevenLowAlarmId,
	eChildlockIconLowAlarmId,
	eContactNumberLowAlarmId,
	eTubeIconLowAlarmId,
	eTimerUpdateFRInLowAlarmId,
	eChildlockTimerInLowAlarmId,
	eToplineLowAlarmScrId,
	eBottomlineLowAlarmScrId,
	eAnypalConnectTxtLowAlarmScrId,
	eAnypalChargingTxtLowAlarmScrId,
	eTimerChargingAnypalLowAlarmId,
	eSilenceBtnLowAlarmId,
	eImageSilenceIconLowAlarmId,
	eTimerRestartAudioLowAlarmId
};

WM_HWIN g_lowAlarmScreen;

static GUI_BITMAP s_bitmapContactAlarm;
static GUI_BITMAP s_bitmapAnypalDisconnect;
static GUI_BITMAP s_bitmapAnimation1;
static GUI_BITMAP s_bitmapAnimation2;
static GUI_BITMAP s_bitmapTube1;
static GUI_BITMAP s_bitmapTube2;
static GUI_BITMAP s_bitmapAnypalconnect;
static GUI_BITMAP s_bitmapAroundCharging;
static GUI_BITMAP s_bitmapCharging;
static GUI_BITMAP s_bitmapChildLock;
static GUI_BITMAP s_bitmapSilenceIcon;
static IMAGE_Handle s_contactAlarm;
static IMAGE_Handle s_anyPalDisconnect;
static IMAGE_Handle s_animationAlarmOne;
static IMAGE_Handle s_animationAlarmTwo;
static IMAGE_Handle s_animationAlarmThree;
static IMAGE_Handle s_animationAlarmFour;
static IMAGE_Handle s_animationAlarmFive;
static IMAGE_Handle s_animationAlarmSix;
static IMAGE_Handle s_animationAlarmSeven;
static IMAGE_Handle s_tubeOne;
static IMAGE_Handle s_childlockIcon;
static IMAGE_Handle s_aroundCharging;
static IMAGE_Handle s_charging;
static IMAGE_Handle s_silenceIcon;
static TEXT_Handle s_bottomText;
static TEXT_Handle s_roundText;
static TEXT_Handle s_contactNumberText;
static TEXT_Handle s_flowRateAlarm;
static WM_HTIMER s_timerUpdateFR;
static WM_HTIMER s_timerChildLock;
static TEXT_Handle s_bottomLine;
static TEXT_Handle s_topLine;
static TEXT_Handle s_anypalConnectTxt;
static TEXT_Handle s_anypalChargingTxt;
static BUTTON_Handle s_silenceBtn;
static int s_counter5s = 0;
static E_Touch s_touchAlarmPos = eTouchScrOne;
static WM_HTIMER s_timerStartAudio;
static bool s_changedFR = false;

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
void lowAlarmScreen_HandlerTouchPass(int posx, int posy)
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
/* General 	-lowAlarmScreen_ShowAnypal	 								       			*/
/*                                                                                      */
/* Details  -Show anypal when connect anypal										   	*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE														 		    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_anypalConnectTxt : anypal connect text                     */
/*                     (O) s_anyPalDisconnect : anypal disconnect image 				*/
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_anypalChargingTxt : anypal charging text			        */
/*                     (O) s_charging : charging icon 						            */
/*                     (O) s_aroundCharging : around charging icon  					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowAnypal(void)
{
	if(anypalDisplayHandler_GetAnypalConnectStatus() == true)
	{
		TEXT_SetTextColor(s_anypalConnectTxt,GUI_WHITE);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			WM_MoveChildTo(s_anypalConnectTxt,85,30);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_anypalConnectTxt, "Anypal respond OK");
		}
		else
		{
			WM_MoveChildTo(s_anypalConnectTxt,90,25);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontJapan3L25R2PP);
			TEXT_SetText(s_anypalConnectTxt,"エニィパル接続");
		}

		IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalconnect);
		WM_ShowWindow(s_anyPalDisconnect);
		WM_ShowWindow(s_topLine);
		WM_ShowWindow(s_anypalConnectTxt);
	}
	else
	{
		if(alarmScreenMgr_GetCurrentAlarmId() == eOverCurrentId)
		{
			IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalDisconnect);
			WM_ShowWindow(s_anyPalDisconnect);
		}
		else
		{
			WM_HideWindow(s_anyPalDisconnect);
		}

		WM_HideWindow(s_topLine);
		WM_HideWindow(s_anypalConnectTxt);
		WM_HideWindow(s_anypalChargingTxt);
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_UpdateIconTube 								                */
/*                                                                                      */
/* Details  -update tube icon 		 			 		                                */
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
/*                     (O) s_tubeOne : tube icon                                        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_UpdateIconTube(void)
{
	static int counterChangeIcon = 0;
	static bool isChangeIcon = false;
	counterChangeIcon++;

	if(counterChangeIcon > COUNTER_CHANGE_TUBE)
	{
		counterChangeIcon = 0;
		if(isChangeIcon == false)
		{
			WM_MoveChildTo(s_tubeOne,30,325);
			IMAGE_SetBitmap(s_tubeOne,&s_bitmapTube1);//Tube B
		}
		else
		{
			WM_MoveChildTo(s_tubeOne,31,325);
			IMAGE_SetBitmap(s_tubeOne,&s_bitmapTube2);//Tube A
		}
		isChangeIcon = !isChangeIcon;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_UpdateCannulaAnimation 								        */
/*                                                                                      */
/* Details  -update medium animation 			 		                                */
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
void lowAlarmScreen_UpdateCannulaAnimation(void)
{
	static int counter = 0;
	switch(counter)
	{
	case 0:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_ONE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_TWO:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_THREE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_FOUR:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_FIVE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_SIX:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
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
/* General 	-lowAlarmScreen_UpdateLowAnimation	 								        */
/*                                                                                      */
/* Details  -update low animation	 			 		                                */
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
void lowAlarmScreen_UpdateLowAnimation(void)
{
	static int counter = 0;
	switch(counter)
	{
	case 0:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_ONE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_TWO:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_THREE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_FOUR:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_FIVE:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation2);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);
		break;
	case IMAGE_ANIM_SIX:
		guiTask_IMAGESetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);
		guiTask_IMAGESetBitmap(s_animationAlarmSeven,&s_bitmapAnimation2);
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
/* General 	-lowAlarmScreen_RepaintFlowRate 								       		*/
/*                                                                                      */
/* Details  -Re paint flow rate setting													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE											 						*/
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
void lowAlarmScreen_RepaintFlowRate(void)
{
	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			GUI_SetColor(GUI_WHITE);
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringAt("In operation ...",20,240);

			GUI_SetColor(RECT_FR_NAME_COLOR_LOW_ALARM);
			GUI_AA_DrawRoundedRect(660,120,780,190,10);
			guiTask_SetFont(&GUI_FontEng3L36B2PP);
			GUI_DispStringHCenterAt("FR", 720, 130);
			GUI_DispStringAt("LPM",660,220);
		}
		else
		{
			GUI_SetColor(GUI_WHITE);
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringAt("運転中です. . .",20,230);

			GUI_SetColor(RECT_FR_NAME_COLOR_LOW_ALARM);
			GUI_AA_DrawRoundedRect(660,130,780,194,10);
			guiTask_SetFont(&GUI_FontJapan3L36B2PP);
			GUI_DispStringHCenterAt("流量", 720, 136);
			GUI_DispStringAt("L/分",662,205);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_FlowRateCallback 		 								    */
/*                                                                                      */
/* Details  -Call back function for flow rate setting								    */
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_FlowRateCallback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	int8_t buffer[SIZE_BUFFER_FLOW_RATE] = {'\0'};
	double floatValue = (double)guiTask_GetFlowrate()/FLOWRATE_SCALE;
	sprintf((char*)buffer,"%.2f",floatValue);

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
	{
		switch(alarmScreenMgr_GetCurrentAlarmId())
		{
		case eCannulaUnder27sId:
		case eCannulaMore27sId:
		case eFilterCleaningId:
		case eSpeakerId:
		case eExternalFlowPathpressureId:
		case eAnypalBatterId:
		case eAnypalRTCId:
		case eAnypalCommunicationId:
		case eOverCurrentId:
			GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
			GUI_GetClientRect(&Rect);
			GUI_ClearRectEx(&Rect);

			GUI_SetColor(GUI_WHITE);
			guiTask_SetFont(&GUI_FontEng3L130R2PP);
			GUI_DispStringAt((char*)buffer,Rect.x0,Rect.y0);
			break;
		default:
			break;
		}
		break;
	}
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_Callback 								                    */
/*                                                                                      */
/* Details  -Call back function for low alarm screen, handle event from 				*/
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
/*                     (O) s_counter5s : counter 5 seconds when change flow in childlock*/
/*                     (O) s_flowRateAlarm : flow rate setting						    */
/*                     (O) s_timerChildLock : timer hide show flow rate					*/
/*                     (O) s_silenceIcon : silence icon 								*/
/*                     (O) s_silenceBtn : silence button								*/
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_Callback(WM_MESSAGE *pMsg)
{
	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_Clear();

		passwordDialog_SetBackgroundColor(PATIENT_BACKGROUND_COLOR);

		GUI_SetColor(GUI_WHITE);
		GUI_SetPenSize(3);

		if(alarmScreenMgr_GetCurrentAlarmId() != eCannulaUnder27sId &&
				alarmScreenMgr_GetCurrentAlarmId() != eCannulaMore27sId)
		{
			if(strlen((char*)setting_GetContact()) > 0)
			{
				GUI_DrawEllipse(595,352,32,32);
			}
			else
			{
				GUI_DrawEllipse(595,382,32,32);
			}
		}

		lowAlarmScreen_RepaintFlowRate();
		break;
	case WM_TIMER:
	{
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eChildlockTimerInLowAlarmId:
			if(WM_IsVisible(g_lowAlarmScreen))
			{
				if(modeMgr_GetCurrentMode() == ePatientMode)
				{
					if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
					{
						if(s_changedFR == true)
						{
							s_counter5s++;

							if(s_counter5s >= COUNTER_CHILD_LOCK_LOW_ALARM)
							{
								guiTask_SetFowRate(guiTask_GetPenddingFlowRate());
								WM_ShowWindow(s_flowRateAlarm);
								WM_Paint(s_flowRateAlarm);

								s_changedFR = false;
								s_counter5s = 0;
								guihandler_StartTimerPlayChildLockAlarm();
								guiTask_SetBlinkingFR(false);

								//Display flow rate in password dialog
								passwordDialog_DisplayFlowRate(guiTask_GetPenddingFlowRate());
							}
							else
							{
								if(WM_IsVisible(s_flowRateAlarm))
								{
									WM_HideWindow(s_flowRateAlarm);
								}
								else
								{
									WM_ShowWindow(s_flowRateAlarm);
								}

								WM_RestartTimer(s_timerChildLock,TIMER_500_MS);
							}
						}
					}
				}
			}
			break;
		case eTimerUpdateFRInLowAlarmId:
			WM_Paint(s_flowRateAlarm);
			break;
		case eTimerRestartAudioLowAlarmId:
			if(WM_IsVisible(g_lowAlarmScreen))
			{
				if(WM_IsVisible(s_silenceIcon))
				{
					WM_HideWindow(s_silenceIcon);
					WM_ShowWindow(s_silenceBtn);
					guihandler_StartTimerPlayMediumAlarm();
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
			lowAlarmScreen_HandlerTouchPass(StatePID.x,StatePID.y);
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
/* General 	-lowAlarmScreen_CallbackBottomLine 								            */
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
void lowAlarmScreen_CallbackBottomLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);

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
/* General 	-lowAlarmScreen_CallbackTopLine 								            */
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
void lowAlarmScreen_CallbackTopLine(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(GUI_WHITE);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);

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
/* General 	-lowAlarmScreen_CallbackSilenceBtn 								        	*/
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
/*                     (O) s_timerStartAudio : timer 2 minutes play alarm               */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_CallbackSilenceBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_WHITE);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BACKGROUND_SILENCE_LOW_BTN_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0+8,Rect.y0+8,Rect.x1-8,Rect.y1-8,8);

		GUI_SetBkColor(BACKGROUND_SILENCE_LOW_BTN_COLOR);
		GUI_SetColor(GUI_WHITE);
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
			if(alarmScreenMgr_GetCurrentAlarmId() == eCannulaMore27sId)
			{
				audioMgr_StopMediumAlarm();
				WM_RestartTimer(s_timerStartAudio,TIMER_RESTART_AUDIO);
			}
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
/* General 	-lowAlarmScreen_CallbackSilenceIcon 								        */
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
void lowAlarmScreen_CallbackSilenceIcon(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(GUI_WHITE);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BACKGROUND_SILENCE_LOW_ICON_BTN_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0+8,Rect.y0+8,Rect.x1-8,Rect.y1-8,8);

		GUI_DrawBitmap(&s_bitmapSilenceIcon,Rect.x0+15,Rect.y0+12);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_silenceIcon);
			WM_ShowWindow(s_silenceBtn);
			if(alarmScreenMgr_GetCurrentAlarmId() == eCannulaMore27sId)
			{
				guihandler_StartTimerPlayMediumAlarm();
			}
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
/* General 	-lowAlarmScreen_Init	 								       				*/
/*                                                                                      */
/* Details  -Initialize low alarm screen											    */
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
/*                     (O) g_lowAlarmScreen : low alarm screen                          */
/*                     (O) s_bitmapContactAlarm : contact icon						    */
/*                     (O) s_bitmapAnypalDisconnect : anypal disconnect icon			*/
/*                     (O) s_bitmapAnimation1 : animation icon						    */
/*                     (O) s_bitmapAnimation2 : animation icon						   	*/
/*                     (O) s_bitmapTube1 : tube icon						            */
/*                     (O) s_bitmapTube2 : tube icon						            */
/*                     (O) s_bitmapChildLock : childlock icon						    */
/*                     (O) s_bitmapAnypalconnect : anypal connect icon					*/
/*                     (O) s_bitmapCharging : icon charging						        */
/*                     (O) s_bitmapAroundCharging : around charging icon				*/
/*                     (O) s_bitmapSilenceIcon : silence icon			 		     	*/
/*                     (O) s_contactAlarm : contact image						        */
/*                     (O) s_bottomText : bottom text						            */
/*                     (O) s_roundText : round text						                */
/*                     (O) s_anyPalDisconnect : anypal disconnect image              	*/
/*                     (O) s_aroundCharging : around charging image			        	*/
/*                     (O) s_charging : charging image						            */
/*                     (O) s_bottomLine : bottom line						            */
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_anypalConnectTxt : anypal connect text						*/
/*                     (O) s_anypalChargingTxt : anypal charging text					*/
/*                     (O) s_flowRateAlarm : flow rate setting						    */
/*                     (O) s_animationAlarmOne : animation image				       	*/
/*                     (O) s_animationAlarmTwo : animation image         				*/
/*                     (O) s_animationAlarmThree : animation image	           			*/
/*                     (O) s_animationAlarmFour : animation image	            		*/
/*                     (O) s_animationAlarmFive : animation image                       */
/*                     (O) s_animationAlarmSix	: animation image                       */
/*                     (O) s_animationAlarmSeven : animation image				       	*/
/*                     (O) s_tubeOne : tube image						                */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_childlockIcon : childlock image						    */
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_contactNumberText : contact number text					*/
/*                     (O) s_timerUpdateFR : timer update flow rate						*/
/*                     (O) s_timerChildLock : timer childlock				          	*/
/*                     (O) s_timerStartAudio : timer 2 minutes play alarm               */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_Init(void)
{
	g_lowAlarmScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_mainWindow,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,lowAlarmScreen_Callback,0);

	//Init bitmap
	s_bitmapContactAlarm = bmContactAlarm;
	s_bitmapAnypalDisconnect = bmAnypalDisconnect;
	s_bitmapAnimation1 = bmAnimation1;
	s_bitmapAnimation2 = bmAnimation2;
	s_bitmapTube1 = bmTube1;
	s_bitmapTube2 = bmTube2;
	s_bitmapChildLock = bmchildlock;
	s_bitmapAnypalconnect = bmAnypalconnect;
	s_bitmapCharging = bmCharging;
	s_bitmapAroundCharging = bmAroundCharging;
	s_bitmapChildLock = bmchildlock;
	s_bitmapSilenceIcon = bmsilenceIconLow;

	s_contactAlarm = IMAGE_CreateEx(25,325,125,128,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eIconContactLowAlarmId);
	IMAGE_SetBitmap(s_contactAlarm,&s_bitmapContactAlarm);

	s_bottomText = TEXT_CreateEx(170, 320, 550, 60,g_lowAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eBottomTextLowAlarmId,"");

	s_roundText = TEXT_CreateEx(702, 335, 50, 40,g_lowAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eRoundTextLowAlarmId,"");

	s_anyPalDisconnect = IMAGE_CreateEx(20,10,61,62,g_lowAlarmScreen,WM_CF_HIDE,IMAGE_CF_TILE,eAnypalConnectLowAlarmId);
	IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalDisconnect);

	s_aroundCharging = IMAGE_CreateEx(610,30,21,22,g_lowAlarmScreen,WM_CF_HIDE,IMAGE_CF_MEMDEV,eAnypalRoundChargingLowAlarmId);
	IMAGE_SetBitmap(s_aroundCharging,&s_bitmapAroundCharging);

	s_charging = IMAGE_CreateEx(751,22,29,40,g_lowAlarmScreen,WM_CF_HIDE,IMAGE_CF_TILE,eAnypalChargingLowAlarmId);
	IMAGE_SetBitmap(s_charging,&s_bitmapCharging);

	s_bottomLine = TEXT_CreateEx(20,280,760,3,g_lowAlarmScreen,WM_CF_SHOW,TEXT_CF_LEFT,eBottomlineLowAlarmScrId,"");
	WM_SetCallback(s_bottomLine,lowAlarmScreen_CallbackBottomLine);

	s_topLine = TEXT_CreateEx(20,81,760,3,g_lowAlarmScreen,WM_CF_HIDE,TEXT_CF_LEFT,eToplineLowAlarmScrId,"");
	WM_SetCallback(s_topLine,lowAlarmScreen_CallbackTopLine);

	s_anypalConnectTxt = TEXT_CreateEx(85,10,250,50,g_lowAlarmScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalConnectTxtLowAlarmScrId,"");

	s_anypalChargingTxt = TEXT_CreateEx(641,30,100,50,g_lowAlarmScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalChargingTxtLowAlarmScrId,"");

	s_flowRateAlarm = TEXT_CreateEx(206, 95, 452, 180,g_lowAlarmScreen,WM_CF_SHOW | WM_CF_MEMDEV,TEXT_CF_HCENTER,eFRLowAlarmId,"");
	WM_SetCallback(s_flowRateAlarm,lowAlarmScreen_FlowRateCallback);

	s_animationAlarmOne = IMAGE_CreateEx(20,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimOneLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmOne,&s_bitmapAnimation1);

	s_animationAlarmTwo = IMAGE_CreateEx(40,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimTwoLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmTwo,&s_bitmapAnimation1);

	s_animationAlarmThree = IMAGE_CreateEx(60,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimThreeLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmThree,&s_bitmapAnimation1);

	s_animationAlarmFour = IMAGE_CreateEx(80,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimFourLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmFour,&s_bitmapAnimation1);

	s_animationAlarmFive = IMAGE_CreateEx(100,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimFiveLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmFive,&s_bitmapAnimation1);

	s_animationAlarmSix = IMAGE_CreateEx(120,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimSixLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmSix,&s_bitmapAnimation1);

	s_animationAlarmSeven = IMAGE_CreateEx(140,130,13,72,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eAnimSevenLowAlarmId);
	IMAGE_SetBitmap(s_animationAlarmSeven,&s_bitmapAnimation1);

	s_tubeOne = IMAGE_CreateEx(30,325,116,117,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eTubeIconLowAlarmId);
	IMAGE_SetBitmap(s_tubeOne,&s_bitmapTube2);

	s_silenceBtn = BUTTON_CreateEx(660,330,110,110, g_lowAlarmScreen, WM_CF_SHOW, 0, eSilenceBtnLowAlarmId);
	WM_SetCallback(s_silenceBtn,lowAlarmScreen_CallbackSilenceBtn);

	s_childlockIcon = IMAGE_CreateEx(692,85,71,32,g_lowAlarmScreen,WM_CF_SHOW,IMAGE_CF_TILE,eChildlockIconLowAlarmId);
	IMAGE_SetBitmap(s_childlockIcon,&s_bitmapChildLock);

	s_silenceIcon = IMAGE_CreateEx(660,330,110,110,g_lowAlarmScreen,WM_CF_HIDE,IMAGE_CF_MEMDEV,eImageSilenceIconLowAlarmId);
	WM_SetCallback(s_silenceIcon, lowAlarmScreen_CallbackSilenceIcon);

	s_contactNumberText = TEXT_CreateEx(170, 380, 550, 60,g_lowAlarmScreen,WM_CF_SHOW,TEXT_CF_RIGHT,eContactNumberLowAlarmId,"");

	s_timerUpdateFR = WM_CreateTimer(g_lowAlarmScreen,eTimerUpdateFRInLowAlarmId,TIMER_100_MS,0);
	s_timerChildLock = WM_CreateTimer(g_lowAlarmScreen, eChildlockTimerInLowAlarmId, TIMER_500_MS, 0);
	s_timerStartAudio = WM_CreateTimer(g_lowAlarmScreen, eTimerRestartAudioLowAlarmId, TIMER_100_MS, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_SetNumberAlarm								       			*/
/*                                                                                      */
/* Details  -Set number for alarm														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 					*/
/*					  - range : 0-31													*/
/*					  - unit : NONE	                                                    */
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
void lowAlarmScreen_SetNumberAlarm(E_AlarmId alarmId)
{
	int8_t buffer[SIZE_NUMBER_ALARM_BUFFER] = {'\0'};

	uint8_t number = 0;

	switch(alarmId)
	{
	case eCannulaUnder27sId:
	case eCannulaMore27sId:
		number = ERROR_CODE_CANNULA;
		break;
	case eFilterCleaningId:
		number = ERROR_CODE_FILTER_CLEANING;
		break;
	case eSpeakerId:
		number = ERROR_CODE_SPEAKER;
		break;
	case eAnypalBatterId:
	case eAnypalRTCId:
	case eAnypalCommunicationId:
	case eOverCurrentId:
		number = ERROR_CODE_ANYPAL;
		break;
	case eExternalFlowPathpressureId:
		number = ERROR_CODE_EXTERNAL_FLOW_PATH;
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
/* General 	-lowAlarmScreen_ShowCannulaAlarm		 									*/
/*                                                                                      */
/* Details  -Show cannula alarm screen													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 					*/
/*					  - range : 0-31													*/
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_roundText : round text                                     */
/*                     (O) s_contactAlarm : contact image						        */
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_tubeOne : tube image						                */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_bottomText : bottom text						            */
/*                     (O) s_contactNumberText : contact number text	                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowCannulaAlarm(E_AlarmId alarmId)
{
	WM_HideWindow(s_roundText);
	WM_HideWindow(s_contactAlarm);
	WM_HideWindow(s_silenceIcon);

	WM_ShowWindow(s_tubeOne);
	if(alarmId == eCannulaMore27sId)
	{
		WM_ShowWindow(s_silenceBtn);
	}
	else
	{
		WM_HideWindow(s_silenceBtn);
	}

	WM_SetSize(s_bottomText,470,70);
	WM_MoveChildTo(s_bottomText,175,305);
	TEXT_SetTextColor(s_bottomText, GUI_WHITE);
	TEXT_SetTextAlign(s_bottomText, GUI_TA_LEFT);

	WM_MoveChildTo(s_contactNumberText,205,390);
	WM_SetSize(s_contactNumberText,462, 80);
	TEXT_SetTextColor(s_contactNumberText, TEXT_COLOR_CANNULA);
	TEXT_SetTextAlign(s_contactNumberText, GUI_TA_LEFT);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_bottomText,&GUI_FontEng3L40B2PP);
		TEXT_SetText(s_bottomText,"Check the tube");

		guiTaskTEXT_SetFont(s_contactNumberText,&GUI_FontEng3L22B2PP);
		TEXT_SetText(s_contactNumberText,"Please make sure that\nthe state is correct tube");
	}
	else
	{
		guiTaskTEXT_SetFont(s_bottomText,&GUI_FontJapan3L48R2PP);
		TEXT_SetText(s_bottomText,"チューブを確認");

		guiTaskTEXT_SetFont(s_contactNumberText,&GUI_FontJapan3L26R2PP);
		TEXT_SetText(s_contactNumberText,"チューブが正しい状態に\nあるか確認してください");
	}

	WM_MoveChildTo(s_tubeOne,31,325);
	IMAGE_SetBitmap(s_tubeOne,&s_bitmapTube2);

	if(alarmId == eCannulaMore27sId)
	{
		alarmScreenMgr_SendToDevice(alarmId, eActive);
		guiTask_SendPendingEvent(ePlayLedMediumAlarmChangeId,eOn);
		audioMgr_Request(eBuzzer200msAudio, eMediumAlarmAudio);
	}

	lowAlarmScreen_ShowAnypal();
	lowAlarmScreen_ShowAnypalCharging();
	lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ShowAnypalAlarm		 									    */
/*                                                                                      */
/* Details  -Show anypal alarm screen													*/
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
/*                     (O) s_tubeOne : tube image                                       */
/*                     (O) s_flowRateAlarm : flow rate setting						   	*/
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_silenceBtn : silence button						        */
/*                     (O) s_contactAlarm : contact image						        */
/*                     (O) s_roundText : round text						                */
/*                     (O) s_anypalConnectTxt : anypal connect text		            	*/
/*                     (O) s_anyPalDisconnect : anypal disconnect image					*/
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_animationAlarmOne : animation image			          	*/
/*                     (O) s_animationAlarmTwo : animation image			        	*/
/*                     (O) s_animationAlarmThree : animation image			         	*/
/*                     (O) s_animationAlarmFour : animation image					  	*/
/*                     (O) s_animationAlarmFive	: animation image						*/
/*                     (O) s_animationAlarmSix : animation image						*/
/*                     (O) s_animationAlarmSeven : animation image						*/
/*                     (O) s_flowRateAlarm : animation image						    */
/*                     (O) s_childlockIcon : childlock image						    */
/*                     (O) s_bottomText : bottom text						            */
/*                     (O) s_contactNumberText : contact number text					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowAnypalAlarm(void)
{
	WM_HideWindow(s_tubeOne);
	WM_HideWindow(s_flowRateAlarm);
	WM_HideWindow(s_silenceIcon);
	WM_HideWindow(s_silenceBtn);

	WM_ShowWindow(s_contactAlarm);
	WM_ShowWindow(s_roundText);

	TEXT_SetTextColor(s_anypalConnectTxt,GUI_WHITE);
	if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
	{
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			WM_MoveChildTo(s_anypalConnectTxt,85,30);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_anypalConnectTxt,"Anypal not respond");
		}
		else
		{
			WM_MoveChildTo(s_anypalConnectTxt,90,25);
			guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontJapan3L25R2PP);
			TEXT_SetText(s_anypalConnectTxt,"エニィパル異常");
		}
	}

	IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalDisconnect);
	WM_ShowWindow(s_anyPalDisconnect);
	WM_ShowWindow(s_anypalConnectTxt);
	WM_ShowWindow(s_topLine);

	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		WM_ShowWindow(s_animationAlarmOne);
		WM_ShowWindow(s_animationAlarmTwo);
		WM_ShowWindow(s_animationAlarmThree);
		WM_ShowWindow(s_animationAlarmFour);
		WM_ShowWindow(s_animationAlarmFive);
		WM_ShowWindow(s_animationAlarmSix);
		WM_ShowWindow(s_animationAlarmSeven);
		WM_ShowWindow(s_flowRateAlarm);

		if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
		{
			WM_ShowWindow(s_childlockIcon);
		}
		else
		{
			WM_HideWindow(s_childlockIcon);
		}
	}
	else
	{
		WM_HideWindow(s_animationAlarmOne);
		WM_HideWindow(s_animationAlarmTwo);
		WM_HideWindow(s_animationAlarmThree);
		WM_HideWindow(s_animationAlarmFour);
		WM_HideWindow(s_animationAlarmFive);
		WM_HideWindow(s_animationAlarmSix);
		WM_HideWindow(s_animationAlarmSeven);
		WM_HideWindow(s_flowRateAlarm);
		WM_HideWindow(s_childlockIcon);
	}

	WM_SetSize(s_bottomText,450, 60);
	TEXT_SetTextColor(s_bottomText, GUI_WHITE);
	TEXT_SetTextAlign(s_bottomText, GUI_TA_LEFT);

	TEXT_SetTextColor(s_roundText, GUI_WHITE);

	WM_SetSize(s_contactNumberText,450,60);
	TEXT_SetTextColor(s_contactNumberText, GUI_WHITE);
	TEXT_SetTextAlign(s_contactNumberText, GUI_TA_LEFT);
	guiTaskTEXT_SetFont(s_contactNumberText,&GUI_FontJapan3L40R2PP);
	TEXT_SetText(s_contactNumberText,(char*)setting_GetContact());

	WM_MoveChildTo(s_contactNumberText,200, 400);

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

	if(modeMgr_GetCurrentMode() == ePatientMode || modeMgr_GetCurrentMode() == eServiceMode)
	{
		guihandler_StartTimerAnimationAlarm();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ShowLowAlarm		 									    */
/*                                                                                      */
/* Details  -Show low alarm screen														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 					*/
/*						- range : 0-31													*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_tubeOne : tube image                                       */
/*                     (O) s_silenceIcon : silence image						        */
/*                     (O) s_silenceBtn : silence button                                */
/*                     (O) s_contactAlarm : contact image                               */
/*                     (O) s_roundText : round text                                     */
/*                     (O) s_bottomText : bottom text                                   */
/*                     (O) s_contactNumberText : contact number text                    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowLowAlarm(E_AlarmId alarmId)
{
	WM_HideWindow(s_tubeOne);
	WM_HideWindow(s_silenceIcon);
	WM_HideWindow(s_silenceBtn);

	WM_ShowWindow(s_contactAlarm);
	WM_ShowWindow(s_roundText);

	WM_SetSize(s_bottomText,450,60);
	TEXT_SetTextColor(s_bottomText, GUI_WHITE);
	TEXT_SetTextAlign(s_bottomText, GUI_TA_LEFT);

	TEXT_SetTextColor(s_roundText, GUI_WHITE);

	WM_SetSize(s_contactNumberText,450,60);
	TEXT_SetTextColor(s_contactNumberText, GUI_WHITE);
	TEXT_SetTextAlign(s_contactNumberText, GUI_TA_LEFT);
	guiTaskTEXT_SetFont(s_contactNumberText,&GUI_FontJapan3L40R2PP);
	TEXT_SetText(s_contactNumberText,(char*)setting_GetContact());

	WM_MoveChildTo(s_contactNumberText,200, 400);

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

	lowAlarmScreen_SetNumberAlarm(alarmId);

	guihandler_StartTimerAnimationAlarm();

	lowAlarmScreen_ShowAnypal();
	lowAlarmScreen_ShowAnypalCharging();
	lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_Show		 									       		*/
/*                                                                                      */
/* Details  -Show low alarm screen													    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId : - alarm id					 					*/
/*					  - range : 0-31													*/
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_touchAlarmPos : touch position                             */
/*                     (O) s_counter5s : counter 5 seconds when change flow in childlock*/
/*                     (O) s_childlockIcon : childlock image                            */
/*                     (O) s_animationAlarmOne : animation image		            	*/
/*                     (O) s_animationAlarmTwo : animation image  		 			   	*/
/*                     (O) s_animationAlarmThree : animation image		             	*/
/*                     (O) s_animationAlarmFour : animation image  					   	*/
/*                     (O) s_animationAlarmFive	: animation image			           	*/
/*                     (O) s_animationAlarmSix : animation image  					   	*/
/*                     (O) s_animationAlarmSeven : animation image				       	*/
/*                     (O) s_flowRateAlarm : flow rate setting   					   	*/
/*                     (O) g_lowAlarmScreen : low alarm screen				          	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_Show(E_AlarmId alarmId)
{
	guihandler_StopTimerSendFRToDevice();
	s_touchAlarmPos = eTouchScrOne;
	passwordDialog_SetBackgroundColor(PATIENT_BACKGROUND_COLOR);
	s_counter5s = 0;
	s_changedFR = false;

	if(standbyScreen_GetStateMachine() == eStartSequence)
	{
		standbyScreen_SetStateMachine(eOperating);
	}

	if(guiTask_GetFlagEnableChangeFR() == false)
	{
		guiTask_SetFlagEnableChangeFR(true);
	}

	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		WM_ShowWindow(s_childlockIcon);
	}
	else
	{
		WM_HideWindow(s_childlockIcon);
	}

	//When flow rate change in operation screen(child lock ON), low alarm active
	//set flow rate on RAM = setting flow rate on EEPROM
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		guiTask_SetFowRate(guiInterface_GetSettingValue(eFlowRateSettingId));
	}

	WM_ShowWindow(s_animationAlarmOne);
	WM_ShowWindow(s_animationAlarmTwo);
	WM_ShowWindow(s_animationAlarmThree);
	WM_ShowWindow(s_animationAlarmFour);
	WM_ShowWindow(s_animationAlarmFive);
	WM_ShowWindow(s_animationAlarmSix);
	WM_ShowWindow(s_animationAlarmSeven);
	WM_ShowWindow(s_flowRateAlarm);

	switch(alarmId)
	{
	case eCannulaUnder27sId:
	case eCannulaMore27sId:
		lowAlarmScreen_ShowCannulaAlarm(alarmId);
		break;
	case eFilterCleaningId:
	case eSpeakerId:
	case eExternalFlowPathpressureId:
		if (alarmId == eFilterCleaningId)
		{
			alarmScreenMgr_SendToDevice(eFilterCleaningId, eActive);
		}

		lowAlarmScreen_ShowLowAlarm(alarmId);
		break;
	case eAnypalBatterId:
	case eAnypalRTCId:
	case eAnypalCommunicationId:
	case eOverCurrentId:
		lowAlarmScreen_ShowAnypalAlarm();
		break;
	default:
		break;
	}

	lowAlarmScreen_SetNumberAlarm(alarmId);

	if(WM_IsVisible(g_lowAlarmScreen))
	{
		WM_HideWindow(g_lowAlarmScreen);
	}

	WM_ShowWindow(g_lowAlarmScreen);
	WM_SetFocus(g_lowAlarmScreen);

	guihandler_StartTimerAnimationAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_Hide		 									       		*/
/*                                                                                      */
/* Details  -Hide low alarm screen													    */
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
/*                     (O) s_counter5s : counter 5 seconds when change flow in childlock*/
/*                     (O) g_lowAlarmScreen : low alarm screen						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_Hide(void)
{
	s_counter5s = 0;
	WM_HideWindow(g_lowAlarmScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm	 							*/
/*                                                                                      */
/* Details  -Show alarm anypal while other alarm active									*/
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
/*                     (O) s_charging : charging image                                  */
/*                     (O) s_aroundCharging : around charging image			         	*/
/*                     (O) s_anypalChargingTxt : anypal charging text           		*/
/*                     (O) s_anypalConnectTxt : anypal connect text	                 	*/
/*                     (O) s_anyPalDisconnect : anypal disconnect image         		*/
/*                     (O) s_topLine : top line						                    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm(void)
{
	if(anypalDisplayHandler_GetAnypalConnectStatus() == true)
	{
		if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
		{
			WM_HideWindow(s_charging);
			WM_HideWindow(s_aroundCharging);
			WM_HideWindow(s_anypalChargingTxt);

			TEXT_SetTextColor(s_anypalConnectTxt,GUI_WHITE);

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				WM_MoveChildTo(s_anypalConnectTxt,85,30);
				guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_anypalConnectTxt,"Anypal not respond");
			}
			else
			{
				WM_MoveChildTo(s_anypalConnectTxt,90,25);
				guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontJapan3L25R2PP);
				TEXT_SetText(s_anypalConnectTxt,"エニィパル異常");
			}

			IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalDisconnect);
			WM_ShowWindow(s_anyPalDisconnect);
			WM_ShowWindow(s_anypalConnectTxt);
			WM_ShowWindow(s_topLine);
		}
	}
	else
	{
		if(anypalDisplayHandler_GetAnypalError() != eNoneApyPalError)
		{
			if(alarmScreenMgr_GetCurrentAlarmId() == eOverCurrentId)
			{
				WM_HideWindow(s_charging);
				WM_HideWindow(s_aroundCharging);
				WM_HideWindow(s_anypalChargingTxt);

				TEXT_SetTextColor(s_anypalConnectTxt,GUI_WHITE);

				if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
				{
					WM_MoveChildTo(s_anypalConnectTxt,85,30);
					guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontEng3L16B2PP);
					TEXT_SetText(s_anypalConnectTxt,"Anypal not respond");
				}
				else
				{
					WM_MoveChildTo(s_anypalConnectTxt,90,25);
					guiTaskTEXT_SetFont(s_anypalConnectTxt,&GUI_FontJapan3L25R2PP);
					TEXT_SetText(s_anypalConnectTxt,"エニィパル異常");
				}

				IMAGE_SetBitmap(s_anyPalDisconnect,&s_bitmapAnypalDisconnect);
				WM_ShowWindow(s_anyPalDisconnect);
				WM_ShowWindow(s_anypalConnectTxt);
				WM_ShowWindow(s_topLine);
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ShowAnypalCharging	 								       	*/
/*                                                                                      */
/* Details  -Show anypal charging when connect anypal									*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE														 		    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_anypalChargingTxt : anypal charging text               	*/
/*                     (O) s_charging : charging image						            */
/*                     (O) s_aroundCharging : around charging image						*/
/*                     (O) s_anypalChargingTxt : anypal charging text					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ShowAnypalCharging(void)
{
	if(anypalDisplayHandler_GetChargingStatus() == eAnyPalCharging)
	{
		TEXT_SetTextColor(s_anypalChargingTxt,GUI_WHITE);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			WM_MoveChildTo(s_aroundCharging,610,30);

			WM_MoveChildTo(s_anypalChargingTxt,638,30);
			WM_SetSize(s_anypalChargingTxt,120,50);
			guiTaskTEXT_SetFont(s_anypalChargingTxt,&GUI_FontEng3L16B2PP);
			TEXT_SetText(s_anypalChargingTxt, "Charging");
		}
		else
		{
			WM_MoveChildTo(s_aroundCharging,624,32);

			WM_MoveChildTo(s_anypalChargingTxt,655,30);
			WM_SetSize(s_anypalChargingTxt,100,50);
			guiTaskTEXT_SetFont(s_anypalChargingTxt,&GUI_FontJapan3L25R2PP);
			TEXT_SetText(s_anypalChargingTxt, "充電中");
		}
		WM_ShowWindow(s_charging);
		WM_ShowWindow(s_aroundCharging);
		WM_ShowWindow(s_anypalChargingTxt);

	}
	else if(anypalDisplayHandler_GetChargingStatus() == eAnyPalChargedFull)
	{
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
		WM_HideWindow(s_anypalChargingTxt);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmScreen_SendFRToDevice	 		 										*/
/*                                                                                      */
/* Details  -Send FR to device after change FR											*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_lowAlarmScreen : low alarm screen						  	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_SendFRToDevice(void)
{
	if(WM_IsVisible(g_lowAlarmScreen))
	{
		uint8_t flowRate = guiTask_GetFlowrate();
		uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
		uint8_t flowrateSetting = 0;
		if(flowRate != penddingFlowRate)
		{
			guiTask_SetPenddingFowRate(flowRate);

			if(guiInterface_GetSettingValue(eChildLockSettingId) == eOff)
			{
				guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
				setting_SaveAllSettingValue();
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
			}

			flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

			if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
			{
				guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
						guiTask_GetConditionO2(),guiTask_GetConditionTemp());
			}
		}

		/** >>#0012
			Not check current flow rate and pendding flow rate when play flow rate
		#0012 >>**/
		audioMgr_PlayFlowRate(flowRate);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ChangeFlowRate								       			*/
/*                                                                                      */
/* Details  -Change flow rate by encoder							   					*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue : - flow rate change					 			*/
/*						- range : 5-60													*/
/*                      - unit  : LPM                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowRateAlarm : flow rate setting         				   	*/
/*                     (O) s_timerChildLock : timer hide show flow in child lock       	*/
/*                     (O) s_timerUpdateFR : timer update flow rate				       	*/
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ChangeFlowRate(uint8_t FRPSAValue)
{
	s_changedFR = true;
    WM_ShowWindow(s_flowRateAlarm);
	WM_Paint(s_flowRateAlarm);

	//Child lock
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		s_counter5s = 0;
		WM_RestartTimer(s_timerChildLock,TIMER_500_MS);
		guiTask_SetBlinkingFR(true);
	}
	else
	{
		/** >>#0013
			Remove play flow rate audio immediately when change flow rate. Play flow rate audio after 1 seconds.
		#0013 >>**/
		guihandler_StartTimerSendFRToDevice();
	}

	WM_RestartTimer(s_timerUpdateFR,TIMER_100_MS);

	//Display flow rate in password dialog
	passwordDialog_DisplayFlowRate(FRPSAValue);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-lowAlarmScreen_ConfirmChildLock 								       		*/
/*                                                                                      */
/* Details  -Confirm flow rate when child lock on							   			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) uint8_t FRPSAValue : flow rate change					 			*/
/*						- range : 5-60													*/
/*                      - unit  : LPM                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_flowRateAlarm : flow rate setting     					   	*/
/*                     (O) s_timerUpdateFR : timer update flow rate			         	*/
/*                     (O) s_changedFR : flag change flow rate				         	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void lowAlarmScreen_ConfirmChildLock(uint8_t FRPSAValue)
{
	s_changedFR = false;
	if(guiInterface_GetSettingValue(eChildLockSettingId) == eOn)
	{
		if(alarmScreenMgr_GetFlagAlarmScreen() == true)
		{
			WM_ShowWindow(s_flowRateAlarm);

			uint8_t flowRate = guiTask_GetFlowrate();
			uint8_t penddingFlowRate = guiTask_GetPenddingFlowRate();
			uint8_t flowrateSetting = 0;
			if(flowRate != penddingFlowRate)
			{
				s_counter5s = 0;
				WM_RestartTimer(s_timerUpdateFR,TIMER_50_MS);
				guiInterface_SetSettingValue(eFlowRateSettingId,guiTask_GetFlowrate());
				setting_SaveAllSettingValue();

				flowrateSetting = guiInterface_GetSettingValue(eFlowRateSettingId);

				if(flowrateSetting >= SETTING_FR_025L && flowrateSetting <= SETTING_FR_300L)
				{
					guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,flowrateSetting,
							guiTask_GetConditionO2(),guiTask_GetConditionTemp());
				}
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eChangingFlowRateLogId);
			}
		}
	}

	return;
}

/* end of files */

