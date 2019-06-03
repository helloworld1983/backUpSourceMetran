#ifndef SETTINGBUTTON_H_
#define SETTINGBUTTON_H_

/********************************************************************/
/*                                                                  */
/* File Name    : SettingButton.h                                 	*/
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "EDIT.h"
#include "stdint.h"
#include "stdbool.h"

#define MAX_O2_TEST							100
#define MAX_FR_TEST							3.5
#define SIZE_BUFFER_OF_VALUE  				10
#define SIZE_BUFFER_INIT_VALUE				5
#define MAX_FC_OUTPUT						200
#define MAX_MAX_PSA_TIME					60
#define MIN_FC_OUTPUT						1

#define STEP_COOLING_FAN					5
#define STEP_COMPRESSOR_RPM					1
#define STEP_BRIGHTNESS						1
#define STEP_VOLUME							1
#define STEP_O2_TEST						1
#define STEP_FR_TEST						0.01
#define STEP_HIGH_TEMP						1
#define STEP_LOW_TEMP						1
#define STEP_O2_LIMT						1
#define STEP_SOLENOID_TIME_ONEND			10
#define STEP_PSA_PRESSURE					5
#define STEP_SOLENOID_TIME					1
#define STEP_FC_OUTPUT						1
#define STEP_MAX_PSA_TIME					1

#define SCALE_FR_TEST						100
#define FC_OUTPUT_SCALE_SETTING				0.2
#define SCALE_SOLENOID_TIME					10.0
#define SCALE_SETTING_100					100
#define SCALE_SETTING_10					10
#define TYPE_CONVERSION_STEP				0.5

//Define id value for setting button
typedef enum
{
	eFirstValueSettingId = 0,
	eValueHightempSettingId,
	eValueLowTempSettingId,
	eValueOxyLimitSettingId,
	eValueInitSettingId,
	eValueSettingDutyId,
	eValueO2ConcentrationTestId,
	eValueFlowRateTestTestId,
	eValueSettingRPMId,
	eValueSettingFRPSAId,
	eValueSettingPSAPressureId,
	eValueSettingSolenoid1OnOffSetTimeId,
	eValueSettingSolenoid1OffOffSetTimeId,
	eValueSettingSolenoid2OnOffSetTimeId,
	eValueSettingSolenoid2OffOffSetTimeId,
	eValueSettingSolenoid3OnOffSetTimeId,
	eValueSettingSolenoid3OffOffSetTimeId,
	eValueSettingSolenoid4OnOffSetTimeId,
	eValueSettingSolenoid4OffOffSetTimeId,
	eValueSettingSolenoid5OnOffSetTimeAId,
	eValueSettingSolenoid5OnOffSetTimeBId,
	eValueSettingSolenoid5OpenTimeAId,
	eValueSettingSolenoid5OpenTimeBId,
	eValueSettingCompressorRPMPSAId,
	eValueSettingFCOutputPSAId,
	eValueSettingCoolingFanDutyPSAId,
	eValueSettingMaxPSATimeId,
	eValueSettingSolenoidOpenTimeOnEndId,
	eValueSettingGuidanceVolumeId,
	eValueSettingAlarmVolumeId,
	eValueSettingBuzzerVolumeId,
	eValueSettingRestingFRId,
	eValueSettingExertionFRId,
	eValueSettingBedtimeFRId,
	eValueSettingLCDBackLightId,
	eValueSettingMAXId
}E_ValueSettingBtnId;

//Init setting button save to EPPROM
void settingButton_InitSave(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,int valueId,
		int settingId,GUI_RECT Rect);
//Init setting large button save to EPPROM
void settingButton_InitSaveLargeBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,int valueId,
		int settingId,GUI_RECT Rect);
//Init setting button not save to EPPROM
void settingButton_InitNoSave(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,
		int valueId,GUI_RECT Rect);
//Init setting button not save to EPPROM
void settingButton_InitNoSaveLargeBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,
		int valueId,GUI_RECT Rect);
//Init setting button not save to EPPROM
void settingButton_InitNoSaveSmallBtn(EDIT_Handle obj, int upBtnId,int downBtnId, E_ValueSettingBtnId posArr,
		int valueId,GUI_RECT Rect);
//Custom setting button
void settingButton_Callback(WM_MESSAGE * pMsg);
//Set value for setting button
void settingButton_SetValue(E_ValueSettingBtnId valueEdit,const int8_t * value);
//Get value for setting button
uint16_t settingButton_GetValue(E_ValueSettingBtnId valueEdit);

#endif /* SETTINGBUTTON_H_ */
/* end of file */

