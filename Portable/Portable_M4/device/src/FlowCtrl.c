/********************************************************************/
/*                                                                  */
/* File Name     : FlowCtrol.c		                               	*/
/*                                                                  */
/* General       : This module contain function for initialize and	*/
/* 				   communicate with flow controller                 */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/23      Quyen Ngo  	new file       		    */
/*																	*/
/*                                                                  */
/********************************************************************/

#include <assert.h>
#include <FlowCtrl.h>
#include <Gpio.h>
#include <string.h>
#include <stdlib.h>
#include <TaskCommon.h>
#include "board.h"
#include "util.h"
const uint16_t g_FlowRateTable[eFlowRateLast]={15,25,50,75,100,125,150,175,200,225,250,275,300,0};//day change 15-sept

//#define DEBUG_FLOW_EN
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

#define PRESSURE_SENSOR_ERR_BIT 		1<<0
#define ULTRASONIC_SENSOR_ERR_BIT 		1<<1
#define TEMPERATURE_SENSOR_ERR_BIT 		1<<2
#define CMD_TIMEOUT_ERR_BIT				1<<3
#define RESET_NOTI_BIT					1<<4
#define COMMUNICATION_ERR_BIT			1<<7

#define CHECKSUM_ERR_BIT 		1<<0
#define COMMAND_ERR_BIT 		1<<1
#define PARAM_ERR_BIT 			1<<2

#define FLOW_INTERVAL_TIME_SEND			1000/DEVICE_TASK_DELAY_TIME
#define MAX_TIME_OF_RECEIVE_CMD 		700/DEVICE_TASK_DELAY_TIME //(700ms/50ms = 14)
#define MAX_RESEND_CNT					5
#define FC_RESET_TIME_100MS				100/DEVICE_TASK_DELAY_TIME //(devTask loop 50ms 50ms*2 = 100ms: reset time)
#define FC_REBOOT_TIME1S            	1000/DEVICE_TASK_DELAY_TIME //1000ms/50ms
#define FC_RESPONSE_LENGTH 				9

const unsigned char ConfirmCommunicationCode = '1';
const unsigned char FlowRateCode = '2';
const unsigned char StatusAcquisition1Code = '3';
const unsigned char StatusAcquisition2Code = '4';
const unsigned char VersionCode = '5';
const unsigned char ErrorStatCode = '6';
const unsigned char ModeSettingCode = '7';
const unsigned char OpenAndCloseSettingCode = '8';
const unsigned char ComunicationConfirm[4] = {'Z','Z','Z','Z'};
const unsigned char StatusAcquisition[4] = {'Z','Z','Z','Z'};
const unsigned char VersionAcquisition[4] = {'Z','Z','Z','Z'};
const unsigned char ErrorStatus[4] = {'Z','Z','Z','Z'};
const unsigned char ModeSettingContinuous[4] = {'0','Z','Z','Z'};
const unsigned char ModeSettingSynchronized[4] = {'1','Z','Z','Z'};
unsigned char OpenSetting[4] = {'Z','Z','Z','Z'};
unsigned char s_version[8] = {'0','0','0','0','0','0','0','\0'};

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

const unsigned char FLOW_TABLE[FC_OUTPUT_ARR_SIZE][4] ={
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

static uint16_t gs_flowMeasureVal = 0;

static uint16_t gs_O2Percent = 0;

static uint16_t gs_temperature = 0;

static uint16_t gs_pressure = 0;
static RINGBUFF_T gs_txringFc, gs_rxringFc;
static uint8_t gs_rxbuff[UART_RRB_SIZE], gs_txbuff[UART_SRB_SIZE];

//static FC_Data gs_FCData;

static uint8_t gs_fcTimeReceiveCmd = 0;

static uint8_t gs_fcReSendCnt = 0;

static uint8_t gs_fcStatus = 0x00;

static uint8_t gs_fcCommunicateStatus = 0x00;// truong add

static E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus = eFlowControllerNoErr;

static E_FlowCtrlSequence gs_fcSeq = FC_NORMAL_SEQ;

static uint8_t s_fcRebootTimeCnt = 0;

static uint8_t s_fcResetTimeCnt = 0;

static uint16_t s_currentFlowSet = 0;

static E_FlowCtrlRequestCmd s_RequestSendCmd = FC_SEND_VERSION;

static E_FlowCtrlRequestCmd s_preRequestSendCmd = FC_SEND_VERSION;
static ModeSetting settingMode = ContinuousMode;
static bool gs_isGetVersionDone = false;
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_RequestSendClearErrStatusCmd	              				*/
/*                                                                                      */
/* Details: Request send command to clear Error status									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_RequestSendClearErrStatusCmd()
{
	s_RequestSendCmd = FC_SEND_CLEAR_ERR;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_CreateCRC						              			    */
/*                                                                                      */
/* Details:        Create CRC for flow controller communication message					*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
static unsigned char flowCtrl_CreateCRC(unsigned char data[], char nbrOfBytes)
{
	unsigned char sum = 0;
	for(int i = 0; i < nbrOfBytes; i ++)
	{
		sum ^= data[i];
	}
	return sum;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_HandleResetNoti				              				*/
/*                                                                                      */
/* Details: Request send command to clear Error status									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResetNoti()
{
	DEBUG_FLOW("Handle reset \n");

	s_fcErrNoAlarmStatus = eFlowReset;
	flowCtrl_RequestSendClearErrStatusCmd();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_BackToNormalSeq				              				*/
/*                                                                                      */
/* Details: Back to normal sequence														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_BackToNormalSeq()
{
	RingBuffer_Init(&gs_rxringFc, gs_rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&gs_txringFc, gs_txbuff, 1, UART_SRB_SIZE);

	gs_fcSeq = FC_NORMAL_SEQ;
	gs_fcTimeReceiveCmd = 0; //reset for call send CMD, not resend when jump to normal sequence
	s_fcResetTimeCnt = 0;

	if(s_RequestSendCmd != FC_SEND_VERSION)
	{
		s_RequestSendCmd = FC_SEND_CLEAR_ERR;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_ProcessSendCmdRequest		      	        				*/
/*                                                                                      */
/* Details: Send current resquested command to flow controller							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_ProcessSendCmdRequest()
{
	static int16_t s_timeToSendCnt = -1;
	if((s_timeToSendCnt >= FLOW_INTERVAL_TIME_SEND) || (s_timeToSendCnt == -1))
	{
		s_timeToSendCnt = 0;
		gs_fcTimeReceiveCmd = 1;//Allow time receive count
		switch (s_RequestSendCmd) {
		case FC_SEND_VERSION:
			DEBUG_FLOW("V \n");
			s_RequestSendCmd = FC_SEND_CLEAR_ERR;
			flowCtrl_SendVersionAcquisitionCmd();
			s_preRequestSendCmd = FC_SEND_VERSION;
			break;
		case FC_SEND_CLEAR_ERR:
			DEBUG_FLOW("CLR \n");
			s_RequestSendCmd = FC_SEND_FLOW;
			flowCtrl_SendErrorStatusResetCmd();
			s_preRequestSendCmd = FC_SEND_CLEAR_ERR;
			break;
		case FC_SEND_FLOW:
			DEBUG_FLOW("FLW \n");
			s_RequestSendCmd = FC_SEND_ACQ1;
			flowCtrl_SendFlowRateCmd(s_currentFlowSet);
			s_preRequestSendCmd = FC_SEND_FLOW;
			break;
		case FC_SEND_ACQ1:
			DEBUG_FLOW("ACQ 1\n");
			s_RequestSendCmd = FC_SEND_ACQ2;
			flowCtrl_SendStatusAcquisition1Cmd();
			s_preRequestSendCmd = FC_SEND_ACQ1;
			break;
		case FC_SEND_ACQ2:
			DEBUG_FLOW("ACQ 2\n");
			s_RequestSendCmd = FC_SEND_ACQ1;
			flowCtrl_SendStatusAcquisition2Cmd();
			s_preRequestSendCmd = FC_SEND_ACQ2;
			break;
		case FC_SEND_MODE:
			DEBUG_FLOW("SEND MODE \n");
			s_RequestSendCmd = FC_SEND_ACQ1;
			if(settingMode == ContinuousMode)
			{
				flowCtrl_SendCmd(ModeSettingCode,ModeSettingContinuous);
			}
			else
			{
				flowCtrl_SendCmd(ModeSettingCode,ModeSettingSynchronized);
			}
			s_preRequestSendCmd = FC_SEND_MODE;
		default:
			break;
		}
	}
	else
	{
		s_timeToSendCnt++;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_HandleResendCmd				              				*/
/*                                                                                      */
/* Details: Resend command to flow controller											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResendCmd()
{
	if(gs_fcReSendCnt < MAX_RESEND_CNT)
	{
		DEBUG_FLOW("Resend \n");
		s_RequestSendCmd = s_preRequestSendCmd;
		gs_fcReSendCnt++;

		if((s_fcErrNoAlarmStatus != eResponseCodeErr)
				&& (s_fcErrNoAlarmStatus != eResponseCheckSumErr)
				&& (s_fcErrNoAlarmStatus != eResponseLengthErr)
				&& (s_fcErrNoAlarmStatus != eResponseParamErr))
		{
			s_fcErrNoAlarmStatus = eNoResponseErr;

		}
	}
	else
	{
		DEBUG_FLOW("Err FC \n");
		gs_fcStatus = gs_fcStatus | COMMUNICATION_ERR_BIT;
		gs_fcReSendCnt = 0;
		flowCtrl_Reset();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_HandleNormalSeq				              				*/
/*                                                                                      */
/* Details: Handle normal operation of compressor										*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleNormalSeq()
{
	flowCtrl_ReadFromDevice();
	if(gs_fcTimeReceiveCmd >= MAX_TIME_OF_RECEIVE_CMD)
	{
		gs_fcTimeReceiveCmd = 0;
		flowCtrl_HandleResendCmd();
	}
	flowCtrl_ProcessSendCmdRequest();
	if(gs_fcTimeReceiveCmd > 0)
	{
		gs_fcTimeReceiveCmd++;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_HandleResetSeq					              				*/
/*                                                                                      */
/* Details: Handle seset sequence of compressor											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleResetSeq()
{
	s_fcResetTimeCnt++;
	if(s_fcResetTimeCnt >= FC_RESET_TIME_100MS)
	{
		s_fcResetTimeCnt = 0;
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM);
		gs_fcSeq = FC_REBOOT_SEQ;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_HandleRebootSeq				              				*/
/*                                                                                      */
/* Details: Handle reboot sequence of flow controller									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void flowCtrl_HandleRebootSeq()
{
	s_fcRebootTimeCnt++;
	if(s_fcRebootTimeCnt >= FC_REBOOT_TIME1S)
	{
		s_fcRebootTimeCnt = 0;
		flowCtrl_BackToNormalSeq();
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UART3_IRQHandler						             				*/
/*                                                                                      */
/* Details: Handle interrupt for Uart 3													*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void UART3_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(FC_UART_CHANEL, &gs_rxringFc, &gs_txringFc);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_InitCommunication				             				*/
/*                                                                                      */
/* Details: Init UART configuration for flow controller									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_InitCommunication(void)
{
	//First cmd is send version acquistion
	s_RequestSendCmd = FC_SEND_VERSION;

	Board_UART_Init(FC_UART_CHANEL);

	//Set up reset signal
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FC_RESET_PORT_NUM, FC_RESET_BIT_NUM);		/* output */
	// Reset signal usually set high
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM); // Enable power for flow controller

	/* Setup UART for 115.2K8N1 */
	Chip_UART_DeInit(FC_UART_CHANEL);
	Chip_UART_Init(FC_UART_CHANEL);
	Chip_UART_SetBaud(FC_UART_CHANEL, 115200);
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

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(USART3_IRQn, 3);
	NVIC_EnableIRQ(USART3_IRQn);

	//After init, flow controller must be reset for waking up
	flowCtrl_Reset();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendCmd						              				*/
/*                                                                                      */
/* Details: Send command to flow controller														*/
/*                                                                                      */
/* Arguments:	(I) unsigned char code													*/
/*              (O) const unsigned char parameter[]                                     */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendCmd(unsigned char code,const unsigned char parameter[])
{
	unsigned char cmd[6];
	cmd[0] = code;
	cmd[1] = parameter[0];
	cmd[2] = parameter[1];
	cmd[3] = parameter[2];
	cmd[4] = parameter[3];
	cmd[5] = flowCtrl_CreateCRC(cmd,5);
	if(code == FlowRateCode)
		DEBUG_FLOW("Test flow %s\n",cmd);
	Chip_UART_SendRB(FC_UART_CHANEL, &gs_txringFc, cmd, 6);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendComunicationConfirmCmd		        					*/
/*                                                                                      */
/* Details: Send command to flow controller to confirm communication					*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendComunicationConfirmCmd()
{
	flowCtrl_SendCmd(ConfirmCommunicationCode,ComunicationConfirm);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendStatusAcquisition1Cmd			        				*/
/*                                                                                      */
/* Details: Send command to get current status 1 of compressor							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendStatusAcquisition1Cmd()
{
	//	DEBUG_FLOW("flowCtrl_SendStatusAcquisition1Cmd \n");
	flowCtrl_SendCmd(StatusAcquisition1Code,StatusAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendStatusAcquisition2Cmd			        				*/
/*                                                                                      */
/* Details: Send command to get current status 2 of compressor							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendStatusAcquisition2Cmd()
{
	flowCtrl_SendCmd(StatusAcquisition2Code,StatusAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendVersionAcquisitionCmd		              				*/
/*                                                                                      */
/* Details: send command to get version of Flow Controller						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendVersionAcquisitionCmd()
{
	flowCtrl_SendCmd(VersionCode,VersionAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendErrorStatusResetCmd		              				*/
/*                                                                                      */
/* Details: Send error status reset command												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendErrorStatusResetCmd()
{
	flowCtrl_SendCmd(ErrorStatCode,ErrorStatus);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendFlowRateCmd				              				*/
/*                                                                                      */
/* Details: Send command to flow controller to set flow rate							*/
/*                                                                                      */
/* Arguments:	(I) uint16_t flowVal													*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendFlowRateCmd(uint16_t flowVal)
{
	size_t idx;
	idx = find_index(fcOutputSetting,FC_OUTPUT_ARR_SIZE,flowVal);
	if (idx != -1)
	{
		DEBUG_FLOW("idx: %d \n",idx);
		flowCtrl_SendCmd(FlowRateCode,FLOW_TABLE[idx]);
	}
	else
	{
		DEBUG_FLOW("Err FC setting: %d \n",flowVal);
		assert(false);
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendTestFlowCmd				              				*/
/*                                                                                      */
/* Details: Send command to flow controller to get current flow rate					*/
/*                                                                                      */
/* Arguments:	(I) uint16_t flowVal													*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendTestFlowCmd(uint16_t flowVal)
{
	unsigned char  testFlowRateCmd[4];

	itoa(flowVal / 100, (char*)&testFlowRateCmd[0], 10);
	itoa((flowVal % 100) / 10, (char*)&testFlowRateCmd[1], 10);
	itoa((flowVal %100) % 10, (char*)&testFlowRateCmd[2], 10);
	testFlowRateCmd[3] = '0';

	DEBUG_FLOW("%s \n",testFlowRateCmd);

	flowCtrl_SendCmd(ConfirmCommunicationCode,testFlowRateCmd);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendTestO2Cmd				              					*/
/*                                                                                      */
/* Details: Send command to flow controller to get current O2 Concentration				*/
/*                                                                                      */
/* Arguments:	(I) uint16_t flowVal													*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendTestO2Cmd(uint16_t o2Val)
{
	unsigned char  testO2Cmd[4];

	itoa(o2Val / 100,(char*)&testO2Cmd[0],10);
	itoa((o2Val % 100) / 10,(char*)&testO2Cmd[1],10);
	itoa((o2Val %100) % 10,(char*)&testO2Cmd[2],10);
	testO2Cmd[3] = '1';

	DEBUG_FLOW("%s \n",testO2Cmd);

	flowCtrl_SendCmd(ConfirmCommunicationCode,testO2Cmd);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_ReadFromDevice					        					*/
/*                                                                                      */
/* Details: Read and then handle Message returned from flow controller					*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_ReadFromDevice()
{
	unsigned char dataRecv[9];
	unsigned char flowRate[5] = {'0','.','0','0','\0'};
	unsigned char O2Conct[5] = {'0','0','.','0','\0'};
	unsigned char temperature[3] = {'\0'};
	unsigned char pressure[3] = {'\0'};
	//    int  n = ::read(this->fd,dataRecv,9);
	//    ::tcflush(fd,TCIFLUSH);
	int n = Chip_UART_ReadRB(FC_UART_CHANEL, &gs_rxringFc, dataRecv, 9);
	if (n == FC_RESPONSE_LENGTH)
	{
		gs_fcStatus = gs_fcStatus & (~(1<<7));//clear COMMUNICATION_ERR_BIT (truong add)
		if(flowCtrl_CreateCRC(dataRecv,8) == dataRecv[8])
		{
			DEBUG_FLOW("Read %s\n",dataRecv);
			switch (dataRecv[0]){
			case 'A':
				DEBUG_FLOW("Communication Confirm \n");
				for (int j = 1; j < 7; j ++ )
				{
					if(dataRecv[j] != 'R')
					{
						s_fcErrNoAlarmStatus = eResponseParamErr;
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
			case 'B':
				//byte 1,2,3 --> Flow rate
				flowRate[0] = dataRecv[1];
				flowRate[2] = dataRecv[2];
				flowRate[3] = dataRecv[3];
				gs_flowMeasureVal = HexCharToInt(flowRate[0]) * 100 + HexCharToInt(flowRate[2])  *10 +
						HexCharToInt(flowRate[3]); //scale 100
				DEBUG_FLOW("	Flow %d \n",gs_flowMeasureVal);
				//byte 4,5,6 --> O2 Concentration
				O2Conct[0] = dataRecv[4];
				O2Conct[1] = dataRecv[5];
				O2Conct[3] = dataRecv[6];
				gs_O2Percent = HexCharToInt(O2Conct[0]) * 1000 + HexCharToInt(O2Conct[1])  *100 +
						HexCharToInt(O2Conct[3]) * 10; //scale 100
				DEBUG_FLOW("	O2 %d \n",gs_O2Percent);
				//Check err status
				if (dataRecv[7] & PRESSURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | PRESSURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & ULTRASONIC_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | ULTRASONIC_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & TEMPERATURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | TEMPERATURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & CMD_TIMEOUT_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | CMD_TIMEOUT_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & RESET_NOTI_BIT)
				{
					gs_fcStatus = gs_fcStatus | RESET_NOTI_BIT;
					if(gs_fcStatus == RESET_NOTI_BIT)
					{
						flowCtrl_HandleResetNoti();
					}
				}

				gs_fcTimeReceiveCmd = 0;

//				gs_FCData.flowMsrVal = gs_flowMeasureVal;
//				gs_FCData.O2Val = gs_O2Percent;
				break;
			case 'C':
				//byte 1,2,3 --> Flow rate
				flowRate[0] = dataRecv[1];
				flowRate[2] = dataRecv[2];
				flowRate[3] = dataRecv[3];
				gs_flowMeasureVal = HexCharToInt(flowRate[0]) * 100 + HexCharToInt(flowRate[2])  *10 +
						HexCharToInt(flowRate[3]); //scale 100
				DEBUG_FLOW("	Flow %d \n",gs_flowMeasureVal);

				//byte 4,5,6 --> O2 Concentration
				O2Conct[0] = dataRecv[4];
				O2Conct[1] = dataRecv[5];
				O2Conct[3] = dataRecv[6];
				gs_O2Percent = HexCharToInt(O2Conct[0]) * 1000 + HexCharToInt(O2Conct[1])  *100 +
						HexCharToInt(O2Conct[3]) * 10; //scale 100
				DEBUG_FLOW("	O2 %d \n",gs_O2Percent);

				//Check err status
				if (dataRecv[7] & PRESSURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | PRESSURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & ULTRASONIC_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | ULTRASONIC_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & TEMPERATURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | TEMPERATURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & CMD_TIMEOUT_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | CMD_TIMEOUT_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & RESET_NOTI_BIT)
				{
					gs_fcStatus = gs_fcStatus | RESET_NOTI_BIT;
					if(gs_fcStatus == RESET_NOTI_BIT)
					{
						flowCtrl_HandleResetNoti();
					}
				}
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
				s_fcErrNoAlarmStatus = eFlowControllerNoErr;
				break;
			case 'D':
				DEBUG_FLOW("Status 2 Acquisition \n");
				//byte 1,2,3 --> Flow rate
				temperature[0] = dataRecv[1];
				temperature[1] = dataRecv[2];
				temperature[2] = dataRecv[3];
				gs_temperature = HexCharToInt(temperature[0]) * 100 + HexCharToInt(temperature[1])  *10 +
						HexCharToInt(temperature[2]); //scale 100
				//byte 4,5,6 --> O2 Concentration
				pressure[0] = dataRecv[4];
				pressure[1] = dataRecv[5];
				pressure[2] = dataRecv[6];
				gs_pressure = HexCharToInt(pressure[0]) * 1000 + HexCharToInt(pressure[1])  *100 +
						HexCharToInt(pressure[2]) * 10; //scale 100
				//Check err status
				if (dataRecv[7] & PRESSURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | PRESSURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & ULTRASONIC_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | ULTRASONIC_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & TEMPERATURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | TEMPERATURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & CMD_TIMEOUT_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | CMD_TIMEOUT_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & RESET_NOTI_BIT)
				{
					gs_fcStatus = gs_fcStatus | RESET_NOTI_BIT;
					if(gs_fcStatus == RESET_NOTI_BIT)
					{
						flowCtrl_HandleResetNoti();
					}
				}
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
				s_fcErrNoAlarmStatus = eFlowControllerNoErr;
				break;
			case 'E':
				DEBUG_FLOW("Version Acquisition \n");
				s_version[0] = dataRecv[1];
				s_version[1] = dataRecv[2];
				s_version[2] = dataRecv[3];
				s_version[3] = dataRecv[4];
				s_version[4] = dataRecv[5];
				s_version[5] = dataRecv[6];
				s_version[6] = dataRecv[7];
				gs_isGetVersionDone = true;
				break;
			case 'F':
				DEBUG_FLOW("	Error Status Reset \n");
				for (int j = 1; j < 7; j ++ )
				{
					if(dataRecv[j] != 'R')
					{
						s_fcErrNoAlarmStatus = eResponseParamErr;
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
			case 'G':
				DEBUG_FLOW("	Mode Setting \n");
				if(dataRecv[1]=='0')
				{
					DEBUG_FLOW("	Continuous Mode \n");
				}
				else if(dataRecv[1]=='1')
				{
					DEBUG_FLOW("	Synchronized Mode \n");
				}
				//Check err status
				if (dataRecv[7] & PRESSURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | PRESSURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & ULTRASONIC_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | ULTRASONIC_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & TEMPERATURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | TEMPERATURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & CMD_TIMEOUT_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | CMD_TIMEOUT_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & RESET_NOTI_BIT)
				{
					gs_fcStatus = gs_fcStatus | RESET_NOTI_BIT;
					if(gs_fcStatus == RESET_NOTI_BIT)
					{
						flowCtrl_HandleResetNoti();
					}
				}
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
				s_fcErrNoAlarmStatus = eFlowControllerNoErr;
				break;
			case 'H':
				DEBUG_FLOW("	Open and Close Setting \n");
				O2Conct[0] = dataRecv[4];
				O2Conct[1] = dataRecv[5];
				O2Conct[3] = dataRecv[6];
				gs_O2Percent = HexCharToInt(O2Conct[0]) * 1000 + HexCharToInt(O2Conct[1])  *100 +
						HexCharToInt(O2Conct[3]) * 10; //scale 100
				DEBUG_FLOW("	O2 %d \n",gs_O2Percent);
				//Check err status
				if (dataRecv[7] & PRESSURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | PRESSURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & ULTRASONIC_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | ULTRASONIC_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & TEMPERATURE_SENSOR_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | TEMPERATURE_SENSOR_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & CMD_TIMEOUT_ERR_BIT)
				{
					gs_fcStatus = gs_fcStatus | CMD_TIMEOUT_ERR_BIT;
					flowCtrl_Stop();
				}
				if(dataRecv[7] & RESET_NOTI_BIT)
				{
					gs_fcStatus = gs_fcStatus | RESET_NOTI_BIT;
					if(gs_fcStatus == RESET_NOTI_BIT)
					{
						flowCtrl_HandleResetNoti();
					}
				}
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
				s_fcErrNoAlarmStatus = eFlowControllerNoErr;
				break;

			case 'W':// truong add
				DEBUG_FLOW("	Error \n");
				if (dataRecv[1] & CHECKSUM_ERR_BIT)
				{
					gs_fcCommunicateStatus = gs_fcCommunicateStatus | CHECKSUM_ERR_BIT;	// truong add
				}
				else if(dataRecv[1] & COMMAND_ERR_BIT)
				{
					gs_fcCommunicateStatus = gs_fcCommunicateStatus | COMMAND_ERR_BIT; // truong add
				}
				else if(dataRecv[1] & PARAM_ERR_BIT)
				{
					gs_fcCommunicateStatus = gs_fcCommunicateStatus | PARAM_ERR_BIT; // truong add
				}
				gs_fcTimeReceiveCmd = 0;
				gs_fcReSendCnt = 0;
				break;

			default:
				s_fcErrNoAlarmStatus = eResponseCodeErr;

				break;
			}
		}
		else
		{
			s_fcErrNoAlarmStatus = eResponseCheckSumErr;
		}
	}
	else
	{
		if(n > 0)
		{
			s_fcErrNoAlarmStatus = eResponseLengthErr;
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_Handle			     			             				*/
/*                                                                                      */
/* Details: Handle communication with flow controller		     						*/
/*                                                                                      */
/* Arguments:	(I) None										        				*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Handle()
{
	switch (gs_fcSeq)
	{
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
		assert(false);
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetFlowVal			     			        				*/
/*                                                                                      */
/* Details: Get current flow rate														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                    */
/*                                                                                      */
/****************************************************************************************/

uint16_t flowCtrl_GetFlowVal(void)
{
	return gs_flowMeasureVal;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetO2Val			     			        				*/
/*                                                                                      */
/* Details: Get current O2 Concentration												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetO2Val(void)
{
	return gs_O2Percent;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetTemperature		     			        				*/
/*                                                                                      */
/* Details: Get current Temperature														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetTemperature(void)
{
	return gs_temperature;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetPressure		     			        				*/
/*                                                                                      */
/* Details: Get current Pressure														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetPressure(void)
{
	return gs_pressure;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_getErrStatus						        				*/
/*                                                                                      */
/* Details: Get current compressor error status											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t				                                                */
/*                                                                                      */
/****************************************************************************************/
uint8_t flowCtrl_getErrStatus()
{
	//	DEBUG_FLOW("gs_fcStatus: %d\n",gs_fcStatus);
	return gs_fcStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_getCommunicateErrStatus			        				*/
/*                                                                                      */
/* Details: Get current flow controller communication error status						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint8_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint8_t flowCtrl_getCommunicateErrStatus()
{
	//	DEBUG_FLOW("gs_fcStatus: %d\n",gs_fcStatus);
	return gs_fcCommunicateStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_getResponseErrStatus				        				*/
/*                                                                                      */
/* Details: Get current flow controller error response status							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : E_FlowControllerNoAlarmStatus	                                        */
/*                                                                                      */
/****************************************************************************************/
E_FlowControllerNoAlarmStatus flowCtrl_getResponseErrStatus()
{
	//	DEBUG_FLOW("s_fcErrNoAlarmStatus: %d\n",s_fcErrNoAlarmStatus);
	return s_fcErrNoAlarmStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_Reset							        					*/
/*                                                                                      */
/* Details: Reset flow controller														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Reset()
{
	// Set reset signal low 100ms for reset fc
	DEBUG_FLOW("FC Reset \n");
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,FC_RESET_PORT_NUM,FC_RESET_BIT_NUM);
	gs_fcSeq = FC_RESET_SEQ;
	s_fcResetTimeCnt = 0;
	s_fcRebootTimeCnt = 0;
	gs_fcStatus &= 0x80; // truong add
	gs_fcCommunicateStatus = 0; // truong add
	s_fcErrNoAlarmStatus = eFlowControllerNoErr;	// truong add
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetResetStatus					        					*/
/*                                                                                      */
/* Details: Get reset status															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : bool				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
bool flowCtrl_GetResetStatus()
{
	if (gs_fcSeq == FC_NORMAL_SEQ)
	{
		return false;
	}
	else
	{
		return true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_RequestSetFlow					        					*/
/*                                                                                      */
/* Details: Get reset status															*/
/*                                                                                      */
/* Arguments:	(I) uint16_t flow														*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_RequestSetFlow(uint16_t flow)
{
	s_currentFlowSet = flow;
	s_RequestSendCmd = FC_SEND_FLOW;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_Stop							        					*/
/*                                                                                      */
/* Details: Stop flow controller														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Stop()
{
	gs_fcTimeReceiveCmd = 0; //reset for call send CMD, not resend when jump to normal sequence
	s_fcResetTimeCnt = 0;
	flowCtrl_RequestSetFlow(0);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_GetVersion							        				*/
/*                                                                                      */
/* Details: Get version of flow controller driver										*/
/*                                                                                      */
/* Arguments:	(I) char* buffer														*/
/*              (O) char* buffer				                                        */
/* ReturnValue : unsigned char*		                                                   	*/
/*                                                                                      */
/****************************************************************************************/
unsigned char* flowCtrl_GetVersion(char * buffer)
{
	memcpy(buffer,s_version,8);
	return s_version;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendModeSettingCmd				           				    */
/*                                                                                      */
/* Details:        Send command to Flow controller to set its mode					    */
/*                                                                                      */
/* Arguments:	(I)	ModeSetting setting													*/
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendModeSettingCmd(ModeSetting setting)
{
	settingMode = setting;
	s_RequestSendCmd = FC_SEND_MODE;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_SendOpenAndCloseSettingCmd		           				    */
/*                                                                                      */
/* Details:        Send command to Flow controller to let it open for period of time    */
/*                                                                                      */
/* Arguments:	(I)	uint16_t msecond													*/
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendOpenAndCloseSettingCmd(uint16_t *msecond)
{
	uint16_t tickCount = (double)(*msecond)/5.0;
	sprintf ((char*)OpenSetting, "%.4d", tickCount);
	flowCtrl_SendCmd(OpenAndCloseSettingCode,OpenSetting);
	*msecond = tickCount*5;
//	s_RequestSendCmd = FC_SEND_OCSET;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	flowCtrl_isGetVersionDone			              				    */
/*                                                                                      */
/* Details:        Check whether read version process is finished						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*              (O) None                                                                */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool flowCtrl_isGetVersionDone()
{
	return gs_isGetVersionDone;
}

