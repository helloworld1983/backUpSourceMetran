/********************************************************************/
/*                                                                  */
/* File Name     : FlowController.c                               	*/
/*                                                                  */
/* General       : Flow controller communication handling           */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/23      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/*		-add prefix FC of Err status of flow controller				*/
/*		-change s_version to static									*/
/*		-fix lay out file											*/
/* #0002	   2017/09/06	   Quyen Ngo	modify					*/
/*	 - Change to use QUEUE array to handle reques send command		*/
/*	 - flowCtrl_ProcessSendCmdRequest and argument to process send	*/
/*		or resend command. 											*/
/*	 - Delete flowCtrl_GetVerison									*/
/*	 - Delete variable, function not use							*/
/* #0003	   2017/09/15		Quyen Ngo 	modify					*/
/*	 - Does not reset the receive time and resend time when receive */
/*	   error packet (command err, check sum, param err) from flow 	*/
/*	   controller													*/
/*	 - reset interval time send command when back to normal sequence*/
/*     of flow controller										    */
/*	 - Force send cleare error status command when back to normal	*/
/*	   sequence, update s_isSend flag = true						*/
/*	 - Does not allow reset during reset sequence					*/
/*	 - Remove function get reset status								*/
/* #0004	   2017/09/19		Quyen Ngo	modify					*/
/*	 - Add check response code error when receiving unknown response*/
/*	 code.															*/
/* #0005	   2017/10/07		Quyen Ngo	modify					*/
/*	 - Add flowctrl_RequestRecordLog function, the request to record*/
/*	log of err not in alarm case will be prcess via this function	*/
/*	(For fix Bug 1958)												*/
/* #0006	   2017/10/07		Quyen Ngo	modify					*/
/*	 - Add check the argument for O2 test command not exceed 		*/
/*     FC_O2_VALUE_UP_THRESHOLD										*/
/* #0007	   2017/10/07		Quyen Ngo	modify					*/
/*	 - Add new status eFcErrProcessed to E_FlowCtrlNoAlarmStatus for*/
/*	   avoid the problem that the log No response does not record when*/
/*	  FC return some err code and does not return data				*/
/* 	   		   2017/10/12		Quyen Ngo	modify					*/
/* #0008	   -Clear s_fcNoAlarmStatus to eFcErrProcessed when back*/
/*	   			to normal communication after reset (Bug 1958)		*/
/* #0009	   -Add E_FcCommunicationState for state handling of 	*/
/*				FC communication (Bug 1958)							*/
/* #00010	   Add flowCtrl_SetAllowRecordLogNoAlarmFlag(bool isAllow)*/
/*			  (Bug 1958) for allow record log when mode change      */
/* 			   2017/10/20		Quyen Ngo	modify					*/
/* #0011	   -Change from send status acquistion to flow rate cmd */
/*			   for fix bug 1980										*/
/* #0012	   2017/11/02		Quyen Ngo	modify					*/
/* 	  Handle the case IPC queue full when flow controller send  	*/
/*	  request to record log											*/
/* #0013	   2017/11/17		Quyen Ngo	modify					*/
/* 	  Does not reverse data when send test command to FC (Bug 2031) */
/* #0014	   2017/12/13		Quyen Ngo	modify					*/
/* 	  Add void to non parameter function to follow coding rule 		*/
/*	  																*/
/********************************************************************/

#include <gpio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "util.h"
#include "TaskCommon.h"
#include "FlowController.h"
#include "PsaControl.h"
#include "LatchValve.h"
#include "Compressor.h"
#include "OxyCylinder.h"
#include "IpcMsg.h"
#include "IpcTask.h"
#include "ModeHandle.h"

#ifdef DEBUG
//#define DEBUG_FLOW_EN
#endif

#ifdef DEBUG_FLOW_EN
#define DEBUG_FLOW(...) printf(__VA_ARGS__)
#else
#define DEBUG_FLOW(...)
#endif
#define FC_OUTPUT_ARR_SIZE 			201
#define FC_UART_CHANEL				LPC_USART3
/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 				128	/* Send */
#define UART_RRB_SIZE 				32	/* Receive */
#define FC_BAUD_RATE				9600

#define CHECKSUM_ERR_BIT 		0x01
#define COMMAND_ERR_BIT 		0x02
#define PARAM_ERR_BIT 			0x04

#define FLOW_INTERVAL_TIME_SEND			1000/DEVICE_TASK_DELAY_TIME
#define MAX_TIME_OF_RECEIVE_CMD 		700/DEVICE_TASK_DELAY_TIME //(700ms/50ms = 14)
#define MAX_RESEND_CNT					5
#define FC_RESET_TIME_100MS				100/DEVICE_TASK_DELAY_TIME //(devTask loop 50ms 50ms*2 = 100ms: reset time)
#define FC_REBOOT_TIME1S            	1000/DEVICE_TASK_DELAY_TIME //1000ms/50ms
#define FC_RESPONSE_LENGTH 				9

#define FC_MAX_BYTES_RECEIVE 			16

#define SEL4_BYTE						'4'
#define SEL5_BYTE						'5'

#define FC_CONFIRM_RES 				'A'
#define FC_FLOW_RES					'B'
#define FC_STATUS_ACQ_RES 			'C'
#define FC_STATUS_ACQ2_RES 			'D'
#define FC_VERSION_RES				'E'
#define FC_ERR_STATUS_RESET_RES		'F'
#define FC_CMD_ERR_RES				'W'

#define FC_DATA_LENGTH					7
#define FC_QUEUE_LENGTH				16
#define FC_PARAM_LENGTH				4

#define FC_O2_VALUE_UP_THRESHOLD				100

typedef struct {
	uint8_t commandCode; //FC command code
	uint8_t parameter[FC_PARAM_LENGTH]; //FC command parameter
} FC_COMMAND_T;

typedef struct {
	bool status; //Has request or no
	FC_COMMAND_T command; //rtc request set value
} FC_POP_REQUEST_RETURN_T;

typedef enum
{
	FC_WAIT_TYPE,
	FC_WAIT_DATA,
	FC_WAIT_CHECKSUM
}E_FcReceiveDataState;

typedef enum
{
	FC_WAIT_RECEIVE_RESPONSE,
	FC_WAIT_SEND_CMD
}E_FcCommunicationState;

static uint8_t s_version[8] = {'0','0','0','0','0','0','0','\0'};

static uint16_t gs_flowMeasureVal = 0;

static uint16_t gs_O2Percent = 0;

/* Transmit and receive ring buffers */
static RINGBUFF_T gs_txringFc, gs_rxringFc;
/* Transmit and receive buffers */
static uint8_t gs_rxbuff[UART_RRB_SIZE], gs_txbuff[UART_SRB_SIZE];

static uint8_t gs_fcTimeReceiveCmd = 0;

static uint8_t gs_fcReSendCnt = 0;

static int16_t s_timeToSendCnt = -1;

static bool s_isResend = false;

//Flow Err status 8 bit value
//PRESSURE | ULTRASONIC | TEMPERATURE | CMD TIMEOUT | RESET NOTIFICATION | RESERVED | RESERVED | COMMUNICATION ERR
static uint8_t gs_fcStatus = 0x00;

static bool s_isAllowRecordErrNoAlarmLog = true;

static E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus = eFlowControllerNoErr;

static E_FlowCtrlSequence gs_fcSeq = FC_NORMAL_SEQ;

static uint8_t s_fcRebootTimeCnt = 0;

static uint8_t s_fcResetTimeCnt = 0;

static uint16_t s_currentFlowSet = 0;

static uint8_t s_responsePacket[FC_RESPONSE_LENGTH];
static uint8_t s_numOfRequestBytes = FC_RESPONSE_LENGTH;
static uint8_t s_pckId = 0;

static FC_COMMAND_T s_fcCommandQueueArr[FC_QUEUE_LENGTH];
static FC_COMMAND_T s_previousCmd;

static E_FcCommunicationState s_fcCommunicationState = FC_WAIT_SEND_CMD;

static IPC_MSG_DATA_t gs_stIpcFcLogMsgdata;

const uint8_t ConfirmCommunicationCode = '1';
const uint8_t FlowRateCode = '2';
const uint8_t StatusAcquisition1Code = '3';
const uint8_t StatusAcquisition2Code = '4';
const uint8_t VersionCode = '5';
const uint8_t ErrorStatCode = '6';
const uint8_t ComunicationConfirm[4] = {'Z','Z','Z','Z'};
const uint8_t StatusAcquisition[4] = {'Z','Z','Z','Z'};
const uint8_t VersionAcquisition[4] = {'Z','Z','Z','Z'};
const uint8_t ErrorStatus[4] = {'Z','Z','Z','Z'};

const uint16_t fcOutputSetting[FC_OUTPUT_ARR_SIZE] = {
		0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,
		100,105,110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,
		200,205,210,215,220,225,230,235,240,245,250,255,260,265,270,275,280,285,290,295,
		300,305,310,315,320,325,330,335,340,345,350,355,360,365,370,375,380,385,390,395,
		400,405,410,415,420,425,430,435,440,445,450,455,460,465,470,475,480,485,490,495,
		500,505,510,515,520,525,530,535,540,545,550,555,560,565,570,575,580,585,590,595,
		600,605,610,615,620,625,630,635,640,645,650,655,660,665,670,675,680,685,690,695,
		700,705,710,715,720,725,730,735,740,745,750,755,760,765,770,775,780,785,790,795,
		800,805,810,815,820,825,830,835,840,845,850,855,860,865,870,875,880,885,890,895,
		900,905,910,915,920,925,930,935,940,945,950,955,960,965,970,975,980,985,990,995,
		1000
};

const uint8_t FLOW_TABLE[FC_OUTPUT_ARR_SIZE][4] ={
		{'0','0','0','Z'},
		{'0','0','5','Z'},
		{'0','1','0','Z'},
		{'0','1','5','Z'},
		{'0','2','0','Z'},
		{'0','2','5','Z'},
		{'0','3','0','Z'},
		{'0','3','5','Z'},
		{'0','4','0','Z'},
		{'0','4','5','Z'},
		{'0','5','0','Z'},
		{'0','5','5','Z'},
		{'0','6','0','Z'},
		{'0','6','5','Z'},
		{'0','7','0','Z'},
		{'0','7','5','Z'},
		{'0','8','0','Z'},
		{'0','8','5','Z'},
		{'0','9','0','Z'},
		{'0','9','5','Z'},

		{'1','0','0','Z'},
		{'1','0','5','Z'},
		{'1','1','0','Z'},
		{'1','1','5','Z'},
		{'1','2','0','Z'},
		{'1','2','5','Z'},
		{'1','3','0','Z'},
		{'1','3','5','Z'},
		{'1','4','0','Z'},
		{'1','4','5','Z'},
		{'1','5','0','Z'},
		{'1','5','5','Z'},
		{'1','6','0','Z'},
		{'1','6','5','Z'},
		{'1','7','0','Z'},
		{'1','7','5','Z'},
		{'1','8','0','Z'},
		{'1','8','5','Z'},
		{'1','9','0','Z'},
		{'1','9','5','Z'},

		{'2','0','0','Z'},
		{'2','0','5','Z'},
		{'2','1','0','Z'},
		{'2','1','5','Z'},
		{'2','2','0','Z'},
		{'2','2','5','Z'},
		{'2','3','0','Z'},
		{'2','3','5','Z'},
		{'2','4','0','Z'},
		{'2','4','5','Z'},
		{'2','5','0','Z'},
		{'2','5','5','Z'},
		{'2','6','0','Z'},
		{'2','6','5','Z'},
		{'2','7','0','Z'},
		{'2','7','5','Z'},
		{'2','8','0','Z'},
		{'2','8','5','Z'},
		{'2','9','0','Z'},
		{'2','9','5','Z'},

		{'3','0','0','Z'},
		{'3','0','5','Z'},
		{'3','1','0','Z'},
		{'3','1','5','Z'},
		{'3','2','0','Z'},
		{'3','2','5','Z'},
		{'3','3','0','Z'},
		{'3','3','5','Z'},
		{'3','4','0','Z'},
		{'3','4','5','Z'},
		{'3','5','0','Z'},
		{'3','5','5','Z'},
		{'3','6','0','Z'},
		{'3','6','5','Z'},
		{'3','7','0','Z'},
		{'3','7','5','Z'},
		{'3','8','0','Z'},
		{'3','8','5','Z'},
		{'3','9','0','Z'},
		{'3','9','5','Z'},

		{'4','0','0','Z'},
		{'4','0','5','Z'},
		{'4','1','0','Z'},
		{'4','1','5','Z'},
		{'4','2','0','Z'},
		{'4','2','5','Z'},
		{'4','3','0','Z'},
		{'4','3','5','Z'},
		{'4','4','0','Z'},
		{'4','4','5','Z'},
		{'4','5','0','Z'},
		{'4','5','5','Z'},
		{'4','6','0','Z'},
		{'4','6','5','Z'},
		{'4','7','0','Z'},
		{'4','7','5','Z'},
		{'4','8','0','Z'},
		{'4','8','5','Z'},
		{'4','9','0','Z'},
		{'4','9','5','Z'},

		{'5','0','0','Z'},
		{'5','0','5','Z'},
		{'5','1','0','Z'},
		{'5','1','5','Z'},
		{'5','2','0','Z'},
		{'5','2','5','Z'},
		{'5','3','0','Z'},
		{'5','3','5','Z'},
		{'5','4','0','Z'},
		{'5','4','5','Z'},
		{'5','5','0','Z'},
		{'5','5','5','Z'},
		{'5','6','0','Z'},
		{'5','6','5','Z'},
		{'5','7','0','Z'},
		{'5','7','5','Z'},
		{'5','8','0','Z'},
		{'5','8','5','Z'},
		{'5','9','0','Z'},
		{'5','9','5','Z'},

		{'6','0','0','Z'},
		{'6','0','5','Z'},
		{'6','1','0','Z'},
		{'6','1','5','Z'},
		{'6','2','0','Z'},
		{'6','2','5','Z'},
		{'6','3','0','Z'},
		{'6','3','5','Z'},
		{'6','4','0','Z'},
		{'6','4','5','Z'},
		{'6','5','0','Z'},
		{'6','5','5','Z'},
		{'6','6','0','Z'},
		{'6','6','5','Z'},
		{'6','7','0','Z'},
		{'6','7','5','Z'},
		{'6','8','0','Z'},
		{'6','8','5','Z'},
		{'6','9','0','Z'},
		{'6','9','5','Z'},

		{'7','0','0','Z'},
		{'7','0','5','Z'},
		{'7','1','0','Z'},
		{'7','1','5','Z'},
		{'7','2','0','Z'},
		{'7','2','5','Z'},
		{'7','3','0','Z'},
		{'7','3','5','Z'},
		{'7','4','0','Z'},
		{'7','4','5','Z'},
		{'7','5','0','Z'},
		{'7','5','5','Z'},
		{'7','6','0','Z'},
		{'7','6','5','Z'},
		{'7','7','0','Z'},
		{'7','7','5','Z'},
		{'7','8','0','Z'},
		{'7','8','5','Z'},
		{'7','9','0','Z'},
		{'7','9','5','Z'},

		{'8','0','0','Z'},
		{'8','0','5','Z'},
		{'8','1','0','Z'},
		{'8','1','5','Z'},
		{'8','2','0','Z'},
		{'8','2','5','Z'},
		{'8','3','0','Z'},
		{'8','3','5','Z'},
		{'8','4','0','Z'},
		{'8','4','5','Z'},
		{'8','5','0','Z'},
		{'8','5','5','Z'},
		{'8','6','0','Z'},
		{'8','6','5','Z'},
		{'8','7','0','Z'},
		{'8','7','5','Z'},
		{'8','8','0','Z'},
		{'8','8','5','Z'},
		{'8','9','0','Z'},
		{'8','9','5','Z'},

		{'9','0','0','Z'},
		{'9','0','5','Z'},
		{'9','1','0','Z'},
		{'9','1','5','Z'},
		{'9','2','0','Z'},
		{'9','2','5','Z'},
		{'9','3','0','Z'},
		{'9','3','5','Z'},
		{'9','4','0','Z'},
		{'9','4','5','Z'},
		{'9','5','0','Z'},
		{'9','5','5','Z'},
		{'9','6','0','Z'},
		{'9','6','5','Z'},
		{'9','7','0','Z'},
		{'9','7','5','Z'},
		{'9','8','0','Z'},
		{'9','8','5','Z'},
		{'9','9','0','Z'},
		{'9','9','5','Z'},

		{'9','9','5','Z'}
};

static int8_t s_fcHead = - 1;
static int8_t s_fcTail = - 1;

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowctrl_RequestRecordLog											*/
/*                                                                                      */
/* Details:  Process the request record not alarm log, log is record when flow	ctrl	*/
/*			 not in abnormal alarm condition											*/
/*                                                                                      */
/* Arguments :	(I) E_FlowControllerNoAlarmStatus status			 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data :																	*/
/*					   (O) bool s_isAllowRecordErrNoAlarmLog							*/
/*					   (O) E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus			*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowctrl_RequestRecordLog(E_FlowControllerNoAlarmStatus status)
{
	DEBUGOUT("Log record: %d %d \n",s_isAllowRecordErrNoAlarmLog,s_fcErrNoAlarmStatus);
	if(s_fcErrNoAlarmStatus == eFlowControllerNoErr)
	{
		s_isAllowRecordErrNoAlarmLog = true;
	}
	if(s_isAllowRecordErrNoAlarmLog == true)
	{
		s_fcErrNoAlarmStatus = status;

		gs_stIpcFcLogMsgdata.data_id = eFcRequestRecordLog;
		memset(&gs_stIpcFcLogMsgdata.stAlarmStatPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stAlarmStatPacket));
		memset(&gs_stIpcFcLogMsgdata.stPsaParamPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaParamPacket));
		memset(&gs_stIpcFcLogMsgdata.stPsaMonitorPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaMonitorPacket));
		memset(&gs_stIpcFcLogMsgdata.version, 0, sizeof(gs_stIpcFcLogMsgdata.version));
		gs_stIpcFcLogMsgdata.data_value = (uint16_t)s_fcErrNoAlarmStatus;
		if (ipc_PushMsg(IPCEX_ID_GUI, gs_stIpcFcLogMsgdata) == QUEUE_INSERT) {
			DEBUG_FLOW("M0 Push data: %d\n",gs_stIpcFcLogMsgdata.data_id);
			gs_stIpcFcLogMsgdata.data_id = eNoDeviceEventId;
			gs_stIpcFcLogMsgdata.data_value = 0x00;
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_PushRequest													*/
/*                                                                                      */
/* Details:  Push the request into the s_fcCommandQueueArr								*/
/*                                                                                      */
/* Arguments :	(I) FC_COMMAND_T command							 					*/
/*                                                                                      */
/* Macros:	   		FC_QUEUE_LENGTH														*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : (I) FC_COMMAND_T s_fcCommandQueueArr								*/
/*					   (O) int8_t s_fcHead												*/
/*					   (O) int8_t s_fcTail												*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_PushRequest(FC_COMMAND_T command)
{
	if (s_fcHead == FC_QUEUE_LENGTH - 1)
		DEBUGOUT("Queue Overflow \n");
	else
	{
		if (s_fcTail == - 1)
			/*If queue is initially empty */
			s_fcTail = 0;
		DEBUGOUT("Inset the element in queue : ");
		s_fcHead = s_fcHead + 1;
		s_fcCommandQueueArr[s_fcHead].commandCode = command.commandCode;
		memcpy(s_fcCommandQueueArr[s_fcHead].parameter,command.parameter,FC_PARAM_LENGTH);
		DEBUGOUT("PARAM SET: %d %x %x %x %x \n",s_fcHead,s_fcCommandQueueArr[s_fcTail].parameter[0],
				s_fcCommandQueueArr[s_fcTail].parameter[1],
				s_fcCommandQueueArr[s_fcTail].parameter[2],
				s_fcCommandQueueArr[s_fcTail].parameter[3]);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_PopRequest													*/
/*                                                                                      */
/* Details:  Pop the request on the s_fcCommandQueueArr									*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FC_QUEUE_LENGTH														*/
/*																			 			*/
/* ReturnValue : 	FC_POP_REQUEST_RETURN_T												*/
/*                                                                                      */
/* Using Global Data : (I) FC_COMMAND_T s_fcCommandQueueArr								*/
/*					   (O) int8_t s_fcHead												*/
/*					   (O) int8_t s_fcTail												*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static FC_POP_REQUEST_RETURN_T flowCtrl_PopRequest()
{
	FC_POP_REQUEST_RETURN_T request;
	if (s_fcTail == - 1 || s_fcTail > s_fcHead)
	{
		//		DEBUGOUT("Queue Underflow \n");
		request.status = false;
	}
	else
	{
		request.command = s_fcCommandQueueArr[s_fcTail];
		request.status = true;
		DEBUGOUT("PARAM GET: %d %x %x %x %x \n",s_fcTail,s_fcCommandQueueArr[s_fcTail].parameter[0],
				s_fcCommandQueueArr[s_fcTail].parameter[1],
				s_fcCommandQueueArr[s_fcTail].parameter[2],
				s_fcCommandQueueArr[s_fcTail].parameter[3]);
		if(s_fcTail < FC_QUEUE_LENGTH - 1)
		{
			s_fcTail = s_fcTail + 1;
		}
		else
		{
			s_fcTail = -1;
			s_fcHead = -1;
		}
	}
	return request;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_ResetReceiveState											*/
/*                                                                                      */
/* Details:  Reset variable value for ready to receive packet							*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FC_RESPONSE_LENGTH													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t s_pckId = 0;  										*/
/*					   (O) uint8_t s_numOfRequestBytes									*/
/*					   (O) uint8_t s_responsePacket[COMP_RESPONSE_PCK_LENGTH]			*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_ResetReceiveState(void)
{
	s_pckId = 0;
	s_numOfRequestBytes = FC_RESPONSE_LENGTH;
	memset(s_responsePacket, 0, FC_RESPONSE_LENGTH);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_CreateCRC													*/
/*                                                                                      */
/* Details:  Reset variable value for ready to receive packet							*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FC_RESPONSE_LENGTH													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t s_pckId = 0;  										*/
/*					   (O) uint8_t s_numOfRequestBytes									*/
/*					   (O) uint8_t s_responsePacket[COMP_RESPONSE_PCK_LENGTH]			*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static uint8_t flowCtrl_CreateCRC(unsigned char data[], char nbrOfBytes)
{
	uint8_t sum = 0;
	for(int i = 0; i < nbrOfBytes; i ++)
	{
		sum ^= data[i];
	}
	return sum;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_HandleResetNoti												*/
/*                                                                                      */
/* Details:  Handle reset notification: record log and reset flow controller			*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus           */
/*					   (O) E_FlowCtrlRequestCmd s_RequestSendCmd 						*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResetNoti(void)
{
	DEBUG_FLOW("Handle reset  \n");

	s_fcErrNoAlarmStatus = eFlowReset;

	gs_stIpcFcLogMsgdata.data_id = eFcRequestRecordLog;
	memset(&gs_stIpcFcLogMsgdata.stAlarmStatPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stAlarmStatPacket));
	memset(&gs_stIpcFcLogMsgdata.stPsaParamPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaParamPacket));
	memset(&gs_stIpcFcLogMsgdata.stPsaMonitorPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaMonitorPacket));
	memset(&gs_stIpcFcLogMsgdata.version, 0, sizeof(gs_stIpcFcLogMsgdata.version));
	gs_stIpcFcLogMsgdata.data_value = (uint16_t)s_fcErrNoAlarmStatus;
	if (ipc_PushMsg(IPCEX_ID_GUI, gs_stIpcFcLogMsgdata) == QUEUE_INSERT) {
		DEBUG_FLOW("M0 Push data: %d\n",gs_stIpcFcLogMsgdata.data_id);
		gs_stIpcFcLogMsgdata.data_id = eNoDeviceEventId;
		gs_stIpcFcLogMsgdata.data_value = 0x00;
	}
	switch (mode_GetCurrentMode()) {
	case ePatientMode:
	case eServiceMode:
		flowCtrl_Reset();
		flowCtrl_RequestSetFlow(s_currentFlowSet);
		break;
	case eStandyMode:
		flowCtrl_Reset();
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_BackToNormalSeq												*/
/*                                                                                      */
/* Details:  Reset some communication status when back to normal sequence, set correct	*/
/*			 request to send next command to FC based on previous command				*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus           */
/*					   (O) E_FlowCtrlRequestCmd s_RequestSendCmd 						*/
/*                     (O) uint8_t gs_fcTimeReceiveCmd									*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_BackToNormalSeq(void)
{
	DEBUG_FLOW("Back to normal\n");
	RingBuffer_Flush(&gs_rxringFc);
	RingBuffer_Flush(&gs_txringFc);

	gs_fcSeq = FC_NORMAL_SEQ;
	s_fcResetTimeCnt = 0;

	//Force to send clear err status command, not based on the priority on QUEUE
	s_timeToSendCnt = 0;
	gs_fcTimeReceiveCmd = 1;//Allow time receive count
	flowCtrl_ResetReceiveState();
	s_isResend = false;

	s_previousCmd.commandCode = ErrorStatCode;
	memcpy(s_previousCmd.parameter,ErrorStatus,FC_PARAM_LENGTH);
	flowCtrl_SendCmd(ErrorStatCode,ErrorStatus);
	s_fcCommunicationState = FC_WAIT_RECEIVE_RESPONSE;
	s_fcErrNoAlarmStatus = eFcErrProcessed;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_ProcessSendCmdRequest										*/
/*                                                                                      */
/* Details:  Depend on the type of request, send matching command to flow controller	*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	bool true: sent cmd; false: not yet send			   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FlowCtrlRequestCmd s_preRequestSendCmd         		    */
/*					   (I) E_FlowCtrlRequestCmd s_RequestSendCmd 						*/
/*                     (O) uint8_t gs_fcTimeReceiveCmd									*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static bool flowCtrl_ProcessSendCmdRequest(bool isResend)
{
	bool ret = true;
	if((s_timeToSendCnt >= FLOW_INTERVAL_TIME_SEND) || (s_timeToSendCnt == -1))
	{
		if(isResend == false)
		{
			s_timeToSendCnt = 0;
			gs_fcTimeReceiveCmd = 1;//Allow time receive count
			flowCtrl_ResetReceiveState();
			DEBUG_FLOW("POP request \n");
			FC_POP_REQUEST_RETURN_T request = flowCtrl_PopRequest();
			if (request.status ==true)
			{
				s_previousCmd.commandCode = request.command.commandCode;
				memcpy(s_previousCmd.parameter,request.command.parameter,FC_PARAM_LENGTH);
				flowCtrl_SendCmd(request.command.commandCode,request.command.parameter);
			}
			else
			{
				//if QUUEUE is empty, send flow rate command
				size_t idx;
				idx = find_index(fcOutputSetting,FC_OUTPUT_ARR_SIZE,s_currentFlowSet);
				if (idx != -1)
				{
					DEBUG_FLOW("Set flow idx: %d \n",idx);
					s_previousCmd.commandCode = FlowRateCode;
					memcpy(s_previousCmd.parameter,FLOW_TABLE[idx],FC_PARAM_LENGTH);
					flowCtrl_SendCmd(FlowRateCode,FLOW_TABLE[idx]);
				}
			}
		}
		else
		{
			s_timeToSendCnt = 0;
			gs_fcTimeReceiveCmd = 1;//Allow time receive count
			flowCtrl_ResetReceiveState();
			flowCtrl_SendCmd(s_previousCmd.commandCode,s_previousCmd.parameter);
		}
	}
	else
	{
		ret = false;
		s_timeToSendCnt++;
	}
	return ret;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_HandleResendCmd												*/
/*                                                                                      */
/* Details:  Resend command if time out(+reord log) and if the number of resend times is*/
/*	over MAX value, change the err status to FC_COMMUNICATION_ERR_BIT					*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t gs_fcReSendCnt 					         		    */
/*					   (O) E_FlowCtrlRequestCmd s_RequestSendCmd 						*/
/*					   (I) E_FlowCtrlRequestCmd s_RequestSendCmd 						*/
/*                     (O) E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus			*/
/*					   (O) uint8_t gs_fcStatus 											*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResendCmd(void)
{
	if(gs_fcReSendCnt < MAX_RESEND_CNT)
	{
		DEBUG_FLOW("Resend \n");
		gs_fcReSendCnt++;

		if((s_fcErrNoAlarmStatus != eResponseCodeErr)
				&& (s_fcErrNoAlarmStatus != eResponseCheckSumErr)
				&& (s_fcErrNoAlarmStatus != eResponseLengthErr)
				&& (s_fcErrNoAlarmStatus != eResponseParamErr)
				&& (s_fcErrNoAlarmStatus != eChecksumErr)
				&& (s_fcErrNoAlarmStatus != eCommandErr)
				&& (s_fcErrNoAlarmStatus != eParamErr)
		)
		{
			flowctrl_RequestRecordLog(eNoResponseErr);
		}
		else
		{
			s_fcErrNoAlarmStatus = eFcErrProcessed;
		}
	}
	else
	{
		DEBUG_FLOW("Err FC \n");
		gs_fcStatus = gs_fcStatus | FC_COMMUNICATION_ERR_BIT;
		s_isAllowRecordErrNoAlarmLog = false;
		gs_fcReSendCnt = 0;
		flowCtrl_Reset();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_HandleNormalSeq												*/
/*                                                                                      */
/* Details:  Handle normal communication with flow controller, read and send/resend CMD */
/*	to flow controller																	*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   	MAX_TIME_OF_RECEIVE_CMD													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (I) uint8_t gs_fcTimeReceiveCmd					         	    */
/*					   (O)  uint8_t gs_fcTimeReceiveCmd									*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleNormalSeq(void)
{
	if(s_fcCommunicationState == FC_WAIT_RECEIVE_RESPONSE)
	{
		flowCtrl_ReadFromDevice();
	}
	if(gs_fcTimeReceiveCmd >= MAX_TIME_OF_RECEIVE_CMD)
	{
		gs_fcTimeReceiveCmd = 0;
		flowCtrl_HandleResendCmd();
		s_isResend = true;

	}
	if (flowCtrl_ProcessSendCmdRequest(s_isResend) == true)
	{
		//already send cmd, start new cycle
		s_isResend = false;
		s_fcCommunicationState = FC_WAIT_RECEIVE_RESPONSE;
	}
	if(gs_fcTimeReceiveCmd > 0)
	{
		gs_fcTimeReceiveCmd++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_HandleResetSeq												*/
/*                                                                                      */
/* Details:  Reset FC by set reset signal to low in 100MS								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   	FC_RESET_TIME_100MS														*/
/*				FC_RESET_PORT_NUM														*/
/*				FC_RESET_BIT_NUM														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t s_fcResetTimeCnt 					         	    */
/*					   (O) E_FlowCtrlSequence gs_fcSeq									*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResetSeq(void)
{
	s_fcResetTimeCnt++;
	if(s_fcResetTimeCnt > FC_RESET_TIME_100MS)
	{
		s_fcResetTimeCnt = 0;
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM);
		gs_fcSeq = FC_REBOOT_SEQ;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_HandleRebootSeq												*/
/*                                                                                      */
/* Details:  After reset, need wait 1S before send command							    */
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   	FC_REBOOT_TIME1S														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*					   (O) uint8_t s_fcRebootTimeCnt									*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleRebootSeq(void)
{
	DEBUG_FLOW("Reboot \n");
	s_fcRebootTimeCnt++;
	if(s_fcRebootTimeCnt >= FC_REBOOT_TIME1S)
	{
		s_fcRebootTimeCnt = 0;
		flowCtrl_BackToNormalSeq();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Uart3 IRQ handle			                                           		    */
/*                                                                                      */
/* Details  -Handle interrupt for Uart 3		                 						*/
/*											*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void UART3_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(FC_UART_CHANEL, &gs_rxringFc, &gs_txringFc);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Intit flow controller			                                            */
/*                                                                                      */
/* Details  -Init Flow data struct
 * 			-Init uart config for flow controller
 * 			-Create FCdata mutex					             						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/

void flowCtrl_InitCommunication(void)
{
	Board_UART_Init(FC_UART_CHANEL);

	//Set up reset signal
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FC_RESET_PORT_NUM, FC_RESET_BIT_NUM);		/* output */
	// Reset signal usually set high
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM); // Enable power for flow controller

	/* Setup UART for 9600K8N1 */
	Chip_UART_DeInit(FC_UART_CHANEL);
	Chip_UART_Init(FC_UART_CHANEL);
	Chip_UART_SetBaud(FC_UART_CHANEL, FC_BAUD_RATE);
	Chip_UART_ConfigData(FC_UART_CHANEL, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(FC_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(FC_UART_CHANEL);

	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&gs_rxringFc, gs_rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&gs_txringFc, gs_txbuff, 1, UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(FC_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(FC_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

	NVIC_SetPriority(USART3_IRQn, FC_USART3_INT_PRIO);
	NVIC_EnableIRQ(USART3_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General flowCtrl_SendCmd		                       			                        */
/*                                                                                      */
/* Details  -Send commnd to flow controller via UART									*/
/*                                                                                      */
/* Arguments : (I) uint8_t code,					 					  				*/
/*			   (I) const uint8_t parameter[]											*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendCmd(uint8_t code,const uint8_t parameter[])
{
	uint8_t cmd[6];
	cmd[0] = code;
	cmd[1] = parameter[0];
	cmd[2] = parameter[1];
	cmd[3] = parameter[2];
	cmd[4] = parameter[3];
	cmd[5] = flowCtrl_CreateCRC(cmd,5);
	DEBUG_FLOW("Cmd: %x %x %x %x %x\n",cmd[0],cmd[1],cmd[2],cmd[3],cmd[4]);
	RingBuffer_Flush(&gs_txringFc);
	RingBuffer_Flush(&gs_rxringFc);
	Chip_UART_SendRB(FC_UART_CHANEL, &gs_txringFc, cmd, 6);
	return;
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: flowCtrl_SendTestFlowCmd()
//
//    Processing:
//
//    Input Parameters: uint16_t flowVal
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//
//
//    Requirements:
//
//******************************************************************************
void flowCtrl_SendTestFlowCmd(uint16_t flowVal)
{
	uint8_t  testFlowRateCmdParam[4];

	itoa(flowVal / 100, (char*)&testFlowRateCmdParam[0], 10);
	itoa((flowVal % 100) / 10, (char*)&testFlowRateCmdParam[1], 10);
	itoa((flowVal %100) % 10, (char*)&testFlowRateCmdParam[2], 10);
	testFlowRateCmdParam[3] = '0';

	DEBUGOUT("%s \n",testFlowRateCmdParam);
	FC_COMMAND_T command;
	command.commandCode = ConfirmCommunicationCode;
	memcpy(command.parameter,testFlowRateCmdParam,FC_PARAM_LENGTH);
	flowCtrl_PushRequest(command);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: flowCtrl_SendTestCmd
//
//    Detail: Send test command to flow controller based on setting on Development mode
//
//    Arguments: (I) E_MsgDataId id,
//				 Range: {eFlowControllerTest1Id,eFlowControllerTest2Id}
//				 (I)  uint16_t data
//				  Range: 0x00 ~ 0xFF
//				 (O)
//
//    Return Values: bool
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_SendTestCmd(E_MsgDataId id,  uint32_t data)
{
	//check argument
	uint8_t fithByte = 0;
	switch (id) {
	case eFlowControllerTest1Id:
		fithByte = SEL4_BYTE;
		DEBUG_FLOW("test 1 %x \n",data);
		break;
	case eFlowControllerTest2Id:
		fithByte = SEL5_BYTE;
		DEBUG_FLOW("test 2 %x \n",data);
		break;
	default:
		DEBUGOUT("Invalid ID \n");
		return;
		break;
	}
	if(data > 0xFF)
	{
		DEBUGOUT("Data out of range \n");
		return;
	}
	//decode data
	uint8_t revData = (uint8_t)(data & 0xFF);
	uint8_t param[FC_PARAM_LENGTH];
	param[0] = CharToAscci(revData & 0x0F);
	param[1] = CharToAscci((revData & 0xF0)>>4);
	param[2] = 'Z';
	param[3] = fithByte;
	FC_COMMAND_T command;
	command.commandCode = ConfirmCommunicationCode;
	memcpy(command.parameter,param,FC_PARAM_LENGTH);
	flowCtrl_PushRequest(command);
	return;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: flowCtrl_SendTestFlowCmd()
//
//    Processing:
//
//    Input Parameters: uint16_t o2Val
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//
//
//    Requirements:
//
//******************************************************************************
void flowCtrl_SendTestO2Cmd(uint16_t o2Val)
{
	if(o2Val >= FC_O2_VALUE_UP_THRESHOLD)
	{
		//exceed max O2% value for test command
		return;
	}
	uint16_t o2ValSacle = o2Val * 10;
	uint8_t  testO2CmdParam[FC_PARAM_LENGTH];
	itoa(o2ValSacle / 100,(char*)&testO2CmdParam[0],10);
	itoa((o2ValSacle % 100) / 10,(char*)&testO2CmdParam[1],10);
	itoa((o2ValSacle %100) % 10,(char*)&testO2CmdParam[2],10);
	testO2CmdParam[3] = '1';

	DEBUGOUT("%s \n",testO2CmdParam);

	FC_COMMAND_T command;
	command.commandCode = ConfirmCommunicationCode;
	memcpy(command.parameter,testO2CmdParam,FC_PARAM_LENGTH);
	flowCtrl_PushRequest(command);
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: flowCtrl_AnalyzePacket
//
//    Detail: Analyze packet data send from Flow controller
//
//    Arguments: (I) uint8_t* packet
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_AnalyzePacket(uint8_t* dataBuf)
{
	uint8_t flowRate[5] = {'0','.','0','0','\0'};
	uint8_t O2Conct[5] = {'0','0','.','0','\0'};
	switch (dataBuf[0]){
	case FC_CONFIRM_RES:
		DEBUGOUT("Communication Confirm \n");
		for (int j = 1; j <= FC_DATA_LENGTH; j ++ )
		{
			if(dataBuf[j] != 'R')
			{
				flowctrl_RequestRecordLog(eResponseParamErr);

				break;
			}
			if(s_fcErrNoAlarmStatus != eResponseParamErr)
			{
				s_fcErrNoAlarmStatus = eFlowControllerNoErr;
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
			}
		}
		break;
	case FC_FLOW_RES:
		//byte 1,2,3 --> Flow rate
		flowRate[0] = dataBuf[1];
		flowRate[2] = dataBuf[2];
		flowRate[3] = dataBuf[3];
		gs_flowMeasureVal = HexCharToInt(flowRate[0]) * 100 + HexCharToInt(flowRate[2])  *10 +
				HexCharToInt(flowRate[3]); //scale 100
		DEBUG_FLOW("Flow %d \n",gs_flowMeasureVal);
		//byte 4,5,6 --> O2 Concentration
		O2Conct[0] = dataBuf[4];
		O2Conct[1] = dataBuf[5];
		O2Conct[3] = dataBuf[6];
		gs_O2Percent = HexCharToInt(O2Conct[0]) * 1000 + HexCharToInt(O2Conct[1])  *100 +
				HexCharToInt(O2Conct[3]) * 10; //scale 100
		DEBUG_FLOW("O2 %d \n",gs_O2Percent);

		DEBUG_FLOW("Err status: %x \n",dataBuf[7]);

		//Check err status
		if (dataBuf[7] & FC_PRESSURE_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_PRESSURE_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_ULTRASONIC_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_ULTRASONIC_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_TEMPERATURE_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_TEMPERATURE_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_CMD_TIMEOUT_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_CMD_TIMEOUT_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_RESET_NOTI_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_RESET_NOTI_BIT;
			if(gs_fcStatus == FC_RESET_NOTI_BIT)
			{
				flowCtrl_HandleResetNoti();
			}
		}

		gs_fcTimeReceiveCmd = 0;
		gs_fcReSendCnt = 0;
		s_fcErrNoAlarmStatus = eFlowControllerNoErr;

		break;
	case FC_STATUS_ACQ_RES:
		//byte 1,2,3 --> Flow rate
		flowRate[0] = dataBuf[1];
		flowRate[2] = dataBuf[2];
		flowRate[3] = dataBuf[3];
		gs_flowMeasureVal = HexCharToInt(flowRate[0]) * 100 + HexCharToInt(flowRate[2])  *10 +
				HexCharToInt(flowRate[3]); //scale 100
		DEBUG_FLOW("Flow %d \n",gs_flowMeasureVal);

		//byte 4,5,6 --> O2 Concentration
		O2Conct[0] = dataBuf[4];
		O2Conct[1] = dataBuf[5];
		O2Conct[3] = dataBuf[6];
		gs_O2Percent = HexCharToInt(O2Conct[0]) * 1000 + HexCharToInt(O2Conct[1])  *100 +
				HexCharToInt(O2Conct[3]) * 10; //scale 100
		DEBUG_FLOW("O2 %d \n",gs_O2Percent);
		DEBUG_FLOW("Err status: %x \n",dataBuf[7]);
		//Check err status
		//Check err status
		if (dataBuf[7] & FC_PRESSURE_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_PRESSURE_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_ULTRASONIC_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_ULTRASONIC_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_TEMPERATURE_SENSOR_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_TEMPERATURE_SENSOR_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_CMD_TIMEOUT_ERR_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_CMD_TIMEOUT_ERR_BIT;
			s_isAllowRecordErrNoAlarmLog = false;
			flowCtrl_Stop();
		}
		if(dataBuf[7] & FC_RESET_NOTI_BIT)
		{
			gs_fcStatus = gs_fcStatus | FC_RESET_NOTI_BIT;
			if(gs_fcStatus == FC_RESET_NOTI_BIT)
			{
				flowCtrl_HandleResetNoti();
			}
		}

		gs_fcTimeReceiveCmd = 0;
		gs_fcReSendCnt = 0;
		s_fcErrNoAlarmStatus = eFlowControllerNoErr;
		break;
	case FC_STATUS_ACQ2_RES:
		DEBUG_FLOW("Status 2 Acquisition \n");
		gs_fcTimeReceiveCmd = 0;
		gs_fcReSendCnt = 0;
		s_fcErrNoAlarmStatus = eFlowControllerNoErr;
		break;
	case FC_VERSION_RES:
		DEBUG_FLOW("Version Acquisition \n");
		s_version[0] = dataBuf[1];
		s_version[1] = dataBuf[2];
		s_version[2] = dataBuf[3];
		s_version[3] = dataBuf[4];
		s_version[4] = dataBuf[5];
		s_version[5] = dataBuf[6];
		s_version[6] = dataBuf[7];
		//				DEBUG_FLOW("%s \n",s_version);
		DEBUGOUT("flow version %s \n",s_version);

		gs_stIpcFcLogMsgdata.data_id = eFLowVersion;
		gs_stIpcFcLogMsgdata.data_value = 0;
		memset(&gs_stIpcFcLogMsgdata.stAlarmStatPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stAlarmStatPacket));
		memset(&gs_stIpcFcLogMsgdata.stPsaParamPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaParamPacket));
		memset(&gs_stIpcFcLogMsgdata.stPsaMonitorPacket, 0, sizeof(gs_stIpcFcLogMsgdata.stPsaMonitorPacket));
		memcpy(gs_stIpcFcLogMsgdata.version, s_version,8);
		if (ipc_PushMsg(IPCEX_ID_GUI, gs_stIpcFcLogMsgdata) == QUEUE_INSERT) {
			gs_stIpcFcLogMsgdata.data_id = eNoDeviceEventId;
			gs_stIpcFcLogMsgdata.data_value = 0x00;
		}
		s_fcErrNoAlarmStatus = eFlowControllerNoErr;
		gs_fcTimeReceiveCmd = 0;
		gs_fcReSendCnt = 0;
		break;
	case FC_ERR_STATUS_RESET_RES:
		DEBUG_FLOW("Error Status Reset \n");
		for (int j = 1; j < 7; j ++ )
		{
			if(dataBuf[j] != 'R')
			{
				flowctrl_RequestRecordLog(eResponseParamErr);
				break;
			}
		}
		if(s_fcErrNoAlarmStatus != eResponseParamErr)
		{
			s_fcErrNoAlarmStatus = eFlowControllerNoErr;
			gs_fcTimeReceiveCmd = 0;
			gs_fcReSendCnt = 0;
		}
		break;
	case FC_CMD_ERR_RES:
		DEBUG_FLOW("Error %x \n",dataBuf[1]);
		//Check err status
		if (dataBuf[1] & CHECKSUM_ERR_BIT)
		{
			flowctrl_RequestRecordLog(eChecksumErr);
		}
		else if(dataBuf[1] & COMMAND_ERR_BIT)
		{
			flowctrl_RequestRecordLog(eCommandErr);
		}
		else if(dataBuf[1] & PARAM_ERR_BIT)
		{
			flowctrl_RequestRecordLog(eParamErr);
		}
		break;
	default:
	{
		flowctrl_RequestRecordLog(eResponseCodeErr);
		break;
	}
	}
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ReadFromDevice(void)
//
//    Processing:
//
//    Input Parameters:
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//
//
//    Requirements:
//
//******************************************************************************
void flowCtrl_ReadFromDevice(void)
{
	uint8_t dataRecv[FC_MAX_BYTES_RECEIVE];

	int n = Chip_UART_ReadRB(FC_UART_CHANEL, &gs_rxringFc, dataRecv, FC_MAX_BYTES_RECEIVE);
	if ((n > 0) && (n < s_numOfRequestBytes))
	{
		for (int i = 0; i < n; i++)
		{
			s_responsePacket[s_pckId + i] = dataRecv[i];
		}
		s_pckId = s_pckId + n;
		s_numOfRequestBytes = s_numOfRequestBytes - n;
	}
	else if(n == s_numOfRequestBytes)
	{
		for (int i = 0; i < n; i++)
		{
			s_responsePacket[s_pckId + i] = dataRecv[i];
		}
		//receive enough data for packet, check sum and analyze data
		if(flowCtrl_CreateCRC(s_responsePacket,FC_RESPONSE_LENGTH) == 0)
		{
			flowCtrl_AnalyzePacket(s_responsePacket);
		}
		else
		{
			flowctrl_RequestRecordLog(eResponseCheckSumErr);
		}
		flowCtrl_ResetReceiveState();
		s_fcCommunicationState = FC_WAIT_SEND_CMD;
	}
	else if(n > s_numOfRequestBytes)
	{
		flowctrl_RequestRecordLog(eResponseLengthErr);
		flowCtrl_ResetReceiveState();
		s_fcCommunicationState = FC_WAIT_SEND_CMD;
	}

	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_Handle
//
//    Detail: Handle communication with flow controller
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_Handle(void)
{
	flowCtrl_ProcessPendingIpcLogMsg();
	switch (gs_fcSeq) {
	case FC_NORMAL_SEQ:
		flowCtrl_HandleNormalSeq();
		break;
	case FC_REBOOT_SEQ:
		flowCtrl_HandleRebootSeq();
		break;
	case FC_RESET_SEQ:
		flowCtrl_HandleResetSeq();
		break;
	default:
		DEBUGOUT("FC invalid sequence \n");
		break;
	}
	return;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: flowCtrl_GetFlowVal()
//
//    Processing:
//
//    Input Parameters:
//    Output Parameters: None
//    Return Values:     uint16_t flowMesureVal
//    Pre-Conditions:
//
//
//    Requirements:
//
//******************************************************************************
uint16_t flowCtrl_GetFlowVal(void)
{
	return gs_flowMeasureVal;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: flowCtrl_GetO2Val()
//
//    Processing:
//
//    Input Parameters:
//    Output Parameters: None
//    Return Values:     uint16_t O2Percent
//    Pre-Conditions:
//
//
//    Requirements:
//
//******************************************************************************
uint16_t flowCtrl_GetO2Val(void)
{
	return gs_O2Percent;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_getErrStatus
//
//    Detail: Get err status
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: E_FlowControllerStatus
//
//    Using Global Data : (I)
//						  (O) gs_fcStatus
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
uint8_t flowCtrl_getErrStatus(void)
{
	//	DEBUG_FLOW("gs_fcStatus: %d\n",gs_fcStatus);
	return gs_fcStatus;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_ClearAlarmStatus
//
//    Detail: Clear alarm status
//
//    Arguments: (I)
//     (O)
//
//    Return Values: NONE
//
//    Using Global Data : (I)
//        (O)  gs_fcStatus
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_ClearAlarmStatus(void)
{
	gs_fcStatus = 0x00;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_Reset
//
//    Detail: reset flow controller
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: E_FlowControllerStatus
//
//    Using Global Data : (I)
//						  (O) gs_fcStatus
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_Reset(void)
{
	if(gs_fcSeq != FC_RESET_SEQ)
	{
		// Set reset signal low 100ms for reset fc
		DEBUG_FLOW("FC Reset \n");
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM);
		gs_fcSeq = FC_RESET_SEQ;
		s_fcResetTimeCnt = 0;
		s_fcRebootTimeCnt = 0;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_RequestSetFlow
//
//    Detail: request set flow of flow controller, flowCtrl_Handle() function will process this request
//
//    Arguments: (I)
//     (O)
//
//    Return Values: NONE
//
//    Using Global Data :
//        (O) s_RequestSendCmd
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_RequestSetFlow(uint16_t flow)
{
	s_currentFlowSet = flow;
	FC_COMMAND_T command;
	size_t idx;
	idx = find_index(fcOutputSetting,FC_OUTPUT_ARR_SIZE,flow);
	if (idx != -1)
	{
		DEBUGOUT("Set flow idx: %d \n",idx);
		command.commandCode = FlowRateCode;
		memcpy(command.parameter,FLOW_TABLE[idx],FC_PARAM_LENGTH);
		flowCtrl_PushRequest(command);
	}
	else
	{
		DEBUGOUT("Err FC setting: %d \n",flow);
	}
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: flowCtrl_Stop
//
//    Detail: stop flow controller
//
//    Arguments: (I)
//     (O)
//
//    Return Values: NONE
//
//    Using Global Data : (I)
//        (O) E_CompressorSequence s_compressorSeq
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void flowCtrl_Stop(void)
{
	DEBUG_FLOW("Flow ctrl stop \n");
	gs_fcTimeReceiveCmd = 0; //reset for call send CMD, not resend when jump to normal sequence
	s_fcResetTimeCnt = 0;
	flowCtrl_RequestSetFlow(0);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_RequestSendVersion											*/
/*                                                                                      */
/* Details:  Push the request send version												*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FC_PARAM_LENGTH														*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_RequestSendVersion(void)
{
	FC_COMMAND_T command;
	command.commandCode = VersionCode;
	memcpy(command.parameter,VersionAcquisition,FC_PARAM_LENGTH);
	flowCtrl_PushRequest(command);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_SetAllowRecordLogNoAlarmFlag								*/
/*                                                                                      */
/* Details:  Set allow record log flag when mode change Operaion<->Standby				*/
/*                                                                                      */
/* Arguments :		(I)	bool isAllow													*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : (O) bool s_isAllowRecordErrNoAlarmLog							*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SetAllowRecordLogNoAlarmFlag(bool isAllow)
{
	s_isAllowRecordErrNoAlarmLog = isAllow;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  flowCtrl_ProcessPendingIpcLogMsg										*/
/*                                                                                      */
/* Details:  Resends ipc log record  message if previous send is failed					*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																						*/
/* ReturnValue : 		 																*/
/*                                                                                      */
/* Using Global Data : 	(I)	IPC_MSG_DATA_t gs_stIpcFcLogMsgdata						*/
/*					   																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_ProcessPendingIpcLogMsg(void)
{
	if(gs_stIpcFcLogMsgdata.data_id != eNoDeviceEventId)
	{
		if (ipc_PushMsg(IPCEX_ID_GUI, gs_stIpcFcLogMsgdata) != QUEUE_INSERT) {
			DEBUGOUT("M0 Push data Err: %d\n",gs_stIpcFcLogMsgdata.data_id);
		}
		else
		{
			gs_stIpcFcLogMsgdata.data_id = eNoDeviceEventId;
			gs_stIpcFcLogMsgdata.data_value = 0x00;
		}
	}
	return;
}
