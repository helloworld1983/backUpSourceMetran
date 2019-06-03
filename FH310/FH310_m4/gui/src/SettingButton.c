/********************************************************************/
/*                                                                  */
/* File Name    : SettingButton.c                                 	*/
/*                                                                  */
/* General      : The SettingButton category provides               */
/* 				  the user an interface to display and              */
/*                change setting in system                          */
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
/* #0001       2017/09/05       Viet Le             modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/* #0003       2017/12/01	   Viet Le				modify			*/
/*	-Edit : unlogical sentence at settingButton_CallbackDownBtn()	*/
/*		(bug 2049)													*/
/* #0004       2017/12/01	   Viet Le				modify			*/
/*	-Edit : unlogical sentence at settingButton_CallbackDownLargeBtn()*/
/*		(bug 2048)													*/
/* #0005       2017/12/01	   Viet Le				modify			*/
/*	-Change max solenoid valve setting 20 to 10(bug 2051)			*/
/********************************************************************/

#include "string.h"
#include "stdlib.h"
#include "board.h"
#include <StringTools.h>
#include "WM.h"
#include "TEXT.h"
#include "GUI.h"
#include "EDIT.h"
#include "BUTTON.h"
#include "SettingButton.h"
#include "TextLibrary.h"
#include "setting.h"
#include "PSATableMgr.h"
#include "TaskCommon.h"
#include "GuiTask.h"
#include "AudioMgr.h"
#include "SettingOnOffButton.h"
#include "RADIO.h"
#include "Font.h"
#include "MainScreen.h"
#include "PSAParameterChangePageOneScreen.h"
#include "GuiInterface.h"

static BUTTON_Handle s_upBtn;
static BUTTON_Handle s_downBtn;
static EDIT_Handle s_valueSettingList[eValueSettingMAXId];
static E_ItemOfParam s_currentItemPSA = eMaxItemOfParam;
/*******************************************************************
 * *
The points of the arrow
 */
static const GUI_POINT s_aPointUpArrow[] = {
		{39/2,5},
		{5,34},
		{39-5,39-5}
};

static const GUI_POINT s_aPointDownArrow[] = {
		{5,5},
		{34,5},
		{39/2,34}
};

static const GUI_POINT s_aPointUpArrowLarge[] = {
		{49/2,5},
		{5,44},
		{49-5,49-5}
};

static const GUI_POINT s_aPointDownArrowLarge[] = {
		{5,5},
		{44,5},
		{49/2,44}
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SetItemForPSAPara				 						    */
/*                                                                                      */
/* Details  -Set current item psa parameter					 							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int id : - id of setting button										*/
/*					 - range : 0 - 336													*/
/*                   - unit : NONE                                                      */
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_currentItemPSA : psa item                                  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_SetItemForPSAPara(int id)
{
	switch (id)
	{
	case eUpBtnSolenoid1OnOffSetTimeStringId:
	case eDownBtnSolenoid1OnOffSetTimeStringId:
		s_currentItemPSA = eSolenoid1OnOffsetTime;
		break;
	case eUpBtnSolenoid1OffOffSetTimeStringId:
	case eDownBtnSolenoid1OffOffSetTimeStringId:
		s_currentItemPSA = eSolenoid1OffOffsetTime;
		break;
	case eUpBtnSolenoid2OnOffSetTimeStringId:
	case eDownBtnSolenoid2OnOffSetTimeStringId:
		s_currentItemPSA = eSolenoid2OnOffsetTime;
		break;
	case eUpBtnSolenoid2OffOffSetTimeStringId:
	case eDownBtnSolenoid2OffOffSetTimeStringId:
		s_currentItemPSA = eSolenoid2OffOffsetTime;
		break;
	case eUpBtnSolenoid3OnOffSetTimeStringId:
	case eDownBtnSolenoid3OnOffSetTimeStringId:
		s_currentItemPSA = eSolenoid3OnOffsetTime;
		break;
	case eUpBtnSolenoid3OffOffSetTimeStringId:
	case eDownBtnSolenoid3OffOffSetTimeStringId:
		s_currentItemPSA = eSolenoid3OffOffsetTime;
		break;
	case eUpBtnSolenoid4OnOffSetTimeStringId:
	case eDownBtnSolenoid4OnOffSetTimeStringId:
		s_currentItemPSA = eSolenoid4OnOffsetTime;
		break;
	case eUpBtnSolenoid4OffOffSetTimeStringId:
	case eDownBtnSolenoid4OffOffSetTimeStringId:
		s_currentItemPSA = eSolenoid4OffOffsetTime;
		break;
	case eUpBtnSolenoid5OnOffSetTimeAStringId:
	case eDownBtnSolenoid5OnOffSetTimeAStringId:
		s_currentItemPSA = eSolenoid5OnOffsetTimeA;
		break;
	case eUpBtnSolenoid5OnOffSetTimeBStringId:
	case eDownBtnSolenoid5OnOffSetTimeBStringId:
		s_currentItemPSA = eSolenoid5OnOffsetTimeB;
		break;
	case eUpBtnSolenoid5OpenTimeAStringId:
	case eDownBtnSolenoid5OpenTimeAStringId:
		s_currentItemPSA = eSolenoid5OpenTimeA;
		break;
	case eUpBtnSolenoid5OpenTimeBStringId:
	case eDownBtnSolenoid5OpenTimeBStringId:
		s_currentItemPSA = eSolenoid5OpenTimeB;
		break;
	case eUpBtnSolenoidOpenTimeOnEndStringId:
	case eDownBtnSolenoidOpenTimeOnEndStringId:
		s_currentItemPSA = eSolenoidOpenTimeOnEnd;
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_PlayGuidanceLevel 							 				*/
/*                                                                                      */
/* Details  -Play voice with volume							 							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t level :  - current level										*/
/*					 - range : 0-10														*/
/*                   - unit : NONE                                                      */
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
void settingButton_PlayGuidanceLevel(uint8_t level)
{
	uint8_t event = 0;
	switch(level)
	{
	case VOLUME_1:
		event = eLevel01Audio;
		break;
	case VOLUME_2:
		event = eLevel02Audio;
		break;
	case VOLUME_3:
		event = eLevel03Audio;
		break;
	case VOLUME_4:
		event = eLevel04Audio;
		break;
	case VOLUME_5:
		event = eLevel05Audio;
		break;
	case VOLUME_6:
		event = eLevel06Audio;
		break;
	case VOLUME_7:
		event = eLevel07Audio;
		break;
	case VOLUME_8:
		event = eLevel08Audio;
		break;
	case VOLUME_9:
		event = eLevel09Audio;
		break;
	case MAX_VOLUME:
		event = eLevel10Audio;
		break;
	default:
		break;
	}

	audioMgr_Request(event, eNoAlarmAudio);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_PlayAlarmLevel 							 					*/
/*                                                                                      */
/* Details  -Play voice alarm with volume							 				    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t level : current level										*/
/*					 - range : 0-10														*/
/*                   - unit : NONE                                                      */
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
void settingButton_PlayAlarmLevel(uint8_t level)
{
	uint8_t event = 0;
	switch(level)
	{
	case VOLUME_1:
		event = eLevel01AlarmAudio;
		break;
	case VOLUME_2:
		event = eLevel02AlarmAudio;
		break;
	case VOLUME_3:
		event = eLevel03AlarmAudio;
		break;
	case VOLUME_4:
		event = eLevel04AlarmAudio;
		break;
	case VOLUME_5:
		event = eLevel05AlarmAudio;
		break;
	case VOLUME_6:
		event = eLevel06AlarmAudio;
		break;
	case VOLUME_7:
		event = eLevel07AlarmAudio;
		break;
	case VOLUME_8:
		event = eLevel08AlarmAudio;
		break;
	case VOLUME_9:
		event = eLevel09AlarmAudio;
		break;
	case MAX_VOLUME:
		event = eLevel10AlarmAudio;
		break;
	default:
		break;
	}

	audioMgr_Request(event, eNoAlarmAudio);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SaveDisplaySettingPSA				 						*/
/*                                                                                      */
/* Details  -Save and display item psa parameter					 					*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int id :  - id of setting button										*/
/*                    -range : 0-336                                                    */
/*                    -unit : NONE                                                      */
/*                                                                                      */
/* 				   const int8_t * chr : string display								    */
/*                                                                                      */
/*				   uint8_t value :  - value for save									*/
/*                      - range : 0-255                                                 */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_valueSettingList : value setting list               	   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_SaveDisplaySettingPSA(int id,const int8_t * chr, uint8_t value)
{
	E_ItemOfParam item = eMaxItemOfParam;
	E_ValueSettingBtnId valueSettingId = eFirstValueSettingId;

	E_O2ConcentratorType conditionO2 = eNormalO2Type;
	E_TemperatureType conditionTemp = eHighTempType;

	int valueO2Radio = PSAParameterChangePageOneScreen_GetConditionO2();
	int valueTempRadio = PSAParameterChangePageOneScreen_GetConditionTemp();

	uint8_t flowValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);

	if(valueO2Radio >= 0)
	{
		conditionO2 = (E_O2ConcentratorType)valueO2Radio;
	}

	if(valueTempRadio >= 0)
	{
		conditionTemp = (E_TemperatureType)valueTempRadio;
	}

	switch (id)
	{
	case eUpBtnSolenoid1OnOffSetTimeStringId:
	case eDownBtnSolenoid1OnOffSetTimeStringId:
		item = eSolenoid1OnOffsetTime;
		valueSettingId = eValueSettingSolenoid1OnOffSetTimeId;
		break;
	case eUpBtnSolenoid1OffOffSetTimeStringId:
	case eDownBtnSolenoid1OffOffSetTimeStringId:
		item = eSolenoid1OffOffsetTime;
		valueSettingId = eValueSettingSolenoid1OffOffSetTimeId;
		break;
	case eUpBtnSolenoid2OnOffSetTimeStringId:
	case eDownBtnSolenoid2OnOffSetTimeStringId:
		item = eSolenoid2OnOffsetTime;
		valueSettingId = eValueSettingSolenoid2OnOffSetTimeId;
		break;
	case eUpBtnSolenoid2OffOffSetTimeStringId:
	case eDownBtnSolenoid2OffOffSetTimeStringId:
		item = eSolenoid2OffOffsetTime;
		valueSettingId = eValueSettingSolenoid2OffOffSetTimeId;
		break;
	case eUpBtnSolenoid3OnOffSetTimeStringId:
	case eDownBtnSolenoid3OnOffSetTimeStringId:
		item = eSolenoid3OnOffsetTime;
		valueSettingId = eValueSettingSolenoid3OnOffSetTimeId;
		break;
	case eUpBtnSolenoid3OffOffSetTimeStringId:
	case eDownBtnSolenoid3OffOffSetTimeStringId:
		item = eSolenoid3OffOffsetTime;
		valueSettingId = eValueSettingSolenoid3OffOffSetTimeId;
		break;
	case eUpBtnSolenoid4OnOffSetTimeStringId:
	case eDownBtnSolenoid4OnOffSetTimeStringId:
		item = eSolenoid4OnOffsetTime;
		valueSettingId = eValueSettingSolenoid4OnOffSetTimeId;
		break;
	case eUpBtnSolenoid4OffOffSetTimeStringId:
	case eDownBtnSolenoid4OffOffSetTimeStringId:
		item = eSolenoid4OffOffsetTime;
		valueSettingId = eValueSettingSolenoid4OffOffSetTimeId;
		break;
	case eUpBtnSolenoid5OnOffSetTimeAStringId:
	case eDownBtnSolenoid5OnOffSetTimeAStringId:
		item = eSolenoid5OnOffsetTimeA;
		valueSettingId = eValueSettingSolenoid5OnOffSetTimeAId;
		break;
	case eUpBtnSolenoid5OnOffSetTimeBStringId:
	case eDownBtnSolenoid5OnOffSetTimeBStringId:
		item = eSolenoid5OnOffsetTimeB;
		valueSettingId = eValueSettingSolenoid5OnOffSetTimeBId;
		break;
	case eUpBtnSolenoid5OpenTimeAStringId:
	case eDownBtnSolenoid5OpenTimeAStringId:
		item = eSolenoid5OpenTimeA;
		valueSettingId = eValueSettingSolenoid5OpenTimeAId;
		break;
	case eUpBtnSolenoid5OpenTimeBStringId:
	case eDownBtnSolenoid5OpenTimeBStringId:
		item = eSolenoid5OpenTimeB;
		valueSettingId = eValueSettingSolenoid5OpenTimeBId;
		break;
	case eUpBtnSolenoidOpenTimeOnEndStringId:
	case eDownBtnSolenoidOpenTimeOnEndStringId:
		item = eSolenoidOpenTimeOnEnd;
		valueSettingId = eValueSettingSolenoidOpenTimeOnEndId;
		break;
	default:
		break;
	}

	if(item != eMaxItemOfParam)
	{
		EDIT_SetText(s_valueSettingList[valueSettingId],(char*)chr);
		PSATableMgr_SetPSAParam(flowValue,conditionO2,conditionTemp,item,value);
		PSATableMgr_WritePSAParam(flowValue,conditionO2);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_CallbackUpLargeBtn					 						*/
/*                                                                                      */
/* Details  -Call back function for up large button, handle event from the window	    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_aPointUpArrowLarge : point array 				        	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_CallbackUpLargeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int8_t intBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	int8_t floatBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	double floatValue = 0;
	int intValue = 0;

	int8_t testBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	uint16_t coolingFunctionTestValue = settingButton_GetValue(eValueSettingDutyId);
	uint16_t compressorFunctionTestValue = settingButton_GetValue(eValueSettingRPMId);

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

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetColor(GUI_BLACK);
		GUI_FillPolygon(&s_aPointUpArrowLarge[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
	{
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eUpBtnCoolingStringId:
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				coolingFunctionTestValue += STEP_COOLING_FAN;
				if(coolingFunctionTestValue >= MAX_COOLING_FAN_DUTY)
				{
					coolingFunctionTestValue = MAX_COOLING_FAN_DUTY;
				}
				itoa(coolingFunctionTestValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingDutyId],(char*)intBuffer);

				if(settingOnOffButton_GetValue(eValueMotorCoolingId) == true)
				{
					guiTask_SendPendingEvent(eCoolingFanFunctiontestChangeId, settingButton_GetValue(eValueSettingDutyId));
				}
				break;
			case eUpBtnCompressorStringId:
				compressorFunctionTestValue += STEP_COMPRESSOR_RPM;
				if(compressorFunctionTestValue >= MAX_COMPRESSOR_RPM)
				{
					compressorFunctionTestValue = MAX_COMPRESSOR_RPM;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(compressorFunctionTestValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingRPMId],(char*)intBuffer);

				if(settingOnOffButton_GetValue(eValueMotorCompressorId) == true)
				{
					guiTask_SendPendingEvent(eMotorRPMFunctionTestChangeId,settingButton_GetValue(eValueSettingRPMId));
				}
				break;
			case eUpBtnRestingFRStringId:
			{
				uint8_t FRPSAValue = guiInterface_GetSettingValue(eRestingFRSettingId);

				if(FRPSAValue == SETTING_FR_300L)
				{
					break;
				}

				if(FRPSAValue < SETTING_FR_200L)
				{
					FRPSAValue += SETTING_FR_025L;
				}
				else if (FRPSAValue >= SETTING_FR_200L && FRPSAValue <= SETTING_FR_300L)
				{
					FRPSAValue += SETTING_FR_050L;
				}

				if(FRPSAValue > SETTING_FR_300L)
				{
					FRPSAValue = SETTING_FR_300L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingRestingFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eRestingFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eUpBtnExertionFRStringId:
			{
				uint8_t FRPSAValue = guiInterface_GetSettingValue(eExertionFRSettingId);

				if(FRPSAValue == SETTING_FR_300L)
				{
					break;
				}

				if(FRPSAValue < SETTING_FR_200L)
				{
					FRPSAValue += SETTING_FR_025L;
				}
				else if (FRPSAValue >= SETTING_FR_200L && FRPSAValue <= SETTING_FR_300L)
				{
					FRPSAValue += SETTING_FR_050L;
				}

				if(FRPSAValue > SETTING_FR_300L)
				{
					FRPSAValue = SETTING_FR_300L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingExertionFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eExertionFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eUpBtnBedtimeFRStringId:
			{
				uint8_t FRPSAValue = guiInterface_GetSettingValue(eBedtimeFRSettingId);

				if(FRPSAValue == SETTING_FR_300L)
				{
					break;
				}

				if(FRPSAValue < SETTING_FR_200L)
				{
					FRPSAValue += SETTING_FR_025L;
				}
				else if (FRPSAValue >= SETTING_FR_200L && FRPSAValue <= SETTING_FR_300L)
				{
					FRPSAValue += SETTING_FR_050L;
				}

				if(FRPSAValue > SETTING_FR_300L)
				{
					FRPSAValue = SETTING_FR_300L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingBedtimeFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eBedtimeFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eUpBtnLCDBrightnessStringId:
			{
				if(guiInterface_GetSettingValue(eBrightnessSettingId) == MAX_BRIGHTNESS_SETTING)
				{
					break;
				}

				uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
				brightness += STEP_BRIGHTNESS;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(brightness,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingLCDBackLightId],(char*)intBuffer);
				guiInterface_SetSettingValue(eBrightnessSettingId,brightness);
				setting_SaveAllSettingValue();

				guiTask_SendPendingEvent(eBacklightChangeId,brightness);
				break;
			}
			case eUpBtnVoiceGuidanceVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eGuidanceVolumeSettingId) == MAX_VOLUME_SETTING)
				{
					break;
				}

				uint8_t voiceGuidance = guiInterface_GetSettingValue(eGuidanceVolumeSettingId);
				voiceGuidance += STEP_VOLUME;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(voiceGuidance,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingGuidanceVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eGuidanceVolumeSettingId,voiceGuidance);
				setting_SaveAllSettingValue();

				settingButton_PlayGuidanceLevel(voiceGuidance);
				break;
			}
			case eUpBtnAlarmVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eAlarmVolumeSettingId) == MAX_VOLUME_SETTING)
				{
					break;
				}

				uint8_t voiceAlarm = guiInterface_GetSettingValue(eAlarmVolumeSettingId);
				voiceAlarm += STEP_VOLUME;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(voiceAlarm,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingAlarmVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eAlarmVolumeSettingId,voiceAlarm);
				setting_SaveAllSettingValue();

				settingButton_PlayAlarmLevel(voiceAlarm);
				break;
			}
			case eUpBtnBuzzerVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eBuzzerVolumeSettingId) == MAX_VOLUME_SETTING)
				{
					break;
				}

				uint8_t buzzerAlarm = guiInterface_GetSettingValue(eBuzzerVolumeSettingId);
				buzzerAlarm += STEP_VOLUME;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(buzzerAlarm,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingBuzzerVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eBuzzerVolumeSettingId,buzzerAlarm);
				setting_SaveAllSettingValue();

				audioMgr_Request(eBuzzer100msAudio, eNoAlarmAudio);
				break;
			}
			case eUpBtnO2ConcentrationTestStringId:
			{
				uint8_t o2Test = settingButton_GetValue(eValueO2ConcentrationTestId);
				if(o2Test == MAX_O2_TEST)
				{
					break;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				o2Test += STEP_O2_TEST;
				if(o2Test >= MAX_O2_TEST)
				{
					o2Test = MAX_O2_TEST;
				}
				itoa(o2Test,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueO2ConcentrationTestId],(char*)intBuffer);

				break;
			}
			case eUpBtnFlowRateTestStringId:
			{
				double flowRateTestUp = (double)settingButton_GetValue(eValueFlowRateTestTestId)/SCALE_FR_TEST;
				if(flowRateTestUp == MAX_FR_TEST)
				{
					break;
				}

				flowRateTestUp += STEP_FR_TEST;

				if(flowRateTestUp > MAX_FR_TEST)
				{
					flowRateTestUp = MAX_FR_TEST;
				}

				sprintf((char*)&testBuffer[0],"%.2f",flowRateTestUp);
				EDIT_SetText(s_valueSettingList[eValueFlowRateTestTestId],(char*)&testBuffer[0]);
				break;
			}
			case eUpBtnHighTempStringId:
			{
				if(guiInterface_GetSettingValue(eHightempSettingId) == MAX_HIGHTEMP_SETTING)
				{
					break;
				}

				uint8_t maxTempValue = guiInterface_GetSettingValue(eHightempSettingId);
				maxTempValue += STEP_HIGH_TEMP;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(maxTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueHightempSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eHightempSettingId,maxTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eMaxTemperatureChangeId,maxTempValue);

				break;
			}
			case eUpBtnLowTempStringId:
			{
				if(guiInterface_GetSettingValue(eLowTempSettingId) == MAX_LOWTEMP_SETTING)
				{
					break;
				}
				uint8_t lowTempValue = guiInterface_GetSettingValue(eLowTempSettingId);
				lowTempValue += STEP_LOW_TEMP;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(lowTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueLowTempSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eLowTempSettingId,lowTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eMinTemperatureChangeId,lowTempValue);

				break;
			}
			case eUpBtnO2LimitStringId:
			{
				if(guiInterface_GetSettingValue(eOxyLimitSettingId) == MAX_OXY_LIMIT_SETTING)
				{
					break;
				}
				uint8_t O2LimitTempValue = guiInterface_GetSettingValue(eOxyLimitSettingId);
				O2LimitTempValue += STEP_O2_LIMT;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(O2LimitTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueOxyLimitSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eOxyLimitSettingId,O2LimitTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eLimitOxygenConcentratorChangeId,O2LimitTempValue);

				break;
			}
			case eUpBtnFRPSAStringId:
			{
				uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);

				if(FRPSAValue == SETTING_FR_300L)
				{
					break;
				}

				if(FRPSAValue < SETTING_FR_200L)
				{
					FRPSAValue += SETTING_FR_025L;
				}
				else if (FRPSAValue >= SETTING_FR_200L && FRPSAValue <= SETTING_FR_300L)
				{
					FRPSAValue += SETTING_FR_050L;
				}

				if(FRPSAValue > SETTING_FR_300L)
				{
					FRPSAValue = SETTING_FR_300L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingFRPSAId],(char*)floatBuffer);

				guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,FRPSAValue,
						conditionO2,conditionTemp);
				break;
			}
			case eUpBtnSolenoidOpenTimeOnEndStringId:
			{
				uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);
				uint8_t solenoidOpenTimeOnEndValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd);
				if(solenoidOpenTimeOnEndValue == MAX_SOLENOID_OPEN_TIME)
				{
					break;
				}

				solenoidOpenTimeOnEndValue += STEP_SOLENOID_TIME_ONEND;

				if(solenoidOpenTimeOnEndValue >= MAX_SOLENOID_OPEN_TIME)
				{
					solenoidOpenTimeOnEndValue = MAX_SOLENOID_OPEN_TIME;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				intValue = (int)solenoidOpenTimeOnEndValue/STEP_SOLENOID_TIME_ONEND;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingSolenoidOpenTimeOnEndId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd,solenoidOpenTimeOnEndValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				guiTask_SendPendingEvent(eSolEndOpenChangeId,settingButton_GetValue(eValueSettingSolenoidOpenTimeOnEndId));

				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SendPendingSolTime					 						*/
/*                                                                                      */
/* Details  -Send sol time setting to device										    */
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int id :  - id setting												*/
/* 				   	 - range : 0-336						    						*/
/*                   - unit : NONE                                                      */
/* 				   							    										*/
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
void settingButton_SendPendingSolTime(int id)
{
	E_MsgDataId dataId = eNoDeviceEventId;
	uint16_t value = 0;
	switch (id)
	{
	case eUpBtnSolenoid1OnOffSetTimeStringId:
	case eDownBtnSolenoid1OnOffSetTimeStringId:
		dataId = eSolenoid1OnOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid1OnOffSetTimeId);
		break;
	case eUpBtnSolenoid1OffOffSetTimeStringId:
	case eDownBtnSolenoid1OffOffSetTimeStringId:
		dataId = eSolenoid1OffOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid1OffOffSetTimeId);
		break;
	case eUpBtnSolenoid2OnOffSetTimeStringId:
	case eDownBtnSolenoid2OnOffSetTimeStringId:
		dataId = eSolenoid2OnOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid2OnOffSetTimeId);
		break;
	case eUpBtnSolenoid2OffOffSetTimeStringId:
	case eDownBtnSolenoid2OffOffSetTimeStringId:
		dataId = eSolenoid2OffOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid2OffOffSetTimeId);
		break;
	case eUpBtnSolenoid3OnOffSetTimeStringId:
	case eDownBtnSolenoid3OnOffSetTimeStringId:
		dataId = eSolenoid3OnOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid3OnOffSetTimeId);
		break;
	case eUpBtnSolenoid3OffOffSetTimeStringId:
	case eDownBtnSolenoid3OffOffSetTimeStringId:
		dataId = eSolenoid3OffOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid3OffOffSetTimeId);
		break;
	case eUpBtnSolenoid4OnOffSetTimeStringId:
	case eDownBtnSolenoid4OnOffSetTimeStringId:
		dataId = eSolenoid4OnOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid4OnOffSetTimeId);
		break;
	case eUpBtnSolenoid4OffOffSetTimeStringId:
	case eDownBtnSolenoid4OffOffSetTimeStringId:
		dataId = eSolenoid4OffOffSetTimeChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid4OffOffSetTimeId);
		break;
	case eUpBtnSolenoid5OnOffSetTimeAStringId:
	case eDownBtnSolenoid5OnOffSetTimeAStringId:
		dataId = eSolenoid5OnOffSetTimeAChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid5OnOffSetTimeAId);
		break;
	case eUpBtnSolenoid5OnOffSetTimeBStringId:
	case eDownBtnSolenoid5OnOffSetTimeBStringId:
		dataId = eSolenoid5OnOffSetTimeBChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid5OnOffSetTimeBId);
		break;
	case eUpBtnSolenoid5OpenTimeAStringId:
	case eDownBtnSolenoid5OpenTimeAStringId:
		dataId = eSol5TimeAChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid5OpenTimeAId);
		break;
	case eUpBtnSolenoid5OpenTimeBStringId:
	case eDownBtnSolenoid5OpenTimeBStringId:
		dataId = eSol5TimeBChangeId;
		value = settingButton_GetValue(eValueSettingSolenoid5OpenTimeBId);
		break;
	default:
		break;
	}

	if(guiTask_GetSelectPort() == EEPROM_SELECT)
	{
		guiTask_SendPendingEvent(dataId,value);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_CallbackUpBtn					 							*/
/*                                                                                      */
/* Details  -Call back function for up button, handle event from the window				*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_aPointUpArrow : point array							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_CallbackUpBtn(WM_MESSAGE * pMsg)
{
	int8_t intBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	int8_t floatBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};

	float floatValue = 0;
	int intValue = 0;

	E_O2ConcentratorType conditionO2 = eNormalO2Type;
	E_TemperatureType conditionTemp = eHighTempType;

	int valueO2Radio = PSAParameterChangePageOneScreen_GetConditionO2();
	int valueTempRadio = PSAParameterChangePageOneScreen_GetConditionTemp();
	uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);

	if(valueO2Radio >= 0)
	{
		conditionO2 = (E_O2ConcentratorType)valueO2Radio;
	}

	if(valueTempRadio >= 0)
	{
		conditionTemp = (E_TemperatureType)valueTempRadio;
	}

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
		GUI_SetColor(GUI_BLACK);
		GUI_FillPolygon(&s_aPointUpArrow[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
	{
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eUpBtnPSAPressureStringId:
			{
				uint8_t PSAPressValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,ePSAPress);
				if(PSAPressValue == MAX_PSA_PRESSURE)
				{
					break;
				}

				PSAPressValue += STEP_PSA_PRESSURE;
				if(PSAPressValue > MAX_PSA_PRESSURE)
				{
					PSAPressValue = MAX_PSA_PRESSURE;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(PSAPressValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingPSAPressureId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,ePSAPress,PSAPressValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(ePSAPressureChangeId,settingButton_GetValue(eValueSettingPSAPressureId));
				}
				break;
			}
			case eUpBtnSolenoid1OnOffSetTimeStringId:
			case eUpBtnSolenoid1OffOffSetTimeStringId:
			case eUpBtnSolenoid2OnOffSetTimeStringId:
			case eUpBtnSolenoid2OffOffSetTimeStringId:
			case eUpBtnSolenoid3OnOffSetTimeStringId:
			case eUpBtnSolenoid3OffOffSetTimeStringId:
			case eUpBtnSolenoid4OnOffSetTimeStringId:
			case eUpBtnSolenoid4OffOffSetTimeStringId:
			case eUpBtnSolenoid5OnOffSetTimeAStringId:
			case eUpBtnSolenoid5OnOffSetTimeBStringId:
			case eUpBtnSolenoid5OpenTimeAStringId:
			case eUpBtnSolenoid5OpenTimeBStringId:
			{
				settingButton_SetItemForPSAPara(WM_GetId(pMsg->hWin));

				uint8_t SolenoidSetTimeValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,s_currentItemPSA);
				if(SolenoidSetTimeValue == MAX_LOLENOID_OFFSET_TIME)
				{
					break;
				}

				SolenoidSetTimeValue += STEP_SOLENOID_TIME;

				if(SolenoidSetTimeValue > MAX_LOLENOID_OFFSET_TIME)
				{
					SolenoidSetTimeValue = MAX_LOLENOID_OFFSET_TIME;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}

				floatValue = (double)SolenoidSetTimeValue / SCALE_SOLENOID_TIME;
				sprintf((char*)floatBuffer,"%0.1f",floatValue);

				settingButton_SaveDisplaySettingPSA(WM_GetId(pMsg->hWin),(const int8_t*)&floatBuffer[0],SolenoidSetTimeValue);
				settingButton_SendPendingSolTime(WM_GetId(pMsg->hWin));
				break;
			}
			case eUpBtnCoolingFanDutyPSAStringId:
			{
				uint8_t CoolingFanValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCoolingFanDutyCycle);
				if(CoolingFanValue == MAX_COOLING_FAN_DUTY)
				{
					break;
				}

				CoolingFanValue += STEP_COOLING_FAN;

				if(CoolingFanValue > MAX_COOLING_FAN_DUTY)
				{
					CoolingFanValue = MAX_COOLING_FAN_DUTY;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}

				intValue = (int)CoolingFanValue;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingCoolingFanDutyPSAId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eCoolingFanDutyCycle,CoolingFanValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eCoolingFanChangeId,settingButton_GetValue(eValueSettingCoolingFanDutyPSAId));
				}
				break;
			}
			case eUpBtnCompressorRPMPSAStringId:
			{
				uint8_t CompressorPSAValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCompressorRPM);

				if(CompressorPSAValue == MAX_COMPRESSOR_RPM)
				{
					break;
				}

				CompressorPSAValue += STEP_COMPRESSOR_RPM;

				if(CompressorPSAValue > MAX_COMPRESSOR_RPM)
				{
					CompressorPSAValue = MAX_COMPRESSOR_RPM;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}

				itoa(CompressorPSAValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingCompressorRPMPSAId],(char*)intBuffer);

				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eCompressorRPM,CompressorPSAValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eMotorCompressorChangeId,settingButton_GetValue(eValueSettingCompressorRPMPSAId));
				}
				break;
			}
			case eUpBtnFCOutputPSAStringId:
			{
				uint8_t FCOutputValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMFCOutput);
				if(FCOutputValue == MAX_FC_OUTPUT)
				{
					break;
				}

				FCOutputValue += STEP_FC_OUTPUT;
				if(FCOutputValue >= MAX_FC_OUTPUT)
				{
					FCOutputValue = MAX_FC_OUTPUT;
				}
				EDIT_SetText(s_valueSettingList[eValueSettingFCOutputPSAId],(char*)stringtools_ConvertFCFlowToString(FCOutputValue));
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eMFCOutput,FCOutputValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eOxygenFlowChangeId,settingButton_GetValue(eValueSettingFCOutputPSAId));
				}
				break;
			}
			case eUpBtnMaxPSATimeStringId:
			{
				uint8_t MaxPSAValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMaxPSATime);
				if(MaxPSAValue == MAX_MAX_PSA_TIME)
				{
					break;
				}

				MaxPSAValue += STEP_MAX_PSA_TIME;

				if(MaxPSAValue >= MAX_MAX_PSA_TIME)
				{
					MaxPSAValue = MAX_MAX_PSA_TIME;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				intValue = (int)MaxPSAValue;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingMaxPSATimeId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eMaxPSATime,MaxPSAValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eMaxPSATimeChangeId,settingButton_GetValue(eValueSettingMaxPSATimeId));
				}
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_CallbackDownLargeBtn					 						*/
/*                                                                                      */
/* Details  -Call back function for down large button, handle event from the window		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_aPointDownArrowLarge : point array					      	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_CallbackDownLargeBtn(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int8_t intBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	int8_t floatBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	float floatValue = 0;
	int intValue = 0;
	int8_t testBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	uint16_t coolingFunctionTestValue = settingButton_GetValue(eValueSettingDutyId);
	uint16_t compressorFunctionTestValue = settingButton_GetValue(eValueSettingRPMId);

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

	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetColor(GUI_BLACK);
		GUI_DrawRoundedRect(Rect.x0,Rect.y0,Rect.x1,Rect.y1,5);

		GUI_SetColor(GUI_BLACK);
		GUI_FillPolygon(&s_aPointDownArrowLarge[0],3,Rect.x0,Rect.y0);
		break;
	case WM_PID_STATE_CHANGED:
	{
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eDownBtnCoolingStringId:
				coolingFunctionTestValue -= STEP_COOLING_FAN;
				if(coolingFunctionTestValue <= MIN_COOLING_FAN_DUTY)
				{
					coolingFunctionTestValue = MIN_COOLING_FAN_DUTY;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(coolingFunctionTestValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingDutyId],(char*)intBuffer);

				if(settingOnOffButton_GetValue(eValueMotorCoolingId) == true)
				{
					guiTask_SendPendingEvent(eCoolingFanFunctiontestChangeId, settingButton_GetValue(eValueSettingDutyId));
				}
				break;
			case eDownBtnCompressorStringId:
				if(compressorFunctionTestValue == MIN_COMPRESSOR_RPM)
				{
					break;
				}

				compressorFunctionTestValue -= STEP_COMPRESSOR_RPM;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(compressorFunctionTestValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingRPMId],(char*)intBuffer);

				if(settingOnOffButton_GetValue(eValueMotorCompressorId) == true)
				{
					guiTask_SendPendingEvent(eMotorRPMFunctionTestChangeId,settingButton_GetValue(eValueSettingRPMId));
				}
				break;
			case eDownBtnVoiceGuidanceVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eGuidanceVolumeSettingId) == 0)
				{
					break;
				}

				uint8_t voiceGuidance = guiInterface_GetSettingValue(eGuidanceVolumeSettingId);

				if(voiceGuidance != 0)
				{
					voiceGuidance -= STEP_VOLUME;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(voiceGuidance,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingGuidanceVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eGuidanceVolumeSettingId,voiceGuidance);
				setting_SaveAllSettingValue();

				settingButton_PlayGuidanceLevel(voiceGuidance);
				break;
			}
			case eDownBtnAlarmVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eAlarmVolumeSettingId) == MIN_VOLUME_SETTING)
				{
					break;
				}

				uint8_t voiceAlarm = guiInterface_GetSettingValue(eAlarmVolumeSettingId);

				if(voiceAlarm != MIN_VOLUME_SETTING)
				{
					voiceAlarm -= STEP_VOLUME;
				}


				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(voiceAlarm,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingAlarmVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eAlarmVolumeSettingId,voiceAlarm);
				setting_SaveAllSettingValue();

				settingButton_PlayAlarmLevel(voiceAlarm);
				break;
			}
			case eDownBtnBuzzerVolumeStringId:
			{
				if(guiInterface_GetSettingValue(eBuzzerVolumeSettingId) == MIN_VOLUME_SETTING)
				{
					break;
				}

				uint8_t buzzerAlarm = guiInterface_GetSettingValue(eBuzzerVolumeSettingId);

				if(buzzerAlarm != MIN_VOLUME_SETTING)
				{
					buzzerAlarm -= STEP_VOLUME;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(buzzerAlarm,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingBuzzerVolumeId],(char*)intBuffer);
				guiInterface_SetSettingValue(eBuzzerVolumeSettingId,buzzerAlarm);
				setting_SaveAllSettingValue();

				audioMgr_Request(eBuzzer100msAudio, eNoAlarmAudio);
				break;
			}
			case eDownBtnRestingFRStringId:
			{
				uint8_t FRPSAValue = (uint8_t)guiInterface_GetSettingValue(eRestingFRSettingId);
				if(FRPSAValue == SETTING_FR_025L)
				{
					break;
				}

				if(FRPSAValue <= SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_025L;
				}
				else if(FRPSAValue <= SETTING_FR_300L && FRPSAValue > SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_050L;
				}

				if(FRPSAValue < SETTING_FR_025L)
				{
					FRPSAValue = SETTING_FR_025L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingRestingFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eRestingFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eDownBtnExertionFRStringId:
			{
				uint8_t FRPSAValue = (uint8_t)guiInterface_GetSettingValue(eExertionFRSettingId);
				if(FRPSAValue == SETTING_FR_025L)
				{
					break;
				}

				if(FRPSAValue <= SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_025L;
				}
				else if(FRPSAValue <= SETTING_FR_300L && FRPSAValue > SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_050L;
				}

				if(FRPSAValue < SETTING_FR_025L)
				{
					FRPSAValue = SETTING_FR_025L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingExertionFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eExertionFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eDownBtnBedtimeFRStringId:
			{
				uint8_t FRPSAValue = (uint8_t)guiInterface_GetSettingValue(eBedtimeFRSettingId);
				if(FRPSAValue == SETTING_FR_025L)
				{
					break;
				}

				if(FRPSAValue <= SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_025L;
				}
				else if(FRPSAValue <= SETTING_FR_300L && FRPSAValue > SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_050L;
				}

				if(FRPSAValue < SETTING_FR_025L)
				{
					FRPSAValue = SETTING_FR_025L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingBedtimeFRId],(char*)floatBuffer);

				guiInterface_SetSettingValue(eBedtimeFRSettingId,FRPSAValue);
				setting_SaveAllSettingValue();
				break;
			}
			case eDownBtnLCDBrightnessStringId:
			{
				if(guiInterface_GetSettingValue(eBrightnessSettingId) == MIN_BRIGHTNESS_SETTING)
				{
					break;
				}

				uint8_t brightness = guiInterface_GetSettingValue(eBrightnessSettingId);
				if(brightness != MIN_BRIGHTNESS_SETTING)
				{
					brightness -= STEP_BRIGHTNESS;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(brightness,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingLCDBackLightId],(char*)intBuffer);
				guiInterface_SetSettingValue(eBrightnessSettingId,brightness);
				setting_SaveAllSettingValue();

				guiTask_SendPendingEvent(eBacklightChangeId,(uint16_t)guiInterface_GetSettingValue(eBrightnessSettingId));
				break;
			}
			case eDownBtnO2ConcentrationTestStringId:
			{
				uint8_t o2Test = settingButton_GetValue(eValueO2ConcentrationTestId);
				if(o2Test == 0)
				{
					break;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				o2Test -= STEP_O2_TEST;
				if(o2Test <= 0)
				{
					o2Test = 0;
				}
				itoa(o2Test,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueO2ConcentrationTestId],(char*)intBuffer);
				break;
			}
			case eDownBtnFlowRateTestStringId:
			{
				double flowRateTestDown = (double)settingButton_GetValue(eValueFlowRateTestTestId) / SCALE_FR_TEST;
				if(flowRateTestDown == 0)
				{
					break;
				}

				flowRateTestDown -= STEP_FR_TEST;

				if(flowRateTestDown <= 0)
				{
					flowRateTestDown = 0;
				}

				sprintf((char*)&testBuffer[0],"%.2f",flowRateTestDown);
				EDIT_SetText(s_valueSettingList[eValueFlowRateTestTestId],(char*)&testBuffer[0]);
				break;
			}
			case eDownBtnHighTempStringId:
			{
				if(guiInterface_GetSettingValue(eHightempSettingId) == MIN_HIGHTEMP_SETTING)
				{
					break;
				}

				uint8_t maxTempValue = guiInterface_GetSettingValue(eHightempSettingId);
				maxTempValue -= STEP_HIGH_TEMP;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(maxTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueHightempSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eHightempSettingId,maxTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eMaxTemperatureChangeId,maxTempValue);

				break;
			}
			case eDownBtnLowTempStringId:
			{
				if(guiInterface_GetSettingValue(eLowTempSettingId) == 0)
				{
					break;
				}

				uint8_t lowTempValue = guiInterface_GetSettingValue(eLowTempSettingId);
				lowTempValue -= STEP_LOW_TEMP;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(lowTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueLowTempSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eLowTempSettingId,lowTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eMinTemperatureChangeId,lowTempValue);

				break;
			}
			case eDownBtnO2LimitStringId:
			{
				uint8_t O2LimitTempValue = guiInterface_GetSettingValue(eOxyLimitSettingId);

				if(O2LimitTempValue == MIN_OXY_LIMIT_SETTING)
				{
					break;
				}

				O2LimitTempValue -= STEP_O2_LIMT;
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(O2LimitTempValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueOxyLimitSettingId],(char*)intBuffer);
				guiInterface_SetSettingValue(eOxyLimitSettingId,O2LimitTempValue);
				setting_SaveAllSettingValue();
				guiTask_SendPendingEvent(eLimitOxygenConcentratorChangeId,O2LimitTempValue);

				break;
			}
			case eDownBtnFRPSAStringId:
			{
				uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);
				if(FRPSAValue == SETTING_FR_025L)
				{
					break;
				}

				if(FRPSAValue <= SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_025L;
				}
				else if(FRPSAValue <= SETTING_FR_300L && FRPSAValue > SETTING_FR_200L)
				{
					FRPSAValue = FRPSAValue - SETTING_FR_050L;
				}

				if(FRPSAValue < SETTING_FR_025L)
				{
					FRPSAValue = SETTING_FR_025L;
				}

				floatValue = (double)FRPSAValue/FLOWRATE_SCALE;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				sprintf((char*)floatBuffer,"%0.2f",floatValue);
				EDIT_SetText(s_valueSettingList[eValueSettingFRPSAId],(char*)floatBuffer);

				guiTask_SendPSAParaEvent(ePacketPSAParaChangeId,FRPSAValue,
						conditionO2,conditionTemp);
				break;
			}
			case eDownBtnSolenoidOpenTimeOnEndStringId:
			{
				uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);
				uint8_t solenoidOpenTimeOnEndValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd);
				if(solenoidOpenTimeOnEndValue == 0)
				{
					break;
				}

				solenoidOpenTimeOnEndValue -= STEP_SOLENOID_TIME_ONEND;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				intValue = (int)solenoidOpenTimeOnEndValue/STEP_SOLENOID_TIME_ONEND;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingSolenoidOpenTimeOnEndId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eSolenoidOpenTimeOnEnd,solenoidOpenTimeOnEndValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				guiTask_SendPendingEvent(eSolEndOpenChangeId,settingButton_GetValue(eValueSettingSolenoidOpenTimeOnEndId));
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_CallbackDownBtn					 							*/
/*                                                                                      */
/* Details  -Call back function for down button, handle event from the window			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window								*/
/* 				   							    										*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_aPointDownArrow : point array							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_CallbackDownBtn(WM_MESSAGE * pMsg)
{
	int8_t intBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	int8_t floatBuffer[SIZE_BUFFER_OF_VALUE] = {'\0'};
	double floatValue = 0;
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

	uint8_t FRPSAValue = (uint8_t)(settingButton_GetValue(eValueSettingFRPSAId) * FC_OUTPUT_SCALE_SETTING);

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

		GUI_SetColor(GUI_BLACK);

		GUI_FillPolygon(&s_aPointDownArrow[0],3,Rect.x0,Rect.y0);

		break;
	case WM_PID_STATE_CHANGED:
	{
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			switch(WM_GetId(pMsg->hWin))
			{
			case eDownBtnPSAPressureStringId:
			{
				uint8_t PSAPressValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,ePSAPress);
				if(PSAPressValue == MIN_PSA_PRESSURE)
				{
					break;
				}

				PSAPressValue -= STEP_PSA_PRESSURE;
				if(PSAPressValue <= 0)
				{
					PSAPressValue = 0;
				}
				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				itoa(PSAPressValue,(char*)intBuffer,10);
				EDIT_SetText(s_valueSettingList[eValueSettingPSAPressureId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,ePSAPress,PSAPressValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(ePSAPressureChangeId,settingButton_GetValue(eValueSettingPSAPressureId));
				}
				break;
			}
			case eDownBtnSolenoid1OnOffSetTimeStringId:
			case eDownBtnSolenoid1OffOffSetTimeStringId:
			case eDownBtnSolenoid2OnOffSetTimeStringId:
			case eDownBtnSolenoid2OffOffSetTimeStringId:
			case eDownBtnSolenoid3OnOffSetTimeStringId:
			case eDownBtnSolenoid3OffOffSetTimeStringId:
			case eDownBtnSolenoid4OnOffSetTimeStringId:
			case eDownBtnSolenoid4OffOffSetTimeStringId:
			case eDownBtnSolenoid5OnOffSetTimeAStringId:
			case eDownBtnSolenoid5OnOffSetTimeBStringId:
			case eDownBtnSolenoid5OpenTimeAStringId:
			case eDownBtnSolenoid5OpenTimeBStringId:
			{
				settingButton_SetItemForPSAPara(WM_GetId(pMsg->hWin));

				uint8_t SolenoidSetTimeValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,s_currentItemPSA);
				if(SolenoidSetTimeValue == 0)
				{
					break;
				}

				SolenoidSetTimeValue -= STEP_SOLENOID_TIME;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					floatBuffer[i] = '\0';
				}
				floatValue = (double)SolenoidSetTimeValue / SCALE_SOLENOID_TIME;
				sprintf((char*)floatBuffer,"%0.1f",floatValue);

				settingButton_SaveDisplaySettingPSA(WM_GetId(pMsg->hWin),(const int8_t*)&floatBuffer[0],SolenoidSetTimeValue);
				settingButton_SendPendingSolTime(WM_GetId(pMsg->hWin));

				break;
			}
			case eDownBtnCoolingFanDutyPSAStringId:
			{
				uint8_t CoolingFanValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCoolingFanDutyCycle);
				if(CoolingFanValue == MIN_COOLING_FAN_DUTY)
				{
					break;
				}

				CoolingFanValue -= STEP_COOLING_FAN;

				if(CoolingFanValue <= MIN_COOLING_FAN_DUTY)
				{
					CoolingFanValue = MIN_COOLING_FAN_DUTY;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				intValue = (int)CoolingFanValue;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingCoolingFanDutyPSAId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eCoolingFanDutyCycle,CoolingFanValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eCoolingFanChangeId,settingButton_GetValue(eValueSettingCoolingFanDutyPSAId));
				}
				break;
			}
			case eDownBtnCompressorRPMPSAStringId:
			{
				uint8_t CompressorPSAValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eCompressorRPM);

				if(CompressorPSAValue == MIN_COMPRESSOR_RPM)
				{
					break;
				}

				CompressorPSAValue -= STEP_COMPRESSOR_RPM;

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}

				itoa(CompressorPSAValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingCompressorRPMPSAId],(char*)intBuffer);

				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eCompressorRPM,CompressorPSAValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eMotorCompressorChangeId,settingButton_GetValue(eValueSettingCompressorRPMPSAId));
				}
				break;
			}
			case eDownBtnFCOutputPSAStringId:
			{
				uint8_t FCOutputValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMFCOutput);
				if(FCOutputValue == MIN_FC_OUTPUT)
				{
					break;
				}

				FCOutputValue -= STEP_FC_OUTPUT;
				if(FCOutputValue <= MIN_FC_OUTPUT)
				{
					FCOutputValue = MIN_FC_OUTPUT;
				}
				EDIT_SetText(s_valueSettingList[eValueSettingFCOutputPSAId],(char*)stringtools_ConvertFCFlowToString(FCOutputValue));
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eMFCOutput,FCOutputValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);

				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eOxygenFlowChangeId,settingButton_GetValue(eValueSettingFCOutputPSAId));
				}
				break;
			}
			case eDownBtnMaxPSATimeStringId:
			{
				uint8_t MaxPSAValue = PSATableMgr_GetPSAItem(FRPSAValue,conditionO2,conditionTemp,eMaxPSATime);
				if(MaxPSAValue == MIN_PSA_TIME)
				{
					break;
				}
				MaxPSAValue -= STEP_MAX_PSA_TIME;

				if(MaxPSAValue <= MIN_PSA_TIME)
				{
					MaxPSAValue = MIN_PSA_TIME;
				}

				for(int i = 0; i < SIZE_BUFFER_OF_VALUE; i++)
				{
					intBuffer[i] = '\0';
				}
				intValue = (int)MaxPSAValue;
				itoa(intValue,(char*)&intBuffer[0],10);
				EDIT_SetText(s_valueSettingList[eValueSettingMaxPSATimeId],(char*)intBuffer);
				PSATableMgr_SetPSAParam(FRPSAValue,conditionO2,conditionTemp,eMaxPSATime,MaxPSAValue);
				PSATableMgr_WritePSAParam(FRPSAValue,conditionO2);
				if(guiTask_GetSelectPort() == EEPROM_SELECT)
				{
					guiTask_SendPendingEvent(eMaxPSATimeChangeId,settingButton_GetValue(eValueSettingMaxPSATimeId));
				}
				break;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		BUTTON_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SetNameSetting					 							*/
/*                                                                                      */
/* Details  -Set name setting                                                 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
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
void settingButton_SetNameSetting(EDIT_Handle obj, GUI_RECT Rect)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		const int8_t* nameStr = (int8_t*)textLibrary_GetString((E_StringID)WM_GetId(obj));
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt((char*)nameStr,Rect.x0,Rect.y0 + 2);
	}
	else
	{
		switch(WM_GetId(obj))
		{
		case eGuidanceVolumeStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("ガイド音量",Rect.x0,Rect.y0 + 2);
			break;
		case eAlarmVolumeStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("アラーム音量",Rect.x0,Rect.y0 + 2);
			break;
		case eBuzzerVolumeStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("ブザー音量",Rect.x0,Rect.y0 + 2);
			break;
		case eRestingFRStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("安静時流量",Rect.x0,Rect.y0 + 2);
			break;
		case eExertionFRStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("労作時流量",Rect.x0,Rect.y0 + 2);
			break;
		case eBedtimeFRStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("就寝時流量",Rect.x0,Rect.y0 + 2);
			break;
		case eLCDBrightnessStringId:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("LCD 輝度",Rect.x0,Rect.y0 + 2);
			break;
		case eHighTemperatureStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("パラメータ変更条件 高温",Rect.x0,Rect.y0 + 2);
			break;
		case eLowTemperatureStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("パラメータ変更条件 低温",Rect.x0,Rect.y0 + 2);
			break;
		case eLimitOxygenConcentratorStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("パラメータ変更条件 濃度低下",Rect.x0,Rect.y0 + 2);
			break;
		case eFlowRatePSAStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("設定流量",Rect.x0,Rect.y0 + 2);
			break;
		case ePSAPressureStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("PSA 圧力",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid1OnOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド1 オン オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid1OffOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド1 オフ オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid2OnOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド2 オン オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid2OffOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド2 オフ オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid3OnOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド3 オン オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid3OffOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド3 オフ オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid4OnOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド4 オン オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid4OffOffSetTimeStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド4 オフ オフセット時間",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid5OnOffSetTimeAStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド5 オン オフセット時間A",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid5OnOffSetTimeBStringID:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド5 オン オフセット時間B",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid5OpenTimeAStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド5 オープン時間A",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoid5OpenTimeBStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("ソレノイド5 オープン時間B",Rect.x0,Rect.y0 + 2);
			break;
		case eCompressorRPMPSAStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("コンプレッサ回転数",Rect.x0,Rect.y0 + 2);
			break;
		case eFCOutputPSAStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("フローコントローラ出力",Rect.x0,Rect.y0 + 2);
			break;
		case eCoolingFanDutyPSAStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("冷却ファンデューティ",Rect.x0,Rect.y0 + 2);
			break;
		case eMaxPSATimeStringId:
			guiTask_SetFont(&GUI_FontJapan3L10B2PP);
			GUI_DispStringAt("MAX PSA 時間",Rect.x0,Rect.y0 + 2);
			break;
		case eMotorRPMFunctionTestStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("設定回転数",Rect.x0,Rect.y0 + 2);
			break;
		case eCoolingFanFunctiontestStringID:
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("設定デューティ",Rect.x0,Rect.y0 + 2);
			break;
		case eSolenoidOpenTimeOnEndStringId://TimeAOnEnd
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("ソレノイドオープン時間(停止)",Rect.x0,Rect.y0 + 2);
			break;
		case eFlowRateTestStringID:
			break;
		case eO2ConcentrationTestStringID:
			break;
		default:
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SetUintSetting					 							*/
/*                                                                                      */
/* Details  -Set uint setting                                                 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
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
void settingButton_SetUintSetting(EDIT_Handle obj, GUI_RECT Rect)
{
	GUI_SetColor(GUI_BLACK);
	GUI_SetTextAlign(GUI_TA_LEFT);
	switch(WM_GetId(obj))
	{
	case eO2ConcentrationTestStringID:
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt(textLibrary_GetString(eUnitPercentStringId),200,Rect.y1 - 27);
		break;
	case eCoolingFanDutyPSAStringId:
	case eCompressorRPMPSAStringId:
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt(textLibrary_GetString(eUnitPercentStringId),160,Rect.y1 - 27);
		break;
	case eCoolingFanFunctiontestStringID:
	case eMotorRPMFunctionTestStringID:
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt(textLibrary_GetString(eUnitPercentStringId),200,Rect.y1 - 29);
		break;
	case eHighTemperatureStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("℃",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("℃超過",200,Rect.y1 - 27);
		}
		break;
	case eLowTemperatureStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("℃",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("℃未満",200,Rect.y1 - 27);
		}
		break;
	case eLimitOxygenConcentratorStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt(textLibrary_GetString(eUnitLimitO2StringId),200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("％未満",200,Rect.y1 - 27);
		}

		break;
	case eFlowRatePSAStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("L/min",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("L/分",200,Rect.y1 - 27);
		}

		break;
	case ePSAPressureStringID:
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt("kPa",160,Rect.y1 - 27);

		break;
	case eSolenoid1OnOffSetTimeStringID:
	case eSolenoid1OffOffSetTimeStringID:
	case eSolenoid2OnOffSetTimeStringID:
	case eSolenoid2OffOffSetTimeStringID:
	case eSolenoid3OnOffSetTimeStringID:
	case eSolenoid3OffOffSetTimeStringID:
	case eSolenoid4OnOffSetTimeStringID:
	case eSolenoid4OffOffSetTimeStringID:
	case eSolenoid5OnOffSetTimeAStringID:
	case eSolenoid5OnOffSetTimeBStringID:
	case eSolenoid5OpenTimeAStringId:
	case eSolenoid5OpenTimeBStringId:
	case eMaxPSATimeStringId:
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt("sec",160,Rect.y1 - 27);
		break;
	case eSolenoidOpenTimeOnEndStringId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("sec",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("sec",200,Rect.y1 - 27);
		}
		break;
	case eFlowRateTestStringID:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("L/min",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("L/分",200,Rect.y1 - 27);
		}
		break;
	case eFCOutputPSAStringId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("L/min",160,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("L/分",160,Rect.y1 - 27);
		}
		break;
	case eRestingFRStringId:
	case eExertionFRStringId:
	case eBedtimeFRStringId:
		if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
		{
			guiTask_SetFont(&GUI_FontEng3L10B2PP);
			GUI_DispStringAt("L/min",200,Rect.y1 - 27);
		}
		else
		{
			guiTask_SetFont(&GUI_FontJapan3L12B2PP);
			GUI_DispStringAt("L/分",200,Rect.y1 - 27);
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_InitSave					 									*/
/*                                                                                      */
/* Details  -Initialize setting button save to EPPROM                                   */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int downBtnId : - id of down button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueSettingBtnId posArr : - id of edit value 						*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int settingId : id of setting button									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_InitSave(EDIT_Handle obj, int upBtnId,int downBtnId,
		E_ValueSettingBtnId posArr,int valueId,int settingId ,GUI_RECT Rect)
{
	int8_t buffer[SIZE_BUFFER_INIT_VALUE];
	itoa(guiInterface_GetSettingValue(settingId),(char*)buffer,10);

	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingButton_CallbackUpBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 40,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingButton_CallbackDownBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 80,Rect.y0 + 20,100,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);

	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	EDIT_SetText(s_valueSettingList[posArr],(char*)buffer);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_InitSaveLargeBtn					 							*/
/*                                                                                      */
/* Details  -Initialize setting button save to EPPROM with large button                 */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int downBtnId : - id of down button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueSettingBtnId posArr : - id of edit value 						*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int settingId : id of setting button									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_InitSaveLargeBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,int valueId,
		int settingId,GUI_RECT Rect)
{
	int8_t buffer[SIZE_BUFFER_INIT_VALUE];
	itoa(guiInterface_GetSettingValue(settingId),(char*)buffer,10);

	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingButton_CallbackUpLargeBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingButton_CallbackDownLargeBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,100,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);

	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	EDIT_SetText(s_valueSettingList[posArr],(char*)buffer);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_InitNoSave					 								*/
/*                                                                                      */
/* Details  -Initialize setting button not save to EPPROM                               */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int downBtnId : - id of down button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueSettingBtnId posArr : - id of edit value 						*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int settingId : id of setting button									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_InitNoSave(EDIT_Handle obj, int upBtnId,int downBtnId,
		E_ValueSettingBtnId posArr,int valueId,GUI_RECT Rect)
{
	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingButton_CallbackUpBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 40,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingButton_CallbackDownBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 80,Rect.y0 + 20,100,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_InitNoSaveSmallBtn					 						*/
/*                                                                                      */
/* Details  -Initialize setting small button not save to EPPROM                         */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int downBtnId : - id of down button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueSettingBtnId posArr : - id of edit value 						*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int settingId : id of setting button									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_InitNoSaveSmallBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,
		int valueId,GUI_RECT Rect)
{
	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingButton_CallbackUpBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 40,Rect.y0 + 20,40,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingButton_CallbackDownBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 80,Rect.y0 + 20,80,40,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_InitNoSaveLargeBtn					 						*/
/*                                                                                      */
/* Details  -Initialize setting large button not save to EPPROM                         */
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int upBtnId : - id of up button 										*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int downBtnId : - id of down button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueSettingBtnId posArr : - id of edit value 						*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : - id of setting button 								*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int settingId : id of setting button									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   GUI_RECT Rect : rectangle of object setting					    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_upBtn : up button                                          */
/*                     (O) s_downBtn : down button						                */
/*                     (O) s_valueSettingList : setting list						    */
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_InitNoSaveLargeBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,
		int valueId,GUI_RECT Rect)
{
	s_upBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,upBtnId);
	WM_SetCallback(s_upBtn,settingButton_CallbackUpLargeBtn);

	s_downBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,downBtnId);
	WM_SetCallback(s_downBtn,settingButton_CallbackDownLargeBtn);

	s_valueSettingList[posArr] = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,100,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_valueSettingList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_valueSettingList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_valueSettingList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_SetValue					 									*/
/*                                                                                      */
/* Details  -set value and display for setting button                             		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int valueId : id of setting button 									*/
/*                     - range : 0-35                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   const int8_t * value : value display									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_valueSettingList : setting list                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingButton_SetValue(E_ValueSettingBtnId valueEdit,const int8_t * value)
{
	EDIT_SetText(s_valueSettingList[valueEdit],(char*)value);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_GetValue					 									*/
/*                                                                                      */
/* Details  -Get value for setting button by id                            				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int valueId : id of setting button 									*/
/*                     - range : 0-35                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_valueSettingList : setting list					     	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint16_t settingButton_GetValue(E_ValueSettingBtnId valueEdit)
{
	int8_t s_bufferSettingValue[SIZE_BUFFER_OF_VALUE] = {'\0'};
	uint16_t value = 0;
	EDIT_GetText(s_valueSettingList[valueEdit],(char*)&s_bufferSettingValue[0],10);
	double temp = atof((char*)s_bufferSettingValue) * SCALE_SETTING_100;
	double tempSolenoid = atof((char*)s_bufferSettingValue) * SCALE_SETTING_10;
	switch(valueEdit)
	{
	case eValueSettingFCOutputPSAId:
	case eValueFlowRateTestTestId:
	case eValueSettingFRPSAId:
		value = (uint16_t)(temp + TYPE_CONVERSION_STEP);
		break;
	case eValueSettingSolenoid1OnOffSetTimeId:
	case eValueSettingSolenoid1OffOffSetTimeId:
	case eValueSettingSolenoid2OnOffSetTimeId:
	case eValueSettingSolenoid2OffOffSetTimeId:
	case eValueSettingSolenoid3OnOffSetTimeId:
	case eValueSettingSolenoid3OffOffSetTimeId:
	case eValueSettingSolenoid4OnOffSetTimeId:
	case eValueSettingSolenoid4OffOffSetTimeId:
	case eValueSettingSolenoid5OnOffSetTimeAId:
	case eValueSettingSolenoid5OnOffSetTimeBId:
	case eValueSettingSolenoid5OpenTimeAId:
	case eValueSettingSolenoid5OpenTimeBId:
	case eValueSettingSolenoidOpenTimeOnEndId:
		value = (uint16_t)(tempSolenoid + TYPE_CONVERSION_STEP);
		break;
	default:
		value = atoi((char*)s_bufferSettingValue);
		break;
	}
	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingButton_Callback								 						*/
/*                                                                                      */
/* Details  -Call back function for setting button, handle from the window              */
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
void settingButton_Callback(WM_MESSAGE * pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		settingButton_SetNameSetting(pMsg->hWin,Rect);
		settingButton_SetUintSetting(pMsg->hWin,Rect);
		break;
	default:
		EDIT_Callback(pMsg);
		break;
	}

	return;
}

/* end of files */
