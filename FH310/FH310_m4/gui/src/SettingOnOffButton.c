/********************************************************************/
/*                                                                  */
/* File Name    : SettingOnOffButton.c                            	*/
/*                                                                  */
/* General      : The SettingOnOffButton category provides          */
/* 				  the user an interface to display and              */
/*                change setting on/off in system             		*/
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
/* #0001       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode: use the new event 				*/
/* #0002       2017/09/05       Viet Le             modify			*/
/*     remove #include "assert.h"                                   */
/* #0003       2017/10/27	   Viet Le				modify			*/
/*	-Edit function : EDIT_SetFont()->guiTaskEDIT_SetFont()(bug 1997)*/
/********************************************************************/

#include "stdlib.h"
#include "string.h"
#include "SettingOnOffButton.h"
#include "BUTTON.h"
#include "board.h"
#include "EDIT.h"
#include "TextLibrary.h"
#include "TaskCommon.h"
#include "AudioMgr.h"
#include "GuiTask.h"
#include "setting.h"
#include "ModeMgr.h"
#include "Font.h"
#include "MainScreen.h"
#include "SettingButton.h"
#include "GuiHandler.h"
#include "RealTimeClock.h"
#include "BluetoothTask.h"
#include "GuiInterface.h"

//Define item id
enum{
	eOffCompressorTimerId,
	eOffValve1TimerId,
	eOffValve2TimerId,
	eOffValve3TimerId,
	eOffValve4TimerId,
	eOffValve5TimerId
};

static BUTTON_Handle s_onBtn;
static BUTTON_Handle s_offBtn;
static EDIT_Handle s_ValueList[eValueMaxOnOffId];

static int8_t s_bufferOnOffValue[SIZE_BUFFER_ONOFF];
static E_ValueOnOffId s_currentValueOnOffid = eValueValve1Id;

static WM_HTIMER s_timerOffCompressor;
static WM_HTIMER s_timerOffValve1;
static WM_HTIMER s_timerOffValve2;
static WM_HTIMER s_timerOffValve3;
static WM_HTIMER s_timerOffValve4;
static WM_HTIMER s_timerOffValve5;

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_SetValueIdAndIpcRequest					 				*/
/*                                                                                      */
/* Details  -Set ipc request, id of value send to device		 						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int id : id of setting button										*/
/*					- range : 0-336														*/
/*                  - unit : NONE                                                       */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_currentValueOnOffid : value id                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_SetValueIdAndIpcRequest(int id)
{
	switch(id)
	{
	case eOnBtnValve1StringId:
	case eOffBtnValve1StringId:
		s_currentValueOnOffid = eValueValve1Id;
		break;
	case eOnBtnValve2StringId:
	case eOffBtnValve2StringId:
		s_currentValueOnOffid = eValueValve2Id;
		break;
	case eOnBtnValve3StringId:
	case eOffBtnValve3StringId:
		s_currentValueOnOffid = eValueValve3Id;
		break;
	case eOnBtnValve4StringId:
	case eOffBtnValve4StringId:
		s_currentValueOnOffid = eValueValve4Id;
		break;
	case eOnBtnValve5StringId:
	case eOffBtnValve5StringId:
		s_currentValueOnOffid = eValueValve5Id;
		break;
	case eOnBtnMotorCoolingStringId:
	case eOffBtnMotorCoolingStringId:
		s_currentValueOnOffid = eValueMotorCoolingId;
		break;
	case eOnBtnMotorFilterStringId:
	case eOffBtnMotorFilterStringId:
		s_currentValueOnOffid = eValueMotorFilterId;
		break;
	case eOnBtnMotorCompressorStringId:
	case eOffBtnMotorCompressorStringId:
		s_currentValueOnOffid = eValueMotorCompressorId;
		break;
	case eOnBtnBuzzerStringId:
	case eOffBtnBuzzerStringId:
		s_currentValueOnOffid = eValueBuzzerId;
		break;
	case eOnBtnAlarmRedLEDStringId:
	case eOffBtnAlarmRedLEDStringId:
		s_currentValueOnOffid = eValueRedLedId;
		break;
	case eOnBtnAlarmYellowLEDStringId:
	case eOffBtnAlarmYellowLEDStringId:
		s_currentValueOnOffid = eValueYellowLedId;
		break;
	case eOnBtnPowerLEDStringId:
	case eOffBtnPowerLEDStringId:
		s_currentValueOnOffid = eValuePowerLedId;
		break;
	case eOnBtnVoiceStringId:
	case eOffBtnVoiceStringId:
		s_currentValueOnOffid = eValueVoiceId;
		break;
	case eOnBtnChildLockStringId:
	case eOffBtnChildLockStringId:
		s_currentValueOnOffid = eValueChildLockId;
		break;
	case eOnBtnRTCStringId:
	case eOffBtnRTCStringId:
		s_currentValueOnOffid = eValueRTCId;
		break;
	case eOnBtnBluetoothStringId:
	case eOffBtnBluetoothStringId:
		s_currentValueOnOffid = eValueBluetoothId;
		break;
	case eOnBtnBluetoothTransferStringId:
	case eOffBtnBluetoothTransferStringId:
		s_currentValueOnOffid = eValueBluetoothTransferId;
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_SendPendingEvent					 					*/
/*                                                                                      */
/* Details  -Send event to M0									 						*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int id : id of setting button										*/
/*					- range : 0-336														*/
/*                  - unit : NONE                                                       */
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
void settingOnOffButton_SendPendingEvent(int id)
{
	E_MsgDataId dataId = eNoDeviceEventId;
	uint16_t value = 0;

	switch(id)
	{
	case eOnBtnValve1StringId:
	case eOffBtnValve1StringId:
		dataId = eTurnValve1ChangeId;
		value = settingOnOffButton_GetValue(eValueValve1Id);
		break;
	case eOnBtnValve2StringId:
	case eOffBtnValve2StringId:
		dataId = eTurnValve2ChangeId;
		value = settingOnOffButton_GetValue(eValueValve2Id);
		break;
	case eOnBtnValve3StringId:
	case eOffBtnValve3StringId:
		dataId = eTurnValve3ChangeId;
		value = settingOnOffButton_GetValue(eValueValve3Id);
		break;
	case eOnBtnValve4StringId:
	case eOffBtnValve4StringId:
		dataId = eTurnValve4ChangeId;
		value = settingOnOffButton_GetValue(eValueValve4Id);
		break;
	case eOnBtnValve5StringId:
	case eOffBtnValve5StringId:
		dataId = eTurnValve5ChangeId;
		value = settingOnOffButton_GetValue(eValueValve5Id);
		break;
	case eOnBtnMotorCoolingStringId:
	case eOffBtnMotorCoolingStringId:
		dataId = eTurnMotorCoolingFANChangeId;
		value = settingOnOffButton_GetValue(eValueMotorCoolingId);
		break;
	case eOnBtnMotorFilterStringId:
	case eOffBtnMotorFilterStringId:
		dataId = eTurnMotorFilterCleaningChangeId;
		value = settingOnOffButton_GetValue(eValueMotorFilterId);
		break;
	case eOnBtnMotorCompressorStringId:
	case eOffBtnMotorCompressorStringId:
		dataId = eTurnMotorCompressorChangeId;
		value = settingOnOffButton_GetValue(eValueMotorCompressorId);
		break;
	case eOnBtnAlarmRedLEDStringId:
	case eOffBtnAlarmRedLEDStringId:
		dataId = eTurnAlarmRedLEDChangeId;
		value = settingOnOffButton_GetValue(eValueRedLedId);
		break;
	case eOnBtnAlarmYellowLEDStringId:
	case eOffBtnAlarmYellowLEDStringId:
		dataId = eTurnAlarmYellowLEDChangeId;
		value = settingOnOffButton_GetValue(eValueYellowLedId);
		break;
	case eOnBtnPowerLEDStringId:
	case eOffBtnPowerLEDStringId:
		dataId = eTurnAlarmPowerLEDChangeId;
		value = settingOnOffButton_GetValue(eValuePowerLedId);
		break;
	default:
		break;
	}

	guiTask_SendPendingEvent(dataId,value);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_CallbackOnLargeBtn							 			*/
/*                                                                                      */
/* Details  -Call back function for on large button, handle from the window       		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window 								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentValueOnOffid : value id							    */
/*                     (O) s_ValueList : value list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_CallbackOnLargeBtn(WM_MESSAGE *pMsg)
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
		guiTask_SetFont(&GUI_FontEng3L12B2PP);
		GUI_DispStringAt("ON",Rect.x0+8 ,Rect.y0+16);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			settingOnOffButton_SetValueIdAndIpcRequest(WM_GetId(pMsg->hWin));
			if(settingOnOffButton_GetValue(s_currentValueOnOffid) == eOff)
			{
				EDIT_SetText(s_ValueList[s_currentValueOnOffid], "ON");
				if(s_currentValueOnOffid == eValueVoiceId)
				{
					audioMgr_Request(eTestAudio, eNoAlarmAudio);
				}
				else if (s_currentValueOnOffid == eValueBuzzerId)
				{
					audioMgr_Request(eBuzzer3sAudio, eNoAlarmAudio);
				}
				else if (s_currentValueOnOffid == eValueChildLockId)
				{
					guiInterface_SetSettingValue(eChildLockSettingId,eOn);
					setting_SaveAllSettingValue();
				}
				else if(s_currentValueOnOffid == eValueRTCId)
				{
					realtimeclock_StartTest();
					guihandler_StartTimerCheckRTC();
				}
				else if (s_currentValueOnOffid == eValueBluetoothId) {
					BTTask_SendEvent(eOnBluetoothId);
				}
				else if (s_currentValueOnOffid == eValueBluetoothTransferId)
				{
					LOG_REQUEST_t request;
					request.id = eCopyLogToEXTRamid;
					logInterface_SendLogRequestEvent(request);
				}
				else
				{
					settingOnOffButton_SendPendingEvent(WM_GetId(pMsg->hWin));

					/**********Sub switch*************/
					switch(s_currentValueOnOffid)
					{
					case eValueMotorCompressorId:
						guiTask_SendPendingEvent(eMotorRPMFunctionTestChangeId,settingButton_GetValue(eValueSettingRPMId));
						WM_RestartTimer(s_timerOffCompressor, TIMER_CLOSE_COMPRESSOR);
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve1Id], "ON");
						guiTask_SendPendingEvent(eTurnValve1ChangeId, settingOnOffButton_GetValue(eValueValve1Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve2Id], "ON");
						guiTask_SendPendingEvent(eTurnValve2ChangeId, settingOnOffButton_GetValue(eValueValve2Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve3Id], "ON");
						guiTask_SendPendingEvent(eTurnValve3ChangeId, settingOnOffButton_GetValue(eValueValve3Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve4Id], "ON");
						guiTask_SendPendingEvent(eTurnValve4ChangeId, settingOnOffButton_GetValue(eValueValve4Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve5Id], "ON");
						guiTask_SendPendingEvent(eTurnValve5ChangeId, settingOnOffButton_GetValue(eValueValve5Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueMotorCoolingId], "ON");

						guiTask_SendPendingEvent(eTurnMotorCoolingFANChangeId,settingOnOffButton_GetValue(eValueMotorCoolingId));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						guiTask_SendPendingEvent(eCoolingFanFunctiontestChangeId,settingButton_GetValue(eValueSettingDutyId));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);
						break;
					case eValueValve1Id:
						WM_RestartTimer(s_timerOffValve1, TIMER_CLOSE_VALVE);
						break;
					case eValueValve2Id:
						WM_RestartTimer(s_timerOffValve2, TIMER_CLOSE_VALVE);
						break;
					case eValueValve3Id:
						WM_RestartTimer(s_timerOffValve3, TIMER_CLOSE_VALVE);
						break;
					case eValueValve4Id:
						WM_RestartTimer(s_timerOffValve4, TIMER_CLOSE_VALVE);
						break;
					case eValueValve5Id:
						WM_RestartTimer(s_timerOffValve5, TIMER_CLOSE_VALVE);
						break;
					case eValueMotorCoolingId:
						guiTask_SendPendingEvent(eCoolingFanFunctiontestChangeId,settingButton_GetValue(eValueSettingDutyId));
						break;
					default:
						break;
					}
					/**********Sub switch*************/
				}
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
/* General 	-settingOnOffButton_CallbackOffLargeBtn								 		*/
/*                                                                                      */
/* Details  -Call back function for off button, handle from the window       			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) WM_MESSAGE * pMsg : message from window 								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_currentValueOnOffid : value id							    */
/*                     (O) s_ValueList : value list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_CallbackOffLargeBtn(WM_MESSAGE *pMsg)
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
		guiTask_SetFont(&GUI_FontEng3L12B2PP);
		GUI_DispStringAt("OFF",Rect.x0 + 8 - 2 ,Rect.y0 + 16);
		break;
	case WM_PID_STATE_CHANGED:
		if(((WM_PID_STATE_CHANGED_INFO *)(pMsg->Data.p))->State == 0)
		{
			settingOnOffButton_SetValueIdAndIpcRequest(WM_GetId(pMsg->hWin));
			if(settingOnOffButton_GetValue(s_currentValueOnOffid) == eOn)
			{
				EDIT_SetText(s_ValueList[s_currentValueOnOffid], "OFF");
				if(s_currentValueOnOffid == eValueVoiceId)
				{
					audioMgr_StopAudio();
				}
				else if (s_currentValueOnOffid == eValueBuzzerId)
				{
					audioMgr_StopAudio();
				}
				else if (s_currentValueOnOffid == eValueChildLockId)
				{
					guiInterface_SetSettingValue(eChildLockSettingId,eOff);
					setting_SaveAllSettingValue();
				}
				else if(s_currentValueOnOffid == eValueRTCId)
				{
					realtimeclock_StopTest();
					guihandler_StopTimerCheckRTC();
				}
				else if (s_currentValueOnOffid == eValueBluetoothId) {
					BTTask_SendEvent(eOffBluetoothId);
				}
				else if (s_currentValueOnOffid == eValueBluetoothTransferId)
				{
					BTTask_SendEvent(eOffBluetoothTranferId);
				}
				else
				{
					settingOnOffButton_SendPendingEvent(WM_GetId(pMsg->hWin));

					if(s_currentValueOnOffid == eValueMotorCompressorId)
					{
						EDIT_SetText(s_ValueList[eValueValve1Id], "OFF");
						guiTask_SendPendingEvent(eTurnValve1ChangeId, settingOnOffButton_GetValue(eValueValve1Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve2Id], "OFF");
						guiTask_SendPendingEvent(eTurnValve2ChangeId, settingOnOffButton_GetValue(eValueValve2Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve3Id], "OFF");
						guiTask_SendPendingEvent(eTurnValve3ChangeId, settingOnOffButton_GetValue(eValueValve3Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve4Id], "OFF");
						guiTask_SendPendingEvent(eTurnValve4ChangeId, settingOnOffButton_GetValue(eValueValve4Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueValve5Id], "OFF");
						guiTask_SendPendingEvent(eTurnValve5ChangeId, settingOnOffButton_GetValue(eValueValve5Id));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);

						EDIT_SetText(s_ValueList[eValueMotorCoolingId], "OFF");
						guiTask_SendPendingEvent(eTurnMotorCoolingFANChangeId, settingOnOffButton_GetValue(eValueMotorCoolingId));
						GUI_X_Delay(DELAY_SEND_VIA_IPC);
					}
				}
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
/* General 	-settingOnOffButton_SetName										 			*/
/*                                                                                      */
/* Details  -Set name for setting										       			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting										*/
/*                 GUI_RECT Rect : rectangle for setting								*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_SetName(EDIT_Handle obj, GUI_RECT Rect)
{
	if(guiInterface_GetSettingValue(eLanguageSettingId) == eEnglish)
	{
		const int8_t* nameStr = (int8_t*)textLibrary_GetString((E_StringID)WM_GetId(obj));
		guiTask_SetFont(&GUI_FontEng3L10B2PP);
		GUI_DispStringAt((char*)nameStr,Rect.x0,Rect.y0 + 2);
	}
	else
	{
		guiTask_SetFont(&GUI_FontJapan3L12B2PP);
		switch(WM_GetId(obj))
		{
		case eChildlockStringID:
			GUI_DispStringAt("チャイルドロック",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnBluetoothStringID:
			GUI_DispStringAt("Bluetooth サーチ",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnBluetoothTranferStringID:
			GUI_DispStringAt("Bluetooth 転 送",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnValve1StringID:
			GUI_DispStringAt("バルブ1",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnValve2StringID:
			GUI_DispStringAt("バルブ2",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnValve3StringID:
			GUI_DispStringAt("バルブ3",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnValve4StringID:
			GUI_DispStringAt("バルブ4",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnValve5StringID:
			GUI_DispStringAt("バルブ5",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnMotorCompressorStringID:
		case eTurnMotorCoolingFANStringID:
		case eTurnMotorFilterCleaningStringID:
			GUI_DispStringAt("モータ",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnAlarmRedLEDStringID:
			GUI_DispStringAt("アラームLED(赤)",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnAlarmYellowLEDStringID:
			GUI_DispStringAt("アラームLED(黄)",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnLEDPowerAnomaliesStringID:
			GUI_DispStringAt("電源異常LED",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnVoiceGuidanceStringID:
			GUI_DispStringAt("音声ガイダンス",Rect.x0,Rect.y0 + 2);
			break;
		case eTurnBuzzerStringID:
			GUI_DispStringAt("ブザー",Rect.x0,Rect.y0 + 2);
			break;
		default:
			break;
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffLargeButton_Init					 							*/
/*                                                                                      */
/* Details  -Initialize setting on/off large button                       				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) EDIT_Handle obj : object setting 									*/
/* 				   int onBtnId :  - id of on button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int offBtnId : - id of off button 									*/
/*                     - range : 0-336                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   E_ValueOnOffId posArr :  - id of edit value 							*/
/*                     - range : 0-17                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   int valueId : id of setting button 									*/
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
/*                     (O) s_onBtn : on button                                          */
/*                     (O) s_offBtn : off button						                */
/*                     (O) s_ValueList : value list						                */
/*                     (O) s_timerOffCompressor : timer off compressor					*/
/*                     (O) s_timerOffValve1 : timer off valve 1						    */
/*                     (O) s_timerOffValve2 : timer off valve 2						    */
/*                     (O) s_timerOffValve3 : timer off valve 3						    */
/*                     (O) s_timerOffValve4 : timer off valve 4						    */
/*                     (O) s_timerOffValve5 : timer off valve 5						    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffLargeButton_Init(EDIT_Handle obj, int onBtnId,int offBtnId,E_ValueOnOffId posArr,
		int valueId, GUI_RECT Rect)
{
	s_onBtn = BUTTON_CreateEx(Rect.x0,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,onBtnId);
	WM_SetCallback(s_onBtn,settingOnOffButton_CallbackOnLargeBtn);

	s_offBtn = BUTTON_CreateEx(Rect.x0 + 50,Rect.y0 + 20,50,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,offBtnId);
	WM_SetCallback(s_offBtn,settingOnOffButton_CallbackOffLargeBtn);

	s_ValueList[posArr] = EDIT_CreateEx(Rect.x0 + 100,Rect.y0 + 20,100,50,obj,WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_LATE_CLIP,0,valueId,10);
	EDIT_SetFocussable(s_ValueList[posArr],EDIT_CI_DISABLED);
	guiTaskEDIT_SetFont(s_ValueList[posArr],&GUI_FontEng3L14B2PP);
	EDIT_SetTextAlign(s_ValueList[posArr],GUI_TA_VCENTER | GUI_TA_HCENTER);
	EDIT_SetText(s_ValueList[posArr],"OFF");

	switch(WM_GetId(obj))
	{
	case eTurnMotorCompressorStringID:
		s_timerOffCompressor = WM_CreateTimer(obj, eOffCompressorTimerId,TIMER_10_MS, 0);
		break;
	case eTurnValve1StringID:
		s_timerOffValve1 = WM_CreateTimer(obj, eOffValve1TimerId,TIMER_10_MS,0);
		break;
	case eTurnValve2StringID:
		s_timerOffValve2 = WM_CreateTimer(obj, eOffValve2TimerId,TIMER_10_MS,0);
		break;
	case eTurnValve3StringID:
		s_timerOffValve3 = WM_CreateTimer(obj, eOffValve3TimerId,TIMER_10_MS,0);
		break;
	case eTurnValve4StringID:
		s_timerOffValve4 = WM_CreateTimer(obj, eOffValve4TimerId,TIMER_10_MS,0);
		break;
	case eTurnValve5StringID:
		s_timerOffValve5 = WM_CreateTimer(obj, eOffValve5TimerId,TIMER_10_MS,0);
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_SetValue					 							*/
/*                                                                                      */
/* Details  -set value setting on/off button                       						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ValueOnOffId valueEdit : id of edit value 							*/
/*                     - range : 0-17                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/* 				   const int8_t * value : value setting									*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_ValueList : value list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_SetValue(E_ValueOnOffId valueEdit,const int8_t * value)
{
	EDIT_SetText(s_ValueList[valueEdit],(char*)value);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_GetValue					 							*/
/*                                                                                      */
/* Details  -Get valuesetting on/off button                       						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ValueOnOffId posArr : id of edit value  							*/
/*                     - range : 0-17                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint16_t : 0 if is on													*/
/* 							1 if is off		                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_ValueList : value list							            */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint16_t settingOnOffButton_GetValue(E_ValueOnOffId posArr)
{
	for(int i = 0; i < SIZE_BUFFER_ONOFF; i++)
	{
		s_bufferOnOffValue[i] = '\0';
	}

	uint16_t value = eOff;
	EDIT_GetText(s_ValueList[posArr],(char*)s_bufferOnOffValue,5);

	int compareValue = strcmp((char*)s_bufferOnOffValue,"ON");

	if(compareValue == 0)//ON
	{
		value = eOn;
	}
	else//OFF
	{
		value = eOff;
	}

	return value;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_Callback							 					*/
/*                                                                                      */
/* Details  -Call back function for setting on/off button, handle from the window       */
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
/*                     (O) s_ValueList : value list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_Callback(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	switch(pMsg->MsgId)
	{
	case WM_PAINT:
		GUI_GetClientRect(&Rect);
		GUI_SetBkColor(SERVICE_BACKGROUND_COLOR);
		GUI_ClearRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		settingOnOffButton_SetName(pMsg->hWin,Rect);
		break;
	case WM_TIMER:
	{
		/*********************Sub switch*************************/
		switch(WM_GetId(pMsg->hWin))
		{
		case eTurnMotorCompressorStringID:
			if(settingOnOffButton_GetValue(eValueMotorCompressorId) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve1Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve1ChangeId, settingOnOffButton_GetValue(eValueValve1Id));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueValve2Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve2ChangeId, settingOnOffButton_GetValue(eValueValve2Id));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueValve3Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve3ChangeId, settingOnOffButton_GetValue(eValueValve3Id));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueValve4Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve4ChangeId, settingOnOffButton_GetValue(eValueValve4Id));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueValve5Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve5ChangeId, settingOnOffButton_GetValue(eValueValve5Id));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueMotorCompressorId], "OFF");
				guiTask_SendPendingEvent(eTurnMotorCompressorChangeId,settingOnOffButton_GetValue(eValueMotorCompressorId));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);

				EDIT_SetText(s_ValueList[eValueMotorCoolingId], "OFF");
				guiTask_SendPendingEvent(eTurnMotorCoolingFANChangeId,settingOnOffButton_GetValue(eValueMotorCoolingId));
				GUI_X_Delay(DELAY_SEND_VIA_IPC);
			}
			break;
		case eTurnValve1StringID:
			if(settingOnOffButton_GetValue(eValueValve1Id) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve1Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve1ChangeId, settingOnOffButton_GetValue(eValueValve1Id));
			}
			break;
		case eTurnValve2StringID:
			if(settingOnOffButton_GetValue(eValueValve2Id) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve2Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve2ChangeId, settingOnOffButton_GetValue(eValueValve2Id));
			}
			break;
		case eTurnValve3StringID:
			if(settingOnOffButton_GetValue(eValueValve3Id) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve3Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve3ChangeId, settingOnOffButton_GetValue(eValueValve3Id));
			}
			break;
		case eTurnValve4StringID:
			if(settingOnOffButton_GetValue(eValueValve4Id) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve4Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve4ChangeId, settingOnOffButton_GetValue(eValueValve4Id));
			}
			break;
		case eTurnValve5StringID:
			if(settingOnOffButton_GetValue(eValueValve5Id) == true)
			{
				EDIT_SetText(s_ValueList[eValueValve5Id], "OFF");
				guiTask_SendPendingEvent(eTurnValve5ChangeId, settingOnOffButton_GetValue(eValueValve5Id));
			}
			break;
		default:
			break;
		}
		/*********************Sub switch*************************/
		break;
	}
	default:
		EDIT_Callback(pMsg);
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-settingOnOffButton_SetOffAllDevice						 					*/
/*                                                                                      */
/* Details  -Set off all device	when exit function test							        */
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
/*                     (O) s_ValueList : value list                                     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void settingOnOffButton_SetOffAllDevice(void)
{
	if(settingOnOffButton_GetValue(eValueValve1Id) == eOn)
	{
		guiTask_SendPendingEvent(eTurnValve1ChangeId, eOff);
		EDIT_SetText(s_ValueList[eValueValve1Id], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueValve2Id) == eOn)
	{
		guiTask_SendPendingEvent(eTurnValve2ChangeId, eOff);
		EDIT_SetText(s_ValueList[eValueValve2Id], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueValve3Id) == eOn)
	{
		guiTask_SendPendingEvent(eTurnValve3ChangeId, eOff);
		EDIT_SetText(s_ValueList[eValueValve3Id], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueValve4Id) == eOn)
	{
		guiTask_SendPendingEvent(eTurnValve4ChangeId, eOff);
		EDIT_SetText(s_ValueList[eValueValve4Id], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueValve5Id) == eOn)
	{
		guiTask_SendPendingEvent(eTurnValve5ChangeId, eOff);
		EDIT_SetText(s_ValueList[eValueValve5Id], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueMotorCompressorId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnMotorCompressorChangeId,eOff);
		EDIT_SetText(s_ValueList[eValueMotorCompressorId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueMotorCoolingId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnMotorCoolingFANChangeId,eOff);
		EDIT_SetText(s_ValueList[eValueMotorCoolingId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueMotorFilterId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnMotorFilterCleaningChangeId,eOff);
		EDIT_SetText(s_ValueList[eValueMotorFilterId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueRedLedId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnAlarmRedLEDChangeId,eOff);
		EDIT_SetText(s_ValueList[eValueRedLedId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValueYellowLedId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnAlarmYellowLEDChangeId,eOff);
		EDIT_SetText(s_ValueList[eValueYellowLedId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	if(settingOnOffButton_GetValue(eValuePowerLedId) == eOn)
	{
		guiTask_SendPendingEvent(eTurnAlarmPowerLEDChangeId,eOff);
		EDIT_SetText(s_ValueList[eValuePowerLedId], "OFF");
		GUI_X_Delay(DELAY_SEND_VIA_IPC);
	}

	audioMgr_StopAudio();
	GUI_X_Delay(DELAY_SEND_VIA_IPC);

	EDIT_SetText(s_ValueList[eValueVoiceId], "OFF");
	EDIT_SetText(s_ValueList[eValueBuzzerId], "OFF");

	return;
}

/* end of files */

