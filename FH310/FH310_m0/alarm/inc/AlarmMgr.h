
#ifndef ALARMMRG_HEADER_H
	#define ALARMMRG_HEADER_H

/********************************************************************/
/*                                                                  */
/* File Name    : MrgAlarm.h                                    */
/*                                                                  */
/* General       : AlarmMrg provide all of methods for alarm		    */
/*                task processing		                            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                             */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/09     Linh Nguyen(MV)  	    new file        */
/*             2016/10/19     Linh Nguyen(MV)  	    modify	        */
/* #0001 update RISSING_COMPRESSOR_OUTLET_HIGH_TEMPERATURE for latest spec*/
/********************************************************************/

/* include system headers */



/* include user headers */
#include "TaskCommon.h"
#include "DualCoreCommon.h"

/* constants, definitions */
#define HIGH_UNIT_INTERNAL_TEMPERATURE 70
#define LOW_UNIT_INTERNAL_TEMPERATURE -30
#define PRODUCT_TANK_HIGH_PRESSURE 170
#define PRODUCT_TANK_LOW_PRESSURE 50
#define O2_CONCENTRATION_75_PERCENT 75
#define O2_CONCENTRATION_82_PERCENT 82
#define O2_CONCENTRATION_98_PERCENT 98
#define O2_CONCENTRATION_10_PERCENT 10
#define COMPRESSOR_OUTLET_HIGH_TEMPERATURE 90
#define COMPRESSOR_OUTLET_LOW_TEMPERATURE -20
#define RISSING_COMPRESSOR_OUTLET_HIGH_TEMPERATURE 83
#define VACUUM_PUMP_HIGH_PRESSURE -10
#define VACUUM_PUMP_LOW_PRESSURE -95
#define EXTERNAL_FLOW_PATH_HIGH_PRESSURE 165
#define CANNULA_HIGHT_PRESSURE 10

#define TIMER_1S 20
#define TIMER_100MS 3

#define FR_MAX_VALUE 10

#define TEMPERATURE_ALARM_TIMEOUT 					10 * TIMER_1S
#define START_HIGH_PRESSURE_ALARM_DETECT			10 * TIMER_1S
#define HIGH_PRESSURE_ALARM_TIMEOUT					TIMER_100MS
#define LOW_PRESSURE_ALARM_TIMEOUT					30* TIMER_1S
#define COOLING_FAN_ALARM_TIMEOUT					20* TIMER_1S
#define START_O2_CONCENTRATION_ALARM_DETECT			20 *60* TIMER_1S
#define O2_CONCENTRATION_ALARM_CHANGE_FR			20 *60* TIMER_1S
#define O2_CONCENTRATION_ALARM_TIMEOUT				3*60* TIMER_1S
#define CANNULA_UNDER_27S_ALARM_TIMEOUT				3* TIMER_1S
#define CANNULA_MORE_27S_ALARM_TIMEOUT				30* TIMER_1S
#define START_LOWERING_O2_ALARM_DETECT				20 *60* TIMER_1S
#define LOWERING_O2_ALARM_TIMEOUT					3*60* TIMER_1S
#define LOWERING_O2_ALARM_CHANGE_FR					20 *60* TIMER_1S
#define STOP_LOWERING_O2_ALARM_TIMEOUT				10* TIMER_1S
#define O2_CONCENTRATION_SENSOR_ALARM_TIMEOUT 		10* TIMER_1S
#define FLOW_RATE_ALARM_TIMEOUT						40* TIMER_1S
#define START_FLOW_RATE_ALARM_DETECT				60* TIMER_1S
#define FLOW_RATE_ALARM_CHANGE_FR 					60* TIMER_1S
#define COMPRESSOR_OUTLET_ALARM_TIMEOUT 			5 *TIMER_1S
#define RISING_COMPRESSOR_OUTLET_ALARM_TIMEOUT 		5 *TIMER_1S
#define FLOW_CONTROLLER_COMMUNICATION_ALARM_TIMEOUT	TIMER_1S
#define EXTERNAL_FLOW_PATH_ALARM_TIMEOUT			TIMER_1S
/* prototype declarations */

//Initial Alarm
void alarmMgr_InitAlarm(void);

//Clear all of alarm when turn off device
void alarmMgr_ClearAllAlarm(void);

//Update Alarm monitor value for alarm task
void alarmMgr_UpdateAlarmMonitor(void);

//check all alarm trigger condition
void alarmMgr_CheckAllAlarm(void);

//update status for all of alarm
void alarmMgr_UpdateAllAlarm(void);

#endif
/* end of file */

