/********************************************************************/
/*                                                                  */
/* File Name    : SettingExistNoButton.c                            */
/*                                                                  */
/* General      : The SettingExistNoButton category provides        */
/* 				  the user an interface to display and              */
/*                change setting O2 cynlinder in system             */
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
/*                                                                  */
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include "SettingExistNoButton.h"
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

static BUTTON_Handle s_nonBtn;
static BUTTON_Handle s_existBtn;
static EDIT_Handle s_noExistValue;
static bool s_statusCylinder = false;

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingExistNoButton_CallbackExistBtn					 					*/
/*                                                                                      */
/* Details  -Call back function for exist button, handle event from the window      	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : message from the window 							*/
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
void settingExistNoButton_CallbackExistBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringAt("Exist",Rect.x0+5 +3,Rect.y0+13+6);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("有",Rect.x0+10+3,Rect.y0+13+3);
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
/* General 	-settingExistNoButton_CallbackNonBtn				 					    */
/*                                                                                      */
/* Details  -Call back function for non button, handle event from the window      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : message from the window 							*/
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
void settingExistNoButton_CallbackNonBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringAt("Non",Rect.x0 + 8+3 ,Rect.y0 + 13+6);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("無",Rect.x0 + 13 +3,Rect.y0 + 13+3);
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
/* General 	-settingExistNoButton_SetName					 						    */
/*                                                                                      */
/* Details  -Set name setting      														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I)EDIT_Handle obj : object setting										*/
/*  			  GUI_RECT Rect : rectangle for setting									*/
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
void settingExistNoButton_SetName(EDIT_Handle obj, GUI_RECT Rect)
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

		if(WM_GetId(obj) == eTurnOxygenCylinderStringID)
		{
			GUI_DispStringAt("酸素ボンベ設置",Rect.x0,Rect.y0);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingExistNoButton_InitExistNoBtn				 						*/
/*                                                                                      */
/* Details  -Initialize exist/no button save to EPPROM                               	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int existBtnId : - id of exist button 								*/
/*                      - range : 0-336                                                 */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/* 				   int noBtnId : - id of no button 									    */
/*                      - range : 0-336                                                 */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                      - range : 0-336                                                 */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_existBtn : exist button                                    */
/*                     (O) s_nonBtn : non button						                */
/*                     (O) s_noExistValue : display status						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingExistNoButton_InitExistNoBtn(EDIT_Handle obj,int existBtnId,int noBtnId, int valueId, GUI_RECT Rect)
{
	s_existBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,existBtnId);
	WM_SetCallback(s_existBtn,settingExistNoButton_CallbackExistBtn);

	s_nonBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,noBtnId);
	WM_SetCallback(s_nonBtn,settingExistNoButton_CallbackNonBtn);

	s_noExistValue = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,100,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_noExistValue,EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_noExistValue,GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingExistNoButton_GetValue					 							*/
/*                                                                                      */
/* Details  -Get value setting           				                             	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE								 									*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : true if value is exist		                                    */
/*        		 		else if value is non	                                        */
/*                                                                                      */
/* Using Global Data : (I) s_statusCylinder : status cylinder					    	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool settingExistNoButton_GetValue(void)
{
	return s_statusCylinder;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingExistNoButton_SetValue				 								*/
/*                                                                                      */
/* Details  -Set value setting														    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/*                 const int8_t * value : value setting									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_noExistValue : display status                              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingExistNoButton_SetValue(void)
{
	uint8_t cylinder = guiInterface_GetSettingValue(eO2CylinderSettingId);

	if(cylinder == eOn)
	{
		s_statusCylinder = true;
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontEng3L14B2PP);
			EDIT_SetText(s_noExistValue, "Exist");
		}
		else
		{
			guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontJapan3L14B2PP);
			EDIT_SetText(s_noExistValue, "有");
		}
	}
	else
	{
		s_statusCylinder = false;
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontEng3L14B2PP);
			EDIT_SetText(s_noExistValue, "Non");
		}
		else
		{
			guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontJapan3L14B2PP);
			EDIT_SetText(s_noExistValue, "無");
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingExistNoButton_Callback					 							*/
/*                                                                                      */
/* Details  -Call back function for setting button, handle event from the windown       */
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : message from the window 							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_noExistValue : display status                              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingExistNoButton_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		settingExistNoButton_SetName(pMsg->hWin,Rect);
		break;
	case WM_NOTIFY_PARENT:
	{
		switch(WM_GetId(pMsg->hWinSrc))
		{
		case eExistBtnStringId:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(s_statusCylinder == false)
				{
					s_statusCylinder = true;
					if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
					{
						guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontEng3L14B2PP);
						EDIT_SetText(s_noExistValue, "Exist");
					}
					else
					{
						guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontJapan3L14B2PP);
						EDIT_SetText(s_noExistValue, "有");
					}

					guiInterface_SetSettingValue(eO2CylinderSettingId,eOn);
					setting_SaveAllSettingValue();

					guiTask_SendPendingEvent(eOxyCylinderChangeId,settingExistNoButton_GetValue());
				}
			}
			break;
		case eNoExistBtnStringId:
			if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
			{
				if(s_statusCylinder == true)
				{
					s_statusCylinder = false;
					if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
					{
						guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontEng3L14B2PP);
						EDIT_SetText(s_noExistValue, "Non");
					}
					else
					{
						guiTaskEDIT_SetFont(s_noExistValue,&GUI_FontJapan3L14B2PP);
						EDIT_SetText(s_noExistValue, "無");
					}

					guiInterface_SetSettingValue(eO2CylinderSettingId,eOff);
					setting_SaveAllSettingValue();

					guiTask_SendPendingEvent(eOxyCylinderChangeId,settingExistNoButton_GetValue());
				}
			}
			break;
		default:
			break;
		}
		break;
	}
	default:
		EDIT_Callback(pMsg);
		break;
	}

	return;
}

/* end of files */
