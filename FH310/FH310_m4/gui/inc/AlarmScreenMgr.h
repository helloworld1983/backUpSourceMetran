#ifndef INC_ALARMSCREENMGR_H_
#define INC_ALARMSCREENMGR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : AlarmScreenMgr.h                                  */
/*                                                                  */
/* General      : Alarm screen manager 				                */
/* 								                                    */
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
/* #0001       2017/09/16		Viet Le				modify			*/
/*			-change mode mgr design, add function to get high alarm */
/*			in standby mode, set alarm when recover mode and check  */
/*			whether theres is a alarm active when back to home screen*/
/* #0002       2017/09/19		Viet Le				modify			*/
/*          -remove define ERROR_CODE_NEGATIVE_PRESSURE             */
/* #0003       2017/10/14		Viet Le				modify			*/
/*			-Add function clear flag high alarm in standby(bug 1976)*/
/********************************************************************/
#include "DualCoreCommon.h"

#define ERROR_CODE_TEMPERATURE 			3
#define ERROR_CODE_HIGH_PRESSURE 		4
#define ERROR_CODE_LOW_PRESSURE			5
#define ERROR_CODE_COMPRESSOR			6
#define ERROR_CODE_COOLING_FAN			7
#define ERROR_CODE_O2_CONCENTRATION	    8
#define ERROR_CODE_FLOWRATE				13
#define ERROR_CODE_COMP_OUTLET_TEMP		17
#define ERROR_CODE_FLOW_CONTROLLER 		19
#define ERROR_CODE_NVRAM       			21

#define ERROR_CODE_LOW_O2				11
#define ERROR_CODE_O2_SENSOR			12
#define ERROR_CODE_RISING_COMP_OUTLET	18

#define ERROR_CODE_CANNULA				10
#define ERROR_CODE_FILTER_CLEANING		14
#define ERROR_CODE_SPEAKER				15
#define ERROR_CODE_ANYPAL				16
#define ERROR_CODE_EXTERNAL_FLOW_PATH	22
#define TIMER_RESTART_AUDIO				120000
#define SIZE_NUMBER_ALARM_BUFFER 		3

//Init alarm screen manager
void alarmScreenMgr_Init(void);
//Show alarm screen depend on individual alarm
void alarmScreenMgr_Show(void);
//Hide alarm screen depend on individual alarm
void alarmScreenMgr_Hide(void);
//Send infor alarm to device
void alarmScreenMgr_SendToDevice(E_AlarmId alarmId, E_AlarmStatus status);
//Get invisible alarm
bool alarmScreenMgr_GetFlagAlarmScreen(void);
//Set invisible alarm
void alarmScreenMgr_SetFlagAlarmScreen(bool flag);
//Set alarm list
void alarmScreenMgr_Set(ALARM_STAT_T alarmStat);
//Clear alarm list
void alarmScreenMgr_Clear(void);
//Get current alarmId
E_AlarmId alarmScreenMgr_GetCurrentAlarmId(void);
//Get previous alarmId
E_AlarmId alarmScreenMgr_GetPrevAlarmId(void);
//Set previous alarmId
void alarmScreenMgr_SetPrevALarmId(E_AlarmId prevAlarmId);
//Update animation in alarm
void alarmScreenMgr_UpdateAnimation(void);
//Get alarm extern flow path
bool alarmScreenMgr_GetAlarmExternFlowPath(void);
//Check remain alarm in list
bool alarmScreenMgr_RemainAlarm(void);
//Get high alarm
E_AlarmId alarmScreenMgr_GetHighAlarmInStandby(void);
//Clear high alarm in standby mode
void alarmScreenMgr_ClearHighAlarmInStandby(void);
//Set alarm list when recover
void alarmScreenMgr_SetAlarmRecover(ALARM_STAT_T alarmStat);

#endif /* INC_ALARMSCREENMGR_H_ */
/* end of file */
