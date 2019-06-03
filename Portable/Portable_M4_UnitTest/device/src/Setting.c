/********************************************************************/
/*                                                                  */
/* File Name    : setting.c                                			*/
/*                                                                  */
/* General      : setting provides allow user to write              */
/* 				  read setting from EPPROM                          */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Thao Ha(MV)	  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include <AudioMgr.h>
#include <DualCoreCommon.h>
#include <EEPRom.h>
#include <FlowCtrl.h>
#include <PanelLed.h>
#include <RTC.h>
#include "Setting.h"
#include "string.h"
#include "util.h"
#include "queue.h"
#include "stdlib.h"
#include "board.h"
#include "string.h"
#include "BreathTrigger.h"
#include "LogMgr.h"
#include "LogTask.h"

//#define DEBUG_SETTING_EN
#ifdef DEBUG_SETTING_EN
#define DEBUG_SETTING(...) printf(__VA_ARGS__)
#else
#define DEBUG_SETTING(...)
#endif
#define CHKTAG          "OC-POR"
#define CHKTAG_SIZE     8
#define EEPROM_TAG_PAGE 0x00
#define EEPROM_SETTING_PAGE 0x01
#define EEPROM_SYSTEMINFO_PAGE 0x16
#define EEPROM_OFFSET_PAGE 0x17
#define NUMBER_OF_SETTING eLastSettingID
#define NUMBER_OF_OFFSET eLastOffsetID
#define BYTE_PER_PAGE 128
#define MAX_OPERATION_TIME 6000000
#define MIN_OPERATION_TIME 0
#define SCALE_FOR_SYNCHRONIZE_FLOWRATE 5
#define VERSION_LENGTH 10
#define HOUR_LENGTH 8
#define TRIGGER_SENSITIVITY_SCALE 100.0
#define FLOWRATE_OFFSET_SCALE 100.0
#define O2_OFFSET_SCALE 10.0

#define MAX_TRIGGER_SENSITIVITY_OFFSET 0.11
#define MIN_TRIGGER_SENSITIVITY_OFFSET -0.11
#define MAX_FLOWRATE_OFFSET 0.5
#define MIN_FLOWRATE_OFFSET -0.5
#define MAX_O2_OFFSET 5.0
#define MIN_O2_OFFSET -5.0
#define MAX_DISCHARGE_OFFSET 50.0
#define MIN_DISCHARGE_OFFSET -50.0
#define MAX_O2_ALARM_DETECTION_OFFSET 57
#define MIN_O2_ALARM_DETECTION_OFFSET 0
#define SAMPLE_OF_RECORDED_PRESSURE_DATA 300
#define DELAY_100MS_TICK_COUNT 200/DEVICE_TASK_DELAY_TIME
static SettingItem settingList[NUMBER_OF_SETTING];
static OffsetItem offsetList[NUMBER_OF_OFFSET];
static char gs_softwareVersionBuffer[VERSION_LENGTH]={'\0'};
static char gs_totalOperationTimeContinuousBuffer[HOUR_LENGTH]={'\0'};
static char gs_totalOperationTimeSynchronizeBuffer[HOUR_LENGTH]={'\0'};
static char gs_operationTimeAfterMaintenanceContinuousBuffer[HOUR_LENGTH]={'\0'};
static char gs_operationTimeAfterMaintenanceSynchronizeBuffer[HOUR_LENGTH]={'\0'};
static E_PressureData gs_pressData={'\0'};
static uint8_t gs_pressTankDataCurr[SAMPLE_OF_RECORDED_PRESSURE_DATA]={'\0'};
static int8_t gs_pressVacuumDataCurr[SAMPLE_OF_RECORDED_PRESSURE_DATA]={'\0'};
static uint8_t gs_pressSupplyDataCurr[SAMPLE_OF_RECORDED_PRESSURE_DATA]={'\0'};
static uint16_t gs_cntPressData=0;
static bool gs_isSetDefault = false;




/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_delete			                                        	*/
/*                                                                                      */
/* Details		: 	Delete all setting information from EEPROM							*/
/*                                                                                      */
/* Arguments 	: 	(I)  NONE					                                  		*/
/*                  (O)  NONE           	                                            */
/*                                                                                      */
/****************************************************************************************/
void setting_delete()
{
	EEPROM_Erase(EEPROM_OFFSET_PAGE);
	EEPROM_Erase(EEPROM_TAG_PAGE);
	EEPROM_Erase(EEPROM_SETTING_PAGE);
	EEPROM_Erase(EEPROM_SYSTEMINFO_PAGE);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetCurrentSetting                                        	*/
/*                                                                                      */
/* Details		: 	Get current Mode, Flowrate and trigger sensitivity value from 		*/
/* 					setting		 												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)  SettingValue* setting	                                  		*/
/*                  (O)  NONE           	                                            */
/*                                                                                      */
/****************************************************************************************/
void setting_GetCurrentSetting(SettingValue* setting)
{
	setting->mode = setting_Get(eModeSettingID);
	if(setting->mode == eContinuosMode)
	{
		setting->flowRate=setting_Get(eContinuousFLowRateSettingID);
	}
	else
	{
		setting->flowRate=setting_Get(eSynchronizedFlowRateSettingID)-SCALE_FOR_SYNCHRONIZE_FLOWRATE;
	}
	setting->triggerSensitivity = setting_Get(eTriggerSensitivitySettingID);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_CheckVersion    				     						*/
/*                                                                                      */
/* Details:     Check whether the current Version is valid                              */
/* Arguments:	(I) char* version														*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static bool setting_CheckVersion(char* version)
{
	bool error = false;
	if(strlen(version)>VERSION_LENGTH)
	{
		error = true;
	}
	else
		if((version[0]<'0') && (version[0]>'9')
				&& (version[1]<'0') && (version[1]>'9')
				&& (version[2]!='.')
				&& (version[3]<'0') && (version[3]>'9')
				&& (version[4]<'0') && (version[4]>'9')
				&& (version[5]!='.')
				&& (version[6]<'0') && (version[6]>'9')
				&& (version[7]<'0') && (version[7]>'9')
				&& (version[8]<'0') && (version[8]>'9'))
		{
			error = true;
		}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_CheckHour	    				     						*/
/*                                                                                      */
/* Details:     Check whether the Hour is valid			                                */
/* Arguments:	(I) char* time															*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static bool setting_CheckHour(char* time)
{
	bool error = false;
	if(strlen(time)>HOUR_LENGTH)
	{
		error = true;
	}
	else
	{
		int32_t totalTime = (time[0]-'0')*1000000
				+(time[1]-'0')*100000
				+(time[2]-'0')*10000
				+(time[3]-'0')*1000
				+(time[4]-'0')*100
				+(time[5]-'0')*10
				+(time[6]-'0');
		if((totalTime<MIN_OPERATION_TIME) || (totalTime>MAX_OPERATION_TIME) )
		{
			error = true;
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_CheckRange	    				     						*/
/*                                                                                      */
/* Details:     Check range of all the settings saved in EEPROM                         */
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static bool setting_CheckRange()
{
	bool error = false;
	if((setting_Get(eModeSettingID)!=eSynchronizedMode) && (setting_Get(eModeSettingID)!=eContinuosMode))
	{
		error = true;
	}
	else if(setting_Get(eContinuousFLowRateSettingID)>eFlowRateOneZeroZero)
	{
		error = true;
	}
	else if((setting_Get(eSynchronizedFlowRateSettingID)>=eFlowRateLast)||(setting_Get(eSynchronizedFlowRateSettingID)<eFlowRateOneTwoFive))
	{
		error = true;
	}
	else if(setting_CheckVersion(gs_softwareVersionBuffer)==true)
	{
		error = true;
	}
	else if(setting_CheckHour(gs_totalOperationTimeContinuousBuffer)==true)
	{
		error = true;
	}
	else if(setting_CheckHour(gs_totalOperationTimeSynchronizeBuffer)==true)
	{
		error = true;
	}
	else if(setting_CheckHour(gs_operationTimeAfterMaintenanceContinuousBuffer)==true)
	{
		error = true;
	}
	else if(setting_CheckHour(gs_operationTimeAfterMaintenanceSynchronizeBuffer)==true)
	{
		error = true;
	}
	else if((setting_Get(eAudioVolumeSettingID)>eAudioVolumeLevel5)||(setting_Get(eAudioVolumeSettingID)<eAudioVolumeLevel1))
	{
		error = true;
	}
	else if((setting_Get(eTriggerSensitivitySettingID)>eTriggerSensitivityLevel3)||(setting_Get(eTriggerSensitivitySettingID)<eTriggerSensitivityLevel1))
	{
		error = true;
	}
	else if((setting_OffsetGet(eTriggerSensitivityOffsetID)>MAX_TRIGGER_SENSITIVITY_OFFSET)||(setting_OffsetGet(eTriggerSensitivityOffsetID)<MIN_TRIGGER_SENSITIVITY_OFFSET))
	{
		error = true;
	}
	else if((setting_OffsetGet(eFlowrateOffsetID)>MAX_FLOWRATE_OFFSET)||(setting_OffsetGet(eFlowrateOffsetID)<MIN_FLOWRATE_OFFSET))
	{
		error = true;
	}
	else if((setting_OffsetGet(eO2OffsetID)>MAX_O2_OFFSET)||(setting_OffsetGet(eO2OffsetID)<MIN_O2_OFFSET))
	{
		error = true;
	}
	else if((setting_OffsetGet(eDischargeVolumeOffsetID)>MAX_DISCHARGE_OFFSET)||(setting_OffsetGet(eDischargeVolumeOffsetID)<MIN_DISCHARGE_OFFSET))
	{
		error = true;
	}

	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Restore		    				     						*/
/*                                                                                      */
/* Details:     Read all the settings from EEPROM and check whether they are valid by	*/
/*                  Checksum															*/
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static bool setting_Restore()
{
	bool error = false;
	unsigned char tag[CHKTAG_SIZE]={"\0"};
	EEPROM_Read(0,EEPROM_TAG_PAGE, (uint32_t*)&tag[0],CHKTAG_SIZE);
	if((tag[0]!='O') || (tag[1]!='C') || (tag[2]!='-') || (tag[3]!='P')
			|| (tag[4]!='O') || (tag[5]!='R'))
	{
		DEBUG_SETTING("No tag \n");
		error = true;
	}
	else
	{
		unsigned char dataLength = NUMBER_OF_SETTING;
		if(dataLength % 4)
			dataLength =  (dataLength/4 + 1)*4;
		uint16_t data[dataLength];
		for(int i = 0; i < dataLength; i++)
			data[i] = 0;
		EEPROM_Read(0,EEPROM_SETTING_PAGE, (uint32_t*)&data[0], (uint32_t)(dataLength*2));
		char SystemInformationBuffer[64]={'\0'};
		EEPROM_Read(0,EEPROM_SYSTEMINFO_PAGE,(uint32_t *)SystemInformationBuffer,sizeof(SystemInformationBuffer));
		unsigned char offsetDataLength = NUMBER_OF_OFFSET;
		/*
		This code for development purpose (it 's dead code in this case)
		if(offsetDataLength % 4)
			offsetDataLength =  (offsetDataLength/4 + 1)*4;
		 */
		int16_t offsetData[offsetDataLength];
		for(int i = 0; i < offsetDataLength; i++)
			offsetData[i] = 0;
		int16_t offsetCRC = 0;
		EEPROM_Read(0,EEPROM_OFFSET_PAGE, (uint32_t*)&offsetData[0], (uint32_t)(offsetDataLength*2));
		for(int i = 0;i < eOffsetCRCID; i++)
		{
			offsetCRC ^= offsetData[i];
		}
		uint16_t CRCSystemInformationData=(SystemInformationBuffer[43]<<8) | SystemInformationBuffer[42];
		DEBUG_SETTING("CRC SystemInformation %d\n",CRCSystemInformationData);
		DEBUG_SETTING("Compute CRC SystemInformation %d\n",util_CompCrcCcitt(0x0000,(unsigned char*)SystemInformationBuffer,42));
		DEBUG_SETTING("CRC Setting %d\n",data[eSettingInformationCRCSettingID]);
		DEBUG_SETTING("Compute CRC Setting %d\n",util_CompCrcCcitt(0x0000,(unsigned char*)data,(NUMBER_OF_SETTING-1)*2));
		DEBUG_SETTING("CRC Offset %d\n",offsetData[eSettingInformationCRCSettingID]);
		DEBUG_SETTING("Compute CRC Offset %d\n",util_CompCrcCcitt(0x0000,(unsigned char*)offsetData,(NUMBER_OF_OFFSET-1)*2));
		if((util_CompCrcCcitt(0x0000,(unsigned char*)data,(NUMBER_OF_SETTING-1)*2)==data[eSettingInformationCRCSettingID])
				&& (util_CompCrcCcitt(0x0000,(unsigned char*)SystemInformationBuffer,42)==CRCSystemInformationData)
				&& (offsetCRC==offsetData[eOffsetCRCID]))
		{
			for(int i = eModeSettingID; i < eLastSettingID; i++)
			{
				setting_Set(i, data[i]);
			}
			setting_OffsetSet(eTriggerSensitivityOffsetID,(double)offsetData[eTriggerSensitivityOffsetID]/TRIGGER_SENSITIVITY_SCALE);
			setting_OffsetSet(eTriggerSensitivityOffsetFanLowID,(double)offsetData[eTriggerSensitivityOffsetFanLowID]/TRIGGER_SENSITIVITY_SCALE);
			setting_OffsetSet(eTriggerSensitivityOffsetFanMidID,(double)offsetData[eTriggerSensitivityOffsetFanMidID]/TRIGGER_SENSITIVITY_SCALE);
			setting_OffsetSet(eTriggerSensitivityOffsetFanHighID,(double)offsetData[eTriggerSensitivityOffsetFanHighID]/TRIGGER_SENSITIVITY_SCALE);
			setting_OffsetSet(eFlowrateOffsetID,(double)offsetData[eFlowrateOffsetID]/FLOWRATE_OFFSET_SCALE);
			setting_OffsetSet(eO2OffsetID,(double)offsetData[eO2OffsetID]/O2_OFFSET_SCALE);
			setting_OffsetSet(eDischargeVolumeOffsetID,(double)offsetData[eDischargeVolumeOffsetID]);
			EEPROM_Read(0,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)gs_softwareVersionBuffer,VERSION_LENGTH);
			EEPROM_Read(10,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)gs_totalOperationTimeContinuousBuffer,HOUR_LENGTH);
			EEPROM_Read(18,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)gs_totalOperationTimeSynchronizeBuffer,HOUR_LENGTH);
			EEPROM_Read(26,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)gs_operationTimeAfterMaintenanceContinuousBuffer,HOUR_LENGTH);
			EEPROM_Read(34,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)gs_operationTimeAfterMaintenanceSynchronizeBuffer,HOUR_LENGTH);
			error = false;
		}
		else
		{
			DEBUG_SETTING("Wrong CRC \n");
			error = true;
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetDefault		    				     					*/
/*                                                                                      */
/* Details:     Set all the settings to default value									*/
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void setting_SetDefault()
{
	for(int i = 0; i < NUMBER_OF_SETTING; i++)
	{
		settingList[i].id = (unsigned char)i;
	}
	settingList[eModeSettingID].data = eContinuosMode;
	settingList[eContinuousFLowRateSettingID].data = eFlowRateZeroOneZero;
	settingList[eSynchronizedFlowRateSettingID].data = eFlowRateOneTwoFive;
	settingList[eAudioVolumeSettingID].data = eAudioVolumeLevel5;
	settingList[eTriggerSensitivitySettingID].data = eTriggerSensitivityLevel2;
	settingList[eSol1DelayOpenTime].data = 100;
	settingList[eSol3DelayOpenTime].data = 100;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetOffsetDefault    				     					*/
/*                                                                                      */
/* Details:     Set all the Offset value to default value								*/
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void setting_SetOffsetDefault()
{
	for(int i = 0; i < NUMBER_OF_OFFSET; i++)
	{
		offsetList[i].id = (unsigned char)i;
	}
	offsetList[eTriggerSensitivityOffsetID].data = 0;
	offsetList[eTriggerSensitivityOffsetFanLowID].data = 0;
	offsetList[eTriggerSensitivityOffsetFanMidID].data = 0;
	offsetList[eTriggerSensitivityOffsetFanHighID].data = 0;
	offsetList[eFlowrateOffsetID].data = 0;
	offsetList[eO2OffsetID].data = 0;
	offsetList[eDischargeVolumeOffsetID].data = 0;

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetSystemInformationDefault			     					*/
/*                                                                                      */
/* Details:     Set all the System information to default value							*/
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void setting_SetSystemInformationDefault()
{
	strcpy((char *)gs_softwareVersionBuffer,"00.00.000");
	strcpy((char *)gs_totalOperationTimeContinuousBuffer,"0000000");
	strcpy((char *)gs_totalOperationTimeSynchronizeBuffer,"0000000");
	strcpy((char *)gs_operationTimeAfterMaintenanceContinuousBuffer,"0000000");
	strcpy((char *)gs_operationTimeAfterMaintenanceSynchronizeBuffer,"0000000");
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Init								     					*/
/*                                                                                      */
/* Details:     Init all setting by read these value from EEPROM and check whether they */
/*				are valid. If they are not valid, set them to default value				*/
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_Init()
{

	memset(&gs_pressData.year,0,sizeof(gs_pressData.year));
	memset(&gs_pressData.month,0,sizeof(gs_pressData.month));
	memset(&gs_pressData.day,0,sizeof(gs_pressData.day));
	memset(&gs_pressData.hour,0,sizeof(gs_pressData.hour));
	memset(&gs_pressData.minute,0,sizeof(gs_pressData.minute));
	memset(&gs_pressData.second,0,sizeof(gs_pressData.second));
	memset(&gs_pressData.errr_code,0,sizeof(gs_pressData.errr_code));
	memset(&gs_pressData.tankPressData,0,sizeof(gs_pressData.tankPressData));
	memset(&gs_pressData.CRC,0,sizeof(gs_pressData.CRC));
	if(setting_Restore()==true)
	{
	    DEBUGOUT("setting_Restore()==true\n");
		DEBUG_SETTING("Setting set default\n");
		setting_SetDefault();
		setting_SetSystemInformationDefault();
		setting_SetOffsetDefault();
		setting_Save();
		setting_SystemInformationSave();
		setting_OffsetSave();
		gs_isSetDefault=true;
	}
	if(setting_CheckRange()==true)
	{
	    DEBUGOUT("setting_CheckRange()==true\n");
		DEBUG_SETTING("Setting set default\n");
		setting_SetDefault();
		setting_SetSystemInformationDefault();
		setting_SetOffsetDefault();
		setting_Save();
		setting_SystemInformationSave();
		setting_OffsetSave();
		gs_isSetDefault=true;
	}
	DEBUG_SETTING("Mode %d\n",setting_Get(eModeSettingID));
	DEBUG_SETTING("Continuous FR %d\n",setting_Get(eContinuousFLowRateSettingID));
	DEBUG_SETTING("Synchronize FR %d\n",setting_Get(eSynchronizedFlowRateSettingID));
	DEBUG_SETTING("Set Trigger Sensitivity Offset %f\n",setting_OffsetGet(eTriggerSensitivityOffsetID));
	DEBUG_SETTING("Set Flow rate Offset %f\n",setting_OffsetGet(eFlowrateOffsetID));
	DEBUG_SETTING("Set O2 Offset %f\n",setting_OffsetGet(eO2OffsetID));
	DEBUG_SETTING("Set Discharge Volume Offset %f\n",setting_OffsetGet(eDischargeVolumeOffsetID));
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_OffsetSet								     				*/
/*                                                                                      */
/* Details:     Set Offset Value														*/
/* Arguments:	(I) unsigned char id													*/
/*				(I) double value														*/
/*              (O) None	        			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_OffsetSet(unsigned char id, double value)
{
	offsetList[id].data = value;
	setting_OffsetSave();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_OffsetGet								     				*/
/*                                                                                      */
/* Details:     Get Offset Value														*/
/* Arguments:	(I) unsigned char id													*/
/*              (O) None	        			                                        */
/* ReturnValue : double			    	                                                */
/*                                                                                      */
/****************************************************************************************/
double setting_OffsetGet(unsigned char id)
{
	return offsetList[id].data;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Get										     				*/
/*                                                                                      */
/* Details:     Get setting																*/
/* Arguments:	(I) unsigned char id													*/
/*              (O) None	        			                                        */
/* ReturnValue : uint16_t		    	                                                */
/*                                                                                      */
/****************************************************************************************/
uint16_t setting_Get(unsigned char id)
{
	return settingList[id].data;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Set										     				*/
/*                                                                                      */
/* Details:     Set setting																*/
/* Arguments:	(I) unsigned char id													*/
/*              (I) unsigned char value			                                        */
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_Set(unsigned char id, uint16_t value)
{
	settingList[id].data = value;
	setting_Save();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_OffsetSave								     				*/
/*                                                                                      */
/* Details:     Save current Offset value to EEPROM										*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_OffsetSave()
{
	uint8_t dataLength = NUMBER_OF_OFFSET;
	/*
	This code for development purpose (it 's dead code in this case)
	if(dataLength % 4)
		dataLength =  (dataLength/4 + 1)*4;
	*/
	int16_t offsetData[dataLength];
	offsetData[eTriggerSensitivityOffsetID]=setting_OffsetGet(eTriggerSensitivityOffsetID)*TRIGGER_SENSITIVITY_SCALE;
	offsetData[eTriggerSensitivityOffsetFanLowID]=setting_OffsetGet(eTriggerSensitivityOffsetFanLowID)*TRIGGER_SENSITIVITY_SCALE;
	offsetData[eTriggerSensitivityOffsetFanMidID]=setting_OffsetGet(eTriggerSensitivityOffsetFanMidID)*TRIGGER_SENSITIVITY_SCALE;
	offsetData[eTriggerSensitivityOffsetFanHighID]=setting_OffsetGet(eTriggerSensitivityOffsetFanHighID)*TRIGGER_SENSITIVITY_SCALE;
	offsetData[eFlowrateOffsetID]=setting_OffsetGet(eFlowrateOffsetID)*FLOWRATE_OFFSET_SCALE;
	offsetData[eO2OffsetID]=setting_OffsetGet(eO2OffsetID)*O2_OFFSET_SCALE;
	offsetData[eDischargeVolumeOffsetID]=setting_OffsetGet(eDischargeVolumeOffsetID);
//	offsetData[eOffsetCRCID] = util_CompCrcCcittSigned(0x0000,(unsigned char*)offsetData,(NUMBER_OF_OFFSET-1)*2);
	offsetData[eOffsetCRCID] = 0;
	for(int i = 0;i < eOffsetCRCID; i++)
	{
		offsetData[eOffsetCRCID]^= offsetData[i];
	}
	EEPROM_Erase(EEPROM_OFFSET_PAGE);
	EEPROM_Write(0,EEPROM_OFFSET_PAGE, (uint32_t*)&offsetData[0],dataLength*2);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Save									     				*/
/*                                                                                      */
/* Details:     Save current settings to EEPROM											*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_Save()
{
	unsigned char tag[CHKTAG_SIZE] = CHKTAG;
	EEPROM_Erase(EEPROM_TAG_PAGE);
	EEPROM_Write(0,EEPROM_TAG_PAGE, (uint32_t*)&tag[0],CHKTAG_SIZE);
	uint8_t dataLength = NUMBER_OF_SETTING;
	if(dataLength % 4)
		dataLength =  (dataLength/4 + 1)*4;
	uint16_t data[dataLength];
	for(int i = eModeSettingID; i < eLastSettingID; i++)
		data[i] = setting_Get(i);
	data[eSettingInformationCRCSettingID] = util_CompCrcCcitt(0x0000,(unsigned char*)data,(NUMBER_OF_SETTING-1)*2);
	EEPROM_Erase(EEPROM_SETTING_PAGE);
	EEPROM_Write(0,EEPROM_SETTING_PAGE, (uint32_t*)&data[0],dataLength*2);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SystemInformationSave					     				*/
/*                                                                                      */
/* Details:     Save current system information to EEPROM								*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : None			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void setting_SystemInformationSave()
{
	char SystemInformationBuffer[64]={'\0'};
	memcpy(SystemInformationBuffer,gs_softwareVersionBuffer,HOUR_LENGTH);
	memcpy(&SystemInformationBuffer[10],gs_totalOperationTimeContinuousBuffer,HOUR_LENGTH);
	memcpy(&SystemInformationBuffer[18],gs_totalOperationTimeSynchronizeBuffer,HOUR_LENGTH);
	memcpy(&SystemInformationBuffer[26],gs_operationTimeAfterMaintenanceContinuousBuffer,HOUR_LENGTH);
	memcpy(&SystemInformationBuffer[34],gs_operationTimeAfterMaintenanceSynchronizeBuffer,HOUR_LENGTH);
	uint16_t CRCdata = util_CompCrcCcitt(0x0000,(unsigned char*)SystemInformationBuffer,42);
	memcpy(&SystemInformationBuffer[42],&CRCdata,2);
	EEPROM_Erase(EEPROM_SYSTEMINFO_PAGE);
	EEPROM_Write(0,EEPROM_SYSTEMINFO_PAGE,(uint32_t*)SystemInformationBuffer,sizeof(SystemInformationBuffer));
	char SystemInformationBufferRead[64]={'\0'};
	EEPROM_Read(0,EEPROM_SYSTEMINFO_PAGE,(uint32_t *)SystemInformationBufferRead,sizeof(SystemInformationBufferRead));
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetSoftwareVersion						     				*/
/*                                                                                      */
/* Details:     Set software version													*/
/* Arguments:	(I) char *buffer														*/
/*              (O) None			                                      				*/
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetSoftwareVersion(char *buffer)
{
	if(strlen(buffer)==VERSION_LENGTH-1)
	{
		memcpy(gs_softwareVersionBuffer,buffer,VERSION_LENGTH);
		return true;
	}
	else
	{
		return false;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetTotalOperationTimeContinuous			     				*/
/*                                                                                      */
/* Details:     Set total operation time continuous mode								*/
/* Arguments:	(I) uint32_t buffer														*/
/*              (O) None			                                      				*/
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetTotalOperationTimeContinuous(uint32_t buffer)
{
	if(buffer>MAX_OPERATION_TIME)
	{
		return false;
	}
	else
	{
		sprintf (gs_totalOperationTimeContinuousBuffer,"%.7d",buffer);
		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetTotalOperationTimeSynchronize		     				*/
/*                                                                                      */
/* Details:     Set total operation time Synchronize mode								*/
/* Arguments:	(I) uint32_t buffer														*/
/*              (O) None			                                      				*/
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetTotalOperationTimeSynchronize(uint32_t buffer)
{
	if(buffer>MAX_OPERATION_TIME)
	{
		return false;
	}
	else
	{
		sprintf (gs_totalOperationTimeSynchronizeBuffer,"%.7d",buffer);
		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetOperationTimeAfterMaintenanceContinuous     				*/
/*                                                                                      */
/* Details:     Set total operation time After maintenance Continuous mode				*/
/* Arguments:	(I) uint32_t buffer														*/
/*              (O) None			                                      				*/
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetOperationTimeAfterMaintenanceContinuous(uint32_t buffer)
{
	if(buffer>MAX_OPERATION_TIME)
	{
		return false;
	}
	else
	{
		sprintf (gs_operationTimeAfterMaintenanceContinuousBuffer,"%.7d",buffer);
		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_SetOperationTimeAfterMaintenanceSynchronize    				*/
/*                                                                                      */
/* Details:     Set total operation time After maintenance Synchronize mode				*/
/* Arguments:	(I) uint32_t buffer														*/
/*              (O) None			                                      				*/
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetOperationTimeAfterMaintenanceSynchronize(uint32_t buffer)
{
	if(buffer>MAX_OPERATION_TIME)
	{
		return false;
	}
	else
	{
		sprintf (gs_operationTimeAfterMaintenanceSynchronizeBuffer,"%.7d",buffer);
		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetTotalOperationTimeContinuous    							*/
/*                                                                                      */
/* Details:     Get total operation time Continuous mode								*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : uint32_t			    	                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetTotalOperationTimeContinuous(void)
{
	uint32_t totalOperationTime = 0;
	totalOperationTime = (gs_totalOperationTimeContinuousBuffer[0]-'0')*1000000
			+(gs_totalOperationTimeContinuousBuffer[1]-'0')*100000
			+(gs_totalOperationTimeContinuousBuffer[2]-'0')*10000
			+(gs_totalOperationTimeContinuousBuffer[3]-'0')*1000
			+(gs_totalOperationTimeContinuousBuffer[4]-'0')*100
			+(gs_totalOperationTimeContinuousBuffer[5]-'0')*10
			+(gs_totalOperationTimeContinuousBuffer[6]-'0');
	return totalOperationTime;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetTotalOperationTimeSynchronize   							*/
/*                                                                                      */
/* Details:     Get total operation time Synchronize mode								*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : uint32_t			    	                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetTotalOperationTimeSynchronize(void)
{
	uint32_t totalOperationTime = 0;
	totalOperationTime = (gs_totalOperationTimeSynchronizeBuffer[0]-'0')*1000000
			+(gs_totalOperationTimeSynchronizeBuffer[1]-'0')*100000
			+(gs_totalOperationTimeSynchronizeBuffer[2]-'0')*10000
			+(gs_totalOperationTimeSynchronizeBuffer[3]-'0')*1000
			+(gs_totalOperationTimeSynchronizeBuffer[4]-'0')*100
			+(gs_totalOperationTimeSynchronizeBuffer[5]-'0')*10
			+(gs_totalOperationTimeSynchronizeBuffer[6]-'0');
	return totalOperationTime;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetOperationTimeAfterMaintenanceContinuous					*/
/*                                                                                      */
/* Details:     Get total operation time After maintenance Continuous mode				*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : uint32_t			    	                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetOperationTimeAfterMaintenanceContinuous(void)
{
	uint32_t totalOperationTimeAfterMaintenance= 0;
	totalOperationTimeAfterMaintenance = (gs_operationTimeAfterMaintenanceContinuousBuffer[0]-'0')*1000000
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[1]-'0')*100000
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[2]-'0')*10000
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[3]-'0')*1000
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[4]-'0')*100
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[5]-'0')*10
			+(gs_operationTimeAfterMaintenanceContinuousBuffer[6]-'0');
	return totalOperationTimeAfterMaintenance;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetOperationTimeAfterMaintenanceSynchronize					*/
/*                                                                                      */
/* Details:     Get total operation time After maintenance Synchronize mode				*/
/* Arguments:	(I) None																*/
/*              (O) None			                                      				*/
/* ReturnValue : uint32_t			    	                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetOperationTimeAfterMaintenanceSynchronize(void)
{
	uint32_t totalOperationTimeAfterMaintenance= 0;
	totalOperationTimeAfterMaintenance = (gs_operationTimeAfterMaintenanceSynchronizeBuffer[0]-'0')*1000000
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[1]-'0')*100000
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[2]-'0')*10000
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[3]-'0')*1000
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[4]-'0')*100
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[5]-'0')*10
			+(gs_operationTimeAfterMaintenanceSynchronizeBuffer[6]-'0');
	return totalOperationTimeAfterMaintenance;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_RecordPressData												*/
/*                                                                                      */
/* Details:     record new pressure data												*/
/* Arguments:	(I) float TankPressure													*/
/*              (O) None			                                      				*/
/* ReturnValue : None				    	                                            */
/*                                                                                      */
/****************************************************************************************/
static void setting_RecordPressData(float TankPressure,float VacuumPressure, float SupplyPressure)
{
    vTaskSuspendAll();

	gs_pressTankDataCurr[gs_cntPressData]= (uint8_t)(TankPressure);
	gs_pressVacuumDataCurr[gs_cntPressData]= (int8_t)(VacuumPressure);
	gs_pressSupplyDataCurr[gs_cntPressData]= (uint8_t)(SupplyPressure);
	gs_cntPressData++;
	if(gs_cntPressData == SAMPLE_OF_RECORDED_PRESSURE_DATA)
	{
		gs_cntPressData = 0;
	}

    xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetPressData												*/
/*                                                                                      */
/* Details:     Get pressure data														*/
/* Arguments:	(I) uint8_t* dataTankPressure											*/
/*              (O) uint8_t* dataTankPressure                                      		*/
/* ReturnValue : None				    	                                            */
/*                                                                                      */
/****************************************************************************************/
void setting_GetTankPressData(uint8_t* dataTankPressure)
{
    vTaskSuspendAll();

	for(int i = 0; i < SAMPLE_OF_RECORDED_PRESSURE_DATA; i++)
	{
		uint16_t index = gs_cntPressData+i;
		if((gs_cntPressData + i) >= SAMPLE_OF_RECORDED_PRESSURE_DATA)
		{
			index -= SAMPLE_OF_RECORDED_PRESSURE_DATA;
		}
		dataTankPressure[i] = gs_pressTankDataCurr[index];
	}

    xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetVacuumPressData											*/
/*                                                                                      */
/* Details:     Get pressure data														*/
/* Arguments:	(I) uint8_t* data														*/
/*              (O) uint8_t* data			                                      		*/
/* ReturnValue : None				    	                                            */
/*                                                                                      */
/****************************************************************************************/
void setting_GetVacuumPressData(int8_t* dataVacuumPressure)
{
    vTaskSuspendAll();
	for(int i = 0; i < SAMPLE_OF_RECORDED_PRESSURE_DATA; i++)
	{
		uint16_t index = gs_cntPressData+i;
		if((gs_cntPressData + i) >= SAMPLE_OF_RECORDED_PRESSURE_DATA)
		{
			index-=SAMPLE_OF_RECORDED_PRESSURE_DATA;
		}
		dataVacuumPressure[i] = gs_pressVacuumDataCurr[index];
	}
    xTaskResumeAll();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_GetSupplyPressData											*/
/*                                                                                      */
/* Details:     Get pressure data														*/
/* Arguments:	(I) uint8_t* data														*/
/*              (O) uint8_t* data			                                      		*/
/* ReturnValue : None				    	                                            */
/*                                                                                      */
/****************************************************************************************/
void setting_GetSupplyPressData(uint8_t* dataSupplyPressure)
{
    vTaskSuspendAll();
	for(int i = 0; i < SAMPLE_OF_RECORDED_PRESSURE_DATA; i++)
	{
		uint16_t index = gs_cntPressData+i;
		if((gs_cntPressData+i) >= SAMPLE_OF_RECORDED_PRESSURE_DATA)
		{
			index-=SAMPLE_OF_RECORDED_PRESSURE_DATA;
		}
		dataSupplyPressure[i] = gs_pressSupplyDataCurr[index];
	}
    xTaskResumeAll();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	setting_Handle								              			*/
/*                                                                                      */
/* Details:        Setting handle function is call for get new pressure every 100ms		*/
/*                                                                                      */
/* Arguments:	(I)float TankPressure													*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void setting_Handle(float TankPressure,float VacuumPressure,float SupplyPressure)
{
	static bool s_isStartSequence = true;
	static uint8_t s_Cnt100ms = 0;
	if(s_isStartSequence == true)
	{
		if(gs_isSetDefault==true)
		{
			E_logQueueReceive logQueue;
			POR_LOG_T log ={'\0'};
			log.logCode=eUseDefaultParameterLogId;
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			setting_GetCurrentSetting(&log.newValue);
			logQueue.id = eRequestWriteLog;
			memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
			s_isStartSequence=false;

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				s_isStartSequence=true;
				DEBUG_TASK("Failed to send Item to logQueueRecive from USB_MessageHandle");
			}
		}
		else
		{
			s_isStartSequence = false;
		}
	}
	s_Cnt100ms++;
	if(s_Cnt100ms==DELAY_100MS_TICK_COUNT)
	{
		setting_RecordPressData(TankPressure,VacuumPressure,SupplyPressure);
		s_Cnt100ms=0;
	}
}
