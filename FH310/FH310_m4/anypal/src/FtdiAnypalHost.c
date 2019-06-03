/********************************************************************/
/*                                                                  */
/* File Name    : FtdiAnypalHost.c                       			*/
/*                                                                  */
/* General      : Manager of FTDI host task							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* #0001       2016/12/25      Quyen Ngo(MV)  	    new file        */
/* #0002       2017/08/24       Linh Nguyen	  	    modify          */
/*  Handle reconnect err when turn on Vbus					    	*/
/* #0003       2017/09/12       Linh Nguyen	  	    modify          */
/*  Handle push out anypal event as a message form usbhosttask    	*/
/*  Use new save log function                            			*/
/* #0004       2017/09/20       Linh Nguyen	  	    modify          */
/*  Reset all anypal state after over current alarm 		    	*/
/*	Remove unuse code												*/
/*	Send end getting log when anypal disconnect						*/
/* #0005       2017/09/20       Linh Nguyen	  	    modify          */
/*  Add new define											    	*/
/*  Change function name for coding rule					     	*/
/*  Change design handle of receive data from FTDI module	     	*/
/*  Use one ID for anypal enum complete event				     	*/
/* #0006       2017/11/17       Linh Nguyen	  	    modify          */
/*  Change timing of VBUS OFF sequence (bug 2025)			    	*/
/*			2017/11/30	       Linh Nguyen	  	    modify          */
/*  #0007 Fix bug 2050												*/
/*			2017/12/13	       Linh Nguyen	  	    modify          */
/*  #0008 Add "break"	(coding rule)								*/
/********************************************************************/

/* include system files */

/* include user files */
#include "FtdiAnypalHost.h"
#include "fsusb_cfg.h"
#include "ff.h"
#include "board.h"
#include "AnypalHandler.h"
#include "TaskCommon.h"
#include "setting.h"
#include "GuiTask.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "AnypalInterface.h"
#include "UsbInterface.h"
#include "UsbHostInterface.h"


#ifdef DEBUG
#define DEBUG_ATP_EN
#endif

#ifdef DEBUG_ATP_EN
#define DEBUG_ATP(...) printf(__VA_ARGS__)
#else
#define DEBUG_ATP(...)
#endif

/* constants, macro definition */

/* definitions of structures */

#define TURN_OFF_VBUS 	1000/ANYPAL_TASK_DELAY
#define TURN_ON_VBUS 	10000/ANYPAL_TASK_DELAY

#define ANYPAL_CONNECTED_DELAY 10

/*global variable*/
/* external declaration */
extern QueueHandle_t gs_anypalQueue;

extern E_ATPPacketId g_sendPacketId;

extern Virtual_AnyPal virtualAnyPal;

static AnypalState gs_anypalState = eAnypalNotConnect;
static AnypalState gs_lastAnypalState = eAnypalNotConnect;

//Q_add
static USB_ClassInfo_FTDI_Host_t s_hDiskInterface; //replace FlashDisk_FTDI_Interface by s_hDiskInterface
//end Q_add

/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypalHost_GetAnypalState                                    		*/
/*                                                                                      */
/* Details:  Get Anypal State															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) 											  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : AnypalState	                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_anypalState				                              	*/
/*                                                                                      */
/****************************************************************************************/
AnypalState ftdiAnypalHost_GetAnypalState(void)
{
	return gs_anypalState;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypalHost_SetAnypalState                                    		*/
/*                                                                                      */
/* Details:  Get Anypal State															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) AnypalState anypalState					  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : 				                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_anypalState				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void ftdiAnypalHost_SetAnypalState(AnypalState anypalState)
{
	gs_anypalState = anypalState;
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypalHost_readFTDIData                                    		*/
/*                                                                                      */
/* Details:  read the data from FTDI device												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) None										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/****************************************************************************************/
static void ftdiAnypalHost_readFTDIData(void)
{
	if (USB_HostState[s_hDiskInterface.Config.PortNumber] != HOST_STATE_Configured)
		return;
	volatile int s_countData = 0;
	uint16_t numByte = FTDI_Host_BytesReceived(&s_hDiskInterface);
	for(int i =0; i<numByte ; i++)
	{
		/* Echo received bytes from the attached device through the USART */
		int16_t ReceivedByte = FTDI_Host_ReceiveByte(&s_hDiskInterface);
		s_countData++;
		if((s_countData == 1)||(s_countData == 2))
		{
			ReceivedByte = -1; //Ignore status data
		}
		if (!(ReceivedByte < 0))
		{
			anypal_ReceivePacketFromATP((uint8_t) ReceivedByte);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypalHost_SendPacketToATP                   		          		*/
/*                                                                                      */
/* Details  send a packet to ATP														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const uint8_t* const Buffer				  							*/
/*                                                                                      */
/*             (I) uint32_t size														*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_hDiskInterface                               			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
uint8_t ftdiAnypalHost_SendPacketToATP(const uint8_t* const Buffer, uint32_t size)
{
	uint8_t ErrorCode = FTDI_Host_SendData(&s_hDiskInterface,Buffer,size);
	if (ErrorCode != PIPE_READYWAIT_NoError)
	{
		return ErrorCode;
	}
	ErrorCode =  FTDI_Host_Flush(&s_hDiskInterface);
	if (ErrorCode != PIPE_READYWAIT_NoError)
	{
		return ErrorCode;
	}
	return PIPE_READYWAIT_NoError;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypal_EventHandle                           		          		*/
/*                                                                                      */
/* Details  FTDI USB Anypal event Handle												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_anypalState                               			   	*/
/*                     (O) gs_lastAnypalState		                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void ftdiAnypal_EventHandle(void)
{
	ANYPAL_MSG_STRUCT_t msg;
	if(xQueueReceive(gs_anypalQueue, &msg, 0))
	{
		switch (msg.id)
		{
		case eAnypalEnumComplete:
			s_hDiskInterface = msg.data.hDiskInterface;
			if (gs_anypalState == eAnypalNotConnect)
			{
				gs_anypalState = eAnypalConnecting;

				GUI_EVENT_STRUCT event;
				event.id = eAnypalConnectEventId;
				event.data.charData = true;
				guiInterface_SendGuiEvent(event);

				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAnypalInsertLogId);
			}
			else if (gs_anypalState == eAnypalDiconnecting)
			{
				GUI_EVENT_STRUCT event;
				event.id = eAnypalConnectEventId;
				event.data.charData = false;
				guiInterface_SendGuiEvent(event);
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAnypalPutOutLogId);

				event.data.charData = true;
				guiInterface_SendGuiEvent(event);
				logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAnypalInsertLogId);
				gs_anypalState = eAnypalConnecting;
			}
			else if (gs_anypalState == eAnypalOverCurrent)
			{
				gs_anypalState = eAnypalConnectingAfterOVCR;
			}
			else if(gs_anypalState == eAnypalWaitReconnectForTimeOut)
			{
				anypal_SwitchReconnectErrTimer(false);
				gs_anypalState = eAnypalRetranferForTimeOut;
			}
			break;
			break;
		case eAnypalOverCurrentEvent:
		{
			anypal_StopAlarmOverCurrentOffTimer();
			if (gs_anypalState != eAnypalOverCurrent)
			{
				gs_lastAnypalState = gs_anypalState;
				if (gs_anypalState != eAnypalNotConnect)
				{
					DEBUG_ATP("anypal over current\n");
					anypal_SwitchPushOutAnypalTimer(false);
					anypal_StopAllAnypalTimer();
					GUI_EVENT_STRUCT event;
					event.id = eOverCurrentEventId;
					event.data.charData = eActive;
					guiInterface_SendGuiEvent(event);
					gs_anypalState = eAnypalOverCurrent;
				}
				else
				{
					logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOverCurrentIsDetectedLogId);
				}
			}

			usb_SendEvent(eRequestTurnOffAnypalVbusWhenOVCR);
			break;
		}
		case eAnypalOverCurrentClear:
		{
			if(gs_lastAnypalState != eAnypalNotConnect)
			{
				anypal_StartAlarmOverCurrentOffTimer();
			}
			break;
		}
		case eResetAllAnypalState:
			anypal_ResetAllAnypalState(true);
			break;
		case eAnypalPushOut:
		{
			if((gs_anypalState != eAnypalOverCurrent) && (gs_anypalState != eAnypalWaitReconnectForTimeOut))
			{
				anypal_SwitchPushOutAnypalTimer(true);
			}
			anypal_SendGettingSpO2PulseDataStatus(false);
			break;
		}
		case eCalendarUpdate:
		{
			anypal_SetCalendarUpdate();
			break;
		}
		default:
			break;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: ftdiAnypalHost_StateHandle                      		          		*/
/*                                                                                      */
/* Details  FTDI USB Anypal state Handle												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_anypalState                               			   	*/
/*                     (O) gs_lastAnypalState		                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void ftdiAnypalHost_StateHandle(void)
{
	static uint8_t counterForVBus =0;

	switch(gs_anypalState)
	{
	case eAnypalNotConnect:
		if (counterForVBus ==0)
		{
			anypal_SendRequestSwicthVbus(false);
		}
		else if(counterForVBus == TURN_OFF_VBUS)
		{
			anypal_SendRequestSwicthVbus(true);
		}
		counterForVBus++;
		if(counterForVBus ==TURN_ON_VBUS)
		{
			counterForVBus =0;
		}
	case eAnypalConnectingAfterOVCR:
	case eAnypalOverCurrent:
	case eAnypalSleep:
	case eAnypalAlarm:
	case eAnypalWaitReconnectForTimeOut:
		vTaskDelay(ANYPAL_TASK_DELAY/portTICK_PERIOD_MS);
		break;
	case eAnypalDiconnecting:
	{
		anypal_ResetAllAnypalState(false);
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eAnypalPutOutLogId);

		GUI_EVENT_STRUCT event;
		event.id = eAnypalConnectEventId;
		event.data.charData = false;
		guiInterface_SendGuiEvent(event);

		gs_anypalState = eAnypalNotConnect;

		vTaskDelay(ANYPAL_TASK_DELAY/portTICK_PERIOD_MS);
		break;
	}
	case eAnypalConnecting:
	{
		virtualAnyPal.status.isBatteryAbnomal = false;
		virtualAnyPal.status.isChaging = false;
		virtualAnyPal.status.isCommunication = false;
		virtualAnyPal.status.isRTCAbnormal =false;
		g_sendPacketId = ATPDeviceInfoId;
		gs_anypalState = eAnypalConnected;
		DEBUG_ATP("first ATPDeviceInfoId ");
		anypal_TransferPacket(ATPDeviceInfoId);

		vTaskDelay(ANYPAL_TASK_DELAY/portTICK_PERIOD_MS);
		break;
	}
	case eAnypalRetranferForTimeOut:
	{
		DEBUG_ATP(" RetransferPacket\n");
		gs_anypalState = eAnypalConnected;
		anypal_RetransferPacket(g_sendPacketId);
		vTaskDelay(ANYPAL_TASK_DELAY/portTICK_PERIOD_MS);
		break;
	}
	case eAnypalTransferForNewCycle:
	{
		g_sendPacketId = ATPDeviceInfoId;
		DEBUG_ATP(" TransferPacket ATPDeviceInfoId ");
		gs_anypalState = eAnypalConnected;
		anypal_TransferPacket(ATPDeviceInfoId);
		vTaskDelay(ANYPAL_TASK_DELAY/portTICK_PERIOD_MS);
		break;
	}
	case eAnypalConnected:
	{
		ftdiAnypalHost_readFTDIData();
		vTaskDelay(ANYPAL_CONNECTED_DELAY/portTICK_PERIOD_MS);
		break;
	}

	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-ftdiAnypalHost_Handle													    */
/*                                                                                      */
/* Details  -FTDI USB Anypal Handle														*/
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
/****************************************************************************************/
void ftdiAnypalHost_Handle(void)
{
	DEBUG_ATP("Start***********\n");
	while(1)
	{
		ftdiAnypal_EventHandle();
		ftdiAnypalHost_StateHandle();

	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-ftdiAnypalHost_CreateAnypalTask										    */
/*                                                                                      */
/* Details  -Create Anpal task with														*/
/* 			 task name: Anpal Task
 * 			 task function: ftdiAnypal_Handle
 * 			 task priority: TASK_PRIO_IRDA_TASK = +3					 				*/
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
void ftdiAnypalHost_CreateAnypalTask(void)
{
	/* Start Irda Task */
	xTaskCreate((TaskFunction_t)ftdiAnypalHost_Handle, "ATP Task",
			ANYPAL_TASK_SIZE, (void *)1, TASK_PRIO_FTDI_ANYPAL,
			( TaskHandle_t * ) NULL );
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-ftdiAnypalHost_ReconnectAfterOVCR										    */
/*                                                                                      */
/* Details  -Re-connect After over current												*/
/*                                                                                      */
/* Arguments : (I) bool event			 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_lastAnypalState                             			   	*/
/*                     (O) gs_anypalState						                       	*/
/*                                                                                      */
/****************************************************************************************/
void ftdiAnypalHost_ReconnectAfterOVCR(void)
{
	if(gs_anypalState == eAnypalConnectingAfterOVCR)
	{
		if (gs_lastAnypalState == eAnypalAlarm)
		{
			gs_anypalState = eAnypalAlarm;
		}
		else
		{
			anypal_ResetAllAnypalStateWhenOVCR();
			gs_anypalState = eAnypalSleep;
		}
	}
	else
	{
		gs_anypalState = eAnypalDiconnecting;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-ftdiAnypalHost_InitAnypalTask											    */
/*                                                                                      */
/* Details  -Create Init	Anypal task													*/
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
/*                     (O) gs_anypalQueue                                           	*/
/****************************************************************************************/
void ftdiAnypalHost_InitAnypalTask(void)
{
	gs_anypalQueue = xQueueCreate(ANYPAL_QUEUE_LENGTH, sizeof(ANYPAL_MSG_STRUCT_t));
	return;
}
