/********************************************************************/
/*                                                                  */
/* File Name    : OptionScreen.c                               		*/
/*                                                                  */
/* General      : Option screen display		                	    */
/*                 service option	    				            */
/*				   maintenance option	      						*/
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
/*	-Edit function : MULTIPAGE_SetFont()->guiTaskMULTIPAGE_SetFont()*/
/*     (bug 1997)                                                   */
/********************************************************************/

#include "ServiceScreen.h"
#include "MULTIPAGE.h"
#include "ServiceOptionScreen.h"
#include "MaintenanceOptionScreen.h"
#include "OptionScreen.h"
#include "MainScreen.h"
#include "Font.h"
#include "setting.h"
#include "GuiTask.h"
#include "AlarmInfor.h"
#include "ModeMgr.h"
#include "GuiInterface.h"

WM_HWIN g_optionScreen;

static MULTIEDIT_HANDLE s_tabOption;

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_Callback	                                            		*/
/*                                                                                      */
/* Details  -call back function for option screen, handle event from window		   		*/
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
void optionScreen_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_TabOptionCallback	                                            */
/*                                                                                      */
/* Details  -call back function for tab option, handle event from window		        */
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
void optionScreen_TabOptionCallback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(MULTIPAGE_GetSelection(s_tabOption) == eServiceTabId)
		{
			//Service mode
			GUI_SetColor(GUI_BLACK);
			GUI_AA_DrawLine(400,50,800,50);
			GUI_AA_DrawArc(400,40,10,10,180,270);
			GUI_AA_DrawLine(390,10,390,40);
			GUI_AA_DrawArc(380,10,10,10,0,90);
			GUI_SetColor(BUTTON_BACKGROUND_COLOR);
			GUI_AA_DrawArc(381,10,10,10,0,90);
			GUI_AA_DrawArc(382,10,10,10,0,90);
			GUI_AA_DrawArc(383,10,10,10,0,90);
			GUI_AA_DrawArc(384,10,10,10,0,90);
			GUI_AA_DrawArc(385,10,10,10,0,90);
			GUI_AA_DrawArc(386,10,10,10,0,90);
			GUI_AA_DrawArc(387,10,10,10,0,90);
			GUI_AA_DrawArc(388,10,10,10,0,90);
			GUI_AA_DrawArc(389,10,10,10,0,90);
			GUI_FillRoundedRect(Rect.x0 + 391,Rect.y0,800,49,10);
			GUI_FillRect(780,0,800,49);
			GUI_DrawLine(386,0,800,0);
			GUI_DrawLine(386,1,800,1);

			GUI_SetColor(GUI_BLACK);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L18B2PP);

				GUI_DispStringHCenterAt("Service mode",398/2,10);

				GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
				GUI_DispStringHCenterAt("Maintenance mode",(Rect.x0 + 399 + 799)/2,10);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L20B2PP);
				GUI_DispStringHCenterAt("サービスモード",398/2,10);

				GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
				GUI_DispStringHCenterAt("メンテナンスモード",(Rect.x0 + 399 + 799)/2,10);
			}

		}
		else
		{
			//Maintenance mode
			GUI_SetColor(GUI_BLACK);
			GUI_AA_DrawLine(0,50,380,50);
			GUI_AA_DrawArc(380,40,10,10,-90,0);
			GUI_AA_DrawLine(390,10,390,40);
			GUI_AA_DrawArc(400,10,10,10,90,180);

			GUI_SetColor(BUTTON_BACKGROUND_COLOR);
			GUI_AA_DrawArc(399,10,10,10,90,180);
			GUI_AA_DrawArc(398,10,10,10,90,180);
			GUI_AA_DrawArc(397,10,10,10,90,180);
			GUI_AA_DrawArc(396,10,10,10,90,180);
			GUI_AA_DrawArc(395,10,10,10,90,180);
			GUI_AA_DrawArc(394,10,10,10,90,180);
			GUI_AA_DrawArc(393,10,10,10,90,180);
			GUI_AA_DrawArc(392,10,10,10,90,180);
			GUI_AA_DrawArc(391,10,10,10,90,180);
			GUI_FillRoundedRect(Rect.x0,Rect.y0,389,49,10);

			GUI_FillRect(0,0,20,49);

			GUI_DrawLine(0,0,385,0);
			GUI_DrawLine(0,1,385,1);

			GUI_SetColor(GUI_BLACK);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L18B2PP);

				GUI_DispStringHCenterAt("Maintenance mode",(Rect.x0 + 399 + 799)/2,10);

				GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
				GUI_DispStringHCenterAt("Service mode",398/2,10);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L20B2PP);

				GUI_DispStringHCenterAt("メンテナンスモード",(Rect.x0 + 399 + 799)/2,10);

				GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
				GUI_DispStringHCenterAt("サービスモード",398/2,10);
			}

		}
		break;
	default:
		MULTIPAGE_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_Init								       						*/
/*                                                                                      */
/* Details  -Initialize option screen        									    	*/
/*			  																			*/
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
/*                     (O) g_optionScreen : option screen                               */
/*                     (O) s_tabOption : tab option						                */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void optionScreen_Init(void)
{
	g_optionScreen = WM_CreateWindowAsChild(0, 0, 800, 480,g_serviceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP, optionScreen_Callback, 0);

	s_tabOption = MULTIPAGE_CreateEx(0,0,800,480,g_optionScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,GUI_ID_MULTIPAGE0);
	WM_SetCallback(s_tabOption, optionScreen_TabOptionCallback);

	serviceOptionScreen_Init();
	maintenanceOptionScreen_Init();

	MULTIPAGE_SetTabHeight(s_tabOption, 50);
	guiTaskMULTIPAGE_SetFont(s_tabOption,&GUI_FontEng3L18B2PP);
	MULTIPAGE_AddPage(s_tabOption,g_serviceOptionScreen,"Service mode");
	MULTIPAGE_SetTabWidth(s_tabOption,395,eServiceTabId);
	MULTIPAGE_AddPage(s_tabOption,g_maintenanceOptionScreen,"Maintenance mode");
	MULTIPAGE_SetTabWidth(s_tabOption,395,eMaintenanceTabId);
	MULTIPAGE_SelectPage(s_tabOption,eServiceTabId);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_SetTab								       			        */
/*                                                                                      */
/* Details  -Initialize option screen        									    	*/
/*			  																			*/
/*                                                                                      */
/* Arguments : (I) E_TabIndex index	: - service index and maintenance index				*/
/*						- range : 0-1													*/
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_tabOption : tab option                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void optionScreen_SetTab(E_TabIndex index)
{
	MULTIPAGE_SelectPage(s_tabOption,index);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_Show								       			        	*/
/*                                                                                      */
/* Details  -Show option screen        									    			*/
/*			  																			*/
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
/*                     (O) g_optionScreen : option screen                             	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void optionScreen_Show(void)
{
	if(modeMgr_GetCurrentMode() == eStandyMode || modeMgr_GetCurrentMode() == eStopMode)
	{
		maintenanceOptionScreen_SetEnableBtn();
		serviceOptionScreen_SetEnableBtn();
	}
	else
	{
		maintenanceOptionScreen_SetDisableBtn();
		serviceOptionScreen_SetDisableBtn();
	}

	alarmInfor_HideAlarmInfor();
	WM_ShowWindow(g_optionScreen);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-optionScreen_Hide								       			        	*/
/*                                                                                      */
/* Details  -Hide option screen        									    			*/
/*			  																			*/
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
/*                     (O) g_optionScreen : option screen                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void optionScreen_Hide(void)
{
	WM_HideWindow(g_optionScreen);

	return;
}

/* end of files */

