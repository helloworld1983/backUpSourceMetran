/********************************************************************/
/*                                                                  */
/* File Name    : CreateContactDialog.c                          	*/
/*                                                                  */
/* General       : Create contact dialog			 		        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file       	*/
/*  #0001	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit : EDIT_SetFont()->guiTaskEDIT_SetFont(bug 1997)			*/
/********************************************************************/

#include <EEPROM.h>
#include "CreateContactDialog.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "StringTable.h"
#include "EquipmentSettingScreen.h"
#include "string.h"
#include "setting.h"
#include "board.h"
#include "GuiTask.h"
#include "Font.h"
#include "MainScreen.h"
#include "TextLibrary.h"
#include "GuiInterface.h"

//Define item id
enum
{
	eCloseDialogCreateContactId,
	eClearDialogCreateContactId,
	eBSDialogCreateContactId,
	eDecisionCreateContactId
};

WM_HWIN g_createContactDialog;

static EDIT_Handle s_editContact;
static BUTTON_Handle s_zeroContactBtn;
static BUTTON_Handle s_oneContactBtn;
static BUTTON_Handle s_twoContactBtn;
static BUTTON_Handle s_threeContactBtn;
static BUTTON_Handle s_fourContactBtn;
static BUTTON_Handle s_fineContactBtn;
static BUTTON_Handle s_sixContactBtn;
static BUTTON_Handle s_sevenContactBtn;
static BUTTON_Handle s_eightContactBtn;
static BUTTON_Handle s_nineContactBtn;
static BUTTON_Handle s_otherBtn;
static BUTTON_Handle s_clearBtn;
static BUTTON_Handle s_BSBtn;
static BUTTON_Handle s_decisionBtn;
static BUTTON_Handle s_closeContactBtn;
static int8_t s_contactTmpBuffer[CONTRACT_BUFFER_SIZE];

/****************************************************************************************/
/*                                                                                      */
/* General 	-createContactDialog_Callback 								       			*/
/*                                                                                      */
/* Details  -Call back function for create contact dialog, handle event from the window	*/
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
void createContactDialog_Callback(WM_MESSAGE *pMsg)
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

		GUI_SetColor(BORDER_CONTACT_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,50);

		GUI_SetBkColor(BUTTON_DIALOG_BACKGROUND_COLOR);
		GUI_SetColor(GUI_BLACK);

		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L16B2PP);
			GUI_DispStringHCenterAt("Please enter the emergency phone number", 400,5);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L18B2PP);
			GUI_DispStringHCenterAt("緊急連絡先の電話番号を入力してください", 400,5);
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
/* General 	-createContactDialog_CallbackNumberBtn 								       	*/
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
/*                     (O) s_editContact : display contact number                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_CallbackNumberBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	int8_t numberBuffer[SIZE_BUFFER_NUMBER_CONTACT] = {'\0'};

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
			if(strlen((char*)s_contactTmpBuffer) < MAX_LENGTH_CONTACT)
			{
				BUTTON_GetText(pMsg->hWin,(char*)&numberBuffer[0],10);
				strncat((char*)s_contactTmpBuffer,(char*)numberBuffer,SIZE_BUFFER_NUMBER_CONTACT);
			}

			EDIT_SetText(s_editContact,(char*)s_contactTmpBuffer);
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
/* General 	-createContactDialog_CallbackCloseBtn 								        */
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
void createContactDialog_CallbackCloseBtn(WM_MESSAGE *pMsg)
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
			createContactDialog_Hide();
			equipmentSettingScreen_ShowAll();
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
/* General 	-createContactDialog_callBackClearBtn 								        */
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
/*                     (O) s_editContact : display contact number                       */
/*                     (O) s_contactTmpBuffer : temp buffer of contact number			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_callBackClearBtn(WM_MESSAGE *pMsg)
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
			EDIT_SetText(s_editContact,"");
			memset(s_contactTmpBuffer,0,CONTRACT_BUFFER_SIZE);
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
/* General 	-createContactDialog_callBackBSBtn 								        	*/
/*                                                                                      */
/* Details  -call back fuction for BS button, handle event from the window 				*/
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
/*                     (O) s_contactTmpBuffer : temp buffer of contact number           */
/*                     (O) s_editContact : display contact number						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_callBackBSBtn(WM_MESSAGE *pMsg)
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
		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("BS", Rect.x1/2, 22);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 1)
		{
			s_contactTmpBuffer[strlen((char*)s_contactTmpBuffer) - 1] = '\0';
			EDIT_SetText(s_editContact, (char*)s_contactTmpBuffer);
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
/* General 	-createContactDialog_CallbackDecisionBtn								    */
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
/*                     (O) s_contactTmpBuffer : temp buffer of contact number           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_CallbackDecisionBtn(WM_MESSAGE *pMsg)
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
			for(int i = MAX_LENGTH_CONTACT; i <CONTRACT_BUFFER_SIZE; i++)
			{
				s_contactTmpBuffer[i] = '\0';
			}

			setting_SetContact(s_contactTmpBuffer);
			createContactDialog_Hide();
			equipmentSettingScreen_ShowAll();
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
/* General 	-createContactDialog_Init 								       				*/
/*                                                                                      */
/* Details  -Initialize create contact dialog with virtual keyboard interface			*/
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
/*                     (O) g_createContactDialog : create contact dialog                */
/*                     (O) s_editContact : display contact number						*/
/*                     (O) s_zeroContactBtn : zero number button						*/
/*                     (O) s_oneContactBtn : one number button							*/
/*                     (O) s_twoContactBtn : two number button							*/
/*                     (O) s_threeContactBtn : three number button						*/
/*                     (O) s_fourContactBtn : four number button						*/
/*                     (O) s_fineContactBtn : fine number button						*/
/*                     (O) s_sixContactBtn : six number button							*/
/*                     (O) s_sevenContactBtn : seven number button						*/
/*                     (O) s_eightContactBtn : eight number button						*/
/*                     (O) s_nineContactBtn : nine number button						*/
/*                     (O) s_otherBtn : other button									*/
/*                     (O) s_clearBtn : clear button									*/
/*                     (O) s_BSBtn : backspace button									*/
/*                     (O) s_decisionBtn : decision button								*/
/*                     (O) s_closeContactBtn : close button								*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_Init(void)
{
	g_createContactDialog = WM_CreateWindowAsChild(10, 10, 780, 460,g_equipmentSettingScreen,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,createContactDialog_Callback,0);

	s_editContact = EDIT_CreateEx(220,40,340,50,g_createContactDialog,WM_CF_SHOW,0,GUI_ID_EDIT5,13);
	guiTaskEDIT_SetFont (s_editContact, &GUI_FontEng3L16B2PP);
	EDIT_SetTextColor(s_editContact,EDIT_CI_ENABLED ,GUI_BLACK);
	EDIT_SetTextAlign(s_editContact,GUI_TA_VCENTER | GUI_TA_HCENTER);

	s_zeroContactBtn = BUTTON_CreateEx(345,320,90,70,g_createContactDialog,WM_CF_SHOW,0,eZeroContactBtnStringId);
	BUTTON_SetText(s_zeroContactBtn,"0");
	WM_SetCallback(s_zeroContactBtn,createContactDialog_CallbackNumberBtn);

	s_oneContactBtn = BUTTON_CreateEx(245,245,90,70,g_createContactDialog,WM_CF_SHOW,0,eOneContactBtnStringId);
	BUTTON_SetText(s_oneContactBtn,"1");
	WM_SetCallback(s_oneContactBtn,createContactDialog_CallbackNumberBtn);

	s_twoContactBtn = BUTTON_CreateEx(345,245,90,70,g_createContactDialog,WM_CF_SHOW,0,eTwoContactBtnStringId);
	BUTTON_SetText(s_twoContactBtn,"2");
	WM_SetCallback(s_twoContactBtn,createContactDialog_CallbackNumberBtn);

	s_threeContactBtn = BUTTON_CreateEx(445,245,90,70,g_createContactDialog,WM_CF_SHOW,0,eThreeContactBtnStringId);
	BUTTON_SetText(s_threeContactBtn,"3");
	WM_SetCallback(s_threeContactBtn,createContactDialog_CallbackNumberBtn);

	s_fourContactBtn = BUTTON_CreateEx(245,170,90,70,g_createContactDialog,WM_CF_SHOW,0,eFourContactBtnStringId);
	BUTTON_SetText(s_fourContactBtn,"4");
	WM_SetCallback(s_fourContactBtn,createContactDialog_CallbackNumberBtn);

	s_fineContactBtn = BUTTON_CreateEx(345,170,90,70,g_createContactDialog,WM_CF_SHOW,0,eFineContactBtnStringId);
	BUTTON_SetText(s_fineContactBtn,"5");
	WM_SetCallback(s_fineContactBtn,createContactDialog_CallbackNumberBtn);

	s_sixContactBtn = BUTTON_CreateEx(445,170,90,70,g_createContactDialog,WM_CF_SHOW,0,eSixContactBtnStringId);
	BUTTON_SetText(s_sixContactBtn,"6");
	WM_SetCallback(s_sixContactBtn,createContactDialog_CallbackNumberBtn);

	s_sevenContactBtn = BUTTON_CreateEx(245,95,90,70,g_createContactDialog,WM_CF_SHOW,0,eSevenContactBtnStringId);
	BUTTON_SetText(s_sevenContactBtn,"7");
	WM_SetCallback(s_sevenContactBtn,createContactDialog_CallbackNumberBtn);

	s_eightContactBtn = BUTTON_CreateEx(345,95,90,70,g_createContactDialog,WM_CF_SHOW,0,eEightContactBtnStringId);
	BUTTON_SetText(s_eightContactBtn,"8");
	WM_SetCallback(s_eightContactBtn,createContactDialog_CallbackNumberBtn);

	s_nineContactBtn = BUTTON_CreateEx(445,95,90,70,g_createContactDialog,WM_CF_SHOW,0,eNineContactBtnStringId);
	BUTTON_SetText(s_nineContactBtn,"9");
	WM_SetCallback(s_nineContactBtn,createContactDialog_CallbackNumberBtn);

	s_otherBtn =  BUTTON_CreateEx(445,320,90,70,g_createContactDialog,WM_CF_SHOW,0,eOtherContactBtnStringId);
	BUTTON_SetText(s_otherBtn,"-");
	WM_SetCallback(s_otherBtn,createContactDialog_CallbackNumberBtn);

	s_clearBtn = BUTTON_CreateEx(585,220,120,70,g_createContactDialog,WM_CF_SHOW,0,eClearDialogCreateContactId);
	WM_SetCallback(s_clearBtn,createContactDialog_callBackClearBtn);

	s_BSBtn = BUTTON_CreateEx(585,300,120,70,g_createContactDialog,WM_CF_SHOW,0,eBSDialogCreateContactId);
	WM_SetCallback(s_BSBtn,createContactDialog_callBackBSBtn);

	s_decisionBtn = BUTTON_CreateEx(585,380,120,70,g_createContactDialog,WM_CF_SHOW,0,eDecisionCreateContactId);
	WM_SetCallback(s_decisionBtn,createContactDialog_CallbackDecisionBtn);

	s_closeContactBtn = BUTTON_CreateEx(75,380,120,70,g_createContactDialog,WM_CF_SHOW,0,eCloseDialogCreateContactId);
	WM_SetCallback(s_closeContactBtn,createContactDialog_CallbackCloseBtn);

	WM_HideWindow(g_createContactDialog);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-createContactDialog_Show 								       				*/
/*                                                                                      */
/* Details  -Show create contact dialog with virtual keyboard interface					*/
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
/*                     (O) s_editContact : display contact number                       */
/*                     (O) s_contactTmpBuffer : temp buffer of contact number			*/
/*                     (O) g_createContactDialog : create contact dialog				*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_Show(void)
{
	EDIT_SetText(s_editContact, (char*)setting_GetContact());
	strncpy((char*)s_contactTmpBuffer, (char*)setting_GetContact(),CONTRACT_BUFFER_SIZE - 1);

	WM_ShowWindow(g_createContactDialog);
	WM_BringToTop(g_createContactDialog);
	WM_SetFocus(g_createContactDialog);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-createContactDialog_Hide								       				*/
/*                                                                                      */
/* Details  -Hide create contact dialog with virtual keyboard interface					*/
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
/*                     (O) g_createContactDialog : create contact dialog                */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void createContactDialog_Hide(void)
{
	WM_HideWindow(g_createContactDialog);

	return;
}

/* end of files */
