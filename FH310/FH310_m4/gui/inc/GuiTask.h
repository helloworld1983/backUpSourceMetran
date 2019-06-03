#ifndef INC_GUITASK_H_
#define INC_GUITASK_H_

/********************************************************************/
/*                                                                  */
/* File Name    : GuiTask.h                              			*/
/*                                                                  */
/* General       : Create RTOS task for control and monitor 		*/
/* 				   device operation	                                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       2017/09/16	   Quyen Ngo			modify			*/
/*			-change gui QUEUE to 32									*/
/* #0002       2017/09/19	   Viet Le				modify			*/
/*          - Change int8_t btAddress->uint8_t btAddress            */
/*			   in GUI_EVENT_DATA (bug 1860)   						*/
/*          - Remove guiTask_GetFlagAlarmNVRam(void) (not use)		*/
/* #0003	  2017/09/27       Quyen Ngo			modify			*/
/* 	-remove definition not use										*/
/* #0004       2017/09/27	   Viet Le				modify			*/
/*		- Add guiTask_EnableIrda() function (bug 1918) 			    */
/* #0005       2017/10/27	   Viet Le				modify			*/
/*		-Add function : guiTaskEDIT_SetFont(),guiTaskTEXT_SetFont	*/
/*			guiTaskRADIO_SetFont(),guiTaskLISTVIEW_SetFont()		*/
/*			guiTaskMULTIPAGE_SetFont().(bug 1997)					*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0006 Add the dump SPI flash memory functionality (bug #1992)	*/
/* #0007       2017/12/01	   Viet Le				modify			*/
/*	 -Add function guiTask_SetBlinkingFR()(bug 2046)				*/
/* #0008       2018/02/26	   Viet Le				modify			*/
/*   -Add gui event : update animation, send flow rate to device, 	*/
/*	update calendar and cumulative hour, hide icon charging anypal	*/
/*	(bug 2065)	 					    							*/
/********************************************************************/

#include <IpcMsg.h>
#include <TaskCommon.h>
#include "GUI.h"
#include "IMAGE.h"
#include "LogMgr.h"
#include "PSATableMgr.h"
#include "Bluetooth.h"
#include "AnypalHandler.h"
#include "../../ipc/inc/IpcTask.h"
#include "EDIT.h"
#include "TEXT.h"
#include "RADIO.h"
#include "LISTVIEW.h"
#include "MULTIPAGE.h"

#define SIZE_BUFFER_FLOW_RATE 									5
#define FR_FC_SCALE 		  									5
#define FLOWRATE_SCALE 		  									20.0
#define GUI_STACK_SIZE 											8*1024
#define QUEUE_NO_WAIT 											0
#define GUI_TASK_DELAY_MS										(5/portTICK_PERIOD_MS)
#define GUI_QUEUE_LENGTH 										32
#define ATP_2_EQ_NAME 											"ATP-02"
#define ATP_3_EQ_NAME 											"ATP-03"
#define ATP_2_COMMODITY_CODE 									"650214030\0"
#define ATP_3_COMMODITY_CODE 									"651085390\0"

//Define event send to GUI
typedef enum{
	eNoneGuiEventId,
	eAnypalConnectEventId,
	eAnypalInforEventId,
	eAnypalStatusEventId,
	eBluetoothFwVersionEventId,
	eBluetoothAddrId,
	eBarcodeSerialEventId,
	eOverCurrentEventId,
	eResultSaveLogEventId,
	eCheckFWFileResultEventId,
	eCheckPWFileResultEventId,
	eDumpFileEEPROMResultEventId,
	eDumpFileCBXLogResultEventId,
	eDumpFileAnypalLogResultEventId,
	eDumpFileSpO2PulseLogResultEventId,
	eDumpFileSRAMLogResultEventId,
	eDumpFileSDRAMLogResultEventId,
	eDumpFileFlashALogResultEventId,
	eDumpFileFlashBLogResultEventId,
	eDumpFileFlashSPILogResultEventId,
	eUploadEEPROMResultEventId,
	eUploadCBXResultEventId,
	eUploadAnypalResultEventId,
	eUploadSPO2PulseResultEventId,
	eResetWDTEventId,
	eIrdaOnEventId,
	eIrdaOffEventId,
	eIrdaUpEventId,
	eIrdaDownEventId,
	eIrdaConfirmEventId,
	eIrdaPreset1EventId,
	eIrdaPreset2EventId,
	eIrdaPreset3EventId,
	eIpcDataEventId,
	eCBXGetRealDateid,
	eExportFileDebugLogEventId,
	/** >>#0008
		Add gui event : update animation,play childlock,send flow rate to device
		update calendar and cumulative hour, hide icon charging anypal
	#0008 >>**/
	eUpdateAnimationOperationEventId,
	eUpdateAnimationAlarmEventId,
	eUpdateCalenderCumulativeHoursEventId,
	eHideIconChargingAnypalEventId,
	eSendFlowRateToDeviceEventId
}E_GUI_EVENT_ID;

//Define GUI event data
typedef union
{
	uint8_t charData;//Data 1 byte
	ATP_INFO atpInfor;//Anypal Information
	ATP_STATUS atpStatus;//Anypal status(Charging, not charging, changed full)
	BT_FW_VERSION_T btFwVersion;//Buletooth version
	BARCODE_SERIAL_PACKET_T barcode;//Serial number from barcode
	IPC_MSG_DATA_t ipcData;//Data from M0
	uint8_t btAddress[BT_ADD_LENGTH]; //Bluetooth address
}GUI_EVENT_DATA;

//Define packet send to GUI
typedef struct{
	E_GUI_EVENT_ID id;//Id packet
	GUI_EVENT_DATA data;//Data send
}GUI_EVENT_STRUCT;

/*
 * Create RTOS task
 * Name: "DEVICE task"
 * Cyclic = 50ms
 * Callback function : device_task
 */
void guiTask_Create(void);



/*
 * Init gui task by register handle monitor as call-back function for IPC_receive_msg
 * task
 */
void guiTask_Init(void);

/*
 Depend on m0_msg_data.id, control suitable device
 Parameter input: data.id: ID of Device
 	 	 	 	  data.value: setting value
 This function is registered as call-back function of ipc_reveive_msg task for handle
 receiving message from GUI task
 */
void guiTask_HandleMonitorData(IPC_MSG_DATA_t data);

/*
 *	Send pending even to M0 core
 */
void guiTask_SendPendingEvent(E_MsgDataId dataId, uint32_t value);

void guiTask_SendPSAParaEvent(E_MsgDataId dataId, uint8_t flowRateSetting,
		E_O2ConcentratorType conditionO2,E_TemperatureType conditionTemp);

void guiTask_PlayFlowRate(uint8_t flowRate);

void guiTask_SetFont(const GUI_FONT * pNewFont);

void guiTask_IMAGESetBitmap(IMAGE_Handle hWin, const GUI_BITMAP * pBitmap);

uint8_t guiTask_GetFlowrate(void);
uint8_t guiTask_GetPenddingFlowRate(void);

void guiTask_SetFowRate(uint8_t flowrate);
void guiTask_SetPenddingFowRate(uint8_t flowrate);
void guiTask_SetFlagAlarmNVRam(bool isAlarm);

E_O2ConcentratorType guiTask_GetConditionO2(void);
E_TemperatureType guiTask_GetConditionTemp(void);

PSA_MONITOR_PACKET_t guiTask_GetRealMonitorData(void);

bool guiTask_GetFlagServiceScreen(void);
void guiTask_SetFlagServiceScreen(bool flag);

bool guiTask_GetFlagEnableChangeFR(void);
void guiTask_SetFlagEnableChangeFR(bool flag);
E_PsaPortSelect guiTask_GetSelectPort(void);
E_O2CylinStatus guiTask_GetO2CylinStatus(void);

bool guiTask_GetFlagOnOff(void);
void guiTask_SetFlagOnOff(bool flag);
void guiTask_ProcessEvent(void);
void guiTask_IrdaHandle(E_IrdaEventId irdaEvent);
void guiTask_SetAlarm(E_AlarmId ID, E_AlarmStatus status);
void guiTask_writeAlarmLog(E_AlarmId alarmId);
void guiTask_ClearLatchValveProcessingRequestStatus(void);

//Enable irda
void guiTask_EnableIrda(bool flag);

//Set font
void guiTaskEDIT_SetFont(EDIT_Handle hObj, const GUI_FONT * pFont);
void guiTaskTEXT_SetFont(TEXT_Handle hObj, const GUI_FONT * pFont);
void guiTaskRADIO_SetFont(RADIO_Handle hObj, const GUI_FONT * pFont);
void guiTaskLISTVIEW_SetFont(LISTVIEW_Handle hObj, const GUI_FONT * pFont);
void guiTaskMULTIPAGE_SetFont(MULTIPAGE_Handle hObj, const GUI_FONT * pFont);

//Set flag blinking flow rate
void guiTask_SetBlinkingFR(bool blinking);

#endif /* INC_GUITASK_H_ */
/* end of file */
