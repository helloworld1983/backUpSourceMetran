/********************************************************************/
/*                                                                  */
/* File Name    : PSAParaChangeDialog.c                             */
/*                                                                  */
/* General      : PSAParaChangeDialog allow user change psa 		*/
/* 				  parameter.										*/
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
/* #0001       2017/09/05       Viet Le				modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : TEXT_SetFont()->guiTaskTEXT_SetFont()(bug 1997)*/
/********************************************************************/

#include "stdlib.h"
#include <StringTools.h>
#include "PSAParaChangeDialog.h"
#include "PSAParameterChangePageOneScreen.h"
#include "board.h"
#include "EDIT.h"
#include "SettingButton.h"
#include "StringTable.h"
#include "BUTTON.h"
#include "PSATableMgr.h"
#include "setting.h"
#include "TEXT.h"
#include "MainScreen.h"
#include "GuiTask.h"
#include "RADIO.h"
#include "Font.h"
#include "GuiInterface.h"

WM_HWIN g_PSAParaChangeDialog;

static EDIT_Handle s_PSAPressureBtn;
static EDIT_Handle s_Solenoid1OnOffSetTimeBtn;
static EDIT_Handle s_Solenoid1OffOffSetTimeBtn;
static EDIT_Handle s_Solenoid2OnOffSetTimeBtn;
static EDIT_Handle s_Solenoid2OffOffSetTimeBtn;
static EDIT_Handle s_Solenoid3OnOffSetTimeBtn;
static EDIT_Handle s_Solenoid3OffOffSetTimeBtn;
static EDIT_Handle s_Solenoid4OnOffSetTimeBtn;
static EDIT_Handle s_Solenoid4OffOffSetTimeBtn;
static EDIT_Handle s_Solenoid5OnOffSetTimeABtn;
static EDIT_Handle s_Solenoid5OnOffSetTimeBBtn;
static EDIT_Handle s_Solenoid5OpenTimeABtn;
static EDIT_Handle s_Solenoid5OpenTimeBBtn;
static EDIT_Handle s_CompressorRPMPSABtn;
static EDIT_Handle s_FCOutputPSABtn;
static EDIT_Handle s_CoolingFanDutyPSABtn;
static EDIT_Handle s_maxPSATimeBtn;
static BUTTON_Handle s_closePSABtn;
static TEXT_Handle s_titleDialog;

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParaChangeDialog_Callback                                            	*/
/*                                                                                      */
/* Details  -call back function for PSA parameter dialog, handle event from window		*/
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
void PSAParaChangeDialog_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);

		GUI_SetColor(SERVICE_BACKGROUND_COLOR);
		GUI_AA_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,20);

		GUI_SetColor(BORDER_PSA_CHANGE_DIALOG_COLOR);
		GUI_AA_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,20);
		break;
	default:
		WM_DefaultProc(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParaChangeDialog_CallbackCloseBtn                                       */
/*                                                                                      */
/* Details  -call back function for close button, handle event from window				*/
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
void PSAParaChangeDialog_CallbackCloseBtn(WM_MESSAGE *pMsg)
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
			GUI_DispStringHCenterAt("Close", Rect.x1/2, 15+2);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L16B2PP);
			GUI_DispStringHCenterAt("閉じる", Rect.x1/2, 15+2);
		}
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			if(guiTask_GetSelectPort() == DEFAULT_SELECT)
			{
				DEBUGOUT("\n default port when close psa dialog");
				PSATableMgr_SetDefaultSelectPort();
			}
			else
			{
				DEBUGOUT("\n eeprom port when close psa dialog");
				PSATableMgr_SetEEPROMSelectPort();
			}

			PSAParaChangeDialog_Hide();
			PSAParameterChangePageOneScreen_ShowAllChild();
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
/* General 	-PSAParaChangeDialog_Init 							                		*/
/*                                                                                      */
/* Details  -Initialize PSA parameter dialog 			            	                */
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
/*                     (O) g_PSAParaChangeDialog : psa parameter change dialog          */
/*                     (O) s_titleDialog : title dialog						            */
/*                     (O) s_PSAPressureBtn : psa pressure setting    				   	*/
/*                     (O) s_Solenoid1OnOffSetTimeBtn : sol 1 onoff time setting        */
/*                     (O) s_Solenoid1OffOffSetTimeBtn : sol 1 offoff time setting     	*/
/*                     (O) s_Solenoid2OnOffSetTimeBtn : sol 2 onoff time setting        */
/*                     (O) s_Solenoid2OffOffSetTimeBtn : sol 2 offoff time setting     	*/
/*                     (O) s_Solenoid3OnOffSetTimeBtn : sol 3 onoff time setting        */
/*                     (O) s_Solenoid3OffOffSetTimeBtn : sol 3 offoff time setting     	*/
/*                     (O) s_Solenoid4OnOffSetTimeBtn : sol 4 onoff time setting        */
/*                     (O) s_Solenoid4OffOffSetTimeBtn : sol 4 offoff time setting     	*/
/*                     (O) s_Solenoid5OnOffSetTimeABtn : sol 5 onoff time A setting	    */
/*                     (O) s_Solenoid5OnOffSetTimeBBtn : sol 5 onoff time B setting     */
/*                     (O) s_Solenoid5OpenTimeABtn : sol 5 opentime A setting           */
/*                     (O) s_Solenoid5OpenTimeBBtn : sol 5 opentime B setting     		*/
/*                     (O) s_CompressorRPMPSABtn : compressor RPM setting               */
/*                     (O) s_FCOutputPSABtn : flow controller output setting  			*/
/*                     (O) s_CoolingFanDutyPSABtn : cooling fan duty setting            */
/*                     (O) s_maxPSATimeBtn : max psa time setting 					   	*/
/*                     (O) s_closePSABtn : close button						            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParaChangeDialog_Init(void)
{
	GUI_RECT Rect;

	g_PSAParaChangeDialog = WM_CreateWindowAsChild(5, 5, 790, 470,g_PSAParameterChangePageOneScreen,WM_CF_HIDE | WM_CF_MEMDEV | WM_CF_LATE_CLIP,PSAParaChangeDialog_Callback,0);

	s_titleDialog = TEXT_CreateEx(20,5,760,30,g_PSAParaChangeDialog,WM_CF_SHOW,TEXT_CF_HCENTER,eTitlePSAChangeDialogStringId,"");
	guiTaskTEXT_SetFont(s_titleDialog,&GUI_FontEng3L12B2PP);
	TEXT_SetTextColor(s_titleDialog,GUI_BLACK);
	TEXT_SetTextAlign(s_titleDialog,GUI_TA_CENTER | GUI_TA_VCENTER);

	s_PSAPressureBtn = EDIT_CreateEx(30,50,180+25+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,ePSAPressureStringID,50);
	EDIT_SetFocussable(s_PSAPressureBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_PSAPressureBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_PSAPressureBtn,eUpBtnPSAPressureStringId,eDownBtnPSAPressureStringId,eValueSettingPSAPressureId,eValueOfPSAPressureStringId,Rect);
	WM_SetCallback(s_PSAPressureBtn,settingButton_Callback);

	s_Solenoid1OnOffSetTimeBtn = EDIT_CreateEx(290,50,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid1OnOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid1OnOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid1OnOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid1OnOffSetTimeBtn,eUpBtnSolenoid1OnOffSetTimeStringId,eDownBtnSolenoid1OnOffSetTimeStringId,eValueSettingSolenoid1OnOffSetTimeId,eValueOfSolenoid1OnOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid1OnOffSetTimeBtn,settingButton_Callback);

	s_Solenoid1OffOffSetTimeBtn = EDIT_CreateEx(540,50,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid1OffOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid1OffOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid1OffOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid1OffOffSetTimeBtn,eUpBtnSolenoid1OffOffSetTimeStringId,eDownBtnSolenoid1OffOffSetTimeStringId,eValueSettingSolenoid1OffOffSetTimeId,eValueOfSolenoid1OffOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid1OffOffSetTimeBtn,settingButton_Callback);

	s_Solenoid2OnOffSetTimeBtn = EDIT_CreateEx(290,120,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid2OnOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid2OnOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid2OnOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid2OnOffSetTimeBtn,eUpBtnSolenoid2OnOffSetTimeStringId,eDownBtnSolenoid2OnOffSetTimeStringId,eValueSettingSolenoid2OnOffSetTimeId,eValueOfSolenoid2OnOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid2OnOffSetTimeBtn,settingButton_Callback);

	s_Solenoid2OffOffSetTimeBtn = EDIT_CreateEx(540,120,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid2OffOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid2OffOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid2OffOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid2OffOffSetTimeBtn,eUpBtnSolenoid2OffOffSetTimeStringId,eDownBtnSolenoid2OffOffSetTimeStringId,eValueSettingSolenoid2OffOffSetTimeId,eValueOfSolenoid2OffOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid2OffOffSetTimeBtn,settingButton_Callback);

	s_Solenoid3OnOffSetTimeBtn = EDIT_CreateEx(290,190,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid3OnOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid3OnOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid3OnOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid3OnOffSetTimeBtn,eUpBtnSolenoid3OnOffSetTimeStringId,eDownBtnSolenoid3OnOffSetTimeStringId,eValueSettingSolenoid3OnOffSetTimeId,eValueOfSolenoid3OnOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid3OnOffSetTimeBtn,settingButton_Callback);

	s_Solenoid3OffOffSetTimeBtn = EDIT_CreateEx(540,190,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid3OffOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid3OffOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid3OffOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid3OffOffSetTimeBtn,eUpBtnSolenoid3OffOffSetTimeStringId,eDownBtnSolenoid3OffOffSetTimeStringId,eValueSettingSolenoid3OffOffSetTimeId,eValueOfSolenoid3OffOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid3OffOffSetTimeBtn,settingButton_Callback);

	s_Solenoid4OnOffSetTimeBtn = EDIT_CreateEx(290,260,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid4OnOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid4OnOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid4OnOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid4OnOffSetTimeBtn,eUpBtnSolenoid4OnOffSetTimeStringId,eDownBtnSolenoid4OnOffSetTimeStringId,eValueSettingSolenoid4OnOffSetTimeId,eValueOfSolenoid4OnOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid4OnOffSetTimeBtn,settingButton_Callback);

	s_Solenoid4OffOffSetTimeBtn = EDIT_CreateEx(540,260,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid4OffOffSetTimeStringID,50);
	EDIT_SetFocussable(s_Solenoid4OffOffSetTimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid4OffOffSetTimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid4OffOffSetTimeBtn,eUpBtnSolenoid4OffOffSetTimeStringId,eDownBtnSolenoid4OffOffSetTimeStringId,eValueSettingSolenoid4OffOffSetTimeId,eValueOfSolenoid4OffOffSetTimeStringId,Rect);
	WM_SetCallback(s_Solenoid4OffOffSetTimeBtn,settingButton_Callback);

	s_Solenoid5OnOffSetTimeABtn = EDIT_CreateEx(290,330,180+25 + 3+10 + 8,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid5OnOffSetTimeAStringID,50);
	EDIT_SetFocussable(s_Solenoid5OnOffSetTimeABtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid5OnOffSetTimeABtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid5OnOffSetTimeABtn,eUpBtnSolenoid5OnOffSetTimeAStringId,eDownBtnSolenoid5OnOffSetTimeAStringId,eValueSettingSolenoid5OnOffSetTimeAId,eValueOfSolenoid5OnOffSetTimeAStringId,Rect);
	WM_SetCallback(s_Solenoid5OnOffSetTimeABtn,settingButton_Callback);

	s_Solenoid5OnOffSetTimeBBtn = EDIT_CreateEx(540,330,180+25 + 3+10 + 8,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid5OnOffSetTimeBStringID,50);
	EDIT_SetFocussable(s_Solenoid5OnOffSetTimeBBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid5OnOffSetTimeBBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid5OnOffSetTimeBBtn,eUpBtnSolenoid5OnOffSetTimeBStringId,eDownBtnSolenoid5OnOffSetTimeBStringId,eValueSettingSolenoid5OnOffSetTimeBId,eValueOfSolenoid5OnOffSetTimeBStringId,Rect);
	WM_SetCallback(s_Solenoid5OnOffSetTimeBBtn,settingButton_Callback);

	s_Solenoid5OpenTimeABtn = EDIT_CreateEx(290,400,180+25+10+5,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid5OpenTimeAStringId,50);
	EDIT_SetFocussable(s_Solenoid5OpenTimeABtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid5OpenTimeABtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid5OpenTimeABtn,eUpBtnSolenoid5OpenTimeAStringId,eDownBtnSolenoid5OpenTimeAStringId,eValueSettingSolenoid5OpenTimeAId,eValueOfSolenoid5OpenTimeAStringId,Rect);
	WM_SetCallback(s_Solenoid5OpenTimeABtn,settingButton_Callback);

	s_Solenoid5OpenTimeBBtn = EDIT_CreateEx(540,400,180+25+10+5,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eSolenoid5OpenTimeBStringId,50);
	EDIT_SetFocussable(s_Solenoid5OpenTimeBBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_Solenoid5OpenTimeBBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_Solenoid5OpenTimeBBtn,eUpBtnSolenoid5OpenTimeBStringId,eDownBtnSolenoid5OpenTimeBStringId,eValueSettingSolenoid5OpenTimeBId,eValueOfSolenoid5OpenTimeBStringId,Rect);
	WM_SetCallback(s_Solenoid5OpenTimeBBtn,settingButton_Callback);

	s_CompressorRPMPSABtn = EDIT_CreateEx(30,120,180+25+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eCompressorRPMPSAStringId,50);
	EDIT_SetFocussable(s_CompressorRPMPSABtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_CompressorRPMPSABtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_CompressorRPMPSABtn,eUpBtnCompressorRPMPSAStringId,eDownBtnCompressorRPMPSAStringId,eValueSettingCompressorRPMPSAId,eValueOfCompressorRPMPSAStringId,Rect);
	WM_SetCallback(s_CompressorRPMPSABtn,settingButton_Callback);

	s_FCOutputPSABtn = EDIT_CreateEx(30,190,180+25+10+10,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eFCOutputPSAStringId,50);
	EDIT_SetFocussable(s_FCOutputPSABtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_FCOutputPSABtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_FCOutputPSABtn,eUpBtnFCOutputPSAStringId,eDownBtnFCOutputPSAStringId,eValueSettingFCOutputPSAId,eValueOfFCOutputPSAStringId,Rect);
	WM_SetCallback(s_FCOutputPSABtn,settingButton_Callback);

	s_CoolingFanDutyPSABtn = EDIT_CreateEx(30,260,180+25,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eCoolingFanDutyPSAStringId,50);
	EDIT_SetFocussable(s_CoolingFanDutyPSABtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_CoolingFanDutyPSABtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_CoolingFanDutyPSABtn,eUpBtnCoolingFanDutyPSAStringId,eDownBtnCoolingFanDutyPSAStringId,eValueSettingCoolingFanDutyPSAId,eValueOfCoolingFanDutyPSAStringId,Rect);
	WM_SetCallback(s_CoolingFanDutyPSABtn,settingButton_Callback);

	s_maxPSATimeBtn = EDIT_CreateEx(30,330,180+25,60,g_PSAParaChangeDialog,WM_CF_SHOW | WM_CF_MEMDEV,0,eMaxPSATimeStringId,50);
	EDIT_SetFocussable(s_maxPSATimeBtn,EDIT_CI_DISABLED);
	WM_GetClientRectEx(s_maxPSATimeBtn,&Rect);
	settingButton_InitNoSaveSmallBtn(s_maxPSATimeBtn,eUpBtnMaxPSATimeStringId,eDownBtnMaxPSATimeStringId,eValueSettingMaxPSATimeId,eValueOfMaxPSATimeStringId,Rect);
	WM_SetCallback(s_maxPSATimeBtn,settingButton_Callback);

	s_closePSABtn = BUTTON_CreateEx(40,400,140,60,g_PSAParaChangeDialog,WM_CF_SHOW,0,GUI_ID_BUTTON9);
	WM_SetCallback(s_closePSABtn,PSAParaChangeDialog_CallbackCloseBtn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParaChangeDialog_SetValueItem 							               	*/
/*                                                                                      */
/* Details  -Display PSA parameter item when show dialog            	               	*/
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
void PSAParaChangeDialog_SetValueItem(void)
{
	int8_t buffer[BUFFER_VALUE_PSA] = { 0 };
	double floatValue = 0.0;
	int intValue = 0;

	E_O2ConcentratorType conditionO2 = eNormalO2Type;
	E_TemperatureType conditionTemp = eHighTempType;

	int valueO2Radio = PSAParameterChangePageOneScreen_GetConditionO2();
	int valueTempRadio = PSAParameterChangePageOneScreen_GetConditionTemp();

	if(valueO2Radio >= 0)
	{
		conditionO2 = (E_O2ConcentratorType)valueO2Radio;
	}

	if(valueTempRadio >= 0)
	{
		conditionTemp = (E_TemperatureType)valueTempRadio;
	}

	uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE);

	itoa(PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,ePSAPress),(char*)buffer,10);
	settingButton_SetValue(eValueSettingPSAPressureId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid1OnOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid1OnOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid1OffOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid1OffOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid2OnOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid2OnOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid2OffOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid2OffOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid3OnOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid3OnOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid3OffOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid3OffOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid4OnOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid4OnOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid4OffOffsetTime) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid4OffOffSetTimeId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid5OnOffsetTimeA) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid5OnOffSetTimeAId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid5OnOffsetTimeB) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid5OnOffSetTimeBId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid5OpenTimeA) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid5OpenTimeAId,buffer);

	floatValue = (double)PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoid5OpenTimeB) / PSA_VALUE_SCALE;
	sprintf((char*)buffer,"%0.1f",floatValue);
	settingButton_SetValue(eValueSettingSolenoid5OpenTimeBId,buffer);

	intValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCompressorRPM);
	itoa(intValue,(char*)buffer,10);
	settingButton_SetValue(eValueSettingCompressorRPMPSAId,buffer);

	intValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMFCOutput);
	settingButton_SetValue(eValueSettingFCOutputPSAId,stringtools_ConvertFCFlowToString((uint8_t)intValue));

	itoa(PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCoolingFanDutyCycle),(char*)buffer,10);
	settingButton_SetValue(eValueSettingCoolingFanDutyPSAId,buffer);

	itoa(PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMaxPSATime),(char*)buffer,10);
	settingButton_SetValue(eValueSettingMaxPSATimeId,buffer);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSAParaChangeDialog_Show 							               			*/
/*                                                                                      */
/* Details  -Show PSA parameter dialog 			            	               			*/
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
/*                     (O) s_titleDialog : title popup                                  */
/*                     (O) g_PSAParaChangeDialog : psa parameter dialog		            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParaChangeDialog_Show(void)
{
	int8_t buffer[SIZE_BUFFER_TITLE_PSA] = {'\0'};
	int8_t bufferText[SIZE_BUFFER_TEXT_PSA] = {'\0'};
	int8_t flowBuffer[BUFFER_VALUE_PSA] = {'\0'};

	uint8_t flowValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE);
	DEBUGOUT("\n flowValue = %d", flowValue);

	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		strcpy((char*)bufferText, "Parameter change(Setting flow rate ");
	}
	else
	{
		strcpy((char*)bufferText, "パラメータ変更（設定流量 ");
	}
	strncpy((char*)flowBuffer, (char*)stringtools_ConvertFCFlowToString(flowValue),BUFFER_VALUE_PSA - 1);
	strncat((char*)buffer,(char*)bufferText,SIZE_BUFFER_TEXT_PSA);
	strncat((char*)buffer,(char*)flowBuffer,BUFFER_VALUE_PSA);
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		strcat((char*)buffer,"L/min");
	}
	else
	{
		strcat((char*)buffer,"L/分");
	}

	int conditionTemp = PSAParameterChangePageOneScreen_GetConditionTemp();
	int conditionO2 = PSAParameterChangePageOneScreen_GetConditionO2();

	switch(conditionTemp)
	{
	case eHighTempType:
		if(conditionO2 == eNormalO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", High temp, Normal O2)");
			}
			else
			{
				strcat((char*)buffer, "・ 高温・ 濃度正常)");
			}
		}
		else if(conditionO2 == eLowO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", High temp, Low O2)");
			}
			else
			{
				strcat((char*)buffer, "・ 高温・濃度低下)");
			}
		}
		break;
	case eNormalTempType:
		if(conditionO2 == eNormalO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", Normal temp, Normal O2)");
			}
			else
			{
				strcat((char*)buffer, "・常温・濃度正常)");
			}
		}
		else if(conditionO2 == eLowO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", Normal temp, Low O2)");
			}
			else
			{
				strcat((char*)buffer, "・常温・濃度低下)");
			}
		}
		break;
	case eLowTempType:
		if(conditionO2 == eNormalO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", Low temp, Normal O2)");
			}
			else
			{
				strcat((char*)buffer, "・低温・濃度正常)");
			}
		}
		else if(conditionO2 == eLowO2Type)
		{
			if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
			{
				strcat((char*)buffer, ", Low temp, low O2)");
			}
			else
			{
				strcat((char*)buffer, "・低温・濃度低下)");
			}
		}
		break;
	default:
		break;
	}


	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		guiTaskTEXT_SetFont(s_titleDialog, &GUI_FontEng3L12B2PP);
	}
	else
	{
		guiTaskTEXT_SetFont(s_titleDialog, &GUI_FontJapan3L16B2PP);
	}

	TEXT_SetText(s_titleDialog, (char*)buffer);
	PSAParaChangeDialog_SetValueItem();

	WM_ShowWindow(g_PSAParaChangeDialog);
	WM_BringToTop(g_PSAParaChangeDialog);
	WM_SetFocus(g_PSAParaChangeDialog);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Hide PSA parameter dialog 							               			*/
/*                                                                                      */
/* Details  -Hide PSA parameter dialog 			            	               			*/
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
/*                     (O) g_PSAParaChangeDialog : psa parameter dialog    			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSAParaChangeDialog_Hide(void)
{
	WM_HideWindow(g_PSAParaChangeDialog);

	return;
}

/* end of files */

