/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageTwo.c                 */
/*                                                                  */
/* General       : The user is able to test watch dog, flow rate,RTC*/
/*  																*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file        */
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : TEXT_SetFont()->guiTaskTEXT_SetFont() (bug 1997)		*/
/*                                                                  */
/********************************************************************/
#include "MainScreen.h"
#include "DevelopmentSettingScreenPageTwo.h"
#include "setting.h"
#include "GuiTask.h"
#include "BUTTON.h"
#include "SettingOnOffButton.h"
#include "StringTable.h"
#include "SettingButton.h"
#include "ModeMgr.h"
#include "TEXT.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "DevelopmentSettingScreen.h"
#include "OptionScreen.h"
#include "DevelopmentSettingScreenPageThree.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnDevelopmentPageTwoId,
	eHomeBtnDevelopmentPageTwoId,
	eNextBtnDevelopmentPageTwoId,
	ePrevBtnDevelopmentPageTwoId,
	eWDTM4BtnDevelopmentPageTwoId,
	eWDTM0BtnDevelopmentPageTwoId,
	eO2ConcentrationTestSendCommandBtn,
	eFlowRateSendCommandBtn,
	eDeleteTAGBtn,
	eDeleteChecksumbtn,
	eOkBtnPopUpStringId,
	eCancelBtnPopUpStringId,
	eTitlePopUpDevelopmentPageTwoStringID,
	ePageTextDevelopmentPagetwoId
};

WM_HWIN g_developmentSettingScreenPageTwo;

static WM_HWIN s_popUp;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_prevBtn;
static EDIT_Handle s_RTConOffBtn;
static EDIT_Handle s_O2ConcentrationtestBtn;
static BUTTON_Handle s_O2ConcentrationSendCommandBtn;
static EDIT_Handle s_FlowrateTestBtn;
static BUTTON_Handle s_FlowRateSendCommandBtn;
static BUTTON_Handle s_deleteTagBtn;
static BUTTON_Handle s_deleteChecksumBtn;
static TEXT_Handle s_titlePopUp;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_cancelBtn;
static TEXT_Handle s_pageText;
static bool s_isDeleteTag = false;

/*******************************************************************
 * *
The points of the arrow
 */
static const GUI_POINT s_aPointNextArrow[] = {
		{0,0+10},
		{99-40,0+10},
		{99-40,0},
		{99,59/2},
		{99-40,59},
		{99-40,59-10},
		{0,59-10}
};

static const GUI_POINT s_aPointPrevArrow[] = {
		{99,10},
		{40,10},
		{40,0},
		{0,59/2},
		{40,59},
		{40,59-10},
		{99,59-10}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_HideAllItem	                            */
/*                                                                                      */
/* Details  -Hide all item when pop up show										        */
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										 					  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_pageText : page number						                */
/*                     (O) s_RTConOffBtn : ON/OFF RTC test button						*/
/*                     (O) s_O2ConcentrationtestBtn : O2 test button					*/
/*                     (O) s_O2ConcentrationSendCommandBtn : send command O2 button		*/
/*                     (O) s_FlowrateTestBtn : flow rate test button					*/
/*                     (O) s_FlowRateSendCommandBtn : send command flow rate button		*/
/*                     (O) s_deleteTagBtn : delete tag button						    */
/*                     (O) s_deleteChecksumBtn : delete checksum button					*/
/*                     (O) g_developmentSettingScreenPageTwo : development page two		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_HideAllItem(void)
{
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_nextBtn);
	WM_HideWindow(s_prevBtn);
	WM_HideWindow(s_pageText);

	WM_HideWindow(s_RTConOffBtn);
	WM_HideWindow(s_O2ConcentrationtestBtn);
	WM_HideWindow(s_O2ConcentrationSendCommandBtn);
	WM_HideWindow(s_FlowrateTestBtn);
	WM_HideWindow(s_FlowRateSendCommandBtn);
	WM_HideWindow(s_deleteTagBtn);
	WM_HideWindow(s_deleteChecksumBtn);

	WM_Paint(g_developmentSettingScreenPageTwo);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_ShowAllItem	                            */
/*                                                                                      */
/* Details  -Show all item when pop up hide										        */
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										 					  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_homeBtn : home button                                      */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_pageText : page number						                */
/*                     (O) s_RTConOffBtn : ON/OFF RTC test button						*/
/*                     (O) s_O2ConcentrationtestBtn : O2 test button					*/
/*                     (O) s_O2ConcentrationSendCommandBtn : send command O2 button		*/
/*                     (O) s_FlowrateTestBtn : flow rate test button					*/
/*                     (O) s_FlowRateSendCommandBtn : send command flow rate button		*/
/*                     (O) s_deleteTagBtn : delete tag button						    */
/*                     (O) s_deleteChecksumBtn : delete checksum button					*/
/*                     (O) g_developmentSettingScreenPageTwo : development page two		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_ShowAllItem(void)
{
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_nextBtn);
	WM_ShowWindow(s_prevBtn);
	WM_ShowWindow(s_pageText);

	WM_ShowWindow(s_RTConOffBtn);
	WM_ShowWindow(s_O2ConcentrationtestBtn);
	WM_ShowWindow(s_O2ConcentrationSendCommandBtn);
	WM_ShowWindow(s_FlowrateTestBtn);
	WM_ShowWindow(s_FlowRateSendCommandBtn);
	WM_ShowWindow(s_deleteTagBtn);
	WM_ShowWindow(s_deleteChecksumBtn);

	WM_Paint(g_developmentSettingScreenPageTwo);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_CallbackNextBtn	                        */
/*                                                                                      */
/* Details  -call back function for next button, handle event from window		        */
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
void developmentSettingScreenPageTwo_CallbackNextBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointNextArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);//Border
		GUI_DrawPolygon(&s_aPointNextArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Next", 20,20 + 2);//Next
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringAt("次", 20,18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageTwo_Hide();
			developmentSettingScreenPageThree_Show();
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
/* General 	-developmentSettingScreenPageTwo_Callback	                                */
/*                                                                                      */
/* Details  -call back function for PSA parameter page two, handle event from window	*/
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
void developmentSettingScreenPageTwo_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		if(!WM_IsVisible(s_popUp))
		{
			GUI_SetColor(GUI_BLACK);
			GUI_AA_DrawRoundedRect(10,60,380,225,10);
			GUI_AA_DrawRoundedRect(400,60,790,225,10);
			GUI_AA_DrawRoundedRect(10,240,380,400,10);
			GUI_AA_DrawRoundedRect(400,240,790,400,10);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Development setting", 400,5);

				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_DispStringAt("O2 Concentration test",25,50);
				GUI_DispStringAt("Nonvolatile memory test",415,50);
				GUI_DispStringAt("Flow rate test",25,230);
				GUI_DispStringAt("RTC test",415,230);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("開発用設定", 400,5);

				guiTask_SetFont(&GUI_FontJapan3L12B2PP);
				GUI_DispStringAt("酸素濃度テスト",25,50);
				GUI_DispStringAt("不揮発性メモリ",415,50);
				GUI_DispStringAt("流量テスト",25,230);
				GUI_DispStringAt("RTCテスト",415,230);
			}
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
/* General 	-developmentSettingScreenPageTwo_CallbackBackBtn 						    */
/*                                                                                      */
/* Details  -Call back function for back button, handle event from 			            */
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
void developmentSettingScreenPageTwo_CallbackBackBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Back", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("戻る", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageTwo_Hide();
			maintenanceScreen_Hide();
			optionScreen_SetTab(eMaintenanceTabId);
			optionScreen_Show();
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
/* General 	-developmentSettingScreenPageTwo_CallbackHomeBtn 							*/
/*                                                                                      */
/* Details  -Call back function for home button, handle event from 			            */
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
/* Using Global Data : (I) g_mainWindow : main window							        */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void developmentSettingScreenPageTwo_CallbackHomeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Home", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("ホーム", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageTwo_Hide();
			WM_SendMessageNoPara(g_mainWindow,WM_SHOW_HOMESCREEN);
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
/* General 	-developmentSettingScreenPageTwo_CallbackPrevBtn 							*/
/*                                                                                      */
/* Details  -Call back function for previous button, handle event from 					*/
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
static void developmentSettingScreenPageTwo_CallbackPrevBtn(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_FillPolygon(&s_aPointPrevArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetPenSize(2);
		GUI_SetColor(BORDER_COLOR);//Border
		GUI_DrawPolygon(&s_aPointPrevArrow[0],7,Rect.x0,Rect.y0);

		GUI_SetColor(GUI_BLACK);
		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("Prev", 40,20 + 2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringAt("前", 50,18);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageTwo_Hide();
			developmentSettingScreen_Show();
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
/* General 	-developmentSettingScreenPageTwo_CallbackO2SendCommandBtn 					*/
/*                                                                                      */
/* Details  -Call back function for O2 send command button, handle event from 		    */
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
void developmentSettingScreenPageTwo_CallbackO2SendCommandBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Send command", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("コマンド発行", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			guiTask_SendPendingEvent(eO2ConcentrationTestChangeId,settingButton_GetValue(eValueO2ConcentrationTestId));
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
/* General 	-developmentSettingScreenPageTwo_CallbackFlowRateSendCommand 				*/
/*                                                                                      */
/* Details  -Call back function for Flow rate send command button, handle event from    */
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
void developmentSettingScreenPageTwo_CallbackFlowRateSendCommand(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Send command", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("コマンド発行", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			guiTask_SendPendingEvent(eFlowRateTestChangeId, settingButton_GetValue(eValueFlowRateTestTestId));
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
/* General 	-developmentSettingScreenPageTwo_CallbackDeleteTag 							*/
/*                                                                                      */
/* Details  -Call back function for delete tag button, handle event from 				*/
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
/*                     (O) s_isDeleteTag : flag delete tag                              */
/*                     (O) s_titlePopUp : title pop up						            */
/*                     (O) s_popUp : pop up						            			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_CallbackDeleteTag(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Delete TAG", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("タグ削除", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isDeleteTag = true;
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Delete tag of nonvolatile memory\nIs it OK?");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"不揮発性メモリのタグを削除します\nよろしいですか？");
			}

			WM_ShowWindow(s_popUp);
			WM_BringToTop(s_popUp);

			developmentSettingScreenPageTwo_HideAllItem();
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
/* General 	-developmentSettingScreenPageTwo_CallbackDeleteChecksum 					*/
/*                                                                                      */
/* Details  -Call back function for delete check sum button, handle event from 			*/
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
/*                     (O) s_isDeleteTag : flag delete tag                              */
/*                     (O) s_titlePopUp : title pop up						            */
/*                     (O) s_popUp : pop up						            			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_CallbackDeleteChecksum(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Delete Check sum", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("チェックサム削除", Rect.x1/2, 19);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isDeleteTag = false;

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Delete nonvolatile memory checksum\nIs it OK?");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"不揮発性メモリのチェックサムを削除します\nよろしいですか？");
			}

			WM_ShowWindow(s_popUp);
			WM_BringToTop(s_popUp);

			developmentSettingScreenPageTwo_HideAllItem();
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
/* General 	-developmentSettingScreenPageTwo_CallbackPopUp								*/
/*                                                                                      */
/* Details  -Call back function for pop up, handle event from 							*/
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
void developmentSettingScreenPageTwo_CallbackPopUp(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(DIALOG_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);

		GUI_SetColor(BORDER_DIALOG_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,25);

		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_CallbackOKBtn 								*/
/*                                                                                      */
/* Details  -Call back function for OK button, handle event from 						*/
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
/*                     (O) s_isDeleteTag : flag delete tag                              */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_CallbackOKBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(BUTTON_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10);

		GUI_SetColor(BORDER_COLOR);
		GUI_DrawRoundedFrame(Rect.x0,Rect.y0,Rect.x1,Rect.y1,10,2);

		GUI_SetBkColor(BUTTON_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);
		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("OK", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(s_isDeleteTag == true)
			{
				setting_DeleteTag();
				DEBUGOUT("\n setting_SetFlagDeleteTag");
			}
			else
			{
				setting_DeleteChecksum();
				DEBUGOUT("\n setting_SetFlagDeleteChecksum");
			}

			s_isDeleteTag = false;
			WM_HideWindow(s_popUp);

			developmentSettingScreenPageTwo_ShowAllItem();
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
/* General 	-developmentSettingScreenPageTwo_CallbackCancelBtn 							*/
/*                                                                                      */
/* Details  -Call back function for Cancel button, handle event from 					*/
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
/*                     (O) s_isDeleteTag : flag delete tag                              */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_CallbackCancelBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(DIALOG_BACKGROUND_COLOR);
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
			GUI_DispStringHCenterAt("Cancel", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("キャンセル", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_isDeleteTag = false;

			WM_HideWindow(s_popUp);
			developmentSettingScreenPageTwo_ShowAllItem();
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
/* General 	-developmentSettingScreenPageTwo_Init										*/
/*                                                                                      */
/* Details  -Initialize development setting screen page two		            		    */
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
/*                     (O) g_developmentSettingScreenPageTwo : development page two     */
/*                     (O) s_O2ConcentrationtestBtn : O2 test button					*/
/*                     (O) s_O2ConcentrationSendCommandBtn : send command O2 button		*/
/*                     (O) s_FlowrateTestBtn : flow rate test button					*/
/*                     (O) s_FlowRateSendCommandBtn : send command flow rate button		*/
/*                     (O) s_deleteTagBtn : delete tag button					        */
/*                     (O) s_deleteChecksumBtn : delete checksum button	             	*/
/*                     (O) s_RTConOffBtn : ON/OFF RTC test button					  	*/
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_pageText : page number						                */
/*                     (O) s_popUp : pop up						                        */
/*                     (O) s_titlePopUp : title pop up						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn : cancel button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_Init(void)
{
	GUI_RECT Rect;

	g_developmentSettingScreenPageTwo = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageTwo_Callback,0);

	s_O2ConcentrationtestBtn = EDIT_CreateEx(40,70,180+35+10,70,g_developmentSettingScreenPageTwo,WM_CF_SHOW,0,eO2ConcentrationTestStringID,50);
	WM_GetClientRectEx(s_O2ConcentrationtestBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_O2ConcentrationtestBtn,eUpBtnO2ConcentrationTestStringId,eDownBtnO2ConcentrationTestStringId,eValueO2ConcentrationTestId,eValueOfO2ConcentrationTestStringId,Rect);
	settingButton_SetValue(eValueO2ConcentrationTestId,DEFAULT_O2_TEST_STR);
	WM_SetCallback(s_O2ConcentrationtestBtn,settingButton_Callback);

	s_O2ConcentrationSendCommandBtn = BUTTON_CreateEx(40, 150, 240, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eO2ConcentrationTestSendCommandBtn);
	WM_SetCallback(s_O2ConcentrationSendCommandBtn,developmentSettingScreenPageTwo_CallbackO2SendCommandBtn);

	s_FlowrateTestBtn = EDIT_CreateEx(40,250,180+50+10,70,g_developmentSettingScreenPageTwo,WM_CF_SHOW,0,eFlowRateTestStringID,50);
	WM_GetClientRectEx(s_FlowrateTestBtn,&Rect);
	settingButton_InitNoSaveLargeBtn(s_FlowrateTestBtn,eUpBtnFlowRateTestStringId,eDownBtnFlowRateTestStringId,eValueFlowRateTestTestId,eValueOfFlowRateTestStringId,Rect);
	settingButton_SetValue(eValueFlowRateTestTestId,DEFAULT_FLOWRATE_TEST_STR);
	WM_SetCallback(s_FlowrateTestBtn,settingButton_Callback);

	s_FlowRateSendCommandBtn = BUTTON_CreateEx(40, 330, 240, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eO2ConcentrationTestSendCommandBtn);
	WM_SetCallback(s_FlowRateSendCommandBtn,developmentSettingScreenPageTwo_CallbackFlowRateSendCommand);

	s_deleteTagBtn = BUTTON_CreateEx(430,80,240,60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eDeleteTAGBtn);
	WM_SetCallback(s_deleteTagBtn,developmentSettingScreenPageTwo_CallbackDeleteTag);

	s_deleteChecksumBtn = BUTTON_CreateEx(430, 150, 240, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eDeleteChecksumbtn);
	WM_SetCallback(s_deleteChecksumBtn,developmentSettingScreenPageTwo_CallbackDeleteChecksum);

	s_RTConOffBtn = EDIT_CreateEx(430,250,240,70,g_developmentSettingScreenPageTwo,WM_CF_SHOW,0,eTurnRTCStringID,50);
	WM_GetClientRectEx(s_RTConOffBtn,&Rect);
	settingOnOffLargeButton_Init(s_RTConOffBtn,eOnBtnRTCStringId,eOffBtnRTCStringId,eValueRTCId,eValueOfRTCStringId,Rect);
	WM_SetCallback(s_RTConOffBtn,settingOnOffButton_Callback);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eBackBtnDevelopmentPageTwoId);
	WM_SetCallback(s_backBtn,developmentSettingScreenPageTwo_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eHomeBtnDevelopmentPageTwoId);
	WM_SetCallback(s_homeBtn,developmentSettingScreenPageTwo_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, eNextBtnDevelopmentPageTwoId);
	WM_SetCallback(s_nextBtn,developmentSettingScreenPageTwo_CallbackNextBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_developmentSettingScreenPageTwo, WM_CF_SHOW, 0, ePrevBtnDevelopmentPageTwoId);
	WM_SetCallback(s_prevBtn,developmentSettingScreenPageTwo_CallbackPrevBtn);

	s_pageText = TEXT_CreateEx(380,440, 50, 25,g_developmentSettingScreenPageTwo,WM_CF_SHOW,TEXT_CF_LEFT,ePageTextDevelopmentPagetwoId,"2/5");
	guiTaskTEXT_SetFont(s_pageText, &GUI_FontEng3L18B2PP);

	s_popUp = WM_CreateWindowAsChild(50, 115, 700, 225,g_developmentSettingScreenPageTwo,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageTwo_CallbackPopUp,0);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 30, 700, 50,s_popUp,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpDevelopmentPageTwoStringID,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eOkBtnPopUpStringId);
	WM_SetCallback(s_OKBtn,developmentSettingScreenPageTwo_CallbackOKBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eCancelBtnPopUpStringId);
	WM_SetCallback(s_cancelBtn,developmentSettingScreenPageTwo_CallbackCancelBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_Show										*/
/*                                                                                      */
/* Details  -Show development setting screen page two		            		        */
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
/*                     (O) s_popUp : pop up                                           	*/
/*                     (O) g_developmentSettingScreenPageTwo : development page two		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageTwo_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_HideWindow(s_popUp);
	developmentSettingScreenPageTwo_ShowAllItem();
	WM_ShowWindow(g_developmentSettingScreenPageTwo);
	WM_BringToTop(g_developmentSettingScreenPageTwo);
	WM_SetFocus(g_developmentSettingScreenPageTwo);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageTwo_Hide										*/
/*                                                                                      */
/* Details  -Hide development setting screen page two		            		        */
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
/*                     (O) g_developmentSettingScreenPageTwo : development page two     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/

void developmentSettingScreenPageTwo_Hide(void)
{
	WM_HideWindow(g_developmentSettingScreenPageTwo);

	return;
}

/* end of files */

