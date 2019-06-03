#ifndef SETTING_H
#define SETTING_H

/********************************************************************/
/*                                                                  */
/* File Name    : setting.h                                			*/
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
/*  #0001	   2017/09/07      Linh Nguyen(MV)  	    modify	    */
/*    improve get serial number function design						*/
/*  #0002	   2017/09/19      Viet Le			  	    modify	    */
/*    - Change uint8_t->settingID in setting_SetSettingValue() 		*/
/*			and setting_Get() function (bug 1851)					*/
/*  #0003	   2017/10/19      Viet Le			  	    modify	    */
/*	  - Edit define SETTING_DEFAULT_LOW_TEMP : 15-17 (new PSA spec)	*/
/*  #0004	   2017/10/19      Viet Le			  	    modify	    */
/*	  - Edit define MAX_SOLENOID_OPEN_TIME : 100-200 (new PSA spec)	*/
/********************************************************************/

#include <stdbool.h>
#include "stdint.h"

#include <TaskCommon.h>

/* Tag for checking if a string already exists in EEPROM */
#define CHKTAG          "OC-FH310"
#define CHKTAG_SIZE     8
#define SERIAL_INIT     "00000000"

#define EEPROM_TAG_PAGE 0x00
#define EERROM_CHKTAG_OFFSET 0

#define EEPROM_SETTING_PAGE 0x01
#define EERROM_SETTING_OFFSET 0

#define EEPROM_CUMULATIVE_PAGE 126
#define EERROM_CUMULATIVE_OFFSET 0

#define EEPROM_CONTRACT_PAGE 0x16
#define EERROM_CONTRACT_OFFSET 0
#define CONTRACT_BUFFER_SIZE 16

#define EEPROM_SERIAL_NUMBER_PAGE 0x16
#define EEPROM_SERIAL_NUMBER_OFFSET 16

#define EEPROM_ANYPAL_PAGE 0x18
#define EEPROM_ANYPAL_OFFSET 0

#define EEPROM_PSA_TABLE_OFFSET 		0
#define ZERO_TWO_FINE_NORMAL_O2_PAGE 	0x2
#define ZERO_TWO_FINE_LOW_O2_PAGE 		0x3
#define ZERO_FINE_NORMAL_O2_PAGE 		0x4
#define ZERO_FINE_LOW_O2_PAGE 			0x5
#define ZERO_SEVEN_FINE_NORMAL_O2_PAGE 	0x6
#define ZERO_SEVEN_FINE_LOW_O2_PAGE 	0x7
#define ONE_NORMAL_O2_PAGE 				0x8
#define ONE_LOW_O2_PAGE 				0x9
#define ONE_TWO_FINE_NORMAL_O2_PAGE 	0xA
#define ONE_TWO_FINE_LOW_O2_PAGE 		0xB
#define ONE_FINE_NORMAL_O2_PAGE 		0xC
#define ONE_FINE_LOW_O2_PAGE 			0xD
#define ONE_SEVEN_FINE_NORMAL_O2_PAGE 	0xE
#define ONE_SEVEN_FINE_LOW_O2_PAGE 		0xF
#define TWO_NORMAL_O2_PAGE 				0x10
#define TWO_LOW_O2_PAGE 				0x11
#define TWO_FINE_NORMAL_O2_PAGE 		0x12
#define TWO_FINE_LOW_O2_PAGE 			0x13
#define THREE_NORMAL_O2_PAGE 			0x14
#define THREE_LOW_O2_PAGE 				0x15

#define VALUE_DELETE_CHECKSUM 0xCC
#define SETTING_DEFAULT_SERVICE 5
#define SETTING_DEFAULT_HIGH_TEMP 43
#define SETTING_DEFAULT_LOW_TEMP 17
#define SETTING_DEFAULT_O2_LIMIT 90

//Check sum
#define EEPROM_START_PAGE_ADDR_CHECKSUM 0
#define EEPROM_END_PAGE_ADDR_CHECKSUM 126

#define SETTING_FR_300L 60
#define SETTING_FR_250L 50
#define SETTING_FR_200L 40
#define SETTING_FR_175L 35
#define SETTING_FR_150L 30
#define SETTING_FR_125L 25
#define SETTING_FR_100L 20
#define SETTING_FR_075L 15
#define SETTING_FR_050L 10
#define SETTING_FR_025L 5

#define MAX_PSA_PRESSURE 200
#define MIN_PSA_PRESSURE 50

#define MIN_COOLING_FAN_DUTY 30
#define MAX_COOLING_FAN_DUTY 100

#define MAX_LOLENOID_OFFSET_TIME 100

#define MAX_COMPRESSOR_RPM 100
#define MIN_COMPRESSOR_RPM 0

#define MAX_SOLENOID_OPEN_TIME 200

#define MIN_PSA_TIME 15
#define MAX_PSA_TIME 60

#define MAX_VOLUME_SETTING 10
#define MIN_VOLUME_SETTING 1

#define MAX_BRIGHTNESS_SETTING 10
#define MIN_BRIGHTNESS_SETTING 1

#define MAX_HIGHTEMP_SETTING 60
#define MIN_HIGHTEMP_SETTING 25

#define MAX_LOWTEMP_SETTING 20

#define MAX_OXY_LIMIT_SETTING 95
#define MIN_OXY_LIMIT_SETTING 85

#define MAX_HOUR_OF_CUMULATIVE_HOUR 99999
#define MAX_SEC_OF_CUMULATIVE_HOUR 59
#define MAX_MIN_OF_CUMULATIVE_HOUR 59

#define NOT_RESET_WDT 0xCC
#define RESET_WDT 0x00
#define MAX_LENGTH_CONTACT	12

#define EQUI_NAME_SIZE 		8
#define COMMODITY_CODE_SIZE 16
#define VER_SIZE 			16
#define NUMBER_ID_SIZE		16

//define 2 discrete of digital GPIO on chip
typedef enum
{
	eOff = 0,
	eOn
} E_OnOff;

//define all language that Swell machine supported: here are English and Japanese
typedef enum
{
	eJapanese = 0,
	eEnglish
} E_Language;

//Flow path setting
typedef enum
{
	eFlowPathInternal = 0,
	eFlowPathExternal
}E_Flow_Path;

//Cumualtive operation hour
typedef struct
{
	uint8_t hourH; //hight byte of hour
	uint16_t hourL; //low byte of hour
	uint8_t min; //minute
	uint8_t sec; //second
}CumulativeHour;

//Cumualtive operation hour
typedef struct
{
	uint32_t hour; //hour
	uint8_t min; //minute
	uint8_t sec; //second
}CumulativeHourTime;

//Define all settings ID
typedef enum
{
	eFirstSettingId,
	eCheckSumId= eFirstSettingId,
	eModeSettingId,
	ePrevModeSettingId,
	eUpdateCtrlSWId,
	eCurrentFlowPathSettingId,
	eUpgradeSettingId,
	eFlowRateSettingId,
	eGuidanceVolumeSettingId,
	eAlarmVolumeSettingId,
	eBuzzerVolumeSettingId,
	eRestingFRSettingId,
	eExertionFRSettingId,
	eBedtimeFRSettingId,
	eBrightnessSettingId,
	eChildLockSettingId,
	eO2CylinderSettingId,
	eLanguageSettingId,
	eHightempSettingId,
	eLowTempSettingId,
	eOxyLimitSettingId,
	eAlarmRecoverId,
	eNumberOfLogStore0,
	eNumberOfLogStore1,
	eNumberOfLogStore2,
	eNumberOfLogStore3,
	eResetWatchDogFlag,
	eReservedSetting1,
	eReservedSetting2,
	eReservedSetting3,
	eReservedSetting4,
	eLastSettingId,
	NUM_OF_SETTING = eLastSettingId
}settingID;

//Define all cumulative opeartion hour
typedef enum
{
	FIRST_CUMULATIVE_HOUR = 0,
	patients = FIRST_CUMULATIVE_HOUR,
	uints,
	uints_300,
	units_250,
	uints_200,
	uints_175,
	uints_150,
	uints_125,
	uints_100,
	uints_075,
	uints_050,
	uints_025,
	equipments,
	NUM_OF_CUMULATIVE_HOUR
}AllCumulativeHour;

//Define struct anypal infor
typedef struct
{
	int8_t equipmentName[EQUI_NAME_SIZE]; //anypal equipment name ATP-02 or AT-03
	int8_t commodityCode[COMMODITY_CODE_SIZE]; //anypal commodity code
	int8_t version[VER_SIZE]; //anypal version
	int8_t numberID[NUMBER_ID_SIZE]; //anypal number id
}AnypalInfor;

//Init all setting
void setting_Init(void);
//restore setting from EEPROM
bool setting_Restore(void);
//set settings as default
void setting_SetDefault(void);
//set setting deafult in service mode
void setting_SetDefaultServiceMode(void);
//set value for a setting
void setting_SetSettingValue(settingID id, uint8_t value);
//get value of a  setting id
uint8_t setting_Get(settingID id);
//save setting to EEPROM
void setting_SaveAllSettingValue(void);
//Set number of log stored
void setting_SetNumberOfLogStored(uint32_t numberOfLogStored);
//Get number of log stored
uint32_t setting_GetNumberOfLogStored(void);
//This operation sets  the value for anypal info
void setting_SetAnypalInfo(int8_t* equipmentName, int8_t* commodityCode, int8_t* version, int8_t* numberID);
//Get anypal infor
AnypalInfor setting_GetAnypalInfo(void);
//Check range PSA table
bool setting_CheckrangePSATable(void);
//Check range setting
bool setting_Checkrange(void);
//Init setting when alarm NVRAM activate
void setting_InitWhenAlarm(void);
//Set contact number
void setting_SetContact(int8_t *bufferContract);
//Get contact number
int8_t* setting_GetContact(void);
//Get serial number
void setting_GetSerialNumber(int8_t* serialNumberBuffer);
//Set serial number
void setting_SetSerialNumber(int8_t *bufferSerial);
//Set flag delete tag
void setting_DeleteTag(void);
//Set flag delete check sum
void setting_DeleteChecksum(void);
//Record check sum
void setting_RecordChecksum(void);
//Save anypal infor to EEPROM
void setting_SaveAnypalInfo(void);
//Save cumulative operation hour to EEPROM
void setting_SaveCumulativeHours(void);
//Save all setting to EEPROM
void setting_SaveAll(void);
//Restore setting from EEPROM
void setting_RestoreSetting(void);
//Get cumulative operation hour
CumulativeHourTime setting_GetCumulativeHour(AllCumulativeHour id);
//Set cumulative operation hour
void setting_SetCumulativeHour(AllCumulativeHour id, uint32_t hour, uint8_t min, uint8_t sec);
//Check tag Initialized
bool setting_IsTagInitialized(void);

#endif /* SETTING_H */
/* end of file */
