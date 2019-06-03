/********************************************************************/
/*                                                                  */
/* File Name     : Compressor.c		                                */
/*                                                                  */
/* General       : This file contains the functions for init and    */
/*				  handle compressor communication                   */
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
/*             2016/10/04      Quyen Ngo  	new file       		    */
/*	#0001	   2017/08/25	   Quyen Ngo	Fix layout file, add 	*/
/*					missing return of void function					*/
/*                                                                  */
/*	#0002	   2017/09/01	   Quyen Ngo	modify					*/
/*		-remove assert												*/
/*	#0003	   2017/09/05	   Quyen Ngo	modify					*/
/*		-Does not allow change sequence direcly, only acept change	*/
/*		via comppresor_RequestChangeSequenceTo function, does not 	*/
/*		allow change sequence  during reset and reboot sequence		*/
/*		(fix Bug 1804)												*/
/*      -Change reboot wait time when alram from 1s to 500ms		*/
/*																	*/
/*	#0004      2017/09/12		Quyen Ngo	modify					*/
/*		-Reset compressor restart and compressor wait for start count*/
/*		when compressor stop (PSA END)								*/
/*		-Does not process new reset request when current reset does */
/*		not finish													*/
/*		-Change compressor_requestChangeSequence function for avoid	*/
/*		corrupt sequence when have a reset request during reboot seq*/
/*	#0005      2017/11/17		Quyen Ngo	modify					*/
/*		-Remove variable not use for fix coverity issue				*/
/********************************************************************/
#include <string.h>
#include "math.h"
#include <gpio.h>
#include "board.h"
#include "IpcMsg.h"

#include <Compressor.h>
#include "util.h"
#include "TaskCommon.h"
#include "PsaControl.h"
#include "LatchValve.h"
#include "FlowController.h"
#include "OxyCylinder.h"
#include "PsaSelectMgr.h"
#include "IpcTask.h"
#include "ModeHandle.h"

#ifdef DEBUG
//#define DEBUG_COMP_EN
//#define Q_DEBUG
#endif

#ifdef DEBUG_COMP_EN
#define DEBUG_COMP(...) printf(__VA_ARGS__)
#else
#define DEBUG_COMP(...)
#endif

#define  MAX_BYTES_RECEIVE 16
#define COMP_UART_CHANEL						LPC_USART0
/* Transmit and receive ring buffer sizes */
#define COMP_UART_SRB_SIZE 						128	/* Send */
#define COMP_UART_RRB_SIZE 						32	/* Receive */
#define COMPPRESSOR_BAUD			1200
#define MAX_ROTATION_RATIO			0x7FFF //equal 100% speed
#define COMP_MAX_RESTART_TIMES		2
#define COMP_RESPONSE_PCK_LENGTH	1
#define COMP_MAX_BYTES_RECEIVE		16
#define T_COMP_HSP_TIME_LOWTEMP 			30000 //30s
#define T_COMP_HSP_TIME_NORMAL				5000  //5s
#define COMP_RESET_TIME_100MS 					100/DEVICE_TASK_DELAY_TIME
#define COMP_INTERVAL_TIME_SEND					200/DEVICE_TASK_DELAY_TIME
#define COMP_RESET_TIME_100MS					100/DEVICE_TASK_DELAY_TIME
#define COMP_REBOOT_TIME            			500/DEVICE_TASK_DELAY_TIME //500ms/20ms
#define COMP_RESTART_TIME_40S 					40000/DEVICE_TASK_DELAY_TIME //40000ms/20ms
#define COMP_WAIT_FOR_RESTART_TIME				5000/DEVICE_TASK_DELAY_TIME //5secs = 5000ms
#define COMP_CMD_LENGTH							7

typedef enum
{
	COMP_WAIT_TYPE,
	COMP_WAIT_DATA,
	COMP_WAIT_CHECKSUM
}E_CompressorReceiveDataState;

static uint16_t s_tCompHspTime = 5000/DEVICE_TASK_DELAY_TIME; //5S

/* Transmit and receive ring buffers */
static RINGBUFF_T gs_compRxring, gs_compTxring;
/* Transmit and receive buffers */
static uint8_t gs_compRxbuff[COMP_UART_RRB_SIZE], gs_compTxbuff[COMP_UART_SRB_SIZE];

static uint16_t s_compStatus = 0x0000;

static E_CompressorSequence s_compressorSeq = COMP_NORMAL_SEQ;

static E_CompressorSequence s_compressorPendingSeq = COMP_NORMAL_SEQ;

static uint8_t s_compRebootTimeCnt = 0;

static uint8_t s_compResetTimeCnt = 0;

static uint16_t s_tCompHspTimeCnt = 0;

static uint16_t s_compWaitForRestartTimeCnt = 0;

static uint8_t s_WaitForStartCnt = 0;

static uint8_t s_compRestartCnt = 0;

static uint16_t s_compCurrentRpmSet = 0;

static uint16_t s_compRpmCurrentPsaParam = 0;

static uint8_t s_responsePacket[COMP_RESPONSE_PCK_LENGTH];
static uint8_t s_numOfRequestBytes = COMP_RESPONSE_PCK_LENGTH;
static uint8_t s_pckId = 0;

static void compressor_SendCmd(uint8_t rpmDuty);

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_RequestChangeSequenceTo									*/
/*                                                                                      */
/* Details:  Process the change sequence request										*/
/*                                                                                      */
/* Arguments :	(I) E_CompressorSequence sequence					 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : (O) E_CompressorSequence s_compressorSeq							*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void compressor_RequestChangeSequenceTo(E_CompressorSequence sequence)
{
	switch (s_compressorSeq) {
	case COMP_NORMAL_SEQ:
	case COMP_START_SEQ:
	case COMP_WAIT_FOR_START_SEQ:
		s_compressorPendingSeq = s_compressorSeq;
		s_compressorSeq = sequence;
		break;
	case COMP_EXIT_REBOOT_SEQ:
		s_compressorSeq = sequence;
		break;
	case COMP_RESET_SEQ:
		if((sequence == COMP_NORMAL_SEQ) ||
				(sequence == COMP_START_SEQ) ||
				(sequence == COMP_WAIT_FOR_START_SEQ))
		{
			s_compressorPendingSeq = sequence;
		}
		else if(sequence == COMP_REBOOT_SEQ)
		{
			s_compressorSeq = sequence;
		}
		else
		{
			DEBUGOUT("Compressor Invalid request change sequence \n");
		}
		break;
	case COMP_REBOOT_SEQ:
		if((sequence == COMP_NORMAL_SEQ) ||
				(sequence == COMP_START_SEQ) ||
				(sequence == COMP_WAIT_FOR_START_SEQ))
		{
			s_compressorPendingSeq = sequence;
		}
		else if((sequence == COMP_EXIT_REBOOT_SEQ) ||
				(sequence == COMP_RESET_SEQ))
		{
			s_compressorSeq = sequence;
		}
		else
		{
			DEBUGOUT("Compressor Invalid request change sequence \n");
		}
		break;
	default:
		break;
	}
	DEBUG_COMP("Compressor sequence: current %d pending %d\n",s_compressorSeq,s_compressorPendingSeq);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ResetReceiveState											*/
/*                                                                                      */
/* Details:  Reset variable value for ready to receive packet							*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_RESPONSE_PCK_LENGTH											*/
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
static void compressor_ResetReceiveState(void)
{
	s_pckId = 0;
	s_numOfRequestBytes = COMP_RESPONSE_PCK_LENGTH;
	memset(s_responsePacket, 0, COMP_RESPONSE_PCK_LENGTH);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_BackToNormalSeq											*/
/*                                                                                      */
/* Details:  Reset ring bufer and back to normal sequence communication					*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_RESPONSE_PCK_LENGTH											*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O)RINGBUFF_T gs_compRxring, gs_compTxring 						*/
/*					   (O)E_CompressorSequence s_compressorSeq							*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_BackToNormalSeq(void)
{
	RingBuffer_Flush(&gs_compRxring);
	RingBuffer_Flush(&gs_compTxring);
	compressor_RequestChangeSequenceTo(COMP_EXIT_REBOOT_SEQ);
	compressor_RequestChangeSequenceTo(s_compressorPendingSeq);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ProcessSendCmdRequest										*/
/*                                                                                      */
/* Details:  Send RPM command to compressor	if time pass COMP_INTERVAL_TIME_SEND		*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_INTERVAL_TIME_SEND												*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (I) uint16_t s_compCurrentRpmSet									*/
/*						   unit: 	% 													*/
/*						   range: 0~100													*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_ProcessSendCmdRequest(void)
{
	static int16_t s_timeToSendCnt = -1;
	if((s_timeToSendCnt >= COMP_INTERVAL_TIME_SEND) || (s_timeToSendCnt == -1))
	{
		s_timeToSendCnt = 0;
		compressor_SendCmd(s_compCurrentRpmSet);
	}
	else
	{
		s_timeToSendCnt++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_HandleNormalSeq											*/
/*                                                                                      */
/* Details:  Handle normal communication with compressor								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleNormalSeq(void)
{
	compressor_ReadFromDevice();
	compressor_ProcessSendCmdRequest();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_HandleResetSeq											*/
/*                                                                                      */
/* Details:  Handle seset sequence of compressor										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_RESET_TIME_100MS												*/
/*					LPC_GPIO_PORT														*/
/*					COMP_RESET_PORT_NUM													*/
/*					COMP_RESET_BIT_NUM													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(I)(O) uint8_t s_compResetTimeCnt								*/
/*						range: 0~256													*/
/*						(I) E_CompressorSequence s_compressorSeq						*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleResetSeq(void)
{
	DEBUG_COMP("Comp reset \n");
	s_compResetTimeCnt++;
	if(s_compResetTimeCnt > COMP_RESET_TIME_100MS)
	{
		s_compResetTimeCnt = 0;
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
		compressor_RequestChangeSequenceTo(COMP_REBOOT_SEQ);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_HandleRebootSeq											*/
/*                                                                                      */
/* Details:  Handle reboot sequence of compressor										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_REBOOT_TIME													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(I)(O) static uint8_t s_compRebootTimeCnt = 0					*/
/*						range: 0~256													*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleRebootSeq(void)
{
	DEBUG_COMP("Comp reboot \n");
	s_compRebootTimeCnt++;
	if(s_compRebootTimeCnt >= COMP_REBOOT_TIME)
	{
		s_compRebootTimeCnt = 0;
		compressor_BackToNormalSeq();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_HandleWaitForStartSeq										*/
/*                                                                                      */
/* Details:  count time and it over COMP_WAIT_FOR_RESTART_TIME, start compresssor if the*/
/*			 s_WaitForStartCnt not pass COMP_MAX_RESTART_TIMES							*/
/*			 also handle normal communication of compressor								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_WAIT_FOR_RESTART_TIME											*/
/*					COMP_MAX_RESTART_TIMES												*/
/*					COMP_RESET_PORT_NUM													*/
/*					COMP_RESET_BIT_NUM													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(I)(O) uint16_t s_compWaitForRestartTimeCnt						*/
/*						range: 0~2^16													*/
/*						(I)(O)) uint8_t s_WaitForStartCnt								*/
/*						range: 0~2^8													*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleWaitForStartSeq(void)
{
	s_compWaitForRestartTimeCnt++;
	if(s_compWaitForRestartTimeCnt >= COMP_WAIT_FOR_RESTART_TIME)
	{
		if(s_WaitForStartCnt >= COMP_MAX_RESTART_TIMES)
		{
			s_WaitForStartCnt = 0;
			psa_SetMode(PSA_NORMAL_MODE);
			DEBUGOUT("PSA NORMAL 3 times \n");
		}
		s_WaitForStartCnt++;
		s_compWaitForRestartTimeCnt = 0;
		compressor_Start();
	}
	compressor_HandleNormalSeq();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_HandleStartSeq											*/
/*                                                                                      */
/* Details:  handle start sequence of compressor: start at max speed for s_tCompHspTime */
/*			 After that, set current setting rpm										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		COMP_MAX_RPM														*/
/*					COMP_MAX_RESTART_TIMES												*/
/*					COMP_RESET_PORT_NUM													*/
/*					COMP_RESET_BIT_NUM													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(I) uint16_t s_compRpmCurrentPsaParam							*/
/*						range: 0~100													*/
/*						(I)(O)) uint8=16_t s_tCompHspTimeCnt							*/
/*						range: 0~2^16													*/
/*						(I)	uint16_t s_tCompHspTime										*/
/*						range: 0~2^16													*/
/*						(O) E_CompressorSequence s_compressorSeq						*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleStartSeq(void)
{
	if(s_tCompHspTimeCnt == 0)
	{
		compressor_ReqestSetRpm(COMP_MAX_RPM);
	}
	s_tCompHspTimeCnt++;
	if (s_tCompHspTimeCnt >= s_tCompHspTime)
	{
		s_tCompHspTimeCnt = 0;
		compressor_ReqestSetRpm(s_compRpmCurrentPsaParam);
		compressor_RequestChangeSequenceTo(COMP_NORMAL_SEQ);
		psa_FinishCompressorRestart(); //change PSA_STATE
	}
	compressor_HandleNormalSeq();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_SendCmd													*/
/*                                                                                      */
/* Details:  Send command to compressor to control RPM									*/
/*                                                                                      */
/* Arguments :		uint8_t rpmDuty														*/
/*					range: 0~100														*/
/*                                                                                      */
/* Macros:	   		COMP_UART_CHANEL													*/
/*					COMP_CMD_LENGTH														*/
/*					MAX_ROTATION_RATIO													*/
/*					COMP_MAX_RPM														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(O)RINGBUFF_T gs_compTxring										*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendCmd(uint8_t rpmDuty)
{
	uint16_t ratioRotaion = rpmDuty*MAX_ROTATION_RATIO/COMP_MAX_RPM;
	uint8_t cmd[COMP_CMD_LENGTH];
	cmd[0] = 0x53; //S Fixed See compressor protocol
	cmd[1] = 0x50; //P Fixed See compressor protocol
	cmd[2] = 0x44; //D Fixed See compressor protocol
	cmd[3] = (uint8_t)ratioRotaion & 0x00FF;
	cmd[4] = (uint8_t)(ratioRotaion >> 8) & 0x00FF;
	cmd[5] = ~cmd[3];
	cmd[6] = ~cmd[4];
	Chip_UART_SendRB(COMP_UART_CHANEL, &gs_compTxring, cmd, COMP_CMD_LENGTH);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Uart0 IRQ handle			                                           		    */
/*                                                                                      */
/* Details  -Handle interrupt for Uart 0		                 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Macros:	   		COMP_UART_CHANEL													*/
/*																						*/
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) RINGBUFF_T gs_compTxring										*/
/*                     (O) RINGBUFF_T gs_compRxring					                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void UART0_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(COMP_UART_CHANEL, &gs_compRxring, &gs_compTxring);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: compressor_InitCommunication			                                */
/*                                                                                      */
/* Details  	-Init uart config for compressor										*/
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

void compressor_InitCommunication(void)
{
	//Firts cmd send version acq

	Board_UART_Init(COMP_UART_CHANEL);
	//Set up reset signal
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,COMP_RESET_PORT_NUM, COMP_RESET_BIT_NUM);		/* output */
	// Reset signal usually set high
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM); // Enable power for flow controller

	/* Setup UART for 1200 8N2 */
	Chip_UART_DeInit(COMP_UART_CHANEL);
	Chip_UART_Init(COMP_UART_CHANEL);
	Chip_UART_SetBaud(COMP_UART_CHANEL, COMPPRESSOR_BAUD);
	Chip_UART_ConfigData(COMP_UART_CHANEL, (UART_LCR_WLEN8 | UART_LCR_SBS_2BIT));
	Chip_UART_SetupFIFOS(COMP_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(COMP_UART_CHANEL);

	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&gs_compRxring, gs_compRxbuff, 1, COMP_UART_RRB_SIZE);
	RingBuffer_Init(&gs_compTxring, gs_compTxbuff, 1, COMP_UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(COMP_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(COMP_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

	NVIC_SetPriority(USART0_IRQn, COMP_USART0_INT_PRIO);
	NVIC_EnableIRQ(USART0_IRQn);
	return;

}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_Handle													*/
/*                                                                                      */
/* Details:  Handle communication with compressor based on current sequence				*/
/*                                                                                      */
/* Arguments :																			*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(I)E_CompressorSequence s_compressorSeq							*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Handle(void)
{
	switch (s_compressorSeq) {
	case COMP_NORMAL_SEQ:
		compressor_HandleNormalSeq();
		break;
	case COMP_REBOOT_SEQ:
		compressor_HandleRebootSeq();
		break;
	case COMP_RESET_SEQ:
		compressor_HandleResetSeq();
		break;
	case COMP_WAIT_FOR_START_SEQ:
		compressor_HandleWaitForStartSeq();
		break;
	case COMP_START_SEQ:
		compressor_HandleStartSeq();
		break;
	default:
		DEBUGOUT("Compressor invalid sequence\n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_AnalyzePacketFromDevice									*/
/*                                                                                      */
/* Details:  Analyze packet data send from Compressor									*/
/*                                                                                      */
/* Arguments :		(I) uint8_t* packet													*/
/*                                                                                      */
/* Macros:	   		COMP_ABNORMAL_DC_VOLT_BIT											*/
/*					COMP_ABNORMAL_SW_OVERCURRENT_BIT									*/
/*					COMP_ABNORMAL_TEMP_ERR_BIT											*/
/*					COMP_ROTAION_SPEED_ERR_BIT											*/
/*					COMP_ABNORMAL_HW_OVERCURRENT_BIT									*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(O)uint16_t s_compStatus										*/
/*						range: 0~2^16													*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_AnalyzePacketFromDevice(uint8_t* packet)
{
	switch (packet[0]) {
	case COMP_ABNORMAL_DC_VOLT_BIT:
	case COMP_ABNORMAL_SW_OVERCURRENT_BIT:
	case COMP_ABNORMAL_TEMP_ERR_BIT:
	case COMP_ROTAION_SPEED_ERR_BIT:
	case COMP_ABNORMAL_HW_OVERCURRENT_BIT:
		s_compStatus = packet[0];
		if(mode_GetCurrentMode() == eStandyMode)
		{
			compressor_Reset();
			compressor_ReqestSetRpm(0);
		}
		break;
	default:
		DEBUGOUT("COMPRESSOR INVALID DATA \n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ReadFromDevice											*/
/*                                                                                      */
/* Details:  Read data return from compressor and call analysis function				*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		COMP_RESPONSE_PCK_LENGTH											*/
/*					COMP_UART_CHANEL													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(O)RINGBUFF_T gs_compRxring										*/
/*						(O) uint8_t s_pckId = 0;  										*/
/*					    (O) uint8_t s_numOfRequestBytes									*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ReadFromDevice(void)
{
	uint8_t dataRecv[COMP_RESPONSE_PCK_LENGTH];

	int n = Chip_UART_ReadRB(COMP_UART_CHANEL, &gs_compRxring, dataRecv, COMP_RESPONSE_PCK_LENGTH);
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
		//receive enough data for packet, analyze data
		compressor_AnalyzePacketFromDevice(s_responsePacket);
		compressor_ResetReceiveState();
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_getErrStatus												*/
/*                                                                                      */
/* Details:  Read data return from compressor and call analysis functionGet err status	*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	uint16_t s_compStatus									   			*/
/* 					range: 0~2^8														*/
/*                                                                                      */
/* Using Global Data : 	(I)uint16_t s_compStatus										*/
/* 						range: 0~2^8													*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getErrStatus(void)
{
	//	DEBUG_COMP("s_compStatus: %d \n",s_compStatus);
	return s_compStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_GetCurentSeq												*/
/*                                                                                      */
/* Details: Get curent sequence of compressor											*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		COMP_RESPONSE_PCK_LENGTH											*/
/*					COMP_UART_CHANEL													*/
/*																			 			*/
/* ReturnValue : 	 E_CompressorSequence s_compressorSeq					   			*/
/*                                                                                      */
/* Using Global Data : 	(I) E_CompressorSequence s_compressorSeq						*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
E_CompressorSequence compressor_GetCurentSeq(void)
{
	return s_compressorSeq;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_Reset														*/
/*                                                                                      */
/* Details:  reset Compressor and clear time count variable								*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					COMP_RESET_PORT_NUM													*/
/*					COMP_RESET_BIT_NUM													*/
/*					COMP_REBOOT_TIME													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(O)	uint16_t s_compRebootWaittingTime							*/
/*						(O) E_CompressorSequence s_compressorSeq						*/
/*					    (O) uint8_t s_compRebootTimeCnt									*/
/*						(O) uint8_t s_compResetTimeCnt									*/
/*						(O) uint8_t s_WaitForStartCnt									*/
/*                     								                                 	*/
/*【Note】                                                                            												    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Reset(void)
{
	if(s_compressorSeq != COMP_RESET_SEQ)
	{
		DEBUG_COMP("compressor_Reset\n");
		// Set reset signal low 100ms for reset fc
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
		compressor_RequestChangeSequenceTo(COMP_RESET_SEQ);
		s_compRebootTimeCnt = 0;
		s_compResetTimeCnt = 0;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ResetWhenAlarm											*/
/*                                                                                      */
/* Details:  reset Compressor when alarm with PSA stop									*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					COMP_RESET_PORT_NUM													*/
/*					COMP_RESET_BIT_NUM													*/
/*					COMP_REBOOT_TIME_ALARM												*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 	(O)	uint16_t s_compRebootWaittingTime							*/
/*						(O) E_CompressorSequence s_compressorSeq						*/
/*					    (O) uint8_t s_compRebootTimeCnt									*/
/*						(O) uint8_t s_compResetTimeCnt									*/
/*						(O) uint8_t s_WaitForStartCnt									*/
/*                     								                                 	*/
/*【Note】                                                                            											   	    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ResetWhenAlarm(void)
{
	if(s_compressorSeq != COMP_RESET_SEQ)
	{
		DEBUG_COMP("compressor_Reset\n");
		// Set reset signal low 100ms for reset fc
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
		compressor_RequestChangeSequenceTo(COMP_RESET_SEQ);
		s_compRebootTimeCnt = 0;
		s_compResetTimeCnt = 0;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_Restart													*/
/*                                                                                      */
/* Details:  perform compressor restart sequence										*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		COMP_MAX_RESTART_TIMES												*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data:	(O) E_CompressorSequence s_compressorSeq						*/
/*						(O) uint8_t s_compWaitForRestartTimeCnt							*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Restart(void)
{
	if(s_compRestartCnt <= COMP_MAX_RESTART_TIMES)
	{
		DEBUG_COMP("compressor_Restart\n");
		// Set reset signal low 100ms for reset fc
		compressor_RequestChangeSequenceTo(COMP_WAIT_FOR_START_SEQ);
		s_compWaitForRestartTimeCnt = 0;
		compressor_ReqestSetRpm(0);
		s_compRestartCnt++;
	}
	else
	{
		psa_FinishCompressorRestart();//set psa state back to PSA_BASIC because don't process restart sequence
		s_compRestartCnt = 0;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_Start														*/
/*                                                                                      */
/* Details:  Start compressor, set compressor at Maxspeed for tCompHsp time and			*/
/* 			 start timer 30s for check pressure pass P_comp_stlow or not				*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   		COMP_MAX_RPM														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data:	(O) E_CompressorSequence s_compressorSeq						*/
/*						(O) uint16_t s_tCompHspTimeCnt									*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Start()
{
	DEBUG_COMP("compressor_Start\n");
	compressor_RequestChangeSequenceTo(COMP_START_SEQ);
	s_tCompHspTimeCnt = 0;
	compressor_ReqestSetRpm(COMP_MAX_RPM);
	psa_StartCheckCompRestartSeq();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_Stop														*/
/*                                                                                      */
/* Details:  stop compressor															*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data: 	(O) E_CompressorSequence s_compressorSeq						*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Stop(void)
{
	//Reset restart time count
	s_WaitForStartCnt = 0;
	s_compRestartCnt = 0;
	compressor_RequestChangeSequenceTo(COMP_NORMAL_SEQ);
	compressor_ReqestSetRpm(0);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ClearAlarmStatus											*/
/*                                                                                      */
/* Details:  Clear alarm status															*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data:	(O)  s_compStatus												*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ClearAlarmStatus(void)
{
	s_compStatus = 0x0000;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_ReqestSetRpm												*/
/*                                                                                      */
/* Details:  Request set rpm for compressor, compressor_Handle() function will 			*/
/*			 process this request														*/
/*                                                                                      */
/* Arguments :			 																*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data:	(O) uint16_t s_compCurrentRpmSet								*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ReqestSetRpm(uint16_t rpm)
{
	s_compCurrentRpmSet = rpm;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_UpdatePsaParamRpm											*/
/*                                                                                      */
/* Details:  Upate rpm PSA param														*/
/*                                                                                      */
/* Arguments :		(I) uint16_t psaParamRpm							 				*/
/*					range: 0~100														*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data:	(O) uint16_t s_compRpmCurrentPsaParam							*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_UpdatePsaParamRpm(uint16_t psaParamRpm)
{
	s_compRpmCurrentPsaParam = psaParamRpm;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_getRpm													*/
/*                                                                                      */
/* Details:   get Rpm speed																*/
/*                                                                                      */
/* Arguments :																			*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	uint16_t s_compCurrentRpmSet						   				*/
/*					unit	%															*/
/*					range:	0~100														*/
/*                                                                                      */
/* Using Global Data:	(I) uint16_t s_compCurrentRpmSet								*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getRpm(void)
{
	return s_compCurrentRpmSet;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  compressor_setTcomHspTime											*/
/*                                                                                      */
/* Details:   set s_tcompHspTime. this function is only call from PsaSelectMgr when		*/
/*  		  temperature condition is changed											*/
/*                                                                                      */
/* Arguments :	(I) E_TempertureType type												*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 														   				*/
/*                                                                                      */
/* Using Global Data:	(O) uint16_t s_tCompHspTime										*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void compressor_setTcomHspTime(E_TempertureType type)
{
	switch (type) {
	case NORM_TEMP:
	case HIGH_TEMP:
		s_tCompHspTime = T_COMP_HSP_TIME_NORMAL/DEVICE_TASK_DELAY_TIME;
		break;
	case LOW_TEMP:
		s_tCompHspTime = T_COMP_HSP_TIME_LOWTEMP/DEVICE_TASK_DELAY_TIME;
	default:
		break;
	}
	return;
}


