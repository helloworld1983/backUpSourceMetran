/********************************************************************/
/*                                                                  */
/* File Name    : DevCtrl.c             		                    */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* all operation of Portable device									*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*           Dec 26, 2016      Thao Ha(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/
#include <AlarmMsgHandler.h>
#include <AudioTask.h>
#include <DevCtrl.h>
#include <FlowCtrl.h>
#include <DevInterface.h>
#include "Gpio.h"
#include <OpTime.h>
#include <PanelKey.h>
#include <PanelLed.h>
#include <PsaCtrl.h>
#include <UIMgr.h>
#include <ValveCtrl.h>
#include "TaskCommon.h"
#include "string.h"
#include "PressureSensor.h"
#include "BreathTrigger.h"
#include "Compressor.h"
#include "Setting.h"
#include "Fan.h"
#include "Battery.h"
#include "USBControl.h"
#include "AlarmMgr.h"
#include "LogMgr.h"

#include "LogTask.h"


//#define DEBUG_DEVCTRL_EN
#ifdef DEBUG_DEVCTRL_EN
#define DEBUG_DEVCTRL(...) printf(__VA_ARGS__)
#else
#define DEBUG_DEVCTRL(...)
#endif
#define TEST_AUDIO_VOLUME_OFFSET 1
#define TRIGGER_SENSITIVITY_OFFSET_SCALE 100.0
#define FLOWRATE_OFFSET_SCALE 100.0
#define O2_OFFSET_SCALE 10.0

#define BATT_CAPACITY_LOWEST 200
#define BATT_CAPACITY_HIGHEST 6600
#define BATT_ONE_PECENT  1
static const uint16_t c_flowRateSettingTable [13] = {10 ,25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300};
static const double c_compensatedO2Value [5] = {2.0, 2.0, 0, 0, 0}; //resolution 0,1
uint16_t g_compessorRpm = 0;
#define UPDATE_CURRENT_INPUT_VOLTAGE_FOR_BATTERY_TASK 			BATTERY_TASK_DELAY_TIME/DEVICE_TASK_DELAY_TIME
#define BATTERY_LOOP_TIME 	100/DEVICE_TASK_DELAY_TIME//100ms
static ALARM_MONITOR_t gs_stAlarmMonitor;
static PCAPP_MONITOR_t gs_stPCAppMonitor;
static float gs_tankPress = 0;
static float gs_vacuumPress = 0;
static float gs_supplyPress = 0;
static uint16_t gs_compressorBoardTemp = 0;
static uint16_t gs_compressorTemp = 0;

static E_BatteryStatus gs_currentBatteryStatus;
static float gs_cannulaPressure = 0;
static uint16_t gs_numberOfLog = 0;
static bool gs_isSpeakerConnected = true;
static uint8_t gs_alarmTestType = eEndTestAlarmID;
static uint16_t gs_alarmTestValue = 0;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessLogPacket			         	     				*/
/*                                                                                      */
/* Details: Set current number of log													*/
/*                                                                                      */
/* Arguments:	(I)int16_t numberOfLog													*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessLogPacket(DEV_EVENT_STRUCT logPacket)
{
	gs_numberOfLog = logPacket.msgData.logMgs.numberOfLog;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_SetIsSpeakerConnected			              				*/
/*                                                                                      */
/* Details: Set current Speaker status													*/
/*                                                                                      */
/* Arguments:	(I)bool isSpeakerConnected												*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessAudioPacket(DEV_EVENT_STRUCT audioPacket)
{
	gs_isSpeakerConnected = audioPacket.msgData.audioMsg.isSpeakerConnected;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_UpdateBatteryData				              				*/
/*                                                                                      */
/* Details: Update battery data from battery packet 									*/
/*                                                                                      */
/* Arguments:	(I)DEV_EVENT_STRUCT batteryPacket										*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/

static void devCtrl_UpdateBatteryData(void )
{
	uint16_t batteryRemainingCapacity = battery_GetRemainingCapacity();
	if( ((batteryRemainingCapacity < BATT_CAPACITY_HIGHEST) && (batteryRemainingCapacity > BATT_CAPACITY_LOWEST))
			||(batteryRemainingCapacity == 0) )
		gs_currentBatteryStatus.remainingCapacity = batteryRemainingCapacity;

	uint16_t batteryRemainingPercentage = battery_GetRemainingPercentage();
//	DEBUGOUT("batteryRemainingPercentage....................%d\n",batteryRemainingPercentage);
	if(batteryRemainingPercentage != BATT_ONE_PECENT)
		gs_currentBatteryStatus.remainingPercentage = batteryRemainingPercentage;

	gs_currentBatteryStatus.status = battery_GetStatus();
	gs_currentBatteryStatus.isBatteryCharging = battery_isBatteryCharging();
	gs_currentBatteryStatus.isInvalidBattery =battery_isInvalidBatteryInserted();
	gs_currentBatteryStatus.isBatteryConnected =  battery_isBatteryConnected();
	gs_currentBatteryStatus.isBatteryChargingError = battery_isBatteryChargingError();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessAlarmPacket							              	*/
/*                                                                                      */
/* Details: 	process USB packet receive from battery Task							*/
/*                                                                                      */
/* Arguments:	(I) DEV_EVENT_STRUCT alarmPacket										*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessAlarmPacket(DEV_EVENT_STRUCT alarmPacket)
{
    if(alarmPacket.msgData.alarmMsg.deviceId == eFlowControllerResetCommand)
    {
        flowCtrl_Reset();
    }
    else if (alarmPacket.msgData.alarmMsg.deviceId  == eCompressorResetCommand)
    {
        compressor_Reset();
    }
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_ProcessUSBQueue							              		*/
/*                                                                                      */
/* Details: 	process USB packet receive from battery Task							*/
/*                                                                                      */
/* Arguments:	(I) DEV_EVENT_STRUCT usbPacket											*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_ProcessUSBQueue(DEV_EVENT_STRUCT usbPacket)
{
	E_logQueueReceive logQueue;
	AUDIO_T eventAudio;
	double offset = 0;
	POR_LOG_T log ={'\0'};
	switch(usbPacket.msgData.usbMsg.requestId)
	{
	case eMessageIDGetLog:
		break;
	case eMessageIDSetTriggerSensitivity:
		log.logCode=eChangeTriggerLevelLogId;
		log.EventOrAlarm = eEvent;
		setting_GetCurrentSetting(&log.oldValue);

		setting_Set(eTriggerSensitivitySettingID,usbPacket.msgData.usbMsg.requestData);
		breathTrigger_UpdateTriggerSensitivityLevel();
		setting_GetCurrentSetting(&log.newValue);
		logQueue.id = eRequestWriteLog;
		memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
		if(!logTask_SendToLogQueueReceive(logQueue, 10))
		{
			DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
		}
		break;
	case eMessageIDSetVolume:
		log.oldValue.flowRate=setting_Get(eAudioVolumeSettingID);
		setting_Set(eAudioVolumeSettingID,usbPacket.msgData.usbMsg.requestData);

		log.newValue.flowRate=setting_Get(eAudioVolumeSettingID);
		log.logCode=eChangeSoundVolumeLogId;
		log.EventOrAlarm = eEvent;
		logQueue.id = eRequestWriteLog;
		memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
		if(!logTask_SendToLogQueueReceive(logQueue, 10))
		{
			DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
		}
		break;
	case eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset:
		OperationTime_SetOperationTimeAfterMaintenanceContinuous(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset:
		OperationTime_SetOperationTimeAfterMaintenanceSynchronize(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDClearHistory:
		break;
	case eMessageIDClearPressureLog:
		break;
	case eMessageIDSetDateAndTime:
		break;
	case eMessageIDGetSystemInformation:
		break;
	case eMessageIDGetPressureLog:
		break;
	case eMessageIDTestAudioVolume:
		eventAudio.ID = (E_ID_Audio)(eVolumeLevel01+usbPacket.msgData.usbMsg.requestData-TEST_AUDIO_VOLUME_OFFSET);
		eventAudio.volume = usbPacket.msgData.usbMsg.requestData;
		if(!audioTask_SendToAudioQueue(eventAudio, 10))
		{
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
		break;
	case eMessageIDSetAllLEDOn:
		panelLed_LedDisplay(eOnAllLeds);
		break;
	case eMessageIDSetAllLEDOff:
		panelLED_ClearOnAllLED();
		UIMgr_UpdateMode();
		UIMgr_UpdateBattery();
		if(devInterface_GetChargeSource()!=eBatterySource)
		{
			panelLed_LedDisplay(eACConnect);
		}
		else
		{
			panelLed_LedDisplay(eACDisconnect);
		}
		break;
	case eMessageIDSetTotalOperationTimeContinuousReset:
		OperationTime_SetTotalOperationTimeContinuous(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetTotalOperationTimeSynchronizeReset:

		OperationTime_SetTotalOperationTimeSynchronize(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetTriggerSensitivityOffset:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/TRIGGER_SENSITIVITY_OFFSET_SCALE);
		setting_OffsetSet(eTriggerSensitivityOffsetID,offset);
		breathTrigger_UpdateTriggerSensitivityLevel();
		break;
	case eMessageIDSetTriggerSensitivityOffsetFanLow:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/TRIGGER_SENSITIVITY_OFFSET_SCALE);
		DEBUG_DEVCTRL("Trigger Sensitivity Offset Fan LOW.......... %f\n",offset);
		setting_OffsetSet(eTriggerSensitivityOffsetFanLowID,offset);
		breathTrigger_UpdateTriggerSensitivityLevel();
		break;
	case eMessageIDSetTriggerSensitivityOffsetFanMid:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/TRIGGER_SENSITIVITY_OFFSET_SCALE);
		DEBUG_DEVCTRL("Trigger Sensitivity Offset Fan MID.......... %f\n",offset);
		setting_OffsetSet(eTriggerSensitivityOffsetFanMidID,offset);
		breathTrigger_UpdateTriggerSensitivityLevel();
		break;
	case eMessageIDSetTriggerSensitivityOffsetFanHigh:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/TRIGGER_SENSITIVITY_OFFSET_SCALE);
		DEBUG_DEVCTRL("Trigger Sensitivity Offset Fan HIGH.......... %f\n",offset);
		setting_OffsetSet(eTriggerSensitivityOffsetFanHighID,offset);
		breathTrigger_UpdateTriggerSensitivityLevel();
		break;
	case eMessageIDSetFlowrateOffset:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/FLOWRATE_OFFSET_SCALE);
		setting_OffsetSet(eFlowrateOffsetID,offset);
		break;
	case eMessageIDSetO2Offset:

		offset = ((double)(usbPacket.msgData.usbMsg.requestData)/O2_OFFSET_SCALE);
		setting_OffsetSet(eO2OffsetID,offset);
		break;
	case eMessageIDSetDischargeVolumeOffset:

		offset = (double)(usbPacket.msgData.usbMsg.requestData);
		setting_OffsetSet(eDischargeVolumeOffsetID,offset);
		break;

	case eMessageIDSetPVSAPress:

		psa_setMaxPressure(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol1OnOffsetTime:

		psa_setSol1OnOffsetTime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol3OnOffsetTime:

		psa_setSol3OnOffsetTime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol2OpenTimeA:

		psa_setSol2OpenATime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol2OpenTimeB:

		psa_setSol2OpenBTime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol1DelayOpenTime:
		setting_Set(eSol1DelayOpenTime,usbPacket.msgData.usbMsg.requestData);
		psa_setSol1DelayOpenTime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetSol3DelayOpenTime:
		setting_Set(eSol3DelayOpenTime,usbPacket.msgData.usbMsg.requestData);
		psa_setSol3DelayOpenTime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetCoolingFan:

		if(usbPacket.msgData.usbMsg.requestData==OFF)
		{
			fan_SetVoltage(OFF);
		}
		else
		{

			if(usbPacket.msgData.usbMsg.requestData==OPERATE_5V)
			{
				fan_SetVoltage(OPERATE_5V);
			}
			else
			{

				if(usbPacket.msgData.usbMsg.requestData==OPERATE_9V)
				{
					fan_SetVoltage(OPERATE_9V);
				}
				else
				{

					if(usbPacket.msgData.usbMsg.requestData==OPERATE_12V)
					{
						fan_SetVoltage(OPERATE_12V);
					}
					else
					{
						fan_SetVoltage(OFF);
					}
				}
			}
		}
		break;
	case eMessageIDSetFCOutput:

		flowCtrl_RequestSetFlow(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDSetCompressor:

		g_compessorRpm= usbPacket.msgData.usbMsg.requestData;
		compressor_ReqestSetRpm(g_compessorRpm);
		break;
	case eMessageIDSetMaxPSATime:

		psa_setPVSATime(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDClosePort:
		break;
	case eMessageIDSetTestSynchronizeMode:

		breathTrigger_SetTestSynchronizeMode(usbPacket.msgData.usbMsg.requestData);
		break;
	case eMessageIDIncreaseFlowrate:
		keyPanel_IncreaseButtonToggle();
		break;
	case eMessageIDDecreaseFlowrate:
		keyPanel_DecreaseButtonToggle();
		break;
	case eMessageIDChangeMode:
		keyPanel_ModeChangeButtonToggle();
		break;
	case eMessageIDTestAlarmValue:

		gs_alarmTestType = usbPacket.msgData.usbMsg.requestTestAlarmType;
		gs_alarmTestValue = usbPacket.msgData.usbMsg.requestTestAlarmData;
		break;

	case eMessageIDUpgrade:
		log.logCode = eBootloaderUpdateModeSetId;
		log.EventOrAlarm = eEvent;
		setting_GetCurrentSetting(&log.oldValue);
		setting_GetCurrentSetting(&log.newValue);
		E_logQueueReceive logQueue;
		logQueue.id = eRequestWriteLog;
		memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
		if(!logTask_SendToLogQueueReceive(logQueue, 10)){
			DEBUG_TASK("Failed to send Item to logQueueRecive from alarmCtrl_SendAlarmLog");
		}

		setting_Set(eSetUpdateModeID,usbPacket.msgData.usbMsg.requestData);

		break;
	default:
		break;
	}

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_Init							              				*/
/*                                                                                      */
/* Details: Init devCtrl function by init all device and create semaphoreHandle_t		*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Init(void)
{
	setting_Init();
	keyPanel_Init();
	panelLed_InitLed();
	fan_Init();
	valve_Init();
	flowCtrl_InitCommunication();
	compressor_InitCommunication();
	alarmMsgHandler_Init();
	breathTrigger_InitTrigger();
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_Handle							              				*/
/*                                                                                      */
/* Details: devCtrl function is call function for handle device function every 20ms		*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Handle(void)
{
	static uint16_t s_cnt = BATTERY_LOOP_TIME;
	gs_tankPress = pressure_ReadTankPressure();
	gs_vacuumPress = pressure_ReadVacuumPressure();
	gs_supplyPress = pressure_ReadProvidePressure();
	gs_compressorBoardTemp = compressor_getBoardTemperature();
	gs_compressorTemp = compressor_getMotorTemperature();
	if(gs_alarmTestType!=eEndTestAlarmID)
	{
		switch(gs_alarmTestType)
		{
		case eCompressorBoardTemperatureTestAlarmId:
			gs_compressorBoardTemp= gs_alarmTestValue;
			DEBUG_TASK("Set CompressorBoardTemperature %d \n",gs_compressorBoardTemp);
			break;
		case eCompressorTemperatureTestAlarmId:
			gs_compressorTemp = gs_alarmTestValue;
			DEBUG_TASK("Set CompressorTemperature %d \n",gs_compressorTemp);
			break;
		case eSetDefaultParameterTestAlarmId:
			setting_delete();
			while(1);
			break;
		default:
			break;
		}
	}

	pressure_ReadDiff(&gs_cannulaPressure);
	keyPanel_Handle();
	breathTrigger_Handle(gs_cannulaPressure);
	setting_Handle(gs_tankPress,gs_vacuumPress,gs_supplyPress);
	psa_Handle(gs_tankPress,gs_compressorBoardTemp);
	flowCtrl_Handle();
	compressor_Handle(g_compessorRpm);
	fan_Handle(gs_compressorTemp);
	OperationTime_Handle();
	if (s_cnt >= BATTERY_LOOP_TIME)
	{
		s_cnt = 0;
		battery_Handle();
		devCtrl_UpdateBatteryData();
	}
	s_cnt++;
	UIMgr_Handle(gs_currentBatteryStatus.remainingPercentage,gs_currentBatteryStatus.isBatteryCharging);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_UpdateMonitorInfo				              				*/
/*                                                                                      */
/* Details: devCtrl function is call function for update monitor information every 20ms	*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateMonitorInfo(void)
{
    vTaskSuspendAll();
		gs_stPCAppMonitor.tankPress = gs_tankPress;
		gs_stPCAppMonitor.cannualaPress = gs_cannulaPressure;
		gs_stPCAppMonitor.providePress = gs_supplyPress;
		gs_stPCAppMonitor.vacuumPress = gs_vacuumPress;
		gs_stPCAppMonitor.flowrate = flowCtrl_GetFlowVal() + setting_OffsetGet(eFlowrateOffsetID)*100;
		if(setting_Get(eModeSettingID)==eContinuosMode)
		{
			gs_stPCAppMonitor.o2Concentrator = flowCtrl_GetO2Val() + setting_OffsetGet(eO2OffsetID)*100 + c_compensatedO2Value[setting_Get(eContinuousFLowRateSettingID)]*100;

		}
		else
		{
			gs_stPCAppMonitor.o2Concentrator = flowCtrl_GetO2Val() + setting_OffsetGet(eO2OffsetID)*100;
		}
		gs_stPCAppMonitor.flowPress = flowCtrl_GetPressure();
		gs_stPCAppMonitor.flowTemp = flowCtrl_GetTemperature();
		gs_stPCAppMonitor.motorBoardTemp = gs_currentBatteryStatus.remainingPercentage*10;//gs_compressorBoardTemp;
		gs_stPCAppMonitor.motorTemp = gs_compressorTemp;
		gs_stPCAppMonitor.motorSpeed = compressor_getRpm();
		gs_stPCAppMonitor.batteryRemainCap = gs_currentBatteryStatus.remainingCapacity;
		gs_stPCAppMonitor.batteryPercentage = gs_currentBatteryStatus.remainingPercentage;
		gs_stPCAppMonitor.isInhale = breathTrigger_IsInhale();
		gs_stPCAppMonitor.triggerPressure = breathTrigger_GetTriggerPressure();
		gs_stPCAppMonitor.currentTriggerSensitivity=setting_Get(eTriggerSensitivitySettingID);
		gs_stPCAppMonitor.currentAudioVolume=setting_Get(eAudioVolumeSettingID);
		gs_stPCAppMonitor.currentMode=setting_Get(eModeSettingID);
		gs_stPCAppMonitor.totalOperationTimeContinuous=OperationTime_GetTotalOperationTimeContinuous();
		gs_stPCAppMonitor.totalOperationTimeSynchronize=OperationTime_GetTotalOperationTimeSynchronize();
		gs_stPCAppMonitor.operationTimeContinuous=OperationTime_GetOperationTimeAfterMaintenanceContinuous();
		gs_stPCAppMonitor.operationTimeSynchronize=OperationTime_GetOperationTimeAfterMaintenanceSynchronize();
		gs_stPCAppMonitor.triggerSensitivityOffset=setting_OffsetGet(eTriggerSensitivityOffsetID);
		gs_stPCAppMonitor.triggerSensitivityOffsetFanLow=setting_OffsetGet(eTriggerSensitivityOffsetFanLowID);
		gs_stPCAppMonitor.triggerSensitivityOffsetFanMid=setting_OffsetGet(eTriggerSensitivityOffsetFanMidID);
		gs_stPCAppMonitor.triggerSensitivityOffsetFanHigh=setting_OffsetGet(eTriggerSensitivityOffsetFanHighID);
		gs_stPCAppMonitor.flowrateOffset=setting_OffsetGet(eFlowrateOffsetID);
		gs_stPCAppMonitor.O2Offset=setting_OffsetGet(eO2OffsetID);
		gs_stPCAppMonitor.dischargeVolumeOffset=setting_OffsetGet(eDischargeVolumeOffsetID);
		gs_stPCAppMonitor.sol1DelayOpenTimeValue=setting_Get(eSol1DelayOpenTime);
		gs_stPCAppMonitor.sol3DelayOpenTimeValue=setting_Get(eSol3DelayOpenTime);

		if((flowCtrl_isGetVersionDone()==true)&&(compressor_isGetVersionDone()==true))
		{
			flowCtrl_GetVersion(&gs_stPCAppMonitor.flowControllerVersion[0]);
			compressor_GetVersion(&gs_stPCAppMonitor.compressorVersion[0]);
		}
		else
		{
			for(int i = 0; i < 8; i++)
			{
				gs_stPCAppMonitor.flowControllerVersion[i] = '0';
				gs_stPCAppMonitor.compressorVersion[i] = '0';
			}
		}
		gs_stPCAppMonitor.numberOfLog = gs_numberOfLog;
	xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_UpdateAlarmMonitorStruct		              				*/
/*                                                                                      */
/* Details: devCtrl function is call function for update alarm information every 20ms	*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateAlarmMonitorStruct(void)
{
    vTaskSuspendAll();
		gs_stAlarmMonitor.operationMode = (E_ModeSetting) UIMgr_GetCurrentMode();
		gs_stAlarmMonitor.isAC100VplugIn = UIMgr_isACExist();
		gs_stAlarmMonitor.isBatteryConnected = gs_currentBatteryStatus.isBatteryConnected;
		gs_stAlarmMonitor.batteryStatus = gs_currentBatteryStatus.status;
		gs_stAlarmMonitor.isBreathTrigger = gs_stPCAppMonitor.isInhale;
		gs_stAlarmMonitor.isAudioCommunicationError = false;
		gs_stAlarmMonitor.isSpeakerError = gs_isSpeakerConnected;
		gs_stAlarmMonitor.isDifferentialPressureSensorNoResponse = false;
		gs_stAlarmMonitor.batteryCapacity = gs_currentBatteryStatus.remainingPercentage;
		gs_stAlarmMonitor.providePressure = gs_stPCAppMonitor.providePress;
		gs_stAlarmMonitor.tankPressure = gs_stPCAppMonitor.tankPress;
		gs_stAlarmMonitor.vacuumPressure = gs_stPCAppMonitor.vacuumPress;
		gs_stAlarmMonitor.compressorStatus = compressor_getErrStatus();
		gs_stAlarmMonitor.compressorCommunicateStatus = compressor_getCommunicateErrStatus();
		gs_stAlarmMonitor.compressorResponseStatus = compressor_getResponseErrStatus();
		gs_stAlarmMonitor.O2Concentration =gs_stPCAppMonitor.o2Concentrator ;
		gs_stAlarmMonitor.flowRateSetting = c_flowRateSettingTable[UIMgr_GetCurrentFlowrate()];
		gs_stAlarmMonitor.flowRateSensor = gs_stPCAppMonitor.flowrate;
		gs_stAlarmMonitor.flowPress = gs_stPCAppMonitor.flowPress;
		gs_stAlarmMonitor.flowControllerStatus = flowCtrl_getErrStatus();
		gs_stAlarmMonitor.flowControllerCommunicateStatus = flowCtrl_getCommunicateErrStatus();
		gs_stAlarmMonitor.flowControllerResponseStatus = flowCtrl_getResponseErrStatus();
		gs_stAlarmMonitor.compressorSequence = compressor_GetCurrentSeq();
		gs_stAlarmMonitor.breathRate = breathTrigger_GetBreathRate();
		gs_stAlarmMonitor.differentialPressure = gs_cannulaPressure;
		gs_stAlarmMonitor.isBatteryCharging = gs_currentBatteryStatus.isBatteryCharging;
		gs_stAlarmMonitor.isInvalidBattery = gs_currentBatteryStatus.isInvalidBattery;
		gs_stAlarmMonitor.isBatteryChargingError = gs_currentBatteryStatus.isBatteryChargingError;
		gs_stAlarmMonitor.PSATemperature = psa_GetCurrentPSATemperature();

		if(gs_alarmTestType!=eEndTestAlarmID)
		{
			switch(gs_alarmTestType)
			{
			case eProvidePressTestAlarmID:
				gs_stAlarmMonitor.providePressure = (float)gs_alarmTestValue;
				DEBUG_TASK("Set providePressure %f \n",gs_stAlarmMonitor.providePressure);
				break;
			case eVacuumPressTestAlarmID:
				gs_stAlarmMonitor.vacuumPressure = -1.0*(float)gs_alarmTestValue;
				DEBUG_TASK("Set VacuumPressure %f \n",gs_stAlarmMonitor.vacuumPressure);
				break;
			case eTankPressTestAlarmID:
				gs_stAlarmMonitor.tankPressure = (float)gs_alarmTestValue;
				DEBUG_TASK("Set TankPressure %f \n",gs_stAlarmMonitor.tankPressure);
				break;
			case eBatteryStatusTestAlarmID:
				gs_stAlarmMonitor.batteryStatus = gs_alarmTestValue;
				DEBUG_TASK("Set batteryStatus %d \n",gs_stAlarmMonitor.batteryStatus);
				break;
			case eBatteryCapacityTestAlarmID:
				gs_stAlarmMonitor.batteryCapacity = gs_alarmTestValue;
				DEBUG_TASK("Set batteryCapacity %d \n",gs_stAlarmMonitor.batteryCapacity);
				break;
			case eO2ConcentrationTestAlarmID:
				gs_stAlarmMonitor.O2Concentration = gs_alarmTestValue;
				DEBUG_TASK("Set O2Concentration %d \n",gs_stAlarmMonitor.O2Concentration);
				break;
			case eFlowControllerStatusTestAlarmID:
				gs_stAlarmMonitor.flowControllerStatus =(uint8_t)gs_alarmTestValue;
				DEBUG_TASK("Set flowControllerStatus %d \n",gs_stAlarmMonitor.flowControllerStatus);
				break;
			case eFlowControllerCommunicationTestAlarmID:
				gs_stAlarmMonitor.flowControllerCommunicateStatus =(uint8_t)gs_alarmTestValue;
				DEBUG_TASK("Set flowControllerCommunicateStatus %d \n",gs_stAlarmMonitor.flowControllerCommunicateStatus);
				break;
			case eFlowControllerResponseStatusTestAlarmID:
				gs_stAlarmMonitor.flowControllerResponseStatus = (E_FlowControllerNoAlarmStatus)gs_alarmTestValue;
				DEBUG_TASK("Set flowControllerResponseStatus %d \n",gs_stAlarmMonitor.flowControllerResponseStatus);
				break;
			case eCompressorStatusTestAlarmID:
				gs_stAlarmMonitor.compressorStatus = (uint16_t)gs_alarmTestValue;
				DEBUG_TASK("Set compressorStatus %d \n",gs_stAlarmMonitor.compressorStatus);
				break;
			case eCompressorCommunicationStatusTestAlarmID:
				gs_stAlarmMonitor.compressorCommunicateStatus = (uint8_t)gs_alarmTestValue;
				DEBUG_TASK("Set compressorCommunicateStatus %d \n",gs_stAlarmMonitor.compressorCommunicateStatus);
				break;
			case eCompressorResponseStatusTestAlarmID:
				gs_stAlarmMonitor.compressorResponseStatus = (E_CompressorStatus)gs_alarmTestValue;
				DEBUG_TASK("Set compressorResponseStatus %d \n",gs_stAlarmMonitor.compressorResponseStatus);
				break;
			case eDifferentialPressureTestAlarmID:
				gs_stAlarmMonitor.differentialPressure = ERROR_DIFFPRESS_CONST;
				DEBUG_TASK("Set differentialPressure %f \n",gs_stAlarmMonitor.differentialPressure);
				break;
			case eWatchDogTimerTestAlarmID:
				while(1)
				{
					DEBUG_TASK("WatchDog Timer Testing\n");
				}
				break;
			case eBatteryChargerErrorTestAlarmId:
				gs_stAlarmMonitor.isBatteryChargingError = true;
				DEBUG_TASK("Set isBatteryChargingError\n");
				break;
			case eFlowRateTestAlarmId:
				gs_stAlarmMonitor.flowRateSensor = (uint16_t)gs_alarmTestValue;
				DEBUG_TASK("Set Flowrate %d \n",gs_stAlarmMonitor.flowRateSensor);
			default:
				break;
			}
		}
    xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_GetAlarmMonitorStruct			              				*/
/*                                                                                      */
/* Details: Get current Alarm monitor struct											*/
/*                                                                                      */
/* Arguments:	ALARM_MONITOR_t *storagePlace											*/
/*                                                                                      */
/* ReturnValue : none			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_GetAlarmMonitorStruct(ALARM_MONITOR_t *storagePlace)
{
    vTaskSuspendAll();
	*storagePlace = gs_stAlarmMonitor;
    xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	devCtrl_GetPCAppMonitorStruct			              				*/
/*                                                                                      */
/* Details: Get current PC application monitor struct									*/
/*                                                                                      */
/* Arguments:	PCAPP_MONITOR_t *storagePlace											*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_GetPCAppMonitorStruct(PCAPP_MONITOR_t *storagePlace)
{
    vTaskSuspendAll();
	*storagePlace = gs_stPCAppMonitor;
	xTaskResumeAll();
}
