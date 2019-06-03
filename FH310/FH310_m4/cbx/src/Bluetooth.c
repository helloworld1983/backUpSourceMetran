/********************************************************************/
/*                                                                  */
/* File Name    : Bluetooth.c                       				*/
/*                                                                  */
/* General      : Contains function for init and communicate with	*/
/*                 Bluetooth BT121 module using dumo_bglib          */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* 		       2016/11/09      Quyen Ngo(MV)  	    new file        */
/*  #0001	   2017/09/15      Linh Nguyen		  	modify		    */
/*  Improve BT send packet function when the packet len more than 	*/
/*	253bytes, the FH310 will split to the small packets				*/
/*  #0002	   2017/09/20      Linh Nguyen		  	modify		    */
/*  Rename macro and move to .h file							 	*/
/*	Check packet len when receive packet via RS232					*/
/*  #0003      2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule, remove use code	     	*/
/*	Handle more case of getting packet for CBX function				*/
/* 		       2017/10/12       Linh Nguyen	  	    modify          */
/*  #0004 send the error when paket over packet length (bug 1949)	*/
/*	#0005	   2017/10/29		Quyen Ngo			modify			*/
/*		Move the ring buffer to RAM 10								*/
/*	#0006	   2017/11/20		Quyen Ngo			modify			*/
/*		Change the type of the parameter of bluetooth_SendMsg func	*/
/*		for fix Coverity warning									*/
/*	#0007	   2018/02/05		Quyen Ngo			modify			*/
/*		Reset bluetooth packet receive state when the length of msg	*/
/*		is wrong (Bug 2063)											*/
/*	#0008	   2018/02/08		Quyen Ngo			modify			*/
/*		Reset bluetooth when communication dead for long time		*/
/*		(New spec 2018/02/08)										*/
/*	#0009	   2018/02/21		Quyen Ngo			modify			*/
/*		Change the reset count time condition for correct the reset */
/*		time(100ms)for fix bug 2071									*/
/*	#0010	   2018/02/21		Quyen Ngo			modify			*/
/*		Add disable Uart interrupt before init uart in Bluetooth 	*/
/*		reset sequence (Bug 2069)									*/
/*	#0011	   2018/03/14		Quyen Ngo			modify			*/
/*		Change code for adapte with reset sequnce of Bluetooth in	*/
/*		new specification											*/
/*	#0012	   2018/03/15		Quyen Ngo			modify			*/
/*		Call bluetooth_Setmode directly when mode change to			*/
/*		undiscover mode for fix bug 2074							*/
/*	#0013	   2018/03/15		Quyen Ngo			modify			*/
/*		Move macro define to follow coding rule						*/
/*																	*/
/********************************************************************/
/* include system files */
#include "Bluetooth.h"

#include <errno.h>
#include <stdlib.h>
#include <StringTools.h>
/* include user files */
#include "board.h"
#include "dumo_bglib.h"
#include "TaskCommon.h"
#include "GuiTask.h"
#include "GuiInterface.h"

#include "CBX02Handler.h"
/* constants, macro definition */

#ifdef DEBUG
#define DEBUG_BLE_EN
#endif

#ifdef DEBUG_BLE_EN
#define DEBUG_BLE(...) printf(__VA_ARGS__)
#else
#define DEBUG_BLE(...)
#endif

#ifdef DEBUG
#define DEBUG_CBX_EN
#endif

#ifdef DEBUG_CBX_EN
#define DEBUG_CBX(...) printf(__VA_ARGS__)
#else
#define DEBUG_CBX(...)
#endif

#define MAX_BT_PK_SIZE 253

#define BT_RESET_CNT_TIME_MS		500/BT_TASK_DELAY_TIME_MS
//assign to time count when reseted BT to avoid time count loopback to reset again
#define BT_EXIT_RESET_CNT			0xff

#define BL_RESET_PORT 				6
#define BL_RESET_BIT				0

#define BT_ADDR_MAX_INDEX			5

#define BT_RESPONSE_CODE			0x20
#define BT_EVENT_CODE				0xa0

#define AMOUNT_TO_SEND (1 * (250*4 * 250*4) )

#define RFCOM_SDP_ENTRY_ID					2
#define RFCOM_STREAM_DESTINATION_ENDPOINT	0
#define SM_ALLOW_BONDING_NOT_MITM			0
#define SM_REQUIRE_MITM						1
#define ALLOW_BONDING						1
#define PINCODE_LENGTH						8
#define BT_GAP_DISCOVERABLE					1
#define BT_GAP_NONDISCOVER					0
#define BT_GAP_CONECTABLE					1
#define BT_GAP_NON_CONECTABLE				0

/** >>#0013
		Move macro define to beginning of file for follow coding rule
#0013 >>**/
#define BT_INQ_RESET_CNT_TIME_MS		50/BT_TASK_DELAY_TIME_MS

/** >>#008
	Reset bluetooth when communication dead for long time	
	(New sec 2018/02/08)
#008 >>**/
/** >>#0011
		Remove unused code because the BT reset sequence specification change
	#0011 >>**/
/** >>#0011
		Change code because the BT reset sequence specification change
	#0011 >>**/
#define T_BLCOMP							7*60000/BT_TASK_DELAY_TIME_MS

BGLIB_DEFINE();

typedef enum
{
	BT_NORMAL,
	/** >>#0011
		Remove unused state because the BT reset sequence specification change
	#0011 >>**/
	BT_RESET
}E_BtInQuiryState;

/* definitions of structures */
/* Transmit and receive ring buffers */
static RINGBUFF_T s_txringBle, s_rxringBle;
/* Transmit and receive buffers */
__BSS(RAM10) static uint8_t s_rxbuff[BT_UART_RRB_SIZE], s_txbuff[BT_UART_SRB_SIZE];
/** Pointer to wifi packet*/
struct dumo_cmd_packet* pck;
/** Buffer for storing data from the serial port. */
static uint8_t buffer[BGLIB_MSG_MAXLEN];

static int8_t s_btAddr[BT_ADD_LENGTH] = {'\0'};

static E_BtMode s_btMode = UNDISCOVER_MODE;

/** >>#008
	Reset bluetooth when communication dead for long time	
	(New sec 2018/02/08)
#008 >>**/
static E_BtInQuiryState s_btInquiryState = BT_NORMAL;
static uint32_t s_idleTimeCnt = 0;
/** >>#0011
	Remove unused code because the BT reset sequence specification change
#0011 >>**/

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_SendMsg			                                         	*/
/*                                                                                      */
/* Details  -Function pass into bgapi lib		                 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) msg_len Length of the message					 					*/
/*				msg_data Message data, including the header.							*/
/*              data_len Optional variable data length.                                 */
/*			    data Optional variable data.											*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*																				  		*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_SendMsg(uint8 msg_len, uint8* msg_data, uint16 data_len, uint8* data)
{
	/** Variable for storing function return values. */
	int ret;

	DEBUG_BLE("ble_SendMsg() %d \n",msg_len);

	ret = Chip_UART_SendRB(LPC_UART1, &s_txringBle, msg_data, msg_len);
	for(int i = 0; i< msg_len; i++)
	{
		DEBUG_BLE("msg pk[%d]: %x \n",i,msg_data[i]);
	}
	if(ret < 0)
	{
		DEBUG_BLE("Err send UART \n");
	}

	if(data_len && data)
	{
		for(int i = 0; i< data_len; i++)
		{
			DEBUG_BLE("data pk[%d]: %x \n",i,data[i]);
		}
		ret = Chip_UART_SendRB(LPC_UART1, &s_txringBle, data, data_len);
		if(ret < 0)
		{
			DEBUG_BLE("Err send UART \n");
		}
	}
	return;
}


/** >>#008
	Reset bluetooth when communication dead for long time	
	(New sec 2018/02/08)
#008 >>**/
/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_ResetInquiryProcess			                               	*/
/*                                                                                      */
/* Details  -Reset all state and time coount variable for inquiry process				*/
/*																						*/
/*                                                                                      */
/* Arguments : 		NONE																*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) E_BtInQuiryState s_btInquiryState                         	*/
/*                     (O) uint32_t s_idleTimeCnt                          			   	*/
/*                     (O) uint32_t s_btWaitInqResCnt						           	*/
/*					   (O) uint32_t s_btInqRetryCnt									    */
/* local variables   : NONE			                                                  	*/
/*                                         												*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_ResetInquiryProcess(void)
{
	s_btInquiryState = BT_NORMAL;
	s_idleTimeCnt = 0;
	/** >>#0011
		Remove unused code because the BT reset sequence specification change
	#0011 >>**/
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_ResetUart			                           		    	*/
/*                                                                                      */
/* Details  -Reset Uart configuration, ring buffer										*/
/*																						*/
/*                                                                                      */
/* Arguments : 		NONE																*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : 	(O) RINGBUFF_T s_txringBle						            	*/
/*						(O) RINGBUFF_T s_rxringBle										*/
/*																						*/
/* local variables   : NONE			                                                  	*/
/*                                         												*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_ResetUart(void)
{
	/** >>#0010
		Add disable Uart interrupt before init uart in Bluetooth
		reset sequence (Bug 2069)
	#0010 >>**/
	NVIC_DisableIRQ(UART1_IRQn);
	/* Setup UART for 115.2K8N1 */
	Chip_UART_DeInit(LPC_UART1);
	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 115200);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART1);

	Chip_UART_SetModemControl(LPC_UART1,(UART_MCR_AUTO_RTS_EN | UART_MCR_AUTO_CTS_EN ));

	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&s_rxringBle, s_rxbuff, 1, BT_UART_RRB_SIZE);
	RingBuffer_Init(&s_txringBle, s_txbuff, 1, BT_UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, UART1_INT_PRIO);
	NVIC_EnableIRQ(UART1_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_CheckCommunicationIsNormal                      		    	*/
/*                                                                                      */
/* Details  -Reset Uart configuration, ring buffer										*/
/*																						*/
/*                                                                                      */
/* Arguments : 		(I) int32_t numBytesReceivefromBt									*/
/*					range -2^31~2^31						    						*/
/*                                                                                      */
/* ReturnValue : 	bool		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : 	(O) uint32_t s_idleTimeCnt						            	*/
/*																						*/
/*																						*/
/* local variables   : NONE			                                                  	*/
/*                                         												*/
/*                                                                                      */
/****************************************************************************************/
static bool bluetooth_CheckCommunicationIsNormal(int32_t numBytesReceivefromBt)
{
	bool isCommunicationNormal = true;
	if(numBytesReceivefromBt > 0)
	{
		s_idleTimeCnt = 0;
	}
	else
	{
		s_idleTimeCnt++;
		if(s_idleTimeCnt >= T_BLCOMP)
		{
			s_idleTimeCnt = 0;
			isCommunicationNormal = false;
		}
	}
	return isCommunicationNormal;
}

/** >>#0011
	Remove unused code because the BT reset sequence specification change
#0011 >>**/

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_ProcessReset                     	 		 			   	*/
/*                                                                                      */
/* Details  -Set reset pin of BT to low in 100ms to reset BT module						*/
/*																						*/
/*                                                                                      */
/* Arguments : 												    						*/
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : 	(I) RING_BUFFER_T uint32_t s_rxringBle					       	*/
/*																						*/
/*																						*/
/*																						*/
/* local variables   : NONE			                                                  	*/
/*                                         												*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_ProcessReset(void)
{
	/** >>#0011
		Change reset time the BT reset sequence specification change
	#0011 >>**/
	/** >>#0013
		Move macro define to beginning of file for follow coding rule
	#0013 >>**/
	static uint8_t s_resetCntTime = 0;
	/** >>#009
		Change s_resetCntTime condition to get 100ms reset time
	#009 >>**/
	s_resetCntTime++;
	if (s_resetCntTime > BT_INQ_RESET_CNT_TIME_MS)
	{
		s_resetCntTime = 0;
		RingBuffer_Flush(&s_rxringBle);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,BL_RESET_PORT, BL_RESET_BIT);		/* output */
		bluetooth_ResetInquiryProcess();
	}
}


/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_HandleInquirySeq                     	 		 		   	*/
/*                                                                                      */
/* Details  -Check communication and handle inquiry process of BT module				*/
/*																						*/
/*                                                                                      */
/* Arguments : 	(I) int32_t numBytesReceivefromBt										*/
/*					range -2^31~2^31						    						*/
/*                                                                                      */
/* ReturnValue : 	void		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : 	(I)(O) E_BtInQuiryState s_btInquiryState				       	*/
/*																						*/
/*																						*/
/*																						*/
/* local variables   : NONE			                                                  	*/
/*                                         												*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_HandleInquirySeq(int32_t numBytesReceivefromBt)
{
	if(s_btInquiryState == BT_NORMAL)
	{
		if (bluetooth_CheckCommunicationIsNormal(numBytesReceivefromBt) == false)
		{
			/** >>#0011
				Change code because the BT reset sequence specification change
			#0011 >>**/
			bluetooth_Reset();
		}
	}
	/** >>#0011
		Remove unused code because the BT reset sequence specification change
	#0011 >>**/
	if(s_btInquiryState == BT_RESET)
	{
		bluetooth_ProcessReset();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_UpdateAddress			                                    */
/*                                                                                      */
/* Details  -Update address of bluetooth			                 					*/
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
/*ã€Noteã€‘                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void bluetooth_UpdateAddress(bd_addr address)
{
	for (int i = BT_ADDR_MAX_INDEX; i >= 0; i--)
	{
		sprintf((char*)&s_btAddr[(BT_ADDR_MAX_INDEX-i)*2], "%02x", address.addr[i] );
	}
	DEBUG_BLE("%s \n",s_btAddr);
	GUI_EVENT_STRUCT event;
	event.id = eBluetoothAddrId;
	memcpy(event.data.btAddress, s_btAddr,BT_ADD_LENGTH);
	guiInterface_SendGuiEvent(event);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name bluetooth_PrintAddress	                                       		    */
/*                                                                                      */
/* Details  -Print address of bluetooth			                 						*/
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
/*ã€Noteã€‘                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void bluetooth_PrintAddress(bd_addr address)
{
	for (int i = BT_ADDR_MAX_INDEX; i >= 0; i--)
	{
		DEBUG_BLE("%02x", address.addr[i]);
		if (i > 0)
			DEBUG_BLE(":");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name print_name			                                       		    */
/*                                                                                      */
/* Details  -Print name of bluetooth			                 						*/
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
/*ã€Noteã€‘                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void bluetooth_PrintName(uint8array name)
{
	for (int i = name.len; i >= 0; i--)
	{
		DEBUG_BLE("%02x", name.data[i]);
		if (i > 0)
			DEBUG_BLE(":");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General Uart1 IRQ handle			                                           		    */
/*                                                                                      */
/* Details  -Handle interrupt for Uart 1		                 						*/
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
/****************************************************************************************/
void UART1_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */
	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_UART1, &s_rxringBle, &s_txringBle);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_Init			                                           		    */
/*                                                                                      */
/* Details  Init communication with blue tooth module			  						*/
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
/****************************************************************************************/
void bluetooth_Init(void)
{
	Board_UART_Init(LPC_UART1);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,BL_RESET_PORT, BL_RESET_BIT);		/* output */
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,BL_RESET_PORT, BL_RESET_BIT);		/* output */

	/* Setup UART for 115.2K8N1 */
	Chip_UART_DeInit(LPC_UART1);
	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 115200);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(LPC_UART1);

	Chip_UART_SetModemControl(LPC_UART1,(UART_MCR_AUTO_RTS_EN | UART_MCR_AUTO_CTS_EN ));

	/* Before using the ring buffers, initialize them using the ring
					   buffer init function */
	RingBuffer_Init(&s_rxringBle, s_rxbuff, 1, BT_UART_RRB_SIZE);
	RingBuffer_Init(&s_txringBle, s_txbuff, 1, BT_UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, UART1_INT_PRIO);
	NVIC_EnableIRQ(UART1_IRQn);

	BGLIB_INITIALIZE(bluetooth_SendMsg);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_HandleResponse                                           		    */
/*                                                                                      */
/* Details  Handle response of bluetooth module					  						*/
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
/****************************************************************************************/
void bluetooth_HandleResponse(void)
{
	static uint8_t s_resetCntTime = 0;
	/** Length of message payload data. */
	if(s_resetCntTime < BT_RESET_CNT_TIME_MS)
	{
		s_resetCntTime++;
	}
	else if (s_resetCntTime == BT_RESET_CNT_TIME_MS)
	{
		RingBuffer_Flush(&s_rxringBle);
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,BL_RESET_PORT, BL_RESET_BIT);		/* output */
		s_resetCntTime = BT_EXIT_RESET_CNT;
	}

	uint8_t dataRecv[BGLIB_MSG_MAXLEN];
	/** Variable for storing function return values. */
	int n;
	n = Chip_UART_ReadRB(LPC_UART1, &s_rxringBle, dataRecv, BGLIB_MSG_MAXLEN);
	//	DEBUG_BLE("BT121 response: %d \n",n);
	if (n < 0)
	{
		DEBUGOUT("Read Uart failed \n");
		return;
	}
	for (int i = 0; i < n; i++)
	{
		bluetooth_ProceesPacketByBglib(dataRecv[i]);
	}
	/** >>#008
	Reset bluetooth when communication dead for long time	
	(New sec 2018/02/08)
	#008 >>**/
	bluetooth_HandleInquirySeq(n);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_GetOnePacketFromCBX                                      		    */
/*                                                                                      */
/* Details  get one packet from cbx via bluetooth				  						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t byteData		 					  							*/
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
/****************************************************************************************/
static void bluetooth_GetOnePacketFromCBX(uint8_t* data, int32_t datalen)
{
	int n;//packet[%d
	static E_CBX02ReceivePacketState BTState = CBX_WAIT_STX;
	static uint8_t packetLen;
	static uint8_t cbxReceivePacket[PACKETIN_LENGTH];
	static uint8_t crcCount = 0;
	uint8_t *c = data;
	for (n = 0; n < datalen;n++)
	{
		DEBUG_BLE("CBX command[%d]: %x \n",n,*c);
		switch (BTState)
		{
		case CBX_WAIT_STX:
		{
			if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			break;
		}
		case CBX_WAIT_ETX:
		{
			cbxReceivePacket[packetLen++] = *c;
			if(packetLen > PACKETIN_LENGTH)
			{
				BTState = CBX_WAIT_STX;
				uint8_t mC1;
				if (cbxReceivePacket[1] == DLE_BYTE)
				{
					mC1 = cbxReceivePacket[2];
				}
				else
				{
					mC1 = cbxReceivePacket[1];
				}
				cbx_isPacketFormRS232(false);
				cbx_SendErrPacketToCbx(mC1, ERR_PARAM_LENTH);
			}
			else if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			else if(*c == ETX_BYTE)
			{
				crcCount = 0;
				BTState = CBX_WAIT_CRC;
			}
			break;
		}
		case CBX_WAIT_CRC:
		{
			cbxReceivePacket[packetLen++] = *c;
			crcCount++;
			if(packetLen > PACKETIN_LENGTH)
			{
				BTState = CBX_WAIT_STX;
				uint8_t mC1;
				if (cbxReceivePacket[1] == DLE_BYTE)
				{
					mC1 = cbxReceivePacket[2];
				}
				else
				{
					mC1 = cbxReceivePacket[1];
				}
				cbx_isPacketFormRS232(false);
				cbx_SendErrPacketToCbx(mC1, ERR_PARAM_LENTH);
			}
			else if(*c == STX_BYTE)
			{
				memset(cbxReceivePacket,0,PACKETIN_LENGTH);
				packetLen =0;
				cbxReceivePacket[packetLen++] = *c;
				BTState = CBX_WAIT_ETX;
			}
			else if (crcCount == 4)
			{
				BTState = CBX_WAIT_STX;
				cbx_isPacketFormRS232(false);
				cbx_ReceivePacketFromCbx(cbxReceivePacket, packetLen);
			}
			break;
		}
		default:
			break;
		}
		c++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_ProceesPacketByBglib                                      		    */
/*                                                                                      */
/* Details  Passing one by one data of packet receive and Process BGLIB API				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t byteData		 					  							*/
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
/****************************************************************************************/
void bluetooth_ProceesPacketByBglib(uint8_t byteData)
{
	static E_BTProcesPacketState s_nextState = BT_IDLE;
	static uint16_t s_indexCnt = 0;
	static uint16_t s_msgLength = 0;
	DEBUG_BLE("id: %d byte: %x \n",s_indexCnt,byteData);
	if(s_nextState == BT_IDLE)
	{
		if((byteData == BT_RESPONSE_CODE) || (byteData == BT_EVENT_CODE))
		{
			s_indexCnt = 0;
			s_nextState = BT_WAIT_HEADER;
			memset(buffer, 0, BGLIB_MSG_MAXLEN);
		}
	}
	switch (s_nextState) {
	case BT_WAIT_HEADER:
		buffer[s_indexCnt] = byteData;
		s_indexCnt++;
		if(s_indexCnt == BGLIB_MSG_HEADER_LEN)
		{
			s_msgLength = BGLIB_MSG_LEN(buffer);
			if((s_msgLength > 0) &&(s_msgLength < BGLIB_MSG_MAXLEN - BGLIB_MSG_HEADER_LEN))
			{
				s_nextState = BT_WAIT_DATA;
			}
			else
			{
				/** >>#007
					reset s_nextState to BT_IDLE when s_msgLength is wrong
				#007 >>**/
				s_nextState = BT_IDLE;
				s_indexCnt = 0;
			}
		}
		break;
	case BT_WAIT_DATA:
		buffer[s_indexCnt] = byteData;
		s_indexCnt++;
		if(s_indexCnt == s_msgLength + BGLIB_MSG_HEADER_LEN)
		{
			pck=BGLIB_MSG(buffer);
			switch(BGLIB_MSG_ID(buffer))
			{
			case dumo_rsp_system_set_local_name_id:
				DEBUG_BLE("Set name id %d \n",BGLIB_MSG(buffer)->rsp_system_set_local_name.result);
				dumo_cmd_system_get_local_name();
				break;
			case dumo_rsp_system_get_local_name_id:
				DEBUG_BLE("Get name id %x \n",BGLIB_MSG(buffer)->rsp_system_get_local_name.name.len);
				bluetooth_PrintName(pck->rsp_system_get_local_name.name);
				dumo_cmd_bt_rfcomm_start_server(RFCOM_SDP_ENTRY_ID,RFCOM_STREAM_DESTINATION_ENDPOINT);
				break;
			case dumo_rsp_sm_configure_id:
				dumo_cmd_bt_connection_set_role(endpoint_uart,bt_connection_slave);
				break;
			case dumo_rsp_system_get_bt_address_id:
			{
				bd_addr bluetoothAddr = pck->rsp_system_get_bt_address.address;
				bluetooth_UpdateAddress(bluetoothAddr);
				dumo_cmd_sm_configure(SM_ALLOW_BONDING_NOT_MITM,sm_io_capability_noinputnooutput);
				break;
			}
			case dumo_rsp_bt_connection_set_role_id:
				DEBUG_BLE("Set role %x \n",BGLIB_MSG(buffer)->rsp_bt_gap_set_mode.result);
				dumo_cmd_system_set_local_name(9,"FH-310"); //name length = actual name length(6) + 3: see dumo bglib API
				break;
			case dumo_evt_system_boot_id:
				DEBUG_BLE("dumo_evt_system_boot_id()\n");
				DEBUG_BLE("FW version %d %d %d\n",pck->evt_system_boot.build,pck->evt_system_boot.major,pck->evt_system_boot.minor);
				BT_FW_VERSION_T btVersion;
				btVersion.majorNum = pck->evt_system_boot.major;
				btVersion.minorNum = pck->evt_system_boot.minor;
				btVersion.patchNum = pck->evt_system_boot.patch;
				btVersion.buildNum = pck->evt_system_boot.build;

				GUI_EVENT_STRUCT event;
				event.id = eBluetoothFwVersionEventId;
				event.data.btFwVersion = btVersion;
				guiInterface_SendGuiEvent(event);

				dumo_cmd_system_get_bt_address();
				break;
			case dumo_evt_system_initialized_id:
				DEBUG_BLE("dumo_evt_system_initialized_id()\n");
				/**/
				DEBUG_BLE("Address:");
				DEBUG_BLE("\n");
				dumo_cmd_system_get_bt_address();
				break;
			case dumo_rsp_bt_rfcomm_start_server_id:
				if (pck->rsp_bt_rfcomm_start_server.result == 0)
				{
					DEBUG_BLE("SPP server started\n");
					DEBUG_BLE("Setting un discoverable, connectable & bondable on\n");
					bluetooth_Setmode(s_btMode);
				}
				else
				{
					DEBUG_BLE("SPP server start failed, errorcode %d\n", pck->rsp_bt_rfcomm_start_server.result);
				}
				break;
			case dumo_rsp_bt_gap_set_mode_id:
				if (pck->rsp_bt_gap_set_mode.result == 0)
				{
					if(s_btMode == DISCOVER_MODE)
					{
						/*Set Bluetooth LE mode also to connectable*/
						dumo_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
					}
					else
					{
						dumo_cmd_le_gap_set_mode(le_gap_non_discoverable, le_gap_undirected_connectable);
					}
				}
				break;
			case dumo_rsp_le_gap_set_mode_id:
				if (pck->rsp_le_gap_set_mode.result == 0)
				{
					/*Set bondable mode*/
					dumo_cmd_sm_set_bondable_mode(ALLOW_BONDING);
				}
				break;
			case dumo_rsp_sm_set_bondable_mode_id:
				if (pck->rsp_sm_set_bondable_mode.result == 0)
				{
					DEBUG_CBX("Waiting for connection...\n");
				}
				break;
			case dumo_evt_sm_pin_code_request_id:
			{
				uint8_t pincodeArr[PINCODE_LENGTH] = {0x38, 0x30, 0x35, 0x30, 0x35, 0x36, 0x39, 0x31};
				DEBUG_BLE("Send pin code \n");
				dumo_cmd_sm_enter_pin_code(&pck->evt_sm_pin_code_request.address, PINCODE_LENGTH, pincodeArr);
				break;
			}
			case dumo_rsp_sm_enter_pin_code_id:
				DEBUG_BLE("pin code %x\n",pck->rsp_sm_enter_pin_code.result);
				break;
			case dumo_rsp_endpoint_close_id:
				if (pck->rsp_endpoint_close.result == 0)
				{
					/* Finished */
				}
				break;
			case dumo_evt_sm_bonded_id:
				if (pck->evt_sm_bonded.bonding != 0xff) dumo_cmd_sm_read_bonding(pck->evt_sm_bonded.bonding);
				break;
			case dumo_rsp_sm_read_bonding_id:
				DEBUG_BLE("Bonding from ");
				bluetooth_PrintAddress(pck->rsp_sm_read_bonding.address);
				DEBUG_BLE(" succesful\n");
				break;
			case dumo_evt_bt_rfcomm_opened_id:
				DEBUG_BLE("SPP connection from ");
				bluetooth_PrintAddress(pck->evt_bt_rfcomm_opened.address);
				DEBUG_BLE(" at endpoint %d\n",pck->evt_bt_rfcomm_opened.endpoint);
				break;
			case dumo_evt_endpoint_data_id:
				DEBUG_BLE("Received data from endpoint %d \n", pck->evt_endpoint_data.endpoint);
				bluetooth_GetOnePacketFromCBX(pck->evt_endpoint_data.data.data, pck->evt_endpoint_data.data.len);
				break;
			case dumo_evt_endpoint_closing_id:
				DEBUG_CBX("Closing endpoint:%d\n", pck->evt_endpoint_closing.endpoint);
				/*endpoint is closing, could check for reason here and then close it*/
				dumo_cmd_endpoint_close(pck->evt_endpoint_closing.endpoint);
				break;
			case dumo_rsp_endpoint_send_id:
				break;
			case dumo_evt_endpoint_status_id:
				break;
			case dumo_evt_le_connection_closed_id:
				dumo_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
				break;
				/** >>#008
				Reset bluetooth when communication dead for long time	
				(New sec 2018/02/08)
				#008 >>**/
				/** >>#0011
					Remove unused code because the BT reset sequence specification change
				#0011 >>**/
			}
			s_indexCnt = 0;
			s_nextState = BT_IDLE;
			memset(buffer, 0, BGLIB_MSG_MAXLEN);
		}
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_GetAddr			                                      		    */
/*                                                                                      */
/* Details  Get address of bluetooth													*/
/*																						*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t*	                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_btAddr						                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
int8_t* bluetooth_GetAddr(void)
{
	return s_btAddr;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_SendPacket			                                      		    */
/*                                                                                      */
/* Details  Send packet data via Bluetooth, this function will make BT121 send			*/
/*			data to the Bluetooth device which it is connected(CBX-02)					*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int8_t*	                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_btAddr						                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void bluetooth_SendPacket(uint8_t* packet, uint32_t packetLen)
{
	uint16_t sendTime = packetLen/MAX_BT_PK_SIZE +1;
	for(int i=0;i<sendTime;i++)
	{
		uint16_t len;
		if(packetLen>MAX_BT_PK_SIZE)
		{
			len = MAX_BT_PK_SIZE;
		}
		else
		{
			len =packetLen;
		}
		dumo_cmd_endpoint_send(pck->evt_endpoint_data.endpoint, len, packet);
		packet+=MAX_BT_PK_SIZE;
		packetLen -=MAX_BT_PK_SIZE;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_Setmode			                                      		    */
/*                                                                                      */
/* Details  Set BTmode discover or undiscover											*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_btMode						                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void bluetooth_Setmode(E_BtMode mode)
{
	s_btMode = mode;
	switch (mode) {
	case DISCOVER_MODE:
		dumo_cmd_bt_gap_set_mode(BT_GAP_CONECTABLE, BT_GAP_DISCOVERABLE, bt_gap_discover_generic);
		break;
	case UNDISCOVER_MODE:
		dumo_cmd_bt_gap_set_mode(BT_GAP_CONECTABLE, BT_GAP_NONDISCOVER, bt_gap_discover_generic);
		break;
	}
	return;
}

/** >>#0011
	Add new function because the BT reset sequence specification change
#0011 >>**/
/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_ChangeMode			                                      		    */
/*                                                                                      */
/* Details  Perform user change bluetooth discover mode									*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_btMode						                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void bluetooth_ChangeMode(E_BtMode mode)
{
	/** >>#0012 Reset bluetooth module when change mode
	for fix bug 2074 #0012 >>**/
	s_btMode = mode;
	bluetooth_Reset();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General bluetooth_Reset			                                  	    		    */
/*                                                                                      */
/* Details  Reset bluetooth module														*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) s_btInquiryState						                      	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void bluetooth_Reset(void)
{
	DEBUG_BLE("BT reset \n");
	s_btInquiryState = BT_RESET;
	/** >>#0011
		Change reset process algorithm for adapt with code change
		because the BT reset sequence specification change
	#0011 >>**/
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,BL_RESET_PORT, BL_RESET_BIT);		/* output */
	bluetooth_ResetUart();
	return;
}


