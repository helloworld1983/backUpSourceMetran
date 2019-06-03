/********************************************************************/
/*                                                                  */
/* File Name    : SettingOpenCloseButton.c                          */
/*                                                                  */
/* General      : The SettingOpenCloseButton category provides      */
/* 				  the user an interface to display and              */
/*                change setting open/close in system               */
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
/* #0001       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/* #0002       2017/11/03       Viet Le				modify			*/
/*	  Edit latch valve true meaning of it (bug 2013) 				*/
/* #0003       2017/11/03       Viet Le				modify			*/
/*    Remove settingOpenCloseButton_GetValue() function (not use)   */
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include "SettingOpenCloseButton.h"
#include "BUTTON.h"
#include "board.h"
#include "EDIT.h"
#include "TextLibrary.h"
#include "TaskCommon.h"
#include "setting.h"
#include "GuiTask.h"
#include "Font.h"
#include "MainScreen.h"
#include "GuiInterface.h"

static BUTTON_Handle s_openBtn;
static BUTTON_Handle s_closeBtn;
static EDIT_Handle s_opencloseValue;
static E_LatchValveRequest s_statusValve = eLatchValveClose;

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOpenCloseButton_CallbackOpenBtn								 		*/
/*                                                                                      */
/* Details  -Call back function for setting open button, handle from the window   		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window 								*/
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
void settingOpenCloseButton_CallbackOpenBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L8B2PP);
			GUI_DispStringAt("Open",Rect.x0+6+3 ,Rect.y0+13+6);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("開",Rect.x0+10+6+2 ,Rect.y0+13+3+2);
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
/* General 	-settingOpenCloseButton_CallbackCloseBtn							 		*/
/*                                                                                      */
/* Details  -Call back function for setting close button, handle from the window   		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window 								*/
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
void settingOpenCloseButton_CallbackCloseBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L8B2PP);
			GUI_DispStringAt("Close",Rect.x0 + 6+3 ,Rect.y0 + 13+6);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("閉",Rect.x0 + 10+6+2 ,Rect.y0 + 13+3+2);
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
/* General 	-settingOpenCloseButton_SetName								 				*/
/*                                                                                      */
/* Details  -Set name for setting button												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   GUI_RECT Rect : rectangle for setting								*/
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
void settingOpenCloseButton_SetName(EDIT_Handle obj, GUI_RECT Rect)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		const int8_t* nameStr = (int8_t*)textLibrary_GetString((E_StringID)WM_GetId(obj));
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt((char*)nameStr,Rect.x0,Rect.y0);
	}
	else
	{
		guiTask_SetFont(&GUI_FontJapan3L12B2PP);

		if(WM_GetId(obj) == eTurnBackupSystemStringID)
		{
			GUI_DispStringAt("ラッチバルブ",Rect.x0,Rect.y0);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOpenCloseButton_Init				 								*/
/*                                                                                      */
/* Details  -Initialize setting open/close button                       				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/*                                                                                      */
/* 				   int openBtnId : id of open button 								    */
/*                    - range : 0-336                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/* 				   int closeBtnId : id of close button 								    */
/*                    - range : 0-336                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/* 				   int valueId : id of setting button 									*/
/*                    - range : 0-336                                                   */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_openBtn : open button                                      */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_opencloseValue : display status						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOpenCloseButton_Init(EDIT_Handle obj, int openBtnId,int closeBtnId, int valueId, GUI_RECT Rect)
{
	s_openBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,openBtnId);
	WM_SetCallback(s_openBtn,settingOpenCloseButton_CallbackOpenBtn);

	s_closeBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,closeBtnId);
	WM_SetCallback(s_closeBtn,settingOpenCloseButton_CallbackCloseBtn);

	s_opencloseValue = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,100,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_opencloseValue,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_opencloseValue,GUI_TA_VCENTER | GUI_TA_HCENTER);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontEng3L14B2PP);
		EDIT_SetText(s_opencloseValue,"Close");
	}
	else
	{
		guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontJapan3L14B2PP);
		EDIT_SetText(s_opencloseValue,"閉");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOpenCloseButton_SetValue					 						*/
/*                                                                                      */
/* Details  -set value setting open/close button                       				    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   const int8_t * value : value setting									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_opencloseValue : display status                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOpenCloseButton_SetValue(void)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontEng3L14B2PP);
		EDIT_SetText(s_opencloseValue,"Close");
	}
	else
	{
		guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontJapan3L14B2PP);
		EDIT_SetText(s_opencloseValue,"閉");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOpenCloseButton_Callback							 				*/
/*                                                                                      */
/* Details  -Call back function for setting open/close button, handle from the window   */
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window 								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_opencloseValue : display status                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOpenCloseButton_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		settingOpenCloseButton_SetName(pMsg->hWin,Rect);
		break;
	case WM_NOTIFY_PARENT:
		switch(WM_GetId(pMsg->hWinSrc))
		{
		case eOpenBtnLatchingStringId:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(s_statusValve == eLatchValveClose)
				{
					s_statusValve = eLatchValveOpen;
					if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
					{
						guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontEng3L14B2PP);
						EDIT_SetText(s_opencloseValue, "Open");
					}
					else
					{
						guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontJapan3L14B2PP);
						EDIT_SetText(s_opencloseValue, "開");
					}

					guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveOpen);
				}
			}
			break;
		case eCloseBtnLatchingStringId:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(s_statusValve == eLatchValveOpen)
				{
					s_statusValve = eLatchValveClose;
					if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
					{
						guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontEng3L14B2PP);
						EDIT_SetText(s_opencloseValue, "Close");
					}
					else
					{
						guiTaskEDIT_SetFont(s_opencloseValue,&GUI_FontJapan3L14B2PP);
						EDIT_SetText(s_opencloseValue, "閉");
					}
					guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveClose);
				}
			}
			break;
		default:
			break;
		}
		break;
		default:
			EDIT_Callback(pMsg);
			break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOpenCloseButton_SetOffLatchingValve							 		*/
/*                                                                                      */
/* Details  -Set off latching valve													    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_statusValve : status valve                                 */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOpenCloseButton_SetOffLatchingValve(void)
{
	settingOpenCloseButton_SetValue();

	if(s_statusValve == eLatchValveOpen)
	{
		s_statusValve = eLatchValveClose;
		guiTask_SendPendingEvent(eDeviceBackupSystemChangeId,eLatchValveClose);
	}

	return;
}

/* end of files */
