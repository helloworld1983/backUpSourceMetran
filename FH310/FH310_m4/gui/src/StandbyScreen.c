/********************************************************************/
/*                                                                  */
/* File Name    : StandbyScreen.c                                 	*/
/*                                                                  */
/* General      : Display standby screen			                */
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
/* #0002       2017/09/05	    Viet Le				modify 			*/
/*       remove #include "assert.h"									*/
/* #0003       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)*/
/********************************************************************/
#include "string.h"
#include "StandbyScreen.h"
#include "Font.h"
#include "TEXT.h"
#include "setting.h"
#include "IMAGE.h"
#include "GuiTask.h"
#include "PasswordDialog.h"
#include "ModeMgr.h"
#include "OperationScreen.h"
#include "Bitmap.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "AnyPalDisplayHandler.h"

//Define item id
enum{
	eAnypalConnectStandbyScrId,
	eAnypalRoundChargingStandbyScrId,
	eAnypalChargingStandbyScrId,
	eStatusStringStandbyScrId,
	eTimer1MinutesStandbyScrId,
	eTimer3SecondsStandbyScrId,
	eTimerChargingAnypalStandbyScrId,
	eToplineSatndbyScrId,
	eBottomlineSatndbyScrId,
	eAnypalConnectTxtStandbyScrId,
	eAnypalChargingTxtStandbyScrId
};

WM_HWIN g_standbyScreen;

static GUI_BITMAP s_bitmapAnypalconnect;
static GUI_BITMAP s_bitmapAroundCharging;
static GUI_BITMAP s_bitmapCharging;

static TEXT_Handle s_statusMachineTxt;
static TEXT_Handle s_bottomLine;
static TEXT_Handle s_topLine;
static TEXT_Handle s_anypalConnectTxt;
static TEXT_Handle s_anypalChargingTxt;

static IMAGE_Handle s_anypalConnect;
static IMAGE_Handle s_aroundCharging;
static IMAGE_Handle s_charging;

static E_StateMachine s_stateMachine = eStandby;
static WM_HTIMER s_timer1Minutes;
static WM_HTIMER s_timer3Seconds;

static E_PsaSequence s_sequence = PSA_STANDBY;
static E_Touch s_touchPos = eTouchScrOne;
static bool s_isHideAllItem = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_HandlerTouchPass 								    		*/
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
/*                     (O) s_touchPos : touch position                                  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_HandlerTouchPass(int posx, int posy)
{
	switch(s_touchPos)
	{
	case eTouchScrOne:
		if((posx >= 400) &&
				(posy < 240))
		{
			s_touchPos = eTouchScrTwo;
		}
		break;
	case eTouchScrTwo:
		if((posx > 400) &&
				(posy >= 240))
		{
			s_touchPos = eTouchScrThree;
		}
		break;
	case eTouchScrThree:
		if((posx < 400) &&
				(posy >= 240))
		{
			s_touchPos = eTouchScrFour;
		}
		break;
	case eTouchScrFour:
		if((posx < 400) &&
				(posy< 240))
		{
			s_touchPos = eTouchScrOne;
			uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
			if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
			{
				guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
			}
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
/* General 	-standbyScreen_Callback 								                    */
/*                                                                                      */
/* Details  -Call back function for standby screen, handle event from 					*/
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
/* Using Global Data : (I) g_standbyScreen : standby screen							    */
/*					   (I) g_passwordDialog : password dialog							*/
/*                     (O) s_isHideAllItem : flag hide all item     	   				*/
/*                     (O) s_statusMachineTxt : status machine text				      	*/
/*                     (O) s_bottomLine : bottom line						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch (pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_SetBkColor(PATIENT_BACKGROUND_COLOR);
		GUI_GetClientRect(&Rect);
		GUI_ClearRectEx(&Rect);
		break;
	case WM_TIMER:
	{
		//Sub switch
		switch(WM_GetTimerId(pMsg->Data.v))
		{
		case eTimer1MinutesStandbyScrId:
			if(WM_IsVisible(g_standbyScreen))
			{
				if(guiTask_GetFlagServiceScreen() == false)
				{
					if(s_stateMachine == eStandby)
					{
						if((!WM_IsVisible(g_passwordDialog)) && (anypalDisplayHandler_GetAnypalConnectStatus() == false))
						{
							guiTask_SendPendingEvent(eBacklightChangeId, eOff);
						}
						s_isHideAllItem = true;
						WM_HideWindow(s_statusMachineTxt);
						WM_HideWindow(s_bottomLine);
					}
				}
				else
				{
					if(s_stateMachine == eStandby)
					{
						s_isHideAllItem = true;
						WM_HideWindow(s_statusMachineTxt);
						WM_HideWindow(s_bottomLine);
					}
				}
			}
			break;
		case eTimer3SecondsStandbyScrId:
			if(WM_IsVisible(g_standbyScreen))
			{
				if(guiTask_GetFlagServiceScreen() == false)
				{
					if(s_stateMachine != eStandby)
					{
						if(modeMgr_GetCurrentMode() == eStandyMode && s_stateMachine == eEndSequence)
						{
							if((!WM_IsVisible(g_passwordDialog)) && (anypalDisplayHandler_GetAnypalConnectStatus() == false))
							{
								guiTask_SendPendingEvent(eBacklightChangeId, eOff);
							}
							s_isHideAllItem = true;
							WM_HideWindow(s_statusMachineTxt);
							WM_HideWindow(s_bottomLine);
						}
					}
				}
				else
				{
					if(modeMgr_GetCurrentMode() == eStandyMode)
					{
						s_isHideAllItem = true;
						WM_HideWindow(s_statusMachineTxt);
						WM_HideWindow(s_bottomLine);
					}
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
			standbyScreen_HandlerTouchPass(StatePID.x,StatePID.y);
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
/* General 	-standbyScreen_CallbackBottomLine 								            */
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
void standbyScreen_CallbackBottomLine(WM_MESSAGE *pMsg)
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
/* General 	-standbyScreen_CallbackTopLine 								                */
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
void standbyScreen_CallbackTopLine(WM_MESSAGE *pMsg)
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
/* General 	-standbyScreen_Init	 								       		    		*/
/*                                                                                      */
/* Details  -Initialize standby screen												    */
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
/*                     (O) g_standbyScreen : stanby screen                              */
/*                     (O) s_bitmapAnypalconnect : anypal connect icon		            */
/*                     (O) s_bitmapAroundCharging : around charging icon      			*/
/*                     (O) s_bitmapCharging : charging icon						        */
/*                     (O) s_anypalConnect : anypal connect image                       */
/*                     (O) s_aroundCharging : around charging image		         		*/
/*                     (O) s_charging : charging image                                  */
/*                     (O) s_bottomLine : bottom line						            */
/*                     (O) s_topLine : top line                                         */
/*                     (O) s_anypalConnectTxt : anypal connect text			         	*/
/*                     (O) s_anypalChargingTxt : anypal charging text      			   	*/
/*                     (O) s_statusMachineTxt : status machine text		                */
/*                     (O) s_timer1Minutes : timer 1 minutes black screen    			*/
/*                     (O) s_timer3Seconds : timer 3 seconds black screen		        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_Init(void)
{
	g_standbyScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_mainWindow,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,standbyScreen_Callback,0);

	s_bitmapAnypalconnect = bmAnypalconnect;
	s_bitmapAroundCharging = bmAroundCharging;
	s_bitmapCharging = bmCharging;

	s_anypalConnect = IMAGE_CreateEx(20,10,61,62,g_standbyScreen,WM_CF_HIDE,IMAGE_CF_TILE,eAnypalConnectStandbyScrId);
	IMAGE_SetBitmap(s_anypalConnect,&s_bitmapAnypalconnect);

	s_aroundCharging = IMAGE_CreateEx(610,30,21,22,g_standbyScreen,WM_CF_HIDE,IMAGE_CF_TILE,eAnypalRoundChargingStandbyScrId);
	IMAGE_SetBitmap(s_aroundCharging,&s_bitmapAroundCharging);

	s_charging = IMAGE_CreateEx(751,22,29,40,g_standbyScreen,WM_CF_HIDE,IMAGE_CF_TILE,eAnypalChargingStandbyScrId);
	IMAGE_SetBitmap(s_charging,&s_bitmapCharging);

	s_bottomLine = TEXT_CreateEx(20,350,760,3,g_standbyScreen,WM_CF_SHOW,TEXT_CF_LEFT,eBottomlineSatndbyScrId,"");
	WM_SetCallback(s_bottomLine,standbyScreen_CallbackBottomLine);

	s_topLine = TEXT_CreateEx(20,81,760,3,g_standbyScreen,WM_CF_HIDE,TEXT_CF_LEFT,eToplineSatndbyScrId,"");
	WM_SetCallback(s_topLine,standbyScreen_CallbackTopLine);

	s_anypalConnectTxt = TEXT_CreateEx(85,10,250,50,g_standbyScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalConnectTxtStandbyScrId,"");

	s_anypalChargingTxt = TEXT_CreateEx(641,30,100,50,g_standbyScreen,WM_CF_HIDE,TEXT_CF_LEFT,eAnypalChargingTxtStandbyScrId,"");

	s_statusMachineTxt = TEXT_CreateEx(100,400,620,60,g_standbyScreen,WM_CF_SHOW,TEXT_CF_HCENTER,eStatusStringStandbyScrId,"");
	guiTaskTEXT_SetFont(s_statusMachineTxt,&GUI_FontEng3L28B2PP);
	TEXT_SetTextColor(s_statusMachineTxt,GUI_WHITE);
	TEXT_SetTextAlign(s_statusMachineTxt,GUI_TA_CENTER | GUI_TA_VCENTER);

	//Add black screen timer
	s_timer1Minutes = WM_CreateTimer(g_standbyScreen, eTimer1MinutesStandbyScrId, TIMER_1_MINUTES, 0);
	s_timer3Seconds = WM_CreateTimer(g_standbyScreen, eTimer3SecondsStandbyScrId, TIMER_3_SECONDS, 0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_Show	 									       		    	*/
/*                                                                                      */
/* Details  -Show standby screen													    */
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
/*                     (O) s_touchPos : touch position                                  */
/*                     (O) s_statusMachineTxt : status machine text		             	*/
/*                     (O) s_timer1Minutes : timer 1 minutes black screen	            */
/*                     (O) s_timer3Seconds : timer 3 seconds black screen		        */
/*                     (O) s_bottomLine : bottom line						            */
/*                     (O) s_isHideAllItem : flag hide all item						  	*/
/*                     (O) g_standbyScreen : standby screen						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_Show(void)
{
	s_touchPos = eTouchScrOne;
	passwordDialog_SetBackgroundColor(PATIENT_BACKGROUND_COLOR);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		if(s_stateMachine == eStandby)
		{
			guiTaskTEXT_SetFont(s_statusMachineTxt, &GUI_FontEng3L28B2PP);
			TEXT_SetText(s_statusMachineTxt, "Preparation completion");

			WM_RestartTimer(s_timer1Minutes, TIMER_1_MINUTES);
		}
		else
		{
			s_stateMachine = eEndSequence;
			guiTaskTEXT_SetFont(s_statusMachineTxt, &GUI_FontEng3L28B2PP);
			TEXT_SetText(s_statusMachineTxt, "Operation is finished");

			if(s_sequence == PSA_STANDBY)
			{
				WM_RestartTimer(s_timer3Seconds, TIMER_3_SECONDS);
			}
		}
	}
	else
	{
		if(s_stateMachine == eStandby)
		{
			guiTaskTEXT_SetFont(s_statusMachineTxt,&GUI_FontJapan3L38R2PP);
			TEXT_SetText(s_statusMachineTxt, "運 転 準 備 が  完 了 し ま し た");

			WM_RestartTimer(s_timer1Minutes, TIMER_1_MINUTES);
		}
		else
		{
			s_stateMachine = eEndSequence;
			guiTaskTEXT_SetFont(s_statusMachineTxt,&GUI_FontJapan3L38R2PP);
			TEXT_SetText(s_statusMachineTxt, "運 転 を 終 了 し ま す");

			if(s_sequence == PSA_STANDBY)
			{
				WM_RestartTimer(s_timer3Seconds, TIMER_3_SECONDS);
			}
		}
	}

	WM_ShowWindow(s_statusMachineTxt);
	WM_ShowWindow(s_bottomLine);

	if(s_isHideAllItem == true)
	{
		if((!WM_IsVisible(g_passwordDialog)) && (anypalDisplayHandler_GetAnypalConnectStatus() == false))
		{
			guiTask_SendPendingEvent(eBacklightChangeId, eOff);
		}
		WM_HideWindow(s_statusMachineTxt);
		WM_HideWindow(s_bottomLine);
	}

	if(WM_IsVisible(s_statusMachineTxt) && WM_IsVisible(s_bottomLine))
	{
		s_isHideAllItem = false;
	}

	standbyScreen_ShowAnypal();
	standbyScreen_ShowAnypalCharging();

	operationScreen_Hide();
	WM_ShowWindow(g_standbyScreen);
	WM_SetFocus(g_standbyScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_Hide	 									       		    	*/
/*                                                                                      */
/* Details  -Hide standby screen													    */
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
/*                     (O) s_touchPos : touch position                                  */
/*                     (O) s_isHideAllItem : flag hide all item						  	*/
/*                     (O) g_standbyScreen : standby screen						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_Hide(void)
{
	s_touchPos = eTouchScrOne;
	s_isHideAllItem = false;
	WM_HideWindow(g_standbyScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_SetStateMachine 		 								    	*/
/*                                                                                      */
/* Details  -Set state machine															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) E_StateMachine state :  - status machine								*/
/*						- range : 0-5													*/
/*						- unit : NONE													*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_stateMachine : status machine                              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_SetStateMachine(E_StateMachine state)
{
	s_stateMachine = state;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_GetStateMachine 		 								    	*/
/*                                                                                      */
/* Details  -Get state machine															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : E_StateMachine : status machine		                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_stateMachine : status machine							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
E_StateMachine standbyScreen_GetStateMachine(void)
{
	return s_stateMachine;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_ShowAnypal	 								       			*/
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
/* Using Global Data : (I) s_isHideAllItem : flag hide all item					     	*/
/*                     (I) g_passwordDialog : password dialog             				*/
/*                     (I) g_standbyScreen : standby screen       			            */
/*                     (O) s_anypalConnect : anypal connect image           			*/
/*                     (O) s_topLine : top line						                    */
/*                     (O) s_anypalConnectTxt : anypal connect text						*/
/*                     (O) s_charging : charging image						            */
/*                     (O) s_aroundCharging : around charging image			         	*/
/*                     (O) s_topLine : top line						                    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_ShowAnypal(void)
{
	if(anypalDisplayHandler_GetAnypalConnectStatus() == true)
	{
		uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
		if(brightness >= MIN_BRIGHTNESS_SETTING && brightness <= MAX_BRIGHTNESS_SETTING)
		{
			guiTask_SendPendingEvent(eBacklightChangeId, (uint16_t)brightness);
		}

		WM_ShowWindow(s_anypalConnect);
		WM_ShowWindow(s_topLine);

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
		WM_ShowWindow(s_anypalConnectTxt);
	}
	else
	{
		WM_HideWindow(s_anypalConnect);
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
		WM_HideWindow(s_topLine);
		WM_HideWindow(s_anypalConnectTxt);
		WM_HideWindow(s_anypalChargingTxt);

		if(s_isHideAllItem == true)
		{
			if((!WM_IsVisible(g_passwordDialog)) && (anypalDisplayHandler_GetAnypalConnectStatus() == false))
			{
				if(guiTask_GetFlagServiceScreen() == false)
				{
					if(WM_IsVisible(g_standbyScreen))
					{
						guiTask_SendPendingEvent(eBacklightChangeId, eOff);
					}
				}
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_ShowAnypalCharging	 								       	*/
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
/*                     (O) s_anypalChargingTxt : anypal charging text    			   	*/
/*                     (O) s_aroundCharging : around charging image				       	*/
/*                     (O) s_charging : charging image						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_ShowAnypalCharging(void)
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
	else if (anypalDisplayHandler_GetChargingStatus() == eAnyPalChargedFull)
	{
		WM_HideWindow(s_charging);
		WM_HideWindow(s_aroundCharging);
		WM_HideWindow(s_anypalChargingTxt);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_SetPsaSequence 		 								    	*/
/*                                                                                      */
/* Details  -Set psa sequence															*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) E_PsaSequence sequence : - psa sequence								*/
/*						- range : 0-4													*/
/*						- unit : NONE													*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_standbyScreen : standby screen							    */
/*                     (O) s_sequence :  psa sequence                                   */
/*                     (O) s_timer3Seconds : timer 3 seconds black screen	            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_SetPsaSequence(E_PsaSequence sequence)
{
	if(s_sequence != sequence)
	{
		s_sequence = sequence;

		if(s_sequence == PSA_STANDBY)
		{
			if(WM_IsVisible(g_standbyScreen))
			{
				WM_RestartTimer(s_timer3Seconds, TIMER_3_SECONDS);
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-standbyScreen_RestartTimerBlackScreen 		 								*/
/*                                                                                      */
/* Details  -Restart timer black screen when hide password dialog						*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_standbyScreen : standby screen							    */
/*                     (I) g_passwordDialog : password dialog                           */
/*                     (O) s_isHideAllItem : flag hide all item         			   	*/
/*                     (O) s_statusMachineTxt : status machine text				       	*/
/*                     (O) s_bottomLine : bottom line							       	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void standbyScreen_RestartTimerBlackScreen(void)
{
	if(WM_IsVisible(g_standbyScreen))
	{
		if(s_isHideAllItem == true)
		{
			if((!WM_IsVisible(g_passwordDialog)) && (anypalDisplayHandler_GetAnypalConnectStatus() == false))
			{
				guiTask_SendPendingEvent(eBacklightChangeId, eOff);
			}
			s_isHideAllItem = true;
			WM_HideWindow(s_statusMachineTxt);
			WM_HideWindow(s_bottomLine);
		}
	}

	return;
}

/* end of files */
