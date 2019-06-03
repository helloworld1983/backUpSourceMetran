/********************************************************************/
/*                                                                  */
/* File Name    : setting.c                                			*/
/*                                                                  */
/* General      : setting provides allow user to write              */
/* 				  read setting from EPPROM                          */
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
/*  #0001	   2017/09/01      Linh Nguyen(MV)  	    modify	    */
/*    fix the check rage issue             							*/
/*  #0002	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    improve get serial number function design						*/
/*  #0003	   2017/09/19      Viet Le			  	    modify	    */
/*    - Change uint8_t->settingID in setting_SetSettingValue() 		*/
/*			and setting_Get() function (bug 1851)					*/
/*  #0004	   2017/09/28      Viet Le			  	    modify	    */
/*    - Use true/false for this flag upgrade (bug 1914)				*/
/*	#0005	   2017/10/11      Viet Le			  	    modify		*/
/*	  - Edit check range of FC output in setting_ChecksingleTable()	*/
/*			function (bug 1970)										*/
/*	#0006	   2017/10/18      Viet Le			  	    modify		*/
/*	  - Clear debug log when eeprom is no tags (bug 1979)			*/
/********************************************************************/
#include "string.h"
#include "board.h"
#include <EEPROM.h>
#include <StringTools.h>
#include "setting.h"
#include "PSATableMgr.h"
#include "LogMgr.h"
#include "GuiTask.h"
#include "ModeMgr.h"
#include "MainScreen.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "SettingButton.h"
#include "DebugLog.h"

static int8_t s_contactBuffer[CONTRACT_BUFFER_SIZE];
static int8_t s_serialNumberBuffer[SERIAL_NUMBER_BUFFER_SIZE] = {'\0'};
static AnypalInfor s_anypalInfo;

static uint8_t s_bufferCheck[eMaxTempType][eMaxItemOfParam];
static bool s_isTagInitialized = false;
static CumulativeHour s_cumulativeHourList[NUM_OF_CUMULATIVE_HOUR];
static uint8_t s_settingData[NUM_OF_SETTING];
static uint8_t s_oldSettingData[NUM_OF_SETTING];

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetContact					                                		*/
/*                                                                                      */
/* Details  -Set Contact											      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *bufferContract : buffer contact								*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_contactBuffer : contact buffer							    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void setting_SetContact(int8_t *bufferContract)
{
	strncpy((char*)s_contactBuffer, (char*)bufferContract,CONTRACT_BUFFER_SIZE - 1);
	guiInterface_EEPROMWrite(EERROM_CONTRACT_OFFSET, EEPROM_CONTRACT_PAGE,(uint32_t *)s_contactBuffer, CONTRACT_BUFFER_SIZE);
	setting_RecordChecksum();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_GetSerialNumber					                                */
/*                                                                                      */
/* Details  -Get Contact number											      		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t* : contact number		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_contactBuffer : contact number buffer				        */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
int8_t* setting_GetContact(void)
{
	return s_contactBuffer;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetSerialNumber					                                */
/*                                                                                      */
/* Details  -Set Serial Number										      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *bufferSerial : buffer serial									*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_serialNumberBuffer : serial number buffer					*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void setting_SetSerialNumber(int8_t *bufferSerial)
{
	strncpy((char*)s_serialNumberBuffer, (char*)bufferSerial, SERIAL_NUMBER_BUFFER_SIZE-1);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_GetSerialNumber					                                */
/*                                                                                      */
/* Details  -Get Serial Number										      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t : serial number 		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_serialNumberBuffer : serial number buffer					*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void setting_GetSerialNumber(int8_t* serialNumberBuffer)
{
	memset(serialNumberBuffer, 0,SERIAL_NUMBER_BUFFER_SIZE);
	strncpy((char*)serialNumberBuffer, (char*)s_serialNumberBuffer, SERIAL_NUMBER_BUFFER_SIZE-1);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_DeleteTag					                                		*/
/*                                                                                      */
/* Details  -Delete tag									      							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void setting_DeleteTag(void)
{
	//copy data from settings to data array
	uint8_t data[CHKTAG_SIZE] = {0};
	guiInterface_EEPROMWrite(EERROM_CHKTAG_OFFSET, EEPROM_TAG_PAGE, (uint32_t*)&data[0], (uint32_t)CHKTAG_SIZE);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_DeleteChecksum					                            		*/
/*                                                                                      */
/* Details  -Set flag delete check sum									      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
void setting_DeleteChecksum(void)
{
	guiInterface_SetSettingValue(eCheckSumId,VALUE_DELETE_CHECKSUM);
	setting_SaveAllSettingValue();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetNumberOfLogStored						                        */
/*                                                                                      */
/* Details  -This operation sets  the value for g_numberOfLogStored			      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t numberOfLogStored : - number of log							*/
/*                      - range : 0-4294967295                                          */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE		                                      			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SetNumberOfLogStored(uint32_t numberOfLogStored)
{
	guiInterface_SetSettingValue(eNumberOfLogStore0,(numberOfLogStored & 0xff));
	guiInterface_SetSettingValue(eNumberOfLogStore1,((numberOfLogStored >> 8) & 0xff));
	guiInterface_SetSettingValue(eNumberOfLogStore2,((numberOfLogStored >> 16) & 0xff));
	guiInterface_SetSettingValue(eNumberOfLogStore3,((numberOfLogStored >> 24) & 0xff));
	uint32_t cs=0;
	EEPROM_Read(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);
	uint8_t csValue = (uint8_t)cs;

	EEPROM_Write(EERROM_SETTING_OFFSET, EEPROM_SETTING_PAGE, (uint32_t*)s_settingData, sizeof(s_settingData));

	if(setting_Get(eCheckSumId)==0)
	{
		for(int i=0; i < NUM_OF_SETTING; i++)
		{
			if(s_oldSettingData[i] != s_settingData[i])
			{
				csValue = csValue ^ s_oldSettingData[i] ^ s_settingData[i];
			}
		}
		cs = (uint32_t)(0x000000ff & csValue);
		EEPROM_Write(PAGE_OFFSET_SAVE_CHECKSUM, PAGE_ADDRESS_SAVE_CHECKSUM, (uint32_t*)&cs, SIZE_SAVE_CHECKSUM);
	}

	memcpy(s_oldSettingData,s_settingData,sizeof(s_settingData));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_GetNumberOfLogStored						                        */
/*                                                                                      */
/* Details  -This operation gets  the value for numberOfLogStored			      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint32_t : number of log		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE		                                      			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetNumberOfLogStored(void)
{
	return guiInterface_GetSettingValue(eNumberOfLogStore0) + (guiInterface_GetSettingValue(eNumberOfLogStore1) << 8) + (guiInterface_GetSettingValue(eNumberOfLogStore2) << 16) + (guiInterface_GetSettingValue(eNumberOfLogStore3) << 24);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetAnypalInfo						                                */
/*                                                                                      */
/* Details  -This operation sets  the value for anypal info					      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* equipmentName										        */
/*             (I) int8_t* commodityCode                                                */
/*             (I) int8_t* version                                                  	*/
/*             (I) int8_t* numberID                                                  	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) anypalInfo : anypal information                              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SetAnypalInfo(int8_t* equipmentName, int8_t* commodityCode, int8_t* version, int8_t* numberID)
{
	memset(&s_anypalInfo,0,sizeof(s_anypalInfo));
	strncpy((char*)s_anypalInfo.equipmentName,(char*)equipmentName,EQUI_NAME_SIZE - 1);
	strncpy((char*)s_anypalInfo.commodityCode,(char*)commodityCode,COMMODITY_CODE_SIZE);
	strncpy((char*)s_anypalInfo.version,(char*)version,VER_SIZE);
	strncpy((char*)s_anypalInfo.numberID,(char*)numberID,NUMBER_ID_SIZE);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_GetAnypalInfo						                                */
/*                                                                                      */
/* Details  -This operation gets  the value for anypal info					      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															        */
/*             								                                            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : AnypalInfor : anypal information		                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) anypalInfo : anypal information							    */
/*                     (O) NONE                                      			   		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
AnypalInfor setting_GetAnypalInfo(void)
{
	return s_anypalInfo;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_RestoreAll						                            		*/
/*                                                                                      */
/* Details  -This operation restores setting by reading data from EEPROM, 				*/
/*           then check CRC and apply them if the data is correct      		    		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue :  bool	: result of restoring process									*/
/*					true: restore success												*/
/*					false: restore failed, may no data or wrong data		            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isTagInitialized : flag initialized tag            		*/
/*                     (O) s_settingData : setting data						            */
/*                     (O) s_cumulativeHourList : cumulative hour list					*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static bool setting_RestoreAll(void)
{
	bool rtn = false;	//return value

	uint8_t tagData[CHKTAG_SIZE];
	memset(tagData,0,CHKTAG_SIZE);

	//read tag
	guiInterface_ReadEEPROM(EERROM_CHKTAG_OFFSET, EEPROM_TAG_PAGE,(uint32_t*)&tagData[0],CHKTAG_SIZE);

	uint8_t strTag[CHKTAG_SIZE + 1] = {'\0'};
	strncpy((char*)strTag,(char*)tagData,CHKTAG_SIZE);

	int compareTAG = strcmp((char*)strTag,CHKTAG);

	if(compareTAG == 0)
	{
		s_isTagInitialized = true;
		//read settings from EEPROM
		guiInterface_ReadEEPROM(EERROM_SETTING_OFFSET, EEPROM_SETTING_PAGE, (uint32_t*)s_settingData, (uint32_t)sizeof(s_settingData));
		guiInterface_ReadEEPROM(EERROM_CUMULATIVE_OFFSET, EEPROM_CUMULATIVE_PAGE, (uint32_t*)s_cumulativeHourList, (uint32_t)sizeof(CumulativeHour)*NUM_OF_CUMULATIVE_HOUR);
		rtn = true;
	}
	else
	{
		s_isTagInitialized = false;
		DEBUGOUT("\n is not init eeprom = %s", strTag);//Check byte
	}

	return rtn;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_Init					                                			*/
/*                                                                                      */
/* Details  -This operation recovers all setting is saved on on-chip EEPROM      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_settingData : setting data								    */
/*                     (O) s_contactBuffer : contact number bufer       			   	*/
/*                     (O) s_serialNumberBuffer : serial number buffer		            */
/*                     (O) s_anypalInfo : anypal information				            */
/*                     (O) s_oldSettingData : old setting data		            		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_Init(void)
{
	//read setting from EEPROM
	bool result = setting_RestoreAll();


	if(result == false)		//data not available or wrong data
	{
		uint8_t tagStr[CHKTAG_SIZE] = {'\0'};

		EEPROM_AllErase();

		strncpy((char *)tagStr, CHKTAG, CHKTAG_SIZE);
		guiInterface_EEPROMWrite(EERROM_CHKTAG_OFFSET, EEPROM_TAG_PAGE,(uint32_t*)tagStr, CHKTAG_SIZE);

		setting_SetDefault();	//use default settings

		strcpy((char*)s_contactBuffer,"");
		strcpy((char*)s_serialNumberBuffer,SERIAL_INIT);

		guiInterface_EEPROMWrite(EERROM_CONTRACT_OFFSET, EEPROM_CONTRACT_PAGE,(uint32_t*)s_contactBuffer, CONTRACT_BUFFER_SIZE);
		guiInterface_EEPROMWrite(EEPROM_SERIAL_NUMBER_OFFSET, EEPROM_SERIAL_NUMBER_PAGE,(uint32_t*)s_serialNumberBuffer, SERIAL_NUMBER_BUFFER_SIZE);

		for(int i = FIRST_CUMULATIVE_HOUR; i < NUM_OF_CUMULATIVE_HOUR; i++)
		{
			guiInterface_SetCumulativeHour((AllCumulativeHour)i,0,0,0);
		}

		guiInterface_SetAnypalInfo((int8_t*)"",(int8_t*)"",(int8_t*)"",(int8_t*)"");
		setting_SaveAnypalInfo();

		PSATableMgr_InitTablePSA(true);

		logInterface_DeleteLog(eDeleteAllLogid);

		setting_SaveAll();

		debugLog_ClearLog();
	}
	else
	{
		guiInterface_ReadEEPROM(EERROM_CONTRACT_OFFSET, EEPROM_CONTRACT_PAGE,(uint32_t *)s_contactBuffer, CONTRACT_BUFFER_SIZE);
		guiInterface_ReadEEPROM(EEPROM_SERIAL_NUMBER_OFFSET, EEPROM_SERIAL_NUMBER_PAGE,(uint32_t *)s_serialNumberBuffer, SERIAL_NUMBER_BUFFER_SIZE);

		guiInterface_ReadEEPROM(EEPROM_ANYPAL_OFFSET, EEPROM_ANYPAL_PAGE,(uint32_t*)&s_anypalInfo,sizeof(s_anypalInfo));

		PSATableMgr_InitTablePSA(false);
	}
	memcpy(s_oldSettingData,s_settingData,sizeof(s_settingData));

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_InitWhenAlarm						                                */
/*                                                                                      */
/* Details  -This operation recovers all setting is saved on on-chip EEPROM      		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_contactBuffer : contact number bufer       			   	*/
/*                     (O) s_serialNumberBuffer : serial number buffer		            */
/*                     (O) s_anypalInfo : anypal information				            */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_InitWhenAlarm(void)
{
	setting_SetDefault();	//use default settings

	strcpy((char*)s_contactBuffer,"");
	strcpy((char*)s_serialNumberBuffer,SERIAL_INIT);

	guiInterface_EEPROMWrite(EERROM_CONTRACT_OFFSET, EEPROM_CONTRACT_PAGE,(uint32_t*)s_contactBuffer, CONTRACT_BUFFER_SIZE);
	guiInterface_EEPROMWrite(EEPROM_SERIAL_NUMBER_OFFSET, EEPROM_SERIAL_NUMBER_PAGE,(uint32_t*)s_serialNumberBuffer, SERIAL_NUMBER_BUFFER_SIZE);

	for(int i = FIRST_CUMULATIVE_HOUR; i < NUM_OF_CUMULATIVE_HOUR; i++)
	{
		guiInterface_SetCumulativeHour((AllCumulativeHour)i,0,0,0);
	}

	guiInterface_SetAnypalInfo((int8_t*)"",(int8_t*)"",(int8_t*)"",(int8_t*)"");
	setting_SaveAnypalInfo();

	PSATableMgr_InitTablePSA(true);
	setting_SaveAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetDefault						                            		*/
/*                                                                                      */
/* Details  -This operation set all settings to default value      						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_settingData : setting data                                 */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SetDefault(void)
{
	//init default value for setting list
	s_settingData[eCheckSumId] = 0;
	s_settingData[eModeSettingId] = eStopMode;
	s_settingData[eCurrentFlowPathSettingId] = eFlowPathInternal;
	s_settingData[eUpgradeSettingId] = false;
	s_settingData[eFlowRateSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eGuidanceVolumeSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eAlarmVolumeSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eBuzzerVolumeSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eRestingFRSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eExertionFRSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eBedtimeFRSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eBrightnessSettingId] = SETTING_DEFAULT_SERVICE;
	s_settingData[eChildLockSettingId] = eOff;
	s_settingData[eO2CylinderSettingId] = eOff;
	s_settingData[eLanguageSettingId] = eJapanese;
	s_settingData[eHightempSettingId] = SETTING_DEFAULT_HIGH_TEMP;
	s_settingData[eLowTempSettingId] = SETTING_DEFAULT_LOW_TEMP;
	s_settingData[eOxyLimitSettingId] = SETTING_DEFAULT_O2_LIMIT;
	s_settingData[eAlarmRecoverId] = eLastAlarmId;
	s_settingData[ePrevModeSettingId] = eStopMode;
	s_settingData[eUpdateCtrlSWId] = false;
	s_settingData[eNumberOfLogStore0] = 0;
	s_settingData[eNumberOfLogStore1] = 0;
	s_settingData[eNumberOfLogStore2] = 0;
	s_settingData[eNumberOfLogStore3] = 0;
	s_settingData[eResetWatchDogFlag] = NOT_RESET_WDT;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetDefaultServiceMode						                        */
/*                                                                                      */
/* Details  -This operation set settings to default value in service mode   			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE														            */
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
void setting_SetDefaultServiceMode(void)
{
	guiInterface_SetSettingValue(eGuidanceVolumeSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eAlarmVolumeSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eBuzzerVolumeSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eRestingFRSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eExertionFRSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eBedtimeFRSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eBrightnessSettingId,SETTING_DEFAULT_SERVICE);
	guiInterface_SetSettingValue(eChildLockSettingId,eOff);
	guiInterface_SetSettingValue(eO2CylinderSettingId,eOff);
	setting_SaveAll();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetSettingValue						                            */
/*                                                                                      */
/* Details  -This operation set the temp setting value in ram to an indicated setting id*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) settingID id:  - setting ID											*/
/*                     - range : 0-26                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*		           uint8_t value:	value of setting ID									*/
/*                     - range : 0-255                                                  */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_settingData : setting data                                 */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void setting_SetSettingValue(settingID id, uint8_t value)
{
	s_settingData[id] = value;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_Get						                            			*/
/*                                                                                      */
/* Details  -This operation get value to an indicated setting id      					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) settingID id: - setting ID										    */
/*                     - range : 0-26                                                   */
/*                     - unit : NONE                                                    */
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
uint8_t setting_Get(settingID id)
{
	return s_settingData[id];
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_GetCumulativeHour					                            	*/
/*                                                                                      */
/* Details  -This operation get Cumulative hour to an indicated Cumulative id  			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t id:	cumulative ID											*/
/*                     - range : 0-13                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : CumulativeHourTime : cumulative hour time with	id	                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_cumulativeHourList : cumulative hour list					*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
CumulativeHourTime setting_GetCumulativeHour(AllCumulativeHour id)
{
	CumulativeHourTime time;
	time.hour = (uint32_t)(s_cumulativeHourList[id].hourH << 16)																																							+ s_cumulativeHourList[id].hourL;
	time.min = s_cumulativeHourList[id].min;
	time.sec = s_cumulativeHourList[id].sec;
	return time;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetCumulativeHour					                            	*/
/*                                                                                      */
/* Details  -This operation set Cumulative hour to an indicated Cumulative id  			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) AllCumulativeHour id: - cumulative hour ID							*/
/*                     - range : 0-13                                                   */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint32_t hour : - hour                                               */
/*                     - range : 0-99999                                                */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint8_t min : - minutes                                              */
/*                     - range : 0-59	                                                */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*                 uint8_t sec : - seconds                                              */
/*                     - range : 0-59	                                                */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_cumulativeHourList : cumulative hour list                	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SetCumulativeHour(AllCumulativeHour id, uint32_t hour, uint8_t min, uint8_t sec)
{
	s_cumulativeHourList[id].hourH = hour >> 16;
	s_cumulativeHourList[id].hourL = hour & 0xFFFF;
	s_cumulativeHourList[id].min = min;
	s_cumulativeHourList[id].sec = sec;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- setting_RecordChecksum					                            	*/
/*                                                                                      */
/* Details  -This operation Record Checksum to EEPROM					      		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
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
void setting_RecordChecksum(void)
{
	if(guiInterface_GetSettingValue(eCheckSumId) == 0)//not delete check sum
	{
		//Check sum
		uint8_t cs = guiInterface_EEPROMChecksum(EEPROM_START_PAGE_ADDR_CHECKSUM,EEPROM_END_PAGE_ADDR_CHECKSUM);
		EEPROM_WriteChecksum(cs);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	- setting_SaveAnypalInfo					                           		*/
/*                                                                                      */
/* Details  -This operation Record Anypal Infoto EEPROM					      		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_anypalInfo : anypal information						  	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SaveAnypalInfo(void)
{
	guiInterface_EEPROMWrite(EEPROM_ANYPAL_OFFSET,EEPROM_ANYPAL_PAGE,(uint32_t*)&s_anypalInfo,sizeof(s_anypalInfo));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SaveAllSettingValue					                            */
/*                                                                                      */
/* Details  -This operation save all settings from fist to last to EEPROM      		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_settingData : setting data							        */
/*                     (O) s_oldSettingData : old setting data        				   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SaveAllSettingValue(void)
{
	//erase memory first
	//save setting to EEPROM
	uint8_t csValue = EEPROM_ReadChecksum();
	guiInterface_EEPROMWrite(EERROM_SETTING_OFFSET, EEPROM_SETTING_PAGE, (uint32_t*)s_settingData, sizeof(s_settingData));

	if(guiInterface_GetSettingValue(eCheckSumId)==0)
	{
		for(int i=0; i < NUM_OF_SETTING; i++)
		{
			if(s_oldSettingData[i] != s_settingData[i])
			{
				csValue = csValue ^ s_oldSettingData[i] ^ s_settingData[i];
			}
		}
		EEPROM_WriteChecksum(csValue);
	}

	memcpy(s_oldSettingData,s_settingData,sizeof(s_settingData));

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SaveCumulativeHours					                            */
/*                                                                                      */
/* Details  -This operation save all settings from fist to last to EEPROM      		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_cumulativeHourList : cumulative hour list					*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SaveCumulativeHours(void)
{
	uint8_t checksumPageTemp[EEPROM_PAGE_SIZE] = {0xff};
	uint8_t checksum=0;
	uint8_t *pEepromMem = (uint8_t*)EEPROM_ADDRESS(0,EEPROM_START_PAGE_ADDR_CHECKSUM);
	uint32_t numberOfByte =(EEPROM_END_PAGE_ADDR_CHECKSUM-EEPROM_START_PAGE_ADDR_CHECKSUM)*128;
	uint32_t *checksumRecord = (uint32_t*)&checksumPageTemp[EEPROM_PAGE_SIZE -4];

	memcpy((int8_t*)checksumPageTemp,(int8_t*)s_cumulativeHourList, sizeof(s_cumulativeHourList));

	if(guiInterface_GetSettingValue(eCheckSumId)==0)
	{
		for(int i = 0; i < numberOfByte; i++)
		{
			checksum^=(*pEepromMem);
			pEepromMem++;
		}

		for(int i = 0; i< EEPROM_PAGE_SIZE -4; i++)
		{
			checksum^= checksumPageTemp[i];
		}

		*checksumRecord = (uint32_t)(checksum & 0xff);
	}
	guiInterface_EEPROMWrite(EERROM_CUMULATIVE_OFFSET,EEPROM_CUMULATIVE_PAGE, (uint32_t*)checksumPageTemp, EEPROM_PAGE_SIZE);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SaveAll					                    					*/
/*                                                                                      */
/* Details  -This operation save all settings and Cumulative Hours 		     		    */
/*						from fist to last to EEPROM 									*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_settingData : setting data							        */
/*                     (O) s_oldSettingData : old setting data             			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_SaveAll(void)
{
	guiInterface_EEPROMWrite(EERROM_SETTING_OFFSET, EEPROM_SETTING_PAGE, (uint32_t*)s_settingData, sizeof(s_settingData));
	memcpy(s_oldSettingData,s_settingData,sizeof(s_settingData));
	setting_SaveCumulativeHours();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_RestoreSetting									                    */
/*                                                                                      */
/* Details  -This operation restore all settings from EEPROM to RAM		     		    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_settingData : setting data                                 */
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void setting_RestoreSetting(void)
{
	guiInterface_ReadEEPROM(EERROM_SETTING_OFFSET, EEPROM_SETTING_PAGE, (uint32_t*)s_settingData, (uint32_t)sizeof(s_settingData));

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_CheckContactChar					                                */
/*                                                                                      */
/* Details  -Check contact number									      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t buffer : - character check									*/
/*                     - range : '0'-'9' and '-'                                        */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false		                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_CheckContactChar(char buffer)
{
	bool check=false;
	if(((buffer<'0') || (buffer>'9')) && (buffer!='-'))
	{
		check=false;
	}
	else
	{
		check=true;
	}
	return check;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_CheckSerialChar					                                */
/*                                                                                      */
/* Details  -Check serial number									      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t buffer : - character check									*/
/*                     - range : '0'-'9', 'a' - 'z', 'A'-'Z'                            */
/*                     - unit : NONE                                                    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false		                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_CheckSerialChar(char buffer)
{
	bool check=false;
	if((buffer<'0') || (buffer>'9'))
	{
		check=false;
	}
	else
	{
		check=true;
	}
	return check;

}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_CheckVersion						                                */
/*                                                                                      */
/* Details  -Check version 											      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* buffer											            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false	                  		*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_CheckVersion(int8_t* buffer)
{
	bool check = false;
	if(strlen((char*)buffer)>15)
	{
		check = false;
	}
	else
	{
		if(setting_CheckSerialChar(buffer[0])==true
				&& setting_CheckSerialChar(buffer[1])==true
				&& setting_CheckSerialChar(buffer[2])==true
				&& setting_CheckSerialChar(buffer[3])==true
				&& setting_CheckSerialChar(buffer[4])==true
				&& setting_CheckSerialChar(buffer[5])==true
				&& setting_CheckSerialChar(buffer[6])==true)
		{
			check = true;
		}
		else
		{
			check = false;
		}
	}
	return check;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_CheckrangeFlowRate						                            */
/*                                                                                      */
/* Details  -Check range flow rate setting 											    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t id : setting id											    */
/*                   - range : 0-26                                                     */
/*                   - unit : NONE                                                      */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false		                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_CheckrangeFlowRate(uint8_t id)
{
	bool error = false;

	uint8_t valueSetting = guiInterface_GetSettingValue(id);

	switch(valueSetting)
	{
	case SETTING_FR_300L:
	case SETTING_FR_250L:
	case SETTING_FR_200L:
	case SETTING_FR_175L:
	case SETTING_FR_150L:
	case SETTING_FR_125L:
	case SETTING_FR_100L:
	case SETTING_FR_075L:
	case SETTING_FR_050L:
	case SETTING_FR_025L:
		break;
	default:
		DEBUGOUT("\n setting_CheckrangeFlowRate = %d -- %d", id, valueSetting);
		error = true;
		break;
	}

	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_Checkrange						                            		*/
/*                                                                                      */
/* Details  -Check range all setting	 											    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													            	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false	             			*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_Checkrange(void)
{
	bool error = false;
	if(!((guiInterface_GetSettingValue(eUpdateCtrlSWId)== true) || (guiInterface_GetSettingValue(eUpdateCtrlSWId)== false)))
	{
		return true;
	}
	if(!((guiInterface_GetSettingValue(eCurrentFlowPathSettingId) == true) || (guiInterface_GetSettingValue(eCurrentFlowPathSettingId)== false)) )
	{
		return true;
	}
	if(!((guiInterface_GetSettingValue(eUpgradeSettingId) == true) || (guiInterface_GetSettingValue(eUpgradeSettingId)== false)))
	{
		return true;
	}

	if(guiInterface_GetSettingValue(ePrevModeSettingId)>eLastMode)
	{
		return true;
	}

	if(guiInterface_GetSettingValue(eModeSettingId)>eLastMode)
	{
		return true;
	}

	if(setting_CheckrangeFlowRate(eFlowRateSettingId) == true)
	{
		return true;
	}

	if(guiInterface_GetSettingValue(eGuidanceVolumeSettingId)>MAX_VOLUME_SETTING)
	{
		return true;
	}
	if((guiInterface_GetSettingValue(eAlarmVolumeSettingId)<MIN_VOLUME_SETTING) || (guiInterface_GetSettingValue(eAlarmVolumeSettingId)>MAX_VOLUME_SETTING))
	{
		return true;
	}
	if((guiInterface_GetSettingValue(eBuzzerVolumeSettingId)<MIN_VOLUME_SETTING) || (guiInterface_GetSettingValue(eBuzzerVolumeSettingId)>MAX_VOLUME_SETTING))
	{
		return true;
	}

	if(setting_CheckrangeFlowRate(eRestingFRSettingId) == true)
	{
		return true;
	}

	if(setting_CheckrangeFlowRate(eExertionFRSettingId) == true)
	{
		return true;
	}

	if(setting_CheckrangeFlowRate(eBedtimeFRSettingId) == true)
	{
		return true;
	}


	if((guiInterface_GetSettingValue(eBrightnessSettingId)<MIN_BRIGHTNESS_SETTING) || (guiInterface_GetSettingValue(eBrightnessSettingId)>MAX_BRIGHTNESS_SETTING))
	{
		return true;
	}
	if(!((guiInterface_GetSettingValue(eChildLockSettingId)==false) || (guiInterface_GetSettingValue(eChildLockSettingId)==true)))
	{
		return true;
	}
	if(!((guiInterface_GetSettingValue(eO2CylinderSettingId)==false) || (guiInterface_GetSettingValue(eO2CylinderSettingId)==true)))
	{
		return true;
	}
	if(!((guiInterface_GetSettingValue(eLanguageSettingId)==false) || (guiInterface_GetSettingValue(eLanguageSettingId)==true)))
	{
		return true;
	}
	if((guiInterface_GetSettingValue(eHightempSettingId)<MIN_HIGHTEMP_SETTING) || (guiInterface_GetSettingValue(eHightempSettingId)>MAX_HIGHTEMP_SETTING))
	{
		return true;
	}

	if(guiInterface_GetSettingValue(eLowTempSettingId)>MAX_LOWTEMP_SETTING)
	{
		return true;
	}

	if((guiInterface_GetSettingValue(eOxyLimitSettingId)<MIN_OXY_LIMIT_SETTING) || (guiInterface_GetSettingValue(eOxyLimitSettingId)>MAX_OXY_LIMIT_SETTING))
	{
		return true;
	}

	CumulativeHourTime time;

	time = guiInterface_GetCumulativeHour(patients);
	if(time.hour > MAX_HOUR_OF_CUMULATIVE_HOUR || time.min > MAX_MIN_OF_CUMULATIVE_HOUR || time.sec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		return true;
	}

	time = guiInterface_GetCumulativeHour(uints);
	if(time.hour > MAX_HOUR_OF_CUMULATIVE_HOUR || time.min > MAX_MIN_OF_CUMULATIVE_HOUR || time.sec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		return true;
	}

	time = guiInterface_GetCumulativeHour(equipments);
	if(time.hour > MAX_HOUR_OF_CUMULATIVE_HOUR || time.min > MAX_MIN_OF_CUMULATIVE_HOUR || time.sec > MAX_SEC_OF_CUMULATIVE_HOUR)
	{
		return true;
	}

	int8_t contactBuffer[CONTRACT_BUFFER_SIZE]={'\0'};
	guiInterface_ReadEEPROM(EERROM_CONTRACT_OFFSET, EEPROM_CONTRACT_PAGE,(uint32_t*)contactBuffer, CONTRACT_BUFFER_SIZE);
	if(strlen((char*)contactBuffer) <= MAX_LENGTH_CONTACT)
	{
		for(int i=0;i<strlen((char*)contactBuffer);i++)
		{
			if(setting_CheckContactChar(contactBuffer[i])==0)
			{
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	int8_t serialNumberBuffer[SERIAL_NUMBER_BUFFER_SIZE]={'\0'};
	guiInterface_ReadEEPROM(EEPROM_SERIAL_NUMBER_OFFSET, EEPROM_SERIAL_NUMBER_PAGE, (uint32_t*)serialNumberBuffer, SERIAL_NUMBER_BUFFER_SIZE);
	if(strlen((char*)serialNumberBuffer)==8)
	{
		for(int i=0;i<strlen((char*)serialNumberBuffer);i++)
		{
			if(setting_CheckContactChar(serialNumberBuffer[i])==0)
			{
				return true;
			}
		}
	}
	else
	{
		return true;
	}

	AnypalInfor anypalInfo;
	memset(anypalInfo.version,0,sizeof(anypalInfo.version));
	memset(anypalInfo.numberID,0,sizeof(anypalInfo.numberID));

	guiInterface_ReadEEPROM(EEPROM_ANYPAL_OFFSET, EEPROM_ANYPAL_PAGE,(uint32_t*)&anypalInfo,sizeof(anypalInfo));

	if(!(strcmp((char*)anypalInfo.equipmentName,"")==0 || strcmp((char*)anypalInfo.equipmentName,ATP_2_EQ_NAME)==0
			|| strcmp((char*)anypalInfo.equipmentName,ATP_3_EQ_NAME)==0))
	{
		return true;
	}

	if(strlen((char*)anypalInfo.commodityCode)>9)
	{
		return true;
	}

	if(!(strcmp((char*)anypalInfo.version,"")==0 || setting_CheckVersion(anypalInfo.version)==1) )
	{
		return true;
	}

	if(strlen((char*)anypalInfo.numberID)>10)
	{
		return true;
	}

	if(setting_CheckrangePSATable()==true)
	{
		return true;
	}

	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_ChecksingleTable						                            */
/*                                                                                      */
/* Details  -Check range table PSA parameter	 										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													            	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false	            			*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_bufferCheck : buffer check									*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_ChecksingleTable(void)
{
	uint8_t row=0;
	uint8_t col=0;
	bool error = false;
	//Check PSA pressure
	for(row=0;row<eMaxTempType;row++)
	{
		if((s_bufferCheck[row][ePSAPress]<MIN_PSA_PRESSURE)||(s_bufferCheck[row][ePSAPress]>MAX_PSA_PRESSURE))
		{
			error=true;
			return error;
		}
	}
	//Check range of time
	for(row=0;row<eMaxTempType;row++)
	{
		for(col=eSolenoid1OnOffsetTime;col<=eSolenoid5OpenTimeB;col++)
		{
			if(s_bufferCheck[row][col]>MAX_LOLENOID_OFFSET_TIME)
			{
				error=true;
				return error;
			}
		}
	}
	//Check Cooling fan duty
	for(row=0;row<eMaxTempType;row++)
	{
		if((s_bufferCheck[row][eCoolingFanDutyCycle]<MIN_COOLING_FAN_DUTY)
				|| (s_bufferCheck[row][eCoolingFanDutyCycle]>MAX_COOLING_FAN_DUTY))
		{
			error=true;
			return error;
		}
	}
	//Check Compressor RPM
	for(row=0;row<eMaxTempType;row++)
	{
		if(s_bufferCheck[row][eCompressorRPM] > MAX_COMPRESSOR_RPM)
		{
			error=true;
			return error;
		}
	}
	//Check FC output
	for(row=0;row<eMaxTempType;row++)
	{
		if((s_bufferCheck[row][eMFCOutput] < MIN_FC_OUTPUT) || s_bufferCheck[row][eMFCOutput] > MAX_FC_OUTPUT )
		{
			error=true;
			return error;
		}
	}
	//Check Max PSA time
	for(row=0;row<eMaxTempType;row++)
	{
		if((s_bufferCheck[row][eMaxPSATime]<MIN_PSA_TIME)||(s_bufferCheck[row][eMaxPSATime]>MAX_PSA_TIME))
		{
			error=true;
			return error;
		}
	}
	//Check Solenoid open tim on End
	for(row=0;row<eMaxTempType;row++)
	{
		if(s_bufferCheck[row][eSolenoidOpenTimeOnEnd]>MAX_SOLENOID_OPEN_TIME)
		{
			error=true;
			return error;
		}
	}
	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_CheckrangePSATable				                            		*/
/*                                                                                      */
/* Details  -Check range all item of PSA Table	 										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													            	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : if out of range is true, else is false  						*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_bufferCheck : buffer check						            */
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_CheckrangePSATable(void)
{
	bool error = false;
	uint8_t tempBuffer[SIZE_BUFFER_WRITE] = {0};//temp buffer

	//Get 0.25L normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 0.25L low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 0.5L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 0.5L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 0.75L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 0.75L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.25L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.25L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.5L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.5L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.75L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 1.75L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 2L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 2L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 2.5L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 2.5L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 3L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, THREE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}

	//Get 3L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, THREE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&s_bufferCheck[0][0], &tempBuffer[0], SIZE_BUFFER_READ);
	if(setting_ChecksingleTable()==true)
	{
		error = true;
		return error;
	}
	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_IsTagInitialized				                            		*/
/*                                                                                      */
/* Details  -Check TAG EEPROM					 										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													            	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_isTagInitialized : flag initialized tag		           	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              											    */
/*                                                                                      */
/****************************************************************************************/
bool setting_IsTagInitialized(void)
{
	return s_isTagInitialized;
}

/* end of files */

