/********************************************************************/
/*                                                                  */
/* File Name    : setting.h                                			*/
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
#ifndef DEVICE_INC_SETTING_H_
#define DEVICE_INC_SETTING_H_

#include <TaskCommon.h>

enum
{
	eModeSettingID,
	eContinuousFLowRateSettingID,
	eSynchronizedFlowRateSettingID,
	eTriggerSensitivitySettingID,
	eAudioVolumeSettingID,
	eSetUpdateModeID,
	eSol1DelayOpenTime,
	eSol3DelayOpenTime,
	eSettingInformationCRCSettingID,
	eLastSettingID
};
enum
{
	eTriggerSensitivityOffsetID,
	eFlowrateOffsetID,
	eO2OffsetID,
	eDischargeVolumeOffsetID,
	eTriggerSensitivityOffsetFanLowID,
	eTriggerSensitivityOffsetFanMidID,
	eTriggerSensitivityOffsetFanHighID,
	eOffsetCRCID,
	eLastOffsetID
};
typedef struct
{
	unsigned char id;
	uint16_t data;
} SettingItem;
typedef struct
{
	unsigned char id;
	double data;
} OffsetItem;
typedef enum
{
	eOff = 0,
	eOn
} E_OnOff;
typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t errr_code;
	int16_t tankPressData[300];
	uint16_t CRC;
} E_PressureData;
typedef struct
{
	uint8_t mode;
	uint8_t flowRate;
	uint8_t triggerSensitivity;
} SettingValue;

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_Init                                                        */
/*                                                                                      */
/* Details:     Init all setting by read these value from EEPROM and check whether they */
/*              are valid. If they are not valid, set them to default value             */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_Init();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_Save                                                        */
/*                                                                                      */
/* Details:     Save current settings to EEPROM                                         */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_Save();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_Set                                                         */
/*                                                                                      */
/* Details:     Set setting                                                             */
/* Arguments:   (I) unsigned char id                                                    */
/*              (I) unsigned char value                                                 */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_Set(unsigned char id, uint16_t value);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_Get                                                         */
/*                                                                                      */
/* Details:     Get setting                                                             */
/* Arguments:   (I) unsigned char id                                                    */
/*              (O) None                                                                */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t setting_Get(unsigned char id);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_OffsetSet                                                   */
/*                                                                                      */
/* Details:     Set Offset Value                                                        */
/* Arguments:   (I) unsigned char id                                                    */
/*              (I) double value                                                        */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_OffsetSet(unsigned char id, double value);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_OffsetGet                                                   */
/*                                                                                      */
/* Details:     Get Offset Value                                                        */
/* Arguments:   (I) unsigned char id                                                    */
/*              (O) None                                                                */
/* ReturnValue : double                                                                 */
/*                                                                                      */
/****************************************************************************************/
double setting_OffsetGet(unsigned char id);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_OffsetSave                                                  */
/*                                                                                      */
/* Details:     Save current Offset value to EEPROM                                     */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_OffsetSave();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SystemInformationSave                                       */
/*                                                                                      */
/* Details:     Save current system information to EEPROM                               */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_SystemInformationSave();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SetSoftwareVersion                                          */
/*                                                                                      */
/* Details:     Set software version                                                    */
/* Arguments:   (I) char *buffer                                                        */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetSoftwareVersion(char *buffer);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SetTotalOperationTimeContinuous                             */
/*                                                                                      */
/* Details:     Set total operation time continuous mode                                */
/* Arguments:   (I) uint32_t buffer                                                     */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetTotalOperationTimeContinuous(uint32_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SetTotalOperationTimeSynchronize                            */
/*                                                                                      */
/* Details:     Set total operation time Synchronize mode                               */
/* Arguments:   (I) uint32_t buffer                                                     */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetTotalOperationTimeSynchronize(uint32_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SetOperationTimeAfterMaintenanceContinuous                  */
/*                                                                                      */
/* Details:     Set total operation time After maintenance Continuous mode              */
/* Arguments:   (I) uint32_t buffer                                                     */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetOperationTimeAfterMaintenanceContinuous(uint32_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_SetOperationTimeAfterMaintenanceSynchronize                 */
/*                                                                                      */
/* Details:     Set total operation time After maintenance Synchronize mode             */
/* Arguments:   (I) uint32_t buffer                                                     */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool setting_SetOperationTimeAfterMaintenanceSynchronize(uint32_t);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetTotalOperationTimeContinuous                             */
/*                                                                                      */
/* Details:     Get total operation time Continuous mode                                */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetTotalOperationTimeContinuous(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetTotalOperationTimeSynchronize                            */
/*                                                                                      */
/* Details:     Get total operation time Synchronize mode                               */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetTotalOperationTimeSynchronize(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetOperationTimeAfterMaintenanceContinuous                  */
/*                                                                                      */
/* Details:     Get total operation time After maintenance Continuous mode              */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetOperationTimeAfterMaintenanceContinuous(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetOperationTimeAfterMaintenanceSynchronize                 */
/*                                                                                      */
/* Details:     Get total operation time After maintenance Synchronize mode             */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint32_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint32_t setting_GetOperationTimeAfterMaintenanceSynchronize(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_Handle                                                      */
/*                                                                                      */
/* Details:        Setting handle function is call for get new pressure every 100ms     */
/*                                                                                      */
/* Arguments:   (I)float TankPressure                                                   */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_Handle(float,float,float);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetPressData                                                */
/*                                                                                      */
/* Details:     Get pressure data                                                       */
/* Arguments:   (I) uint8_t* dataTankPressure                                           */
/*              (O) uint8_t* dataTankPressure                                           */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_GetTankPressData(uint8_t* dataTankPressure);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetVacuumPressData                                          */
/*                                                                                      */
/* Details:     Get pressure data                                                       */
/* Arguments:   (I) uint8_t* data                                                       */
/*              (O) uint8_t* data                                                       */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_GetVacuumPressData(int8_t* dataVacuumPressure);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetSupplyPressData                                          */
/*                                                                                      */
/* Details:     Get supply data                                                         */
/* Arguments:   (I) uint8_t* dataSupplyPressure                                         */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void setting_GetSupplyPressData(uint8_t* dataSupplyPressure);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_delete                                                      */
/*                                                                                      */
/* Details      :   Delete all setting information from EEPROM                          */
/*                                                                                      */
/* Arguments    :   (I)  NONE                                                           */
/*                  (O)  NONE                                                           */
/*                                                                                      */
/****************************************************************************************/
void setting_delete();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   setting_GetCurrentSetting                                           */
/*                                                                                      */
/* Details      :   Get current Mode, Flowrate and trigger sensitivity value from       */
/*                  setting                                                             */
/*                                                                                      */
/* Arguments    :   (I)  SettingValue* setting                                          */
/*                  (O)  NONE                                                           */
/*                                                                                      */
/****************************************************************************************/
void setting_GetCurrentSetting(SettingValue* setting);

#endif /* DEVICE_INC_SETTING_H_ */
