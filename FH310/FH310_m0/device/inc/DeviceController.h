#ifndef INC_DEVICECONTROLLER_H_
	#define INC_DEVICECONTROLLER_H_
/********************************************************************/
/*                                                                  */
/* File Name    : DeviceController.h                              	*/
/*                                                                  */
/* General       : This file contain the function for init and handle*/
/* device operation, handle setting sent from GUI and update the    */
/* monitor information												*/
/* 				   						                  			*/
/*                 	 						                    	*/
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            Sep 27, 2016      Quyen Ngo(MV)  	new file         	*/
/* #0001	  Aug 29, 2017		Quyen Ngo(MV)	add include 		*/
/*					DeviceInterface.h for using ALARM_MONITOR_t		*/
/*                                                                  */
/********************************************************************/

/* include system headers */

/* include user headers */
#include "DeviceInterface.h"
#include "DualCoreCommon.h"

//Init all specific device module
void devCtrl_Init(void);

//function for handle msg receive from GUI
void devCtrl_HandleGuiMsg(IPC_MSG_DATA_t ipcMsgData);

//Hanle device
void devCtrl_Handle(void);

//Update monitor data
void devCtrl_UpdateMonitorInfo(void);

//Update alarm monitor data
void devCtrl_UpdateAlarmMonitorStruct(void);

//Send monitor data to GUI
void devCtrl_SendMonitorInfoToGui(void);

void devCtrl_SendFirstMonitorInfoToGui(void);

//Get Alarm Monitor Struct
ALARM_MONITOR_t devCtrl_GetAlarmMonitorStruct(void);

#endif /* INC_DEVICECONTROLLER_H_ */
/* end of file */
