/********************************************************************/
/*                                                                  */
/* File Name    : Compressor.c	                                 	*/
/*                                                                  */
/* General      : This module contains function to control 			*/
/*                compressor				                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include <assert.h>
#include <string.h>
#include <Compressor.h>
#include <Gpio.h>
#include "math.h"
#include <TaskCommon.h>
#include "board.h"
#include "util.h"

//#define DEBUG_COMP_EN
#ifdef DEBUG_COMP_EN
#define DEBUG_COMP(...) printf(__VA_ARGS__)
#else
#define DEBUG_COMP(...)
#endif

#define POLYNOMIAL 								0x8810
#define INVALID 								-1
#define COMP_UART_CHANEL						LPC_USART0
/* Transmit and receive ring buffer sizes */
#define COMP_UART_SRB_SIZE 						128	/* Send */
#define COMP_UART_RRB_SIZE 						32	/* Receive */

#define COMP_CHECKSUM_ERR_BIT 		0x01
#define COMP_COMMAND_ERR_BIT 		0x02
#define COMP_PARAM_ERR_BIT 			0x04

#define COMP_RESPONSE_LENGTH 					9
#define COMP_RESET_TIME_100MS 					100/DEVICE_TASK_DELAY_TIME
#define COMP_MAX_TIME_OF_RECEIVE_CMD 			700/DEVICE_TASK_DELAY_TIME //(700ms/50ms = 14)
#define COMP_INTERVAL_TIME_SEND					1000/DEVICE_TASK_DELAY_TIME
#define COMP_MAX_RESEND_CNT						5
#define COMP_RESET_TIME_100MS					100/DEVICE_TASK_DELAY_TIME //(devTask loop 50ms 50ms*2 = 100ms: reset time)
#define COMP_REBOOT_TIME1S            			1000/DEVICE_TASK_DELAY_TIME //1000ms/50ms
#define COMP_RESTART_TIME_40S 					40000/DEVICE_TASK_DELAY_TIME //40000ms/50ms

static const unsigned char ConfirmCommunicationCode = '1';
static const unsigned char StatusAcquisitionCode = '2';
static const unsigned char CommandRPMCode = '3';
static const unsigned char VersionCode = '4';
static const unsigned char ErrorStatCode = '5';
static const unsigned char TemperatureAcquisitionCode = '6';
static const unsigned char ComunicationConfirm[4] = {'Z','Z','Z','Z'};
static const unsigned char StatusAcquisition[4] = {'Z','Z','Z','Z'};
static const unsigned char VersionAcquisition[4] = {'Z','Z','Z','Z'};
static const unsigned char ErrorStatus[4] = {'Z','Z','Z','Z'};
static const unsigned char TemperatureAcquisition[4] = {'Z','Z','Z','Z'};

unsigned char s_compVersion[8] = {'0','0','0','0','0','0','0','\0'};

/* Transmit and receive ring buffers */
static RINGBUFF_T gs_compRxring, gs_compTxring;
/* Transmit and receive buffers */
static uint8_t gs_compRxbuff[COMP_UART_RRB_SIZE], gs_compTxbuff[COMP_UART_SRB_SIZE];

static uint16_t s_compStatus = 0x0000;

static uint8_t s_compCommunicateStatus = 0x00; //truong add

static E_CompressorStatus s_compErrNoAlarmStatus = eCompressorNoErr;

static uint8_t s_compTimeReceiveCmd = 0;

static uint8_t s_compReSendCnt = 0;

static E_CompressorSequence s_compressorSeq = COMP_NORMAL_SEQ;

static uint8_t s_compRebootTimeCnt = 0;

static uint8_t s_compResetTimeCnt = 0;

static uint16_t s_compRestartTimeCnt = 0;

static uint32_t s_compRpmMsr = 0;

static uint32_t s_compMotorTemp = 0;
static uint32_t s_compBoardTemp = 0;

static uint16_t s_compCurrentRpmSet = 1000;

static bool gs_isGetVersionDone = false;



static E_CompressorRequestCmd s_compRequestSendCmd = COMP_SEND_RPM;

static E_CompressorRequestCmd s_preCompRequestSendCmd = COMP_SEND_RPM;

static void compressor_SendRPMCmd(unsigned int rpm);

static void compressor_SendVersionAcquisitionCmd();

static void compressor_SendErrorStatusResetCmd();

static void compressor_SendTemperatureAcquisitionCmd();
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	comppressor_CreateCRC					              				*/
/*                                                                                      */
/* Details: Create CRC for compressor communication message								*/
/*                                                                                      */
/* Arguments:	(I) unsigned char data[]												*/
/*              (I) char nbrOfBytes                                                     */
/* ReturnValue : unsigned char		                                                    */
/*                                                                                      */
/****************************************************************************************/
static unsigned char comppressor_CreateCRC(unsigned char data[], char nbrOfBytes)
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
/* Function name: 	compressor_RequestSendClearErrStatusCmd	              				*/
/*                                                                                      */
/* Details: Request send command to clear Error status									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_RequestSendClearErrStatusCmd()
{
	s_compRequestSendCmd = COMP_SEND_CLR_ERR;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleResetNoti				              				*/
/*                                                                                      */
/* Details: Request send command to clear Error status									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleResetNoti()
{
	DEBUG_COMP("Handle reset \n");
	s_compErrNoAlarmStatus = eCompressorReset;
	compressor_RequestSendClearErrStatusCmd();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_BackToNormalSeq				              				*/
/*                                                                                      */
/* Details: Back to normal sequence														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_BackToNormalSeq()
{
	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&gs_compRxring, gs_compRxbuff, 1, COMP_UART_RRB_SIZE);
	RingBuffer_Init(&gs_compTxring, gs_compTxbuff, 1, COMP_UART_SRB_SIZE);

	s_compressorSeq = COMP_NORMAL_SEQ;
	s_compTimeReceiveCmd = 0;
	s_compReSendCnt = 0;

	if(s_compRequestSendCmd != COMP_SEND_VERSION)
	{
		s_compRequestSendCmd = COMP_SEND_CLR_ERR;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ProcessSendCmdRequest		              				*/
/*                                                                                      */
/* Details: Send current resquested command to compressor								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_ProcessSendCmdRequest()
{
	static int16_t s_timeToSendCnt = -1;
	if((s_timeToSendCnt >= COMP_INTERVAL_TIME_SEND) || (s_timeToSendCnt == -1))
	{
		s_timeToSendCnt = 0;
		s_compTimeReceiveCmd = 1;//Allow time receive count
		switch (s_compRequestSendCmd) {
		case COMP_SEND_VERSION:
			DEBUG_COMP("V \n");
			s_compRequestSendCmd = COMP_SEND_RPM;	//COMP_SEND_CLR_ERR
			compressor_SendVersionAcquisitionCmd();
			s_preCompRequestSendCmd = COMP_SEND_VERSION;
			break;
		case COMP_SEND_CLR_ERR:
			DEBUG_COMP("COMP CLR \n");
			s_compRequestSendCmd = COMP_SEND_VERSION; //COMP_SEND_RPM
			compressor_SendErrorStatusResetCmd();
			s_preCompRequestSendCmd = COMP_SEND_CLR_ERR;
			break;
		case COMP_SEND_RPM:
			DEBUG_COMP("RPM \n");
			s_compRequestSendCmd = COMP_SEND_ACQ;
			compressor_SendRPMCmd(s_compCurrentRpmSet);
			s_preCompRequestSendCmd = COMP_SEND_RPM;
			break;
		case COMP_SEND_ACQ:
			DEBUG_COMP("COMP ACQ \n");
			s_compRequestSendCmd = COMP_SEND_TEMPERATURE;
			compressor_SendStatusAcquisitionCmd();
			s_preCompRequestSendCmd = COMP_SEND_ACQ;
			break;
		case COMP_SEND_TEMPERATURE:
			DEBUG_COMP("COMP TEMP \n");
			s_compRequestSendCmd = COMP_SEND_ACQ;
			compressor_SendTemperatureAcquisitionCmd();
			s_preCompRequestSendCmd = COMP_SEND_ACQ;
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
/* Function name: 	compressor_HandleResendCmd				              				*/
/*                                                                                      */
/* Details: Resend command to compressor controller										*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleResendCmd()
{
	if(s_compReSendCnt < COMP_MAX_RESEND_CNT)
	{
		DEBUG_COMP("Resend \n");
		s_compRequestSendCmd = s_preCompRequestSendCmd;
		s_compReSendCnt++;

		if((s_compErrNoAlarmStatus != eCompResponseCodeErr)
				&& (s_compErrNoAlarmStatus != eCompResponseChecksumErr)
				&& (s_compErrNoAlarmStatus != eCompResponseLengthErr)
				&& (s_compErrNoAlarmStatus != eCompResponseParamErr))
		{
			s_compErrNoAlarmStatus = eCompNoResponseErr;
		}
	}
	else
	{
		s_compStatus = s_compStatus | COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT;
		DEBUG_COMP("eCommunicationError \n");
		s_compReSendCnt = 0;
		compressor_Reset();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleNormalSeq				              				*/
/*                                                                                      */
/* Details: Handle normal operation of compressor										*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleNormalSeq()
{
	compressor_ReadFromDevice();
	if(s_compTimeReceiveCmd >= COMP_MAX_TIME_OF_RECEIVE_CMD)
	{
		s_compTimeReceiveCmd = 0;
		compressor_HandleResendCmd();
	}
	compressor_ProcessSendCmdRequest();
	if(s_compTimeReceiveCmd > 0)
	{
		s_compTimeReceiveCmd++;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleResetSeq				              				*/
/*                                                                                      */
/* Details: Handle seset sequence of compressor											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleResetSeq()
{
	s_compResetTimeCnt++;
	if(s_compResetTimeCnt >= COMP_RESET_TIME_100MS)
	{
		s_compResetTimeCnt = 0;
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
		s_compressorSeq = COMP_REBOOT_SEQ;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleRebootSeq				              				*/
/*                                                                                      */
/* Details: Handle reboot sequence of compressor										*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleRebootSeq()
{
	s_compRebootTimeCnt++;
	if(s_compRebootTimeCnt >= COMP_REBOOT_TIME1S)
	{
		s_compRebootTimeCnt = 0;
		compressor_BackToNormalSeq();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleRebootForReStartSeq	              				*/
/*                                                                                      */
/* Details: Handle reboot sequence for restart compressor								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleRebootForReStartSeq()
{
	DEBUG_COMP("Boot restrt \n");
	s_compRebootTimeCnt++;
	if(s_compRebootTimeCnt >= COMP_REBOOT_TIME1S)
	{
		s_compRebootTimeCnt = 0;
		s_compressorSeq = COMP_RESTART_SEQ;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleResetForRestartSeq		              				*/
/*                                                                                      */
/* Details: Handle reset sequence for restart compressor								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleResetForRestartSeq()
{
	s_compResetTimeCnt++;
	if(s_compResetTimeCnt >= COMP_RESET_TIME_100MS)
	{
		s_compResetTimeCnt = 0;
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
		s_compressorSeq = COMP_REBOOT_FOR_RESTART_SEQ;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_HandleRestartSeq				              				*/
/*                                                                                      */
/* Details: Handle restart sequence														*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                		                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_HandleRestartSeq(uint16_t rpm)
{
	if(s_compRestartTimeCnt == 0)
	{
		compressor_ReqestSetRpm(COMP_MAX_RPM);
	}
	s_compRestartTimeCnt++;
	if (s_compRestartTimeCnt >= COMP_RESTART_TIME_40S)
	{
		compressor_ReqestSetRpm(rpm);
		s_compressorSeq = COMP_NORMAL_SEQ;
	}
	compressor_HandleNormalSeq();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendCmd						              				*/
/*                                                                                      */
/* Details: send command to compressor driver											*/
/*                                                                                      */
/* Arguments:	(I) unsigned char code													*/
/*              (O) const unsigned char parameter[]                                     */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendCmd(unsigned char code,const unsigned char parameter[])
{
	unsigned char cmd[6];
	cmd[0] = code;
	cmd[1] = parameter[0];
	cmd[2] = parameter[1];
	cmd[3] = parameter[2];
	cmd[4] = parameter[3];
	cmd[5] = comppressor_CreateCRC(cmd,5);
	Chip_UART_SendRB(COMP_UART_CHANEL, &gs_compTxring, cmd, 6);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendVersionAcquisitionCmd	              				*/
/*                                                                                      */
/* Details: send command to get version of compressor Controller						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendVersionAcquisitionCmd()
{
	compressor_SendCmd(VersionCode,VersionAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendErrorStatusResetCmd		              				*/
/*                                                                                      */
/* Details: Send error status reset command												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendErrorStatusResetCmd()
{
	compressor_SendCmd(ErrorStatCode,ErrorStatus);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendTemperatureAcquisitionCmd             				*/
/*                                                                                      */
/* Details: Send command to get current temperature of compressor controller			*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendTemperatureAcquisitionCmd()
{
	compressor_SendCmd(TemperatureAcquisitionCode,TemperatureAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UART0_IRQHandler						             				*/
/*                                                                                      */
/* Details: Handle interrupt for Uart 0													*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void UART0_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(COMP_UART_CHANEL, &gs_compRxring, &gs_compTxring);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_InitCommunication			             				*/
/*                                                                                      */
/* Details: Init UART configuration for compressor controller							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_InitCommunication(void)
{

	//Firts cmd send version acq
	s_compRequestSendCmd = COMP_SEND_VERSION;

	Board_UART_Init(COMP_UART_CHANEL);
	//Set up reset signal
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,COMP_RESET_PORT_NUM, COMP_RESET_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,COMP_POW_ON_PORT_NUM, COMP_POW_ON_BIT_NUM);		/* output */

	// Reset signal usually set high
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM); // Enable power for compressor controller
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,COMP_POW_ON_PORT_NUM, COMP_POW_ON_BIT_NUM);		/* output */

	/* Setup UART for 115.2K8N1 */
	Chip_UART_DeInit(COMP_UART_CHANEL);
	Chip_UART_Init(COMP_UART_CHANEL);
	Chip_UART_SetBaud(COMP_UART_CHANEL, 115200);
	Chip_UART_ConfigData(COMP_UART_CHANEL, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
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

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(USART0_IRQn, 3);
	NVIC_EnableIRQ(USART0_IRQn);

	compressor_Reset();

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Handle			     			        				*/
/*                                                                                      */
/* Details: Handle communication with compressor controller								*/
/*                                                                                      */
/* Arguments:	(I) uint16_t rpm														*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_Handle(uint16_t rpm)
{
	if (rpm > COMP_MAX_RPM)
	{
		DEBUG_COMP("Compressor rpm out of range: \n");
		return;
	}
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
	case COMP_REBOOT_FOR_RESTART_SEQ:
		compressor_HandleRebootForReStartSeq();
		break;
	case COMP_RESET_FOR_RESTART_SEQ:
		compressor_HandleResetForRestartSeq();
		break;
	case COMP_RESTART_SEQ:
		compressor_HandleRestartSeq(rpm);
		break;
	default:
		assert(false);
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendComunicationConfirmCmd		        				*/
/*                                                                                      */
/* Details: Send command to compressor controller to confirm communication				*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_SendComunicationConfirmCmd()
{
	compressor_SendCmd(ConfirmCommunicationCode,ComunicationConfirm);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendStatusAcquisitionCmd			        				*/
/*                                                                                      */
/* Details: Send command to get current status of compressor							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_SendStatusAcquisitionCmd()
{
	compressor_SendCmd(StatusAcquisitionCode,StatusAcquisition);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_SendRPMCmd						        				*/
/*                                                                                      */
/* Details: Send command to compressor to set RPM										*/
/*                                                                                      */
/* Arguments:	(I) unsigned int rpm													*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void compressor_SendRPMCmd(unsigned int rpm)
{
	DEBUG_COMP("rpm set: %d \n",rpm);
	if(rpm > COMP_MAX_RPM)
	{
		DEBUG_COMP("Error RPM out of range \n");
		assert(false);
		return;
	}
	unsigned char numberArray[] = {'0','0','0','0'};
	if(rpm != 0)
	{
		//convert to asii code
		unsigned int number = rpm;
		int n = log10(number) + 1;
		int i;

		for(i = 3 ;i >= 4 - n;--i,number /= 10)
		{
			numberArray[i] = number%10 + '0';
		}
	}
	//done
	const unsigned char* cmd = numberArray;
	compressor_SendCmd(CommandRPMCode,cmd);

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ReadFromDevice					        				*/
/*                                                                                      */
/* Details: Read and then handle Message returned from compressor						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void compressor_ReadFromDevice()
{
	unsigned char dataRecv[9];
	unsigned char CurrentRPM[5] = {'0','0','0','0','\0'};
	unsigned char CurrentMotorTemp[4]={'0','0','0','\0'};
	unsigned char CurrentBoardTemp[4]={'0','0','0','\0'};
	int n = Chip_UART_ReadRB(COMP_UART_CHANEL, &gs_compRxring, dataRecv, 9);
	if (n == COMP_RESPONSE_LENGTH)
	{
		s_compStatus = s_compStatus & (~(1<<15));// Clear COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT bit
		if(comppressor_CreateCRC(dataRecv,8) == dataRecv[8])
		{
			switch (dataRecv[0]){
			case 'A':
				DEBUG_COMP("Communication Confirm \n");
				for (int j = 1; j < 7; j ++ )
				{
					if(dataRecv[j] != 'R')
					{
						s_compErrNoAlarmStatus = eCompResponseParamErr;
						break;
					}
				}
				if (s_compErrNoAlarmStatus != eCompResponseParamErr)
				{
					s_compErrNoAlarmStatus = eCompressorNoErr;
					s_compTimeReceiveCmd = 0;
					s_compReSendCnt = 0;
				}
				break;
			case 'B':

			case 'C':
				DEBUG_COMP("Command RPM \n");
				//byte 1,2,3,4 --> CurrentRPM
				CurrentRPM[0] = dataRecv[1];
				CurrentRPM[1] = dataRecv[2];
				CurrentRPM[2] = dataRecv[3];
				CurrentRPM[3] = dataRecv[4];
				DEBUG_COMP("CurrentRPM %s \n",CurrentRPM);
				s_compRpmMsr = HexCharToInt(CurrentRPM[0]) * 1000 + HexCharToInt(CurrentRPM[1])  *100 +
						HexCharToInt(CurrentRPM[2]) * 10 + HexCharToInt(CurrentRPM[3]); //scale 100
				//Check err status byte 1
				if (dataRecv[5] & DC_OVER_VOLT_ERR_BIT)
				{
					s_compStatus = s_compStatus | DC_OVER_VOLT_ERR_BIT;
				}
				if(dataRecv[5] & DC_LOW_VOLT_ERR_BIT)
				{
					s_compStatus = s_compStatus | DC_LOW_VOLT_ERR_BIT;
				}
				if(dataRecv[5] & SOFT_OVER_CURRENT_ERR_BIT)
				{
					s_compStatus = s_compStatus | SOFT_OVER_CURRENT_ERR_BIT;
				}
				if(dataRecv[5] & HARD_OVER_CURRENT_ERR_BIT)
				{
					s_compStatus = s_compStatus | HARD_OVER_CURRENT_ERR_BIT;
				}
				if(dataRecv[5] & BOARD_TEMP_ERR_BIT)
				{
					s_compStatus = s_compStatus | BOARD_TEMP_ERR_BIT;
					compressor_Stop();
				}
				if(dataRecv[5] & MOTOR_TEMP_ERR_BIT)
				{
					s_compStatus = s_compStatus | MOTOR_TEMP_ERR_BIT;
				}
				if(dataRecv[5] & RPM_ERR_BIT)
				{
					s_compStatus = s_compStatus | RPM_ERR_BIT;
				}
				if(dataRecv[5] & MOTOR_STEP_OUT_ERR_BIT)
				{
					s_compStatus = s_compStatus | MOTOR_STEP_OUT_ERR_BIT;
				}

				DEBUG_COMP("err status %x, %x\n",dataRecv[5],dataRecv[6]);

				//Check err status byte 2

				if((dataRecv[6]<<8) & COMP_CMD_TIME_OUT_BIT)
				{
					s_compStatus = s_compStatus | COMP_CMD_TIME_OUT_BIT;
				}
				if((dataRecv[6]<<8) & COMP_RESET_NOTI_BIT)
				{
					s_compStatus = s_compStatus | COMP_RESET_NOTI_BIT;
					compressor_HandleResetNoti();
				}
				s_compErrNoAlarmStatus = eCompressorNoErr;
				s_compTimeReceiveCmd = 0;
				s_compReSendCnt = 0;
				break;
			case 'D':
				DEBUG_COMP("Version Acquisition \n");
				s_compVersion[0] = dataRecv[1];
				s_compVersion[1] = dataRecv[2];
				s_compVersion[2] = dataRecv[3];
				s_compVersion[3] = dataRecv[4];
				s_compVersion[4] = dataRecv[5];
				s_compVersion[5] = dataRecv[6];
				s_compVersion[6] = dataRecv[7];
				gs_isGetVersionDone = true;
				s_compErrNoAlarmStatus = eCompressorNoErr;
				s_compTimeReceiveCmd = 0;
				s_compReSendCnt = 0;
				break;
			case 'E':
				DEBUG_COMP("Error Status Reset \n");
				for (int j = 1; j < 7; j ++ )
				{
					if(dataRecv[j] != 'R')
					{
						s_compErrNoAlarmStatus = eCompResponseParamErr;
						break;
					}
				}
				if (s_compErrNoAlarmStatus != eCompResponseParamErr)
				{
					s_compErrNoAlarmStatus = eCompressorNoErr;
					s_compTimeReceiveCmd = 0;
					s_compReSendCnt = 0;
				}
				break;
			case 'W':
					DEBUG_COMP("Error \n");
					if (dataRecv[1] & COMP_CHECKSUM_ERR_BIT)
					{
						s_compErrNoAlarmStatus = eCompChecksumErr;
						s_compCommunicateStatus = s_compCommunicateStatus | COMP_CHECKSUM_ERR_BIT;	// truong add
					}
					else if(dataRecv[1] & COMP_COMMAND_ERR_BIT)
					{
						s_compErrNoAlarmStatus = eCompCommandErr;
						s_compCommunicateStatus = s_compCommunicateStatus | COMP_COMMAND_ERR_BIT; // truong add
					}
					else if(dataRecv[1] & COMP_PARAM_ERR_BIT)
					{
						s_compErrNoAlarmStatus = eCompParamErr;
						s_compCommunicateStatus = s_compCommunicateStatus | COMP_PARAM_ERR_BIT; // truong add
					}
					s_compTimeReceiveCmd = 0;
					s_compReSendCnt = 0;
					break;
			case 'F':
				CurrentMotorTemp[0]=dataRecv[1];
				CurrentMotorTemp[1]=dataRecv[2];
				CurrentMotorTemp[2]=dataRecv[3];
				CurrentBoardTemp[0]=dataRecv[4];
				CurrentBoardTemp[1]=dataRecv[5];
				CurrentBoardTemp[2]=dataRecv[6];
				s_compMotorTemp = HexCharToInt(CurrentMotorTemp[0]) * 100 + HexCharToInt(CurrentMotorTemp[1])  *10 +
						HexCharToInt(CurrentMotorTemp[2]);
				s_compBoardTemp = HexCharToInt(CurrentBoardTemp[0]) * 100 + HexCharToInt(CurrentBoardTemp[1])  *10 +
						HexCharToInt(CurrentBoardTemp[2]);
				DEBUG_COMP("Temperature %d %d\n",s_compMotorTemp,s_compBoardTemp);
				s_compErrNoAlarmStatus = eCompressorNoErr;
				s_compTimeReceiveCmd = 0;
				s_compReSendCnt = 0;
				break;
			default:
				s_compErrNoAlarmStatus = eCompResponseCodeErr;
				break;
			}
		}
		else
		{
			s_compErrNoAlarmStatus = eCompResponseChecksumErr;
		}
	}
	else
	{
		if(n >0)
		{
			DEBUG_COMP("Comp Length %d, data %x \n",n,dataRecv[0]);
			s_compErrNoAlarmStatus = eCompResponseLengthErr;
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getErrStatus						        				*/
/*                                                                                      */
/* Details: Get current compressor error status											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t				                                                */
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getErrStatus()
{
	//	DEBUG_COMP("s_compStatus: %d \n",s_compStatus);
	return s_compStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getComunicateErrStatus			        				*/
/*                                                                                      */
/* Details: Get current compressor communication error status							*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint8_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint8_t compressor_getCommunicateErrStatus()
{
	//	DEBUG_COMP("s_compStatus: %d \n",s_compStatus);
	return s_compCommunicateStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getResponseErrStatus				        				*/
/*                                                                                      */
/* Details: Get current compressor error response status								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : E_CompressorStatus	                                                    */
/*                                                                                      */
/****************************************************************************************/
E_CompressorStatus compressor_getResponseErrStatus()
{
	return s_compErrNoAlarmStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_GetCurrentSeq					        				*/
/*                                                                                      */
/* Details: Get current sequence of compressor											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : E_CompressorSequence                                                   */
/*                                                                                      */
/****************************************************************************************/
E_CompressorSequence compressor_GetCurrentSeq()
{
	//	DEBUG_COMP("s_compressorSeq: %d \n",s_compressorSeq);
	return s_compressorSeq;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getRpm							        				*/
/*                                                                                      */
/* Details: Get current RPM of compressor												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getRpm()
{
	return s_compRpmMsr;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getBoardTemperature				        				*/
/*                                                                                      */
/* Details: Get compressor board temperature											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getBoardTemperature()
{
	return s_compBoardTemp;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_getMotorTemperature				        				*/
/*                                                                                      */
/* Details: Get compressor temperature													*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : uint16_t				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t compressor_getMotorTemperature()
{
	return s_compMotorTemp;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Reset							        				*/
/*                                                                                      */
/* Details: Reset compressor															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None			                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Reset()
{
	DEBUG_COMP("compressor_Reset\n");
	// Set reset signal low 100ms for reset fc
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
	s_compressorSeq = COMP_RESET_SEQ;
	s_compRebootTimeCnt = 0;
	s_compResetTimeCnt = 0;
	s_compStatus = 0x8000;	// truong add
	s_compCommunicateStatus = 0;	// truong add
	s_compErrNoAlarmStatus = eCompressorNoErr;	// truong add
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Restart							        				*/
/*                                                                                      */
/* Details: Restart compressor															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Restart()
{
	DEBUG_COMP("compressor_Restart\n");
	// Set reset signal low 100ms for reset fc
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,COMP_RESET_PORT_NUM,COMP_RESET_BIT_NUM);
	s_compressorSeq = COMP_RESET_FOR_RESTART_SEQ;
	s_compRebootTimeCnt = 0;
	s_compResetTimeCnt = 0;
	s_compRestartTimeCnt = 0;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_Stop								        				*/
/*                                                                                      */
/* Details: Stop compressor																*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_Stop()
{
	s_compTimeReceiveCmd = 0;
	s_compReSendCnt = 0;

	compressor_ReqestSetRpm(0);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ClearAlarmStatus					        				*/
/*                                                                                      */
/* Details: Clear Alarm status															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_ClearAlarmStatus()
{
	s_compStatus = 0x0000;
	s_compCommunicateStatus = 0x00;// truong add
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_UpdateRpmSet						        				*/
/*                                                                                      */
/* Details: Update RPM Set																*/
/*                                                                                      */
/* Arguments:	(I) uint16_t rpm														*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_UpdateRpmSet(uint16_t rpm)
{
	s_compCurrentRpmSet = rpm;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_ReqestSetRpm						        				*/
/*                                                                                      */
/* Details: request set rpm for compressor, compressor_Handle() function will process   */
/* 			this request																*/
/*                                                                                      */
/* Arguments:	(I) uint16_t rpm														*/
/*              (O) None						                                        */
/* ReturnValue : None				                                                   	*/
/*                                                                                      */
/****************************************************************************************/
void compressor_ReqestSetRpm(uint16_t rpm)
{
	s_compCurrentRpmSet = rpm;
	s_compRequestSendCmd = COMP_SEND_RPM;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_GetVersion						        				*/
/*                                                                                      */
/* Details: Get version of compressor controller driver									*/
/*                                                                                      */
/* Arguments:	(I) char* buffer														*/
/*              (O) char* buffer				                                        */
/* ReturnValue : unsigned char*		                                                   	*/
/*                                                                                      */
/****************************************************************************************/
unsigned char* compressor_GetVersion(char* buffer)
{
	memcpy(buffer,s_compVersion,8);
	return s_compVersion;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	compressor_isGetVersionDone			              				    */
/*                                                                                      */
/* Details:        Check whether read version process is finished						*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*              (O) None                                                                */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool compressor_isGetVersionDone()
{
	return gs_isGetVersionDone;
}



