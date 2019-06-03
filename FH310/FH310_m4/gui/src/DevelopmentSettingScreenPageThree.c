/********************************************************************/
/*                                                                  */
/* File Name    : DevelopmentSettingScreenPageThree.c               */
/*                                                                  */
/* General       : The user is able to dump file into USB memory	*/
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
/* #0001       2017/09/01      Viet Le				modify			*/
/*     Edit debugLog_ReadNumberLog()-> debugLog_ReadCtrlParameters()*/
/*     (developmentSettingScreenPageThree_Init() function)			*/
/* #0002       2017/09/05      Viet Le				modify			*/
/*     remove #include "assert.h"									*/
/* #0003	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Edit :TEXT_SetFont()->guiTaskTEXT_SetFont() (bug 1997)		*/
/* #0004	   2017/10/27      Viet Le 	    	        modify	    */
/*	  Add return; in void function(coding rules)					*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0005 Add the dump SPI flash memory functionality (bug #1992)	*/
/********************************************************************/

#include "DevelopmentSettingScreenPageThree.h"
#include "MainScreen.h"
#include "BUTTON.h"
#include "setting.h"
#include "GuiTask.h"
#include "TEXT.h"
#include "UsbMgr.h"
#include "Font.h"
#include "MaintenanceScreen.h"
#include "DevelopmentSettingScreenPageTwo.h"
#include "DevelopmentSettingScreenPageFour.h"
#include "OptionScreen.h"
#include "AlarmInfor.h"
#include "TextLibrary.h"
#include "DebugLog.h"
#include "UsbInterface.h"
#include "GuiInterface.h"

//Define item id
enum{
	eBackBtnInDevelopmentPageThreeId,
	eHomeBtnInDevelopmentPageThreeId,
	eNextBtnInDevelopmentPageThreeId,
	ePrevBtnInDevelopmentPageThreeId,
	eEEPROMBtnInDevelopmentPageThreeId,
	eCBXBtnInDevelopmentPageThreeId,
	eAnyPalBtnInDevelopmentPageThreeId,
	eSpo2PulseBtnInDevelopmentPageThreeId,
	eSRAMBtnInDevelopmentPageThreeId,
	eSDRAMBtnInDevelopmentPageThreeId,
	eSPIFlashBtnInDevelopmentPageThreeId,
	eFlashABtnInDevelopmentPageThreeId,
	eFlashBBtnInDevelopmentPageThreeId,
	ePageTextInDevelopmentPageThreeId,
	eTitlePopUpInDevelopmentPageThreeStringID,
	eOkBtnPopUpInDevelopmentPageThreeStringId,
	eCancelBtnPopUpInDevelopmentPageThreeStringId,
	eClosetnPopUpInDevelopmentPageThreeStringId,
	eDebugLogInDevelopmentPageThreeStringId,
	eClearDebugLogInDevelopmentPageThreeStringId,
	eClearBtnPopUpInDevelopmentPageThreeStringId
};

//Define dump id
typedef enum{
	eNotDump,
	eDumpEEPROM,
	eDumpCBX,
	eDumpAnypal,
	eDumpPulseO2,
	eDumpSRAM,
	eDumpSDRAM,
	eDumpSPIFlash,
	eDumpFlashA,
	eDumpFlashB,
	eDebugLog
}E_Dump_File;

WM_HWIN g_developmentSettingScreenPageThree;

static WM_HWIN s_popUp;
static BUTTON_Handle s_backBtn;
static BUTTON_Handle s_homeBtn;
static BUTTON_Handle s_nextBtn;
static BUTTON_Handle s_prevBtn;
static BUTTON_Handle s_EEPROMBtn;
static BUTTON_Handle s_CBXBtn;
static BUTTON_Handle s_AnypalBtn;
static BUTTON_Handle s_Spo2PulseBtn;
static BUTTON_Handle s_SRAMBtn;
static BUTTON_Handle s_SDRAMBtn;
static BUTTON_Handle s_SPIFlashBtn;
static BUTTON_Handle s_FlashABtn;
static BUTTON_Handle s_FlashBBtn;
static TEXT_Handle s_titlePopUp;
static BUTTON_Handle s_OKBtn;
static BUTTON_Handle s_clearBtn;
static BUTTON_Handle s_cancelBtn;
static BUTTON_Handle s_closeBtn;
static BUTTON_Handle s_debugLogBtn;
static BUTTON_Handle s_clearDebugLogBtn;
static TEXT_Handle s_pageText;
static E_Dump_File s_dumpFile = eNotDump;

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
/* General 	-developmentSettingScreenPageThree_HideAllItem	                            */
/*                                                                                      */
/* Details  -Hide all item when show pop up												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						                */
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_SRAMBtn : SRAM button						                */
/*                     (O) s_SDRAMBtn : SDRAM button						            */
/*                     (O) s_SPIFlashBtn : SPIFLash button						        */
/*                     (O) s_FlashABtn : flash A button						            */
/*                     (O) s_FlashBBtn : flash B button						            */
/*                     (O) s_debugLogBtn : debug log button						        */
/*                     (O) s_clearDebugLogBtn : clear debug log button					*/
/*                     (O) s_pageText : page number						                */
/*                     (O) g_developmentSettingScreenPageThree : development page three	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_HideAllItem(void)
{
	WM_HideWindow(s_backBtn);
	WM_HideWindow(s_homeBtn);
	WM_HideWindow(s_nextBtn);
	WM_HideWindow(s_prevBtn);

	WM_HideWindow(s_EEPROMBtn);
	WM_HideWindow(s_CBXBtn);
	WM_HideWindow(s_AnypalBtn);
	WM_HideWindow(s_Spo2PulseBtn);
	WM_HideWindow(s_SRAMBtn);
	WM_HideWindow(s_SDRAMBtn);
	WM_HideWindow(s_SPIFlashBtn);
	WM_HideWindow(s_FlashABtn);
	WM_HideWindow(s_FlashBBtn);
	WM_HideWindow(s_debugLogBtn);
	WM_HideWindow(s_clearDebugLogBtn);
	WM_HideWindow(s_pageText);

	WM_Paint(g_developmentSettingScreenPageThree);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_ShowAllItem	                            */
/*                                                                                      */
/* Details  -Show all item when hide pop up												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_backBtn : back button                                      */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						                */
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_SRAMBtn : SRAM button						                */
/*                     (O) s_SDRAMBtn : SDRAM button						            */
/*                     (O) s_SPIFlashBtn : SPIFLash button						        */
/*                     (O) s_FlashABtn : flash A button						            */
/*                     (O) s_FlashBBtn : flash B button						            */
/*                     (O) s_debugLogBtn : debug log button						        */
/*                     (O) s_clearDebugLogBtn : clear debug log button					*/
/*                     (O) s_pageText : page number						                */
/*                     (O) g_developmentSettingScreenPageThree : development page three	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_ShowAllItem(void)
{
	WM_ShowWindow(s_backBtn);
	WM_ShowWindow(s_homeBtn);
	WM_ShowWindow(s_nextBtn);
	WM_ShowWindow(s_prevBtn);

	WM_ShowWindow(s_EEPROMBtn);
	WM_ShowWindow(s_CBXBtn);
	WM_ShowWindow(s_AnypalBtn);
	WM_ShowWindow(s_Spo2PulseBtn);
	WM_ShowWindow(s_SRAMBtn);
	WM_ShowWindow(s_SDRAMBtn);
	WM_ShowWindow(s_SPIFlashBtn);
	WM_ShowWindow(s_FlashABtn);
	WM_ShowWindow(s_FlashBBtn);
	WM_ShowWindow(s_debugLogBtn);
	WM_ShowWindow(s_clearDebugLogBtn);
	WM_ShowWindow(s_pageText);

	WM_Paint(g_developmentSettingScreenPageThree);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_ShowPopup	                            */
/*                                                                                      */
/* Details  -Show pop up when press button												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															  		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_titlePopUp : title pop up                                  */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_cancelBtn: cancel button						            */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_ShowPopup(void)
{
	WM_MoveChildTo(s_titlePopUp,0,30);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titlePopUp,"The log data will be saved\nAfter you connect the USB memory\nPress the Save button");
	}
	else
	{
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
		TEXT_SetText(s_titlePopUp,"ログデータを保存します\nUSBメモリーを接続してから\n保存ボタンを押してください");
	}

	WM_HideWindow(s_closeBtn);
	WM_HideWindow(s_clearBtn);

	WM_ShowWindow(s_OKBtn);
	WM_ShowWindow(s_cancelBtn);

	WM_ShowWindow(s_popUp);
	WM_BringToTop(s_popUp);

	developmentSettingScreenPageThree_HideAllItem();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_Callback                                 */
/*                                                                                      */
/* Details  -call back function for PSA parameter page three, handle event from window	*/
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
void developmentSettingScreenPageThree_Callback(WM_MESSAGE *pMsg)
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
			GUI_AA_DrawRoundedRect(10,60,780,353,10);
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTask_SetFont(&GUI_FontEng3L16B2PP);
				GUI_DispStringHCenterAt("Development setting", 400,5);

				guiTask_SetFont(&GUI_FontEng3L12B2PP);
				GUI_DispStringAt("Memory dump",25,50);
			}
			else
			{
				guiTask_SetFont(&GUI_FontJapan3L18B2PP);
				GUI_DispStringHCenterAt("開発用設定", 400,5);

				guiTask_SetFont(&GUI_FontJapan3L12B2PP);
				GUI_DispStringAt("メモリダンプ",25,50);
			}
		}
		break;
	case WM_TOUCH:
	case WM_TOUCH_CHILD:
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_CallbackBackBtn	                        */
/*                                                                                      */
/* Details  -call back function for back button											*/
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
void developmentSettingScreenPageThree_CallbackBackBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageThree_Hide();
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
/* General 	-developmentSettingScreenPageThree_CallbackHomeBtn 							*/
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
void developmentSettingScreenPageThree_CallbackHomeBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageThree_Hide();
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
/* General 	-developmentSettingScreenPageThree_CallbackNextBtn 							*/
/*                                                                                      */
/* Details  -Call back function for next button, handle event from 			            */
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
void developmentSettingScreenPageThree_CallbackNextBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageThree_Hide();
			developmentSettingScreenPageFour_Show();
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
/* General 	-developmentSettingScreenPageThree_CallbackPrevBtn 							*/
/*                                                                                      */
/* Details  -Call back function for prev button, handle event from 			            */
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
void developmentSettingScreenPageThree_CallbackPrevBtn(WM_MESSAGE *pMsg)
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
			developmentSettingScreenPageThree_Hide();
			developmentSettingScreenPageTwo_Show();
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
/* General 	-developmentSettingScreenPageThree_CallbackEEPROMBtn 						*/
/*                                                                                      */
/* Details  -Call back function for EEPROM button, handle event from 			        */
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackEEPROMBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("EEPROM", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpEEPROM;
			developmentSettingScreenPageThree_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_CallbackCBXBtn							*/
/*                                                                                      */
/* Details  -Call back function for CBX button, handle event from 			       		*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackCBXBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Usage log (CBX)", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("使用ログ(CBX)", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpCBX;
			developmentSettingScreenPageThree_ShowPopup();
		}
		break;
	case WM_TOUCH:
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_CallbackAnypalBtn 						*/
/*                                                                                      */
/* Details  -Call back function for Anypal button, handle event from 			        */
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackAnypalBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Usage log (Anypal)", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("使用ログ(Anypal)", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpAnypal;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackSpo2PulseBtn						*/
/*                                                                                      */
/* Details  -Call back function for Spo2 pulse button, handle event from 			    */
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
/*                     (O) s_dumpFile : current dump file                           	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackSpo2PulseBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Spo2/Pulse", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("血中酸素/脈拍", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpPulseO2;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackSRAMBtn 							*/
/*                                                                                      */
/* Details  -Call back function for SRAM button, handle event from 					    */
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackSRAMBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("SRAM", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpSRAM;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackSDRAMBtn 						*/
/*                                                                                      */
/* Details  -Call back function for SDRAM button, handle event from 				    */
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackSDRAMBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("SDRAM", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpSDRAM;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackSPIFlashBtn 						*/
/*                                                                                      */
/* Details  -Call back function for SPI FLASH button, handle event from 				*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackSPIFlashBtn(WM_MESSAGE *pMsg)
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

		guiTask_SetFont(&GUI_FontEng3L16B2PP);
		GUI_DispStringHCenterAt("SPI FLASH", Rect.x1/2, 17);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpSPIFlash;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackFlashABtn 						*/
/*                                                                                      */
/* Details  -Call back function for FLASH A button, handle event from 					*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackFlashABtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Internal FLASH A", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("内蔵 FLASH A", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpFlashA;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackFlashBBtn						*/
/*                                                                                      */
/* Details  -Call back function for FLASH B button, handle event from 					*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackFlashBBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Internal FLASH B", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("内蔵FLASH B", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDumpFlashB;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackDebugLogBtn						*/
/*                                                                                      */
/* Details  -Call back function for debug log button, handle event from 				*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackDebugLogBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Debug log", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("デバッグログ", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eDebugLog;
			developmentSettingScreenPageThree_ShowPopup();
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
/* General 	-developmentSettingScreenPageThree_CallbackClearDebugLogBtn					*/
/*                                                                                      */
/* Details  -Call back function for clear debug log button, handle event from 			*/
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
void developmentSettingScreenPageThree_CallbackClearDebugLogBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Debug log clear", Rect.x1/2, 15);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("デバッグログクリア", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			developmentSettingScreenPageThree_ClearPopUp();
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
/* General 	-developmentSettingScreenPageThree_CallbackPopUp							*/
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
void developmentSettingScreenPageThree_CallbackPopUp(WM_MESSAGE *pMsg)
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
/* General 	-developmentSettingScreenPageThree_CallbackOKBtn 							*/
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
/*                     (O) s_OKBtn : OK button                                          */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_titlePopUp : title pop up						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackOKBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Save", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("保存", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			WM_HideWindow(s_OKBtn);
			WM_HideWindow(s_cancelBtn);
			WM_HideWindow(s_closeBtn);

			WM_MoveChildTo(s_titlePopUp,0,85);
			TEXT_SetTextAlign(s_titlePopUp, GUI_TA_VCENTER | GUI_TA_HCENTER);

			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
				TEXT_SetText(s_titlePopUp,"Saving log data");
			}
			else
			{
				guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
				TEXT_SetText(s_titlePopUp,"ログデータを保存しています");
			}

			GUI_Delay(GUI_DELAY_DEVELOP_PAGE_THREE);
			developmentSettingScreenPageThree_SendToUSB();
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
/* General 	-developmentSettingScreenPageThree_CallbackClearBtn 						*/
/*                                                                                      */
/* Details  -Call back function for clear button, handle event from 					*/
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
/*                     (O) s_clearBtn : clear button                                    */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackClearBtn(WM_MESSAGE *pMsg)
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
			DEBUGOUT("\n eClearDebugLog");
			debugLog_ClearLog();

			WM_HideWindow(s_clearBtn);
			WM_HideWindow(s_popUp);

			developmentSettingScreenPageThree_ShowAllItem();
		}
		break;
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_CallbackCancelBtn 						*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_popUp : pop up									            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackCancelBtn(WM_MESSAGE *pMsg)
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
			s_dumpFile = eNotDump;
			WM_HideWindow(s_clearBtn);
			WM_HideWindow(s_popUp);

			developmentSettingScreenPageThree_ShowAllItem();
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
/* General 	-developmentSettingScreenPageThree_CallbackCloseBtn							*/
/*                                                                                      */
/* Details  -Call back function for Close button, handle event from 					*/
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
/*                     (O) s_dumpFile : current dump file                               */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_CallbackCloseBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 17);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 17);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			s_dumpFile = eNotDump;
			WM_HideWindow(s_popUp);

			developmentSettingScreenPageThree_ShowAllItem();
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
/* General 	-developmentSettingScreenPageThree_Init										*/
/*                                                                                      */
/* Details  -Initialize development setting screen page Three	            		    */
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
/*                     (O) g_developmentSettingScreenPageThree : development page three */
/*                     (O) s_EEPROMBtn : eeprom button						            */
/*                     (O) s_CBXBtn : CBX button						                */
/*                     (O) s_AnypalBtn : Anypal button						            */
/*                     (O) s_Spo2PulseBtn : Spo2Pulse button						    */
/*                     (O) s_debugLogBtn : debug log button						        */
/*                     (O) s_clearDebugLogBtn : clear debug log button					*/
/*                     (O) s_SRAMBtn : SRAM button						                */
/*                     (O) s_SDRAMBtn : SDRAM button						            */
/*                     (O) s_SPIFlashBtn : SPIFlash button						        */
/*                     (O) s_FlashABtn : flash A button						            */
/*                     (O) s_FlashBBtn : flash B button						            */
/*                     (O) s_backBtn : back button						                */
/*                     (O) s_homeBtn : home button						                */
/*                     (O) s_nextBtn : next button						                */
/*                     (O) s_prevBtn : previous button						            */
/*                     (O) s_pageText : page number						                */
/*                     (O) s_popUp : pop up						                        */
/*                     (O) s_titlePopUp : title pop up						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_closeBtn : close button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_Init(void)
{
	g_developmentSettingScreenPageThree = WM_CreateWindowAsChild(0, 0, 800, 480,g_maintenanceScreen,WM_CF_HIDE| WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageThree_Callback,0);

	s_EEPROMBtn = BUTTON_CreateEx(30, 70, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eEEPROMBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_EEPROMBtn,developmentSettingScreenPageThree_CallbackEEPROMBtn);

	s_CBXBtn = BUTTON_CreateEx(30, 127, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eCBXBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_CBXBtn,developmentSettingScreenPageThree_CallbackCBXBtn);

	s_AnypalBtn = BUTTON_CreateEx(30, 184, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eAnyPalBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_AnypalBtn,developmentSettingScreenPageThree_CallbackAnypalBtn);

	s_Spo2PulseBtn = BUTTON_CreateEx(30, 241, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eSpo2PulseBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_Spo2PulseBtn,developmentSettingScreenPageThree_CallbackSpo2PulseBtn);

	s_debugLogBtn = BUTTON_CreateEx(30, 298, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eDebugLogInDevelopmentPageThreeStringId);
	WM_SetCallback(s_debugLogBtn,developmentSettingScreenPageThree_CallbackDebugLogBtn);

	s_clearDebugLogBtn = BUTTON_CreateEx(30, 360, 250, 50, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eClearDebugLogInDevelopmentPageThreeStringId);
	WM_SetCallback(s_clearDebugLogBtn,developmentSettingScreenPageThree_CallbackClearDebugLogBtn);

	s_SRAMBtn = BUTTON_CreateEx(420, 70, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eSRAMBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_SRAMBtn,developmentSettingScreenPageThree_CallbackSRAMBtn);

	s_SDRAMBtn = BUTTON_CreateEx(420, 127, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eSDRAMBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_SDRAMBtn,developmentSettingScreenPageThree_CallbackSDRAMBtn);

	s_SPIFlashBtn = BUTTON_CreateEx(420, 184, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eSPIFlashBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_SPIFlashBtn,developmentSettingScreenPageThree_CallbackSPIFlashBtn);

	s_FlashABtn = BUTTON_CreateEx(420, 241, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eFlashABtnInDevelopmentPageThreeId);
	WM_SetCallback(s_FlashABtn,developmentSettingScreenPageThree_CallbackFlashABtn);

	s_FlashBBtn = BUTTON_CreateEx(420, 298, 250, 52, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eFlashBBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_FlashBBtn,developmentSettingScreenPageThree_CallbackFlashBBtn);

	//Back button
	s_backBtn = BUTTON_CreateEx(260, 415, 100, 60, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eBackBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_backBtn,developmentSettingScreenPageThree_CallbackBackBtn);

	//Home button
	s_homeBtn = BUTTON_CreateEx(445, 415, 100, 60, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eHomeBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_homeBtn,developmentSettingScreenPageThree_CallbackHomeBtn);

	//Next button
	s_nextBtn = BUTTON_CreateEx(700, 415, 100, 60, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, eNextBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_nextBtn,developmentSettingScreenPageThree_CallbackNextBtn);

	//Previous button
	s_prevBtn = BUTTON_CreateEx(0, 415, 102, 60, g_developmentSettingScreenPageThree, WM_CF_SHOW, 0, ePrevBtnInDevelopmentPageThreeId);
	WM_SetCallback(s_prevBtn,developmentSettingScreenPageThree_CallbackPrevBtn);

	s_pageText = TEXT_CreateEx(380,440, 50, 25,g_developmentSettingScreenPageThree,WM_CF_SHOW,TEXT_CF_LEFT,ePageTextInDevelopmentPageThreeId,"3/5");
	guiTaskTEXT_SetFont(s_pageText, &GUI_FontEng3L18B2PP);

	s_popUp = WM_CreateWindowAsChild(50, 115, 700, 225,g_developmentSettingScreenPageThree,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,developmentSettingScreenPageThree_CallbackPopUp,0);

	//Title pop up
	s_titlePopUp = TEXT_CreateEx(0, 30, 700, 70,s_popUp,WM_CF_SHOW,TEXT_CF_RIGHT,eTitlePopUpInDevelopmentPageThreeStringID,"");
	TEXT_SetTextColor(s_titlePopUp, GUI_BLACK);
	TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
	guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);

	//OK button
	s_OKBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eOkBtnPopUpInDevelopmentPageThreeStringId);
	WM_SetCallback(s_OKBtn,developmentSettingScreenPageThree_CallbackOKBtn);

	s_clearBtn = BUTTON_CreateEx(50, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eClearBtnPopUpInDevelopmentPageThreeStringId);
	WM_SetCallback(s_clearBtn,developmentSettingScreenPageThree_CallbackClearBtn);

	//Cancel button
	s_cancelBtn = BUTTON_CreateEx(380, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eCancelBtnPopUpInDevelopmentPageThreeStringId);
	WM_SetCallback(s_cancelBtn,developmentSettingScreenPageThree_CallbackCancelBtn);

	s_closeBtn = BUTTON_CreateEx(210, 120, 280, 60, s_popUp, WM_CF_SHOW, 0, eClosetnPopUpInDevelopmentPageThreeStringId);
	WM_SetCallback(s_closeBtn,developmentSettingScreenPageThree_CallbackCloseBtn);

	debugLog_ReadCtrlParameters();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_Show										*/
/*                                                                                      */
/* Details  -Show development setting screen page Three	            		    		*/
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
/*                     (O) g_developmentSettingScreenPageThree : development page three	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_Show(void)
{
	alarmInfor_ShowAlarmInfor();
	WM_HideWindow(s_popUp);
	developmentSettingScreenPageThree_ShowAllItem();
	WM_ShowWindow(g_developmentSettingScreenPageThree);
	WM_BringToTop(g_developmentSettingScreenPageThree);
	WM_SetFocus(g_developmentSettingScreenPageThree);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_Hide										*/
/*                                                                                      */
/* Details  -Hide development setting screen page Three	            		    		*/
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
/*                     (O) g_developmentSettingScreenPageThree : development page three */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_Hide(void)
{
	WM_HideWindow(g_developmentSettingScreenPageThree);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_ClosePopUp								*/
/*                                                                                      */
/* Details  -Close pop up								            		    		*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool saveResult : result save										*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_titlePopUp : title pop up                                  */
/*                     (O) s_closeBtn : close button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_ClosePopUp(bool saveResult)
{
	if(saveResult == false)
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"I was not able to save the log data");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを保存出来ませんでした");
		}
		WM_ShowWindow(s_closeBtn);
	}
	else
	{
		WM_MoveChildTo(s_titlePopUp,0,50);
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontEng3L16B2PP);
			TEXT_SetText(s_titlePopUp,"I have to saved the log data");
		}
		else
		{
			guiTaskTEXT_SetFont(s_titlePopUp, &GUI_FontJapan3L16B2PP);
			TEXT_SetText(s_titlePopUp,"ログデータを保存しました");
		}
		WM_ShowWindow(s_closeBtn);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_ClearPopUp								*/
/*                                                                                      */
/* Details  -Show clear pop up								            		    	*/
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
/*                     (O) s_titlePopUp : title pop up                                  */
/*                     (O) s_closeBtn : close button						            */
/*                     (O) s_OKBtn : OK button						                    */
/*                     (O) s_clearBtn : clear button						            */
/*                     (O) s_cancelBtn : cancel button						            */
/*                     (O) s_popUp : pop up						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_ClearPopUp(void)
{
	WM_MoveChildTo(s_titlePopUp,0,30);
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontEng3L16B2PP);
		TEXT_SetText(s_titlePopUp,"I will clear the log data\nWould you like?");
	}
	else
	{
		TEXT_SetTextAlign(s_titlePopUp, GUI_TA_CENTER);
		guiTaskTEXT_SetFont(s_titlePopUp,&GUI_FontJapan3L16B2PP);
		TEXT_SetText(s_titlePopUp,"ログデータをクリアします\nよろしいですか？");
	}

	WM_HideWindow(s_closeBtn);
	WM_HideWindow(s_OKBtn);

	WM_ShowWindow(s_clearBtn);
	WM_ShowWindow(s_cancelBtn);

	WM_ShowWindow(s_popUp);
	WM_BringToTop(s_popUp);

	developmentSettingScreenPageThree_HideAllItem();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-developmentSettingScreenPageThree_SendToUSB								*/
/*                                                                                      */
/* Details  -Send to usb task								            		    	*/
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
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void developmentSettingScreenPageThree_SendToUSB(void)
{
	switch(s_dumpFile)
	{
	case eDumpEEPROM:
		DEBUGOUT("\n eDumpEEPROM");
		usb_SendEvent(eRequestEEPROMDump);
		break;
	case eDumpCBX:
		DEBUGOUT("\n eDumpCBX");
		usb_SendEvent(eRequestCBXLogDump);
		break;
	case eDumpAnypal:
		DEBUGOUT("\n eDumpAnypal");
		usb_SendEvent(eRequestAnypalLogDump);
		break;
	case eDumpPulseO2:
		DEBUGOUT("\n eDumpPulseO2");
		usb_SendEvent(eRequestSpO2PulseLogDump);
		break;
	case eDumpSRAM:
		DEBUGOUT("\n eDumpSRAM");
		usb_SendEvent(eRequestSRAMDump);
		break;
	case eDumpSDRAM:
		DEBUGOUT("\n eDumpSDRAM");
		usb_SendEvent(eRequestSDRAMDump);
		break;
	case eDumpSPIFlash:
		DEBUGOUT("\n eDumpSPIFlash");
		usb_SendEvent(eRequestFlashSPIDump);
		break;
	case eDumpFlashA:
		DEBUGOUT("\n eDumpFlashA");
		usb_SendEvent(eRequestFlashADump);
		break;
	case eDumpFlashB:
		DEBUGOUT("\n eDumpFlashB");
		usb_SendEvent(eRequestFlashBDump);
		break;
	case eDebugLog:
		DEBUGOUT("\n eDebugLog");
		usb_SendEvent(eRequestExportDebugLog);
		break;
	default:
		DEBUGOUT("\n dump default");
		break;
	}

	return;
}

/* end of files */

