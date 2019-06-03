/********************************************************************/
/*                                                                  */
/* File Name    : AlarmInfor.c                            			*/
/*                                                                  */
/* General       : Displaying alarm infor in service and maintenance*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  	        new file        */
/* #0001       2017/11/01      Viet Le				new file		*/
/*    Show alarm infor in service screen, hide alarm infor on alarm */
/*		and operation(bug 2010)										*/
/********************************************************************/
#include "stdlib.h"
#include "string.h"
#include "TEXT.h"
#include "AlarmInfor.h"
#include "Font.h"
#include "setting.h"
#include "GuiTask.h"
#include "MainScreen.h"
#include "OptionScreen.h"
#include "GuiInterface.h"

//Define item id in AlarmInfor
enum{
	eAlarmInforId
};

static TEXT_Handle s_inforAlarm;
static int s_numberAlarm = 0;

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmInfor_Init												    		*/
/*                                                                                      */
/* Details  -Initialize alarm infor 			    				        			*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																    */
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							    							*/
/*                     (O) s_inforAlarm : information alarm                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmInfor_Init(void)
{
	s_inforAlarm = TEXT_CreateEx(0, 3, 150, 40,g_mainWindow,WM_CF_HIDE,TEXT_CF_HCENTER,eAlarmInforId,"");
	WM_SetCallback(s_inforAlarm, alarmInfor_CallbackAlarmInfor);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmInfor_CallbackAlarmInfor 								                */
/*                                                                                      */
/* Details  -Call back function for information alarm, handle event from 			    */
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
/* Using Global Data : (I) s_inforAlarm : information alarm							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmInfor_CallbackAlarmInfor(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;

	int8_t buffer[SIZE_BUFFER_NUMBER_ALARM] = {'\0'};

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_GetClientRect(&Rect);

		GUI_SetColor(GUI_RED);
		GUI_AA_DrawRoundedRect(Rect.x0 + 7,Rect.y0,Rect.x1,Rect.y1-1,5);
		GUI_DrawEllipse(Rect.x0 + 120,Rect.y0 + 19,15,15);
		guiTask_SetFont(&GUI_FontEng3L10B2PP);

		TEXT_GetText(s_inforAlarm,(char*)buffer,SIZE_BUFFER_NUMBER_ALARM);

		if(strlen((char*)buffer) > 1)
		{
			GUI_DispStringAt((char*)buffer,Rect.x0 + 111,Rect.y0 + 12);
		}
		else
		{
			GUI_DispStringAt((char*)buffer,Rect.x0 + 115,Rect.y0 + 12);
		}

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L14B2PP);
			GUI_DispStringAt("Alarm",Rect.x0 + 18,10);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L14B2PP);
			GUI_DispStringAt("アラーム",Rect.x0 + 18,10);
		}
		break;
	default:
		TEXT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmInfor_SetInforAlarm 								       				*/
/*                                                                                      */
/* Details  -Set error code in service mode									   			*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) int numberAlarm : - error code										*/
/*				   -range : 1-23														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_numberAlarm : error code                                   */
/*                     (O) s_inforAlarm : information alarm						    	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmInfor_SetInforAlarm(int numberAlarm)
{
	s_numberAlarm = numberAlarm;

	if(numberAlarm != 0)
	{
		int8_t buffer[SIZE_BUFFER_NUMBER_ALARM] = {'\0'};

		itoa(numberAlarm,(char*)buffer,10);

		TEXT_SetText(s_inforAlarm,(char*)buffer);

		//Show infor alarm
		WM_ShowWindow(s_inforAlarm);

		if(guiTask_GetFlagServiceScreen() == true)
		{
			if(WM_IsVisible(g_optionScreen))
			{
				WM_HideWindow(s_inforAlarm);
			}
		}
		else//Hide when alarm screen show or operation screen show
		{
			WM_HideWindow(s_inforAlarm);
		}
	}
	else
	{
		WM_HideWindow(s_inforAlarm);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmInfor_ShowAlarmInfor 								       				*/
/*                                                                                      */
/* Details  -Show number alarm in service mode										   	*/
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
/* Using Global Data : (I) s_numberAlarm : error code  							    	*/
/*                     (O) s_inforAlarm : information alarm                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmInfor_ShowAlarmInfor(void)
{
	if(s_numberAlarm != 0)
	{
		WM_ShowWindow(s_inforAlarm);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-alarmInfor_HideAlarmInfor 								       				*/
/*                                                                                      */
/* Details  -Hide number alarm 										   					*/
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
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_inforAlarm : information alarm                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void alarmInfor_HideAlarmInfor(void)
{
	WM_HideWindow(s_inforAlarm);

	return;
}

/* end of files */
