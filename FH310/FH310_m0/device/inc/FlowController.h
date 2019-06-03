#ifndef INC_FLOW_CONTROLLER_H_
	#define INC_FLOW_CONTROLLER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : FlowController.h                                  */
/*                                                                  */
/* General       : This module contain function for init and
 * 				   communicate with flow controller                 */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/23      Quyen Ngo(MV)  	new file        	*/
/* #0001	   2017/08/30	   Quyen Ngo(MV)	modify				*/
/*	 - Change argument forflowCtrl_SendTestCmd(E_MsgDataId id,   	*/
/*	 - uint32_t data) same with the define on FlowController.c 		*/
/*	 - add prefix FC to ERR_BIT define (fix bug 1789)				*/
/*																	*/
/* #0002	   2017/09/07	   Quyen Ngo(MV)	modify				*/
/*	 - Delete flowCtrl_GetVerison									*/
/*	 - Add flowCtrl_Request send version							*/
/*	 - Remove function get reset status								*/
/* 			   2017/10/12	   Quyen Ngo(MV)    modify				*/
/* #0003	   Add flowCtrl_SetAllowRecordLogNoAlarmFlag(bool isAllow)*/
/*			  (Bug 1958) for allow record log when mode change      */
/* #0004		   2017/11/02	   Quyen Ngo(MV)    modify			*/
/* 	 - Add flowCtrl_ProcessPendingIpcLogMsg to handle the case that */
/*	 IPC queue is full(Bug 2007)									*/
/* #0005	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/*			  												        */
/********************************************************************/

#include "DualCoreCommon.h"


#define FC_PRESSURE_SENSOR_ERR_BIT 			1<<0
#define FC_ULTRASONIC_SENSOR_ERR_BIT 		1<<1
#define FC_TEMPERATURE_SENSOR_ERR_BIT 		1<<2
#define FC_CMD_TIMEOUT_ERR_BIT				1<<3
#define FC_RESET_NOTI_BIT					1<<4
#define FC_COMMUNICATION_ERR_BIT			1<<7

typedef enum
{
	FC_NORMAL_SEQ,
	FC_RESET_SEQ,
	FC_REBOOT_SEQ
}E_FlowCtrlSequence;

// read uart data return from flow controller
void flowCtrl_ReadFromDevice(void);

// init uart communication
void flowCtrl_InitCommunication(void);

// send command via UART
void flowCtrl_SendCmd(uint8_t code,const uint8_t parameter[]);

// send confirm cmd
void flowCtrl_SendComunicationConfirmCmd(void);

//send test flow cmd
void flowCtrl_SendTestFlowCmd(uint16_t flowVal);

// send test 02 cmd
void flowCtrl_SendTestO2Cmd(uint16_t o2Val);

void flowCtrl_SendTestCmd(E_MsgDataId id,  uint32_t data);

// send acquistion1 cmd
void flowCtrl_SendStatusAcquisition1Cmd(void);

// send acquistion2 cmd
void flowCtrl_SendStatusAcquisition2Cmd(void);

// send version cmd
void flowCtrl_SendVersionAcquisitionCmd(void);

// send err reset cmd
void flowCtrl_SendErrorStatusResetCmd(void);

// send flow rate cmd
void flowCtrl_SendFlowRateCmd(uint16_t flowVal);

//Request send flow rate cmd
void flowCtrl_RequestSetFlow(uint16_t flow);

//handle flow controller
void flowCtrl_Handle(void);

//reset flow controller
void flowCtrl_Reset(void);

// flow clear alarm status
void flowCtrl_ClearAlarmStatus(void);

// get flow value
uint16_t flowCtrl_GetFlowVal(void);

// get O2 value
uint16_t flowCtrl_GetO2Val(void);

uint8_t flowCtrl_getErrStatus(void);

void flowCtrl_Stop(void);

void flowCtrl_RequestSendVersion(void);

void flowCtrl_SetAllowRecordLogNoAlarmFlag(bool isAllow);

void flowCtrl_ProcessPendingIpcLogMsg(void);

#endif /* INC_FLOW_CONTROLLER_H_ */
