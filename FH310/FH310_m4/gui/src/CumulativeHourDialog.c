/********************************************************************/
/*                                                                  */
/* File Name    : CumulativeHourDialog.c                            */
/*                                                                  */
/* General       : unit cumulative operation hour change		 	*/
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
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include <EEPROM.h>
#include "CumulativeHourDialog.h"
#include "UnitCumulativeHourScreen.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "setting.h"
#include "GuiTask.h"
#include "OperationScreen.h"
#include "Font.h"
#include "MainScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum
{
	eZeroUnitCumulativeHourBtnId,
	eOneUnitCumulativeHourBtnId,
	eTwoUnitCumulativeHourBtnId,
	eThreeUnitCumulativeHourBtnId,
	eFourUnitCumulativeHourBtnId,
	eFiveUnitCumulativeHourBtnId,
	eSixUnitCumulativeHourBtnId,
	eSevenUnitCumulativeHourBtnId,
	eEightUnitCumulativeHourBtnId,
	eNineUnitCumulativeHourBtnId,
	eCloseDialogUnitCumulativeHourId,
	eClearDialogCumulativeHourId,
	eDecisionCumulativeHourId
};

WM_HWIN g_cumulativeHourDialog;

static EDIT_Handle s_editUnitCumualtiveHour;
static BUTTON_Handle s_zeroBtn;
static BUTTON_Handle s_oneBtn;
static BUTTON_Handle s_twoBtn;
static BUTTON_Handle s_threeBtn;
static BUTTON_Handle s_fourBtn;
static BUTTON_Handle s_fineBtn;
static BUTTON_Handle s_sixBtn;
static BUTTON_Handle s_sevenBtn;
static BUTTON_Handle s_eightBtn;
static BUTTON_Handle s_nineBtn;
static BUTTON_Handle s_clearBtn;
static BUTTON_Handle s_decisionBtn;
static BUTTON_Handle s_closeContactBtn;
static uint8_t s_FlowRateShow = 0;
static int8_t s_unitCumulativeBuffer[SIZE_BUFFER_CUMULATIVE] = {'\0'};

/****************************************************************************************/
/*                                                                                      */
/* General 	-cumulativeHourDialog_Callback 								       			*/
/*                                                                                      */
/* Details  -Call back function for unit cumulative operation hour,                     */
/*  			handle event from the window											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I)WM_MESSAGE *pMsg : messenger from window				 				*/
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
void cumulativeHourDialog_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetColor(BORDER_CUMULATIVE_HOUR_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Please enter the unit's operation total hour", 400,10);
			GUI_DispStringAt("Hours",565,62);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L20B2PP);
			GUI_DispStringHCenterAt("機器稼働時間を入力してください", 400,10);

			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringAt("時間",565,62);
		}
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Call back function 								       					*/
/*                                                                                      */
/* Details  -Call back function for number button, handle event from the window			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE *pMsg : messenger from window								*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_unitCumulativeBuffer : unit cumulative hour buffer         */
/*                     (O) s_editUnitCumualtiveHour : display unit cumulative hour		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_CallbackNumberBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	int8_t numberBuffer[SIZE_BUFFER_NUMBER] = {'\0'};

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,0);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,0,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		guiTask_SetFont(&GUI_FontEng3L16B2PP);

		BUTTON_GetText(pMsg->hWin,(char*)&numberBuffer[0],3);
		GUI_DispStringHCenterAt((char*)numberBuffer, Rect.x1/2, 22);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			BUTTON_GetText(pMsg->hWin,(char*)&numberBuffer[0],10);
			strncat((char*)&s_unitCumulativeBuffer[0],(char*)&numberBuffer[0],SIZE_BUFFER_NUMBER);

			EDIT_SetText(s_editUnitCumualtiveHour,(char*)&s_unitCumulativeBuffer[0]);
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
/* General 	-cumulativeHourDialog_callBackClearBtn 								        */
/*                                                                                      */
/* Details  -call back fuction for clear button, handle event from the window 			*/
/*			  														                    */
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
/*                     (O) s_editUnitCumualtiveHour : display unit cumualtive hour      */
/*                     (O) s_unitCumulativeBuffer : buffer unit cumulative hour			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_callBackClearBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Clear", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("クリア", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			EDIT_SetText(s_editUnitCumualtiveHour,"0");

			for(int i = 0; i < SIZE_BUFFER_CUMULATIVE; i++)
			{
				s_unitCumulativeBuffer[i] = '\0';
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
/* General 	-cumulativeHourDialog_ChangeCumulativeFR 								    */
/*                                                                                      */
/* Details  -Change unit cumulative hour for each flow rate					 			*/
/*			  														                    */
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
/*                     (O) s_editUnitCumualtiveHour : display unit cumulative hour      */
/*                     (O) s_FlowRateShow : display flow rate						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_ChangeCumulativeFR(void)
{
	int8_t hourBuffer[SIZE_BUFFER_HOUR] = {'\0'};
	EDIT_GetText(s_editUnitCumualtiveHour,(char*)hourBuffer,6);
	int hoursChange = atoi((char*)hourBuffer);

	if(unitCumulativeHourScreen_GetFlagChangeUnitTotal() == false)
	{
		if(s_FlowRateShow == SETTING_FR_300L)
		{
			operationScreen_ClearUnitCumulativeThree(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_250L)
		{
			operationScreen_ClearUnitCumulativeTwoFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_200L)
		{
			operationScreen_ClearUnitCumulativeTwo(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_175L)
		{
			operationScreen_ClearUnitCumulativeOneSevenFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_150L)
		{
			operationScreen_ClearUnitCumulativeOneFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_125L)
		{
			operationScreen_ClearUnitCumulativeOneTwoFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_100L)
		{
			operationScreen_ClearUnitCumulativeOne(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_075L)
		{
			operationScreen_ClearUnitCumulativeZeroSevenFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_050L)
		{
			operationScreen_ClearUnitCumulativeZeroFive(hoursChange);
		}
		else if (s_FlowRateShow == SETTING_FR_025L)
		{
			operationScreen_ClearUnitCumulativeZeroTwoFive(hoursChange);
		}
	}
	else
	{
		operationScreen_ClearUnitCumulative(hoursChange);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-cumulativeHourDialog_CallbackDecisionBtn								    */
/*                                                                                      */
/* Details  -call back fuction for Decision button, handle event from the window 	    */
/*			  														                    */
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
void cumulativeHourDialog_CallbackDecisionBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Decision", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("決定", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			cumulativeHourDialog_ChangeCumulativeFR();
			cumulativeHourDialog_Hide();

			unitCumulativeHourScreen_ShowAllItem();
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
/* General 	-cumulativeHourDialog_CallbackCloseBtn 								        */
/*                                                                                      */
/* Details  -call back fuction for close button, handle event from the window 			*/
/*			  														                    */
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
void cumulativeHourDialog_CallbackCloseBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 22);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 22);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			cumulativeHourDialog_Hide();

			unitCumulativeHourScreen_ShowAllItem();
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
/* General 	-cumulativeHourDialog_Init 													*/
/*                                                                                      */
/* Details  -Initialize cumulative operation hour change dialog							*/
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
/*                     (O) g_cumulativeHourDialog : cumulative hour dialog              */
/*                     (O) s_editUnitCumualtiveHour : display cumulative hour			*/
/*                     (O) s_zeroBtn : zero number button								*/
/*                     (O) s_oneBtn : one number button									*/
/*                     (O) s_twoBtn : two number button									*/
/*                     (O) s_threeBtn : three number button								*/
/*                     (O) s_fourBtn : four number button								*/
/*                     (O) s_fineBtn : five number button								*/
/*                     (O) s_sixBtn : six number button									*/
/*                     (O) s_sevenBtn : seven number button								*/
/*                     (O) s_eightBtn : eight number button								*/
/*                     (O) s_nineBtn : nine number button								*/
/*                     (O) s_clearBtn : clear button									*/
/*                     (O) s_decisionBtn : decision button								*/
/*                     (O) s_closeContactBtn : close button								*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_Init(void)
{
	g_cumulativeHourDialog = WM_CreateWindowAsChild(10, 10, 780, 460,g_unitCumulativeHourScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,cumulativeHourDialog_Callback,0);

	s_editUnitCumualtiveHour = EDIT_CreateEx(220,40,340,50,g_cumulativeHourDialog,WM_CF_SHOW,0,GUI_ID_EDIT5,5);
	guiTaskEDIT_SetFont (s_editUnitCumualtiveHour, &GUI_FontEng3L16B2PP);
	EDIT_SetTextColor(s_editUnitCumualtiveHour,EDIT_CI_ENABLED ,GUI_BLACK);
	EDIT_SetTextAlign(s_editUnitCumualtiveHour,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_zeroBtn = BUTTON_CreateEx(345,320,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eZeroUnitCumulativeHourBtnId);
	BUTTON_SetText(s_zeroBtn,"0");
	WM_SetCallback(s_zeroBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_oneBtn = BUTTON_CreateEx(245,245,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eOneUnitCumulativeHourBtnId);
	BUTTON_SetText(s_oneBtn,"1");
	WM_SetCallback(s_oneBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_twoBtn = BUTTON_CreateEx(345,245,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eTwoUnitCumulativeHourBtnId);
	BUTTON_SetText(s_twoBtn,"2");
	WM_SetCallback(s_twoBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_threeBtn = BUTTON_CreateEx(445,245,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eThreeUnitCumulativeHourBtnId);
	BUTTON_SetText(s_threeBtn,"3");
	WM_SetCallback(s_threeBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_fourBtn = BUTTON_CreateEx(245,170,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eFourUnitCumulativeHourBtnId);
	BUTTON_SetText(s_fourBtn,"4");
	WM_SetCallback(s_fourBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_fineBtn = BUTTON_CreateEx(345,170,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eFiveUnitCumulativeHourBtnId);
	BUTTON_SetText(s_fineBtn,"5");
	WM_SetCallback(s_fineBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_sixBtn = BUTTON_CreateEx(445,170,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eSixUnitCumulativeHourBtnId);
	BUTTON_SetText(s_sixBtn,"6");
	WM_SetCallback(s_sixBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_sevenBtn = BUTTON_CreateEx(245,95,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eSevenUnitCumulativeHourBtnId);
	BUTTON_SetText(s_sevenBtn,"7");
	WM_SetCallback(s_sevenBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_eightBtn = BUTTON_CreateEx(345,95,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eEightUnitCumulativeHourBtnId);
	BUTTON_SetText(s_eightBtn,"8");
	WM_SetCallback(s_eightBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_nineBtn = BUTTON_CreateEx(445,95,90,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eNineUnitCumulativeHourBtnId);
	BUTTON_SetText(s_nineBtn,"9");
	WM_SetCallback(s_nineBtn,cumulativeHourDialog_CallbackNumberBtn);

	s_clearBtn = BUTTON_CreateEx(585,300,120,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eClearDialogCumulativeHourId);
	WM_SetCallback(s_clearBtn,cumulativeHourDialog_callBackClearBtn);

	s_decisionBtn = BUTTON_CreateEx(585,380,120,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eDecisionCumulativeHourId);
	WM_SetCallback(s_decisionBtn,cumulativeHourDialog_CallbackDecisionBtn);

	s_closeContactBtn = BUTTON_CreateEx(75,380,120,70,g_cumulativeHourDialog,WM_CF_SHOW,0,eCloseDialogUnitCumulativeHourId);
	WM_SetCallback(s_closeContactBtn,cumulativeHourDialog_CallbackCloseBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-cumulativeHourDialog_Show		 											*/
/*                                                                                      */
/* Details  -Show cumulative operation hour change dialog								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flow	: - flow rate setting				 					*/
/*				   -range : 5-60													    */
/*                 -unit : L/M                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) g_cumulativeHourDialog : cumulative hour dialog			    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_Show(uint8_t flow)
{
	cumulativeHourDialog_ShowHour(flow);
	WM_ShowWindow(g_cumulativeHourDialog);
	WM_SetFocus(g_cumulativeHourDialog);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-cumulativeHourDialog_Hide		 											*/
/*                                                                                      */
/* Details  -Hide cumulative operation hour change dialog								*/
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
/*                     (O) g_cumulativeHourDialog : cumulative hour dialog              */
/*                     (O) s_unitCumulativeBuffer : unit cumulative buffer				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_Hide(void)
{
	for(int i = 0; i < SIZE_BUFFER_CUMULATIVE; i++)
	{
		s_unitCumulativeBuffer[i] = '\0';
	}

	unitCumulativeHourScreen_ShowUnitHour();
	WM_HideWindow(g_cumulativeHourDialog);

	unitCumulativeHourScreen_SetFlagChangeUnitTotal(false);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-cumulativeHourDialog_ShowHour 		 										*/
/*                                                                                      */
/* Details  -Show cumulative operation hour 											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flow	: - flow rate setting				 					*/
/*				   -range : 5-60													    */
/*                 -unit : L/M                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_FlowRateShow : display flow rate                           */
/*                     (O) s_editUnitCumualtiveHour : display unit cumulative hour		*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void cumulativeHourDialog_ShowHour(uint8_t flow)
{
	int8_t hourBuffer[SIZE_BUFFER_HOUR] = {'\0'};
	int hour = 0;
	CumulativeHourTime time;

	if(unitCumulativeHourScreen_GetFlagChangeUnitTotal() == false)
	{
		s_FlowRateShow = flow;

		if(flow == SETTING_FR_300L)
		{
			time = guiInterface_GetCumulativeHour(uints_300);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_250L)
		{
			time = guiInterface_GetCumulativeHour(units_250);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_200L)
		{
			time = guiInterface_GetCumulativeHour(uints_200);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_175L)
		{
			time = guiInterface_GetCumulativeHour(uints_175);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_150L)
		{
			time = guiInterface_GetCumulativeHour(uints_150);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_125L)
		{
			time = guiInterface_GetCumulativeHour(uints_125);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_100L)
		{
			time = guiInterface_GetCumulativeHour(uints_100);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_075L)
		{
			time = guiInterface_GetCumulativeHour(uints_075);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_050L)
		{
			time = guiInterface_GetCumulativeHour(uints_050);
			hour = time.hour;
		}
		else if (flow == SETTING_FR_025L)
		{
			time = guiInterface_GetCumulativeHour(uints_025);
			hour = time.hour;
		}
	}
	else
	{
		time = guiInterface_GetCumulativeHour(uints);
		hour = time.hour;
	}

	itoa(hour,(char*)hourBuffer,10);

	EDIT_SetText(s_editUnitCumualtiveHour, (char*)&hourBuffer[0]);

	return;
}

/* end of files */
