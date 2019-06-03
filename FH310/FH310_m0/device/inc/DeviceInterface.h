#ifndef INC_DEVICEINTERFACE_H_
	#define INC_DEVICEINTERFACE_H_
/********************************************************************/
/*                                                                  */
/* File Name    : DeviceInterface.h                                 */
/*                                                                  */
/* General       : This module contain function for interface 		*/
/* 				   between Device Task and Alarm Task               */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/26     	 Quyen Ngo(MV)  	new file        */
/*  #0001      2017/08/30  		 Quyen Ngo(MV)      modify          */
/*  - Move alarm monitor struct from DualCoreCommon.h 				*/
/*         to deviceInterface .h				                    */
/*	- Remove isNonvolatileMemoryCheckingOk, anypalStatus on Alarm   */
/* 		monitor struct												*/
/*																	*/
/* #0002       2016/09/05      Linh Nguyen	  	    modify          */
/*		-remove isResettingFlowControler on alarm monitor struct	*/
/*																	*/
/* #0003	  2017/09/07		 Quyen Ngo   modify					*/
/*		-remove isAC100Vplugin, isBatteryLow in ALARM_MONITOR_t	    */
/* 																	*/
/* #0004	  2017/09/12		Quyen Ngo			modify			*/
/*		-remove isOperationSwPush, speakerCommunication				*/
/********************************************************************/

/* include user headers */
#include "DualCoreCommon.h"
#include "TaskCommon.h"
/* constants, definitions */

/* definietion of structures */
typedef struct {
	E_ModeSetting operationMode; //current mode
	float unitInternalTemperature; //deg C
	float productTankPressure; //kPa
	uint16_t compressorStatus; //compressor Status
	E_CompressorSequence compressorSequence; //compressor Sequence
	bool coolingFanNotRotationState; //1 not rotate, 0 rotate
	int16_t O2Concentration; // %
	int16_t flowRateSetting; //x100 L/min
	float cannulaPressure; //kPa
	int16_t flowRateSensor; // x100 L/min
	bool isFilterErr; //1 signal does not come 3 times, 0 is okS
	float compressorOutletTemperature; //deg C
	uint8_t flowControllerStatus; //flow Controller Status
	float externalFlowPathPressure; //kPa
	E_PsaSequence psaSequence; //psa Sequence
	E_PsaMode psaMode; //psa mode
}ALARM_MONITOR_t;

extern SemaphoreHandle_t g_alarmMonitorMutex;
/* prototype declarations */

//Create mutex
void devInterface_Init(void);

ALARM_MONITOR_t devInterface_GetAlarmMonitor(void);

void devInterface_CleanAlarmMonitor(void);

void devInterface_AlarmProcessGuiRequest(ALARM_STAT_T alarmStat);

bool devInterface_SendMsg(IPC_MSG_DATA_t msg);

void devInterface_HandleMsgQueueReceive(void);



#endif /* INC_DEVICEINTERFACE_H_ */
/* end of file */
