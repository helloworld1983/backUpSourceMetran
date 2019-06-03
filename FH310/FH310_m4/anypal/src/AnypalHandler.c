/********************************************************************/
/*                                                                  */
/* File Name    : AnypalHandler.c                        			*/
/*                                                                  */
/* General      : Anypal Handler									*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* #0001       2016/11/01       Linh Nguyen	  	    new file        */
/*        															*/
/* #0002       2016/08/24       Linh Nguyen	  	    modify          */
/*  - Start gs_AnypalCycleTimer	timer after saving	all usage log	*/
/*  for anypal.          											*/
/*  - Add anypal reconnect err timer and start, stop it function	*/
/*  to handle reconnect err when turn on Vbus					    */
/* #0003	   2017/09/01		Quyen Ngo			modify			*/
/*		-remove assert												*/
/* #0005	   2017/09/03      Linh Nguyen(MV)  	    modify	    */
/*    Fix timer text name											*/
/* #0006	   2017/09/19      Viet Le  	    		modify	    */
/*	  -Change unsigned short->uint16_t in anypal_AnalyzeGetLogData()*/
/* #0007	   2017/09/19      Linh Nguyen(MV)  	    modify	    */
/*    Use the new receive log function for anypal					*/
/*    Store anypal log to anypal even no space						*/
/*    Rename some static global variable							*/
/*	  Add and use anypal_SendGettingSpO2PulseDataStatus function	*/
/*	  Remove unuse code												*/
/*	  Add anypal_GetNextStateAfterGetCalendar						*/
/*    Add anypal_SetCalendarUpdate function							*/
/*  #0008	   2017/09/24      Linh Nguyen(MV)  	    modify	    */
/*  Remove unuse code												*/
/*  Add anypal_ResetAllAnypalStateWhenOVCR function					*/
/*  Change the function anypal_GetDateOfNewestLog					*/
/*  Add and use anypal_reTryWhenWrongRespone function				*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/*  #0009 remove call of PIPE_CLEAR_IN  function for bug #1945		*/
/*  		   2017/10/21      Linh Nguyen(MV)  	    modify	    */
/*  #0010 add () for fix bug #1988									*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/*  #0011 Handle get wrong  usage log (bug #1991)					*/
/*  		   2017/11/02      Linh Nguyen(MV)  	    modify	    */
/*  #0012 Handle the get calendar (bug #2001)						*/
/*  		   2017/11/22      Linh Nguyen(MV)  	    modify	    */
/*  #0013 turn off the synchronization calendar request (bug #2027)	*/
/********************************************************************/

#include "board.h"
#include "AnypalHandler.h"
#include "LogMgr.h"
#include "FtdiAnypalHost.h"
#include "RealTimeClock.h"
#include "timers.h"
#include "TaskCommon.h"
#include "queue.h"
#include "setting.h"
#include "CBX02Handler.h"
#include "BluetoothTask.h"
#include "GuiTask.h"
#include "GuiInterface.h"
#include <stdlib.h>
#include "UsbInterface.h"
#include "LogInterface.h"

#ifdef DEBUG
#define DEBUG_ATP_EN
#endif

#ifdef DEBUG_ATP_EN
#define DEBUG_ATP(...) printf(__VA_ARGS__)
#else
#define DEBUG_ATP(...)
#endif

#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )

static TimerHandle_t gs_AnypalResponseTimer;
static TimerHandle_t gs_AnypalCycleTimer;
static TimerHandle_t gs_AlarmOverCurrentOffTimer;
static TimerHandle_t gs_PushOutAnypalTimer;
static TimerHandle_t gs_AnypalReconnectErrorTimer;

TimerHandle_t gs_turnOnVBUSTimer;

static uint8_t gs_sendPacketNo = 0x01;

static uint8_t gs_packetIn[ATP_PACKET_IN_LEN];

Virtual_AnyPal virtualAnyPal;
//number of FH310 log for usage anypal
static uint32_t gs_numberOfFH310Log;

//End position for get log
static uint32_t gs_mLogNumberNEnd;

//Start position for get log
static uint32_t gs_mLogNumberNTop;

//Position for search log
static uint32_t gs_mLogNumberN;

//Anypal stored data quantity
static uint32_t gs_anypalLogQuantity;

//Top of un-transmitting log from FH310
static uint32_t gs_nTopFH310Log;

//Number of acquisition
static uint8_t gs_mnCount;

//Packet send to AnyPal
E_ATPPacketId g_sendPacketId;

//Description: Count resend packet times for detect alarm Atp-02 err communication
//if resend packet > MAX_RESEND_PCKET ==> active alarm
static uint8_t gs_mResendCnt=0;

//global variables need reset when push out anypal
static bool gs_isGotCalendar = false;
static bool gs_isCharging = false;
static bool gs_isAlarmAnypal = false;

static bool gs_isGotIDNumber = false;
static bool gs_isGotSpO2Log = false;

static bool gs_isCalendarUpdate = false;
//////////////////////////////////////////////////

static E_ATPCurrentState gs_nextState = ATP_Wait_SignatureF;

Real_Time_Clock gs_ATPClockTime;

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SwicthPushOutAnypalTimer                                     	*/
/*                                                                                      */
/* Details  Switch Push OutAnypalTimer													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isStartTimer						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_PushOutAnypalTimer                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_SwitchReconnectErrTimer(bool isStartTimer)
{
	if(isStartTimer)
	{
		if (xTimerStart(gs_AnypalReconnectErrorTimer,TIMER_WAITING) != pdPASS)
		{
			DEBUG_ATP("ERR: Failed to start s_AnypalReconnectErrorTimer \n");
		}
	}
	else
	{
		if (xTimerStop(gs_AnypalReconnectErrorTimer,TIMER_WAITING) != pdPASS)
		{
			DEBUG_ATP("ERR: Failed to stop s_AnypalReconnectErrorTimer \n");
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SwicthPushOutAnypalTimer                                     	*/
/*                                                                                      */
/* Details  Switch Push OutAnypalTimer													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isStartTimer						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_PushOutAnypalTimer                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_SwitchPushOutAnypalTimer(bool isStartTimer)
{
	if(isStartTimer)
	{
		if (xTimerStart(gs_PushOutAnypalTimer,TIMER_WAITING) != pdPASS)
		{
			DEBUG_ATP("ERR: Failed to start s_PushOutAnypalTimer \n");
		}
	}
	else
	{
		if (xTimerStop(gs_PushOutAnypalTimer,TIMER_WAITING) != pdPASS)
		{
			DEBUG_ATP("ERR: Failed to stop s_PushOutAnypalTimer \n");
		}
	}
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SendRequestSwicthVbus                                     		*/
/*                                                                                      */
/* Details  Send Request Turn On Vbus													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isTurnOn							  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) 								                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void anypal_SendRequestSwicthVbus(bool isTurnOn)
{
	USB_EVENT request;
	if(isTurnOn)
	{
		request = eRequestTurnOnAnypalVbus;
	}
	else
	{
		request = eRequestTurnOffAnypalVbus;
	}

	usb_SendEvent(request);
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_GetDateOfNewestLog                                       		*/
/*                                                                                      */
/* Details  Get date of newest log														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t *numLog							  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Real_Time_Clock                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
static Real_Time_Clock anypal_GetDateOfNewestLog(uint32_t numLog)
{
	Real_Time_Clock mXday;
	mXday.year=0;
	mXday.mday=0;
	mXday.month=0;
	mXday.hour=0;
	mXday.min=0;
	mXday.sec=0;
	mXday.yday=0;
	mXday.wday=0;
	if(numLog ==0)
	{
		if(gs_isCalendarUpdate)
		{
			guiInterface_RTCGettime(&mXday);
		}
		else
		{
			mXday = gs_ATPClockTime;
		}
		realtimeclock_Subtract14Day(&mXday);
		mXday.hour =0;
		mXday.min =0;
		mXday.sec =0;
	}
	else
	{
		FH310_LOG_T log[MAX_LOG_ATP_RQ];
		LOG_REQUEST_t getLog;
		getLog.id = eAnypalGetAnypalLogId;
		getLog.beginLog = numLog;
		getLog.numberOfLog =1;
		uint8_t numLogGot =0;

		logInterface_ReceiveLogForAnypal(getLog, log,&numLogGot);

		mXday.year=log->YYYY;
		mXday.mday=log->DD;
		mXday.month=log->MM;
		mXday.hour=log->hh;
		mXday.min=log->min;
		mXday.sec=log->ss;
	}
	return mXday;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_ResetAllAnypalStateWhenOVCR                              		*/
/*                                                                                      */
/* Details  Reset All Anypal State when resume after over current						*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) none										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_nextState                                  			   	*/
/*                     (O) gs_isGotCalendar						                        */
/*                     (O) gs_isAlarmAnypal						                        */
/*                     (O) gs_isCharging						                        */
/*                     (O) gs_isGotIDNumber						                        */
/*                     (O) gs_isGotSpO2Log						                        */
/****************************************************************************************/
void anypal_ResetAllAnypalStateWhenOVCR(void)
{
	DEBUG_ATP("reset ATP state for OVCR\n");
	gs_isAlarmAnypal = false;
	gs_isCharging = false;
	gs_isGotIDNumber = false;
	gs_isGotSpO2Log = false;
	gs_nextState = ATP_Wait_SignatureF;
	anypal_StartTimerAnypalCycleTimer();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_ResetAllAnypalState                                       		*/
/*                                                                                      */
/* Details  Reset All Anypal State														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isDeleteAnypal						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) virtualAnyPal                                   			   	*/
/*                     (O) gs_isGotCalendar						                        */
/*                     (O) gs_isAlarmAnypal						                        */
/*                     (O) gs_isCharging						                        */
/*                     (O) gs_isGotIDNumber						                        */
/*                     (O) gs_isGotSpO2Log						                        */
/****************************************************************************************/
void anypal_ResetAllAnypalState(bool isDeleteAnypal)
{
	DEBUG_ATP("reset ATP state\n");
	if(isDeleteAnypal)
	{
		memset(virtualAnyPal.info.ComdityCode,0,ATP_COMDITY_CODE_LEN);
		virtualAnyPal.info.type=0;
		memset(virtualAnyPal.info.patientID,0,ATP_PATIENT_ID_LEN);
		memset(virtualAnyPal.info.version,0,ATP_VERSION_LEN);
	}
	if(ftdiAnypalHost_GetAnypalState() != eAnypalNotConnect)
	{
		ftdiAnypalHost_SetAnypalState(eAnypalTransferForNewCycle);
	}
	anypal_StopAllAnypalTimer();
	gs_isGotCalendar = false;
	gs_isAlarmAnypal = false;
	gs_isCharging = false;
	gs_isGotIDNumber = false;
	gs_isGotSpO2Log = false;
	gs_nextState = ATP_Wait_SignatureF;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SendPacketToATP                                        		*/
/*                                                                                      */
/* Details  send a packet to ATP														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ATPPacketId packetId					  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_mnCount							                        */
/*                     (O) gs_mLogNumberNEnd                            			   	*/
/*                     (O) gs_mLogNumberNTop						                    */
/*                     (O) gs_mLogNumberN                            			   		*/
/*                     (O) gs_nTopFH310Log                            			   		*/
/****************************************************************************************/
uint8_t anypal_SendPacketToATP(E_ATPPacketId packetId)
{
	//Number to transmit to server
	uint16_t mTransmitCout =0;
	int ParameterLength = 0;
	uint8_t packetType = 0x00;
	//    static uint8_t PacketNum = (uint8_t)0x01;
	uint8_t xorCheck = 0;

	switch(packetId)
	{
	case ATPIndentificationInfoId:
		break;
	case ATPDeviceInfoId:
		packetType = PACKET_ATP_INFO;//(uint8_t)0x01;
		ParameterLength = SEND_PACKET_INFOR_LENGTH;
		break;
	case ATPIdNumberId:
		packetType = PACKET_GET_PATIENTID;//(uint8_t)0x03;
		ParameterLength = SEND_PACKET_PATIENTID_LENGTH;
		break;
	case ATPGetCalendarId:
		packetType = PACKET_GET_CALENDAR;//(uint8_t)0x02;
		ParameterLength = SEND_PACKET_GET_CALENDAR_LENGTH;
		break;
	case ATPSetCalendarId:
		packetType = PACKET_SET_CALENDAR;//(uint8_t)0x12;
		ParameterLength = SEND_PACKET_SET_CALENDAR_LENGTH;
		break;
	case ATPLogQuantityId:
		packetType = PACKET_QUANTITY_DATA;//(uint8_t)0x04;
		ParameterLength = SEND_PACKET_QUANTITY_DATA_LENGTH;
		break;
	case ATPLogDatanEndId:
		packetType = PACKET_GET_LOG_DATA;//(uint8_t)0x33
		ParameterLength = SEND_PACKET_GETDATA_LENGTH;
		break;
	case ATPLogDatanTopId:
		packetType = PACKET_GET_LOG_DATA;//(uint8_t)0x33
		ParameterLength = SEND_PACKET_GETDATA_LENGTH;
		break;
	case ATPLognCountId:
		packetType = PACKET_GET_LOG_DATA;//(uint8_t)0x33
		ParameterLength = SEND_PACKET_GETDATA_LENGTH;
		break;
	case ATPLogForSearchAcqId:
		packetType = PACKET_GET_LOG_DATA;//(uint8_t)0x33
		ParameterLength = SEND_PACKET_GETDATA_LENGTH;
		break;
	case ATPLogQuantityFH310Id:
		packetType = PACKET_QUANTITY_DATA_FH;//(uint8_t)0x05
		ParameterLength = SEND_PACKET_QUANTITY_DATA_FH;
		break;
	case ATPStorageOfOxygenId:
	{
		packetType = PACKET_STORAGE_OXYGEN_DATA;//(uint8_t)0x21
		if((gs_numberOfFH310Log - gs_nTopFH310Log +1) <= MAX_STORE_LOG)
		{
			mTransmitCout = gs_numberOfFH310Log - gs_nTopFH310Log + 1;
		}
		else
		{
			mTransmitCout = MAX_STORE_LOG;
		}
		ParameterLength = PARAMETER_LENGTH_OFFSET + mTransmitCout*TRANSMIT_LOG_DATA_LEN;
		break;
	}
	default:
		break;
	}

	static uint8_t packet[MAX_PARA_LEN];

	packet[0] = F_BYTE;
	packet[1] = D_BYTE;
	packet[2] = H_BYTE;
	packet[3] = MODEL_CODE;
	packet[4] = gs_sendPacketNo;
	packet[5] = packetType;
	packet[6] = DESTINATION;

	switch(packetType)
	{
	case PACKET_ATP_INFO:
	case PACKET_GET_PATIENTID:
	case PACKET_GET_CALENDAR:
	case PACKET_QUANTITY_DATA:
	case PACKET_QUANTITY_DATA_FH:
		packet[7] = NONE_DATA;
		packet[8] = NONE_DATA;
		break;
	case PACKET_SET_CALENDAR:
	{
		Real_Time_Clock currentDate;
		guiInterface_RTCGettime(&currentDate);


		packet[7] = SET_CALENDAR_DATA_LEN;
		packet[8] = NONE_DATA;
		packet[9] = (uint8_t)(currentDate.year%YEAR_SCALE);
		packet[10] = (uint8_t)currentDate.month;
		packet[11] = (uint8_t)currentDate.mday;
		packet[12] = (uint8_t)currentDate.hour;
		packet[13] = (uint8_t)currentDate.min;
		packet[14] = (uint8_t)currentDate.sec;
		break;
	}
	case PACKET_GET_LOG_DATA:
	{
		switch(packetId)
		{
		case ATPLogDatanEndId:
		{
			uint32_t logNumberNEnd = gs_mLogNumberNEnd;

			packet[7] = GET_LOG_DATA_DATA_LEN;
			packet[8] = NONE_DATA;
			packet[9] = (uint8_t) (logNumberNEnd & 0x00ff);
			packet[10] = (uint8_t)((logNumberNEnd >> 8) & 0x00ff);
			packet[11] = (uint8_t)((logNumberNEnd >> 16) & 0x00ff);
			packet[12] = (uint8_t)((logNumberNEnd >> 24) & 0x00ff);
			gs_mnCount=1;
			packet[13] = (uint8_t)gs_mnCount;
			break;
		}
		case ATPLogDatanTopId:
		{
			uint32_t logNumberNTop = gs_mLogNumberNTop;

			packet[7] = GET_LOG_DATA_DATA_LEN;
			packet[8] = NONE_DATA;
			packet[9] = (uint8_t)(logNumberNTop & 0x00ff);
			packet[10] = (uint8_t)((logNumberNTop >> 8) & 0x00ff);
			packet[11] = (uint8_t)((logNumberNTop >> 16) & 0x00ff);
			packet[12] = (uint8_t)((logNumberNTop >> 24) & 0x00ff);
			gs_mnCount=1;
			packet[13] = (uint8_t)gs_mnCount;
			break;
		}
		case ATPLognCountId:
		{
			uint32_t numberlogCount = gs_mLogNumberNTop;

			if ((gs_anypalLogQuantity - gs_mLogNumberNTop) < MAX_TRANSMIT_LOG)
			{
				gs_mnCount = gs_anypalLogQuantity - gs_mLogNumberNTop+1;
			}
			else
			{
				gs_mnCount =MAX_TRANSMIT_LOG;
			}

			packet[7] = GET_LOG_DATA_DATA_LEN;
			packet[8] = NONE_DATA;
			packet[9] = (uint8_t)(numberlogCount & 0x00ff);
			packet[10] = (uint8_t)((numberlogCount >> 8) & 0x00ff);
			packet[11] = (uint8_t)((numberlogCount >> 16) & 0x00ff);
			packet[12] = (uint8_t)((numberlogCount >> 24) & 0x00ff);
			packet[13] = (uint8_t)gs_mnCount;
			break;
		}
		case ATPLogForSearchAcqId:
		{
			uint32_t logNumberN = (uint32_t)gs_mLogNumberN;
			gs_mnCount =1;
			packet[7] = GET_LOG_DATA_DATA_LEN;
			packet[8] = NONE_DATA;
			packet[9] = (uint8_t)(logNumberN & 0x00ff);
			packet[10] = (uint8_t)((logNumberN >> 8) & 0x00ff);
			packet[11] = (uint8_t)((logNumberN >> 16) & 0x00ff);
			packet[12] = (uint8_t)((logNumberN >> 24) & 0x00ff);
			packet[13] = (uint8_t)gs_mnCount;
			break;
		}
		default:
			break;
		}
		break;
	}
	case PACKET_STORAGE_OXYGEN_DATA:
	{
		uint16_t numberLogFH = (uint16_t)(1 + TRANSMIT_LOG_DATA_LEN*mTransmitCout);
		packet[7] = (uint8_t)(numberLogFH & 0x00ff);
		packet[8] = (uint8_t)((numberLogFH >> 8) & 0x00ff);
		packet[9] = (uint8_t)mTransmitCout;

		FH310_LOG_T log[MAX_LOG_ATP_RQ];
		LOG_REQUEST_t getLog;
		getLog.id = eAnypalGetUsageLogId;
		getLog.beginLog = gs_nTopFH310Log;
		getLog.numberOfLog = mTransmitCout;
		uint8_t numLog =0;
		logInterface_ReceiveLogForAnypal(getLog, log,&numLog);
		gs_nTopFH310Log += numLog;

		guiInterface_SetNumberOfLogStored(gs_nTopFH310Log-1);
		if(numLog != mTransmitCout)
		{
			ftdiAnypalHost_SetAnypalState(eAnypalSleep);
			anypal_StartTimerAnypalCycleTimer();
			return PIPE_READYWAIT_NoError;
		}
		for(int i = 0; i <numLog; i++)
		{
			if (log[i].ID == 0) break;

			float flowRate  = (float)log[i].flowRateSetting / FLOWRATE_SCALE;

			packet[12 * i + 10] = (uint8_t)(log[i].YYYY%YEAR_SCALE);
			packet[12 * i + 11] = (uint8_t)log[i].MM;
			packet[12 * i + 12] = (uint8_t)log[i].DD;

			packet[12 * i + 13] = (uint8_t)log[i].hh;
			packet[12 * i + 14] = (uint8_t)log[i].min;
			packet[12 * i + 15] = (uint8_t)log[i].ss;

			if(log[i].ID == eOperationStartLogId)//Operation start
			{
				packet[12 * i + 16] = (uint8_t)0x01;

			}
			else if(log[i].ID == eChangingFlowRateLogId)//Changing flow rate
			{
				packet[12 * i + 16] = (uint8_t)0x02;
			}
			else //Operation stop
			{
				packet[12 * i + 16] = (uint8_t)0x03;
			}

			if(flowRate == 0.25)
			{
				packet[12 * i + 17] = (uint8_t)0x01;
			}
			else if (flowRate == 0.5)
			{
				packet[12 * i + 17] = (uint8_t)0x02;
			}
			else if (flowRate == 0.75)
			{
				packet[12 * i + 17] = (uint8_t)0x03;
			}
			else if (flowRate == 1.00)
			{
				packet[12 * i + 17] = (uint8_t)0x04;
			}
			else if (flowRate == 1.25)
			{
				packet[12 * i + 17] = (uint8_t)0x05;
			}
			else if (flowRate == 1.50)
			{
				packet[12 * i + 17] = (uint8_t)0x06;
			}
			else if (flowRate == 1.75)
			{
				packet[12 * i + 17] = (uint8_t)0x07;
			}
			else if (flowRate == 2.00)
			{
				packet[12 * i + 17] = (uint8_t)0x08;
			}
			else if (flowRate == 2.50)
			{
				packet[12 * i + 17] = (uint8_t)0x09;
			}
			else if (flowRate == 3.00)
			{
				packet[12 * i + 17] = (uint8_t)0x0A;
			}


			packet[12 * i + 18] = (uint8_t)NONE_DATA;
			packet[12 * i + 19] = (uint8_t)NONE_DATA;
			packet[12 * i + 20] = (uint8_t)NONE_DATA;

			packet[12 * i + 21] = packet[12 * i + 10] ^ packet[12 * i + 11] ^ packet[12 * i + 12]
																					 ^ packet[12 * i + 13] ^ packet[12 * i + 14] ^ packet[12 * i + 15]
																																		  ^ packet[12 * i + 16] ^ packet[12 * i + 17] ^ packet[12 * i + 18]
																																															   ^ packet[12 * i + 19] ^ packet[12 * i + 20];
		}
		break;
	}
	default:
		break;
	}

	for(int i = 0; i < ParameterLength - 1; i++)
	{
		xorCheck = xorCheck ^ packet[i];
	}
	packet[ParameterLength - 1] = xorCheck;

	//	for(int i=0; i< ParameterLength; i++)
	//	{
	//		DEBUG_ATP("0x%.2x ",packet[i]);
	//	}
	//	DEBUG_ATP("\n");
	uint8_t errorCode = ftdiAnypalHost_SendPacketToATP(packet, ParameterLength);
	DEBUG_ATP(" ---> send\n");
	return errorCode;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_TransferPacket                                        			*/
/*                                                                                      */
/* Details  Transfer a Paket															*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ATPPacketId sendPacketId				  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE		                                                        	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_mResendCnt				                              	*/
/*                     (O) gs_sendPacketNo                                 			   	*/
/*                     (I) gs_AnypalResponseTimer                                   	*/
/****************************************************************************************/
void anypal_TransferPacket(E_ATPPacketId sendPacketId)
{
	if (gs_mResendCnt != 0)
	{
		gs_mResendCnt =0;
	}

	if(anypal_SendPacketToATP(sendPacketId) != PIPE_READYWAIT_NoError)
	{
		DEBUG_ATP("ERR: Failed to send packet %d \n", sendPacketId);
	}
	gs_sendPacketNo++;
	if (gs_sendPacketNo == MAX_PACKET_NO)
	{
		gs_sendPacketNo = 0x01;
	}
	if (xTimerStart(gs_AnypalResponseTimer,TIMER_WAITING) != pdPASS) {
		DEBUG_ATP("ERR: Failed to start gs_AnypalResponeTimer \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_ActiveAlarmCommunication                                    	*/
/*                                                                                      */
/* Details: Active anypal communication alarm											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) virtualAnyPal                                   			   	*/
/****************************************************************************************/
void anypal_ActiveAlarmCommunication(void)
{
	DEBUG_ATP("ACTIVE ALARM **********************************\n");
	anypal_StopAllAnypalTimer();
	anypal_SendGettingSpO2PulseDataStatus(false);
	virtualAnyPal.status.isCommunication = true;
	ftdiAnypalHost_SetAnypalState(eAnypalAlarm);

	GUI_EVENT_STRUCT event;
	event.id = eAnypalStatusEventId;
	event.data.atpStatus = virtualAnyPal.status;
	guiInterface_SendGuiEvent(event);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_StartTurnONVBusTimer                                        	*/
/*                                                                                      */
/* Details: Start Turn ON VBus 	Timer										 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Using Global Data : (I) gs_turnOnVBUSTimer			                              	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void anypal_StartTurnONVBusTimer(void)
{
	if (xTimerStart(gs_turnOnVBUSTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to start gs_turnOnVBUSTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_StartAlarmOverCurrentOffTimer                                	*/
/*                                                                                      */
/* Details: Start Alarm Over CurrentOff 	Timer										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_StartAlarmOverCurrentOffTimer(void)
{
	if (xTimerStart(gs_AlarmOverCurrentOffTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to start s_AlarmOverCurrentOffTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_StopAlarmOverCurrentOffTimer			                        */
/*                                                                                      */
/* Details:	stop Alarm Over CurrentOff 	Timer										 	*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_StopAlarmOverCurrentOffTimer(void)
{
	if (xTimerStop(gs_AlarmOverCurrentOffTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to stop s_AlarmOverCurrentOffTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_TransferPacket                                        			*/
/*                                                                                      */
/* Details: resend a packet data to PulseOxymeter device if previous sending 			*/
/*        		was failed																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ATPPacketId sendPacketId				  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_RetransferPacket(E_ATPPacketId sendPacketId)
{
	gs_mResendCnt++;
	if (gs_mResendCnt == RESET_VBUS)
	{
		ftdiAnypalHost_SetAnypalState(eAnypalWaitReconnectForTimeOut);
		anypal_SendRequestSwicthVbus(false);
		anypal_StartTurnONVBusTimer();
		anypal_SwitchReconnectErrTimer(true);
	}
	else if (gs_mResendCnt >MAX_RESEND_PCKET)
	{
		anypal_ActiveAlarmCommunication();
	}
	else
	{
		if(anypal_SendPacketToATP(sendPacketId) != PIPE_READYWAIT_NoError)
		{
			DEBUG_ATP("ERR: Failed to send packet %d \n", sendPacketId);
		}
		gs_sendPacketNo++;
		if (gs_sendPacketNo == MAX_PACKET_NO)
		{
			gs_sendPacketNo = 0x01;
		}
		if (xTimerStart(gs_AnypalResponseTimer,TIMER_WAITING) != pdPASS) {
			DEBUG_ATP("ERR: Failed to start gs_AnypalResponeTimer \n");
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_TimerCallback                                        			*/
/*                                                                                      */
/* Details: call back function for timer Interrupt							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) TimerHandle_t xTimer						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_nextState                                    			   	*/
/****************************************************************************************/
static void anypal_TimerCallback( TimerHandle_t xTimer)
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	case ANYPAL_RESPONE_TIMER_ID:
		DEBUG_ATP("\nANYPAL RESPONE time out");
		gs_nextState = ATP_Wait_SignatureF;
		ftdiAnypalHost_SetAnypalState(eAnypalRetranferForTimeOut);
		break;
	case ANYPAL_CYCLE_TIMER_ID:
	{
		DEBUG_ATP("\nANYPAL next cycle");
		ftdiAnypalHost_SetAnypalState(eAnypalTransferForNewCycle);
		gs_nextState = ATP_Wait_SignatureF;
		break;
	}
	case TURN_ON_VBUS_TIMER_ID:
	{
		gs_nextState = ATP_Wait_SignatureF;
		anypal_SendRequestSwicthVbus(true);
		break;
	}
	case OVERCURRENT_TIMER_ID:
	{
		GUI_EVENT_STRUCT event;
		event.id = eOverCurrentEventId;
		event.data.charData = eInactive;
		guiInterface_SendGuiEvent(event);
		ftdiAnypalHost_ReconnectAfterOVCR();
		break;
	}
	case ANYPAL_PUSH_OUT_TIME_ID:
	case ANYPAL_RECONNECT_ERR_TIMER_ID:
	{
		ftdiAnypalHost_SetAnypalState(eAnypalDiconnecting);
		break;
	}
	default:
		DEBUGOUT("Anypal invalid timer id\n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_CreateTimer                                        			*/
/*                                                                                      */
/* Details: create all timer use for Anypal communication					 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_CreateTimer(void)
{
	gs_AnypalResponseTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"ATPResponse",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(ANYPAL_RESPONE_TIMER),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) ANYPAL_RESPONE_TIMER_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					anypal_TimerCallback
			);
	gs_AnypalReconnectErrorTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"ATPRecon",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(ANYPAL_RECONNECT_ERR_TIMER),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) ANYPAL_RECONNECT_ERR_TIMER_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					anypal_TimerCallback
			);

	gs_AnypalCycleTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"ATPCycle",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(ANYPAL_CYCLE_TIMER),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) ANYPAL_CYCLE_TIMER_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					anypal_TimerCallback
			);
	gs_turnOnVBUSTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"ATPVBus",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(TURN_ON_VBUS_TIMER),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) TURN_ON_VBUS_TIMER_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					anypal_TimerCallback
			);
	gs_AlarmOverCurrentOffTimer  = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"ATPAlarmOC",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(ALARM_OVER_CURRENT_TURN_OFF_TIME),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) OVERCURRENT_TIMER_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					anypal_TimerCallback
			);
	gs_PushOutAnypalTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"ATPPushOut",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(ANYPAL_PUSH_OUT_TIME),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) ANYPAL_PUSH_OUT_TIME_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					anypal_TimerCallback
			);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_startTimerAnypalCycleTimer                           			*/
/*                                                                                      */
/* Details: start Anypal Cycle Timer										 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void anypal_StartTimerAnypalCycleTimer(void)
{
	DEBUG_ATP("Start gs_AnypalCycleTimer\n");
	if (xTimerStart(gs_AnypalCycleTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to start gs_AnypalCycleTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_stopAllAnypalTimer			                           			*/
/*                                                                                      */
/* Details:	stop All Anypal 	Timer										 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/****************************************************************************************/
void anypal_StopAllAnypalTimer(void)
{
	if (xTimerStop(gs_AnypalCycleTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to stop gs_AnypalCycleTimer \n");
	}
	if (xTimerStop(gs_AnypalResponseTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to stop gs_AnypalResponeTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeQuantityLogData		                           			*/
/*                                                                                      */
/* Details:	analyze Quantity Log Data response packet							 		*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static void anypal_AnalyzeQuantityLogDataFH(void)
{
	//Free space before to store the FH310
	uint32_t mNStoreFH=0;
	uint8_t *pNumLogN;
	pNumLogN = (uint8_t*)&mNStoreFH;
	*pNumLogN = gs_packetIn[10];
	pNumLogN++;
	*pNumLogN = gs_packetIn[11];
	pNumLogN++;
	*pNumLogN = gs_packetIn[12];
	pNumLogN++;
	*pNumLogN = gs_packetIn[13];
	DEBUG_ATP("gs_mNStoreFH: %d\n",mNStoreFH);

	g_sendPacketId = ATPStorageOfOxygenId;
	DEBUG_ATP("ATPStorageOfOxygenId ");
	anypal_TransferPacket(ATPStorageOfOxygenId);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_GetNumberOfUnTransmittingLog		                           	*/
/*                                                                                      */
/* Details:	Get Number Of  Un-Transmitting FH310 Log		 							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/****************************************************************************************/
static void anypal_GetNumberOfUnTransmittingLog(void)
{
	uint32_t numberOfLogStored= guiInterface_GetNumberOfLogStored();
	gs_numberOfFH310Log = logInterface_GetNumberOfLog(eUsageLog);
	if (numberOfLogStored < gs_numberOfFH310Log)
	{
		gs_nTopFH310Log = numberOfLogStored +1;
		g_sendPacketId = ATPLogQuantityFH310Id;
		DEBUG_ATP("ATPLogQuantityFH310Id ");
		anypal_TransferPacket(ATPLogQuantityFH310Id);
	}
	else
	{
		anypal_StartTimerAnypalCycleTimer();
		ftdiAnypalHost_SetAnypalState(eAnypalSleep);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SendGettingSpO2PulseDataStatus		                           	*/
/*                                                                                      */
/* Details:	Send Getting SpO2 Pulse Data Status	to BT task	 							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool isGetting							  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/****************************************************************************************/
void anypal_SendGettingSpO2PulseDataStatus(bool isGetting)
{
	static bool isStartGetting = false;
	if(isGetting)
	{
		isStartGetting = true;
		BTTask_SendEvent(eBeginGettingSpO2PulseDataId);
	}
	else if(isStartGetting)
	{
		isStartGetting = false;
		BTTask_SendEvent(eEndGettingSpO2PulseDataId);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_GetNextStateAfterGetSpO2Log                          			*/
/*                                                                                      */
/* Details:	Get Next State After Get SpO2 Log							 				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static inline void anypal_GetNextStateAfterGetSpO2Log(void)
{
	gs_isGotSpO2Log = true;
	anypal_GetNumberOfUnTransmittingLog();
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeGetLogData		                           				*/
/*                                                                                      */
/* Details:	analyze get  Log Data response packet							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_sendPacketId							                    */
/*                     (I) gs_packetIn                                           		*/
/*                     (I) gs_mLogNumberN						                        */
/*                     (I) gs_mLogNumberNTop						                    */
/*                     (I) gs_mLogNumberNEnd						                    */
/*                     (I) gs_mnCount						                        	*/
/*                     (I) gs_anypalLogQuantity				                        	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
static void anypal_AnalyzeGetLogData(void)
{
	static Real_Time_Clock s_mXday;
	switch (g_sendPacketId)
	{
	case ATPLogDatanEndId:
	{
		DEBUG_ATP("Response get end log data successful\n");
		uint32_t numLog = logInterface_GetNumberOfLog(eAnypalLog);
		DEBUG_ATP("SpO2: %dLogs\n", numLog);
		s_mXday = anypal_GetDateOfNewestLog(numLog);
		Real_Time_Clock nEndDate;
		nEndDate.year = (uint16_t)(gs_packetIn[12]+YEAR_OFFSET);
		nEndDate.month = gs_packetIn[13];
		nEndDate.mday = gs_packetIn[14];
		nEndDate.hour = gs_packetIn[15];
		nEndDate.min = gs_packetIn[16];
		nEndDate.sec = gs_packetIn[17];
		nEndDate.wday =0;
		nEndDate.yday =0;
		if (realtimeclock_Compare2Date(s_mXday, nEndDate) == false)
		{
			if(numLog == 0)
			{
				//send begin getting SpO2PulseData when there are no SpO2 log was recorded in FH310
				anypal_SendGettingSpO2PulseDataStatus(true);
			}
			g_sendPacketId = ATPLogDatanTopId;
			DEBUG_ATP("ATPLogDatanTopId ");
			anypal_TransferPacket(ATPLogDatanTopId);
		}
		else
		{
			anypal_GetNextStateAfterGetSpO2Log();
		}
		break;
	}
	case ATPLogDatanTopId:
	{
		DEBUG_ATP("Response get top log data successful\n");
		Real_Time_Clock nTopDate;
		nTopDate.year = (uint16_t)(gs_packetIn[12]+YEAR_OFFSET);
		nTopDate.month = gs_packetIn[13];
		nTopDate.mday = gs_packetIn[14];
		nTopDate.hour = gs_packetIn[15];
		nTopDate.min = gs_packetIn[16];
		nTopDate.sec = gs_packetIn[17];
		nTopDate.wday =0;
		nTopDate.yday =0;
		if (realtimeclock_Compare2Date(s_mXday, nTopDate) == false)
		{
			g_sendPacketId = ATPLognCountId;
			DEBUG_ATP("ATPLognCountId ");
			anypal_TransferPacket(ATPLognCountId);
			anypal_SendGettingSpO2PulseDataStatus(false);
		}
		else
		{
			gs_mLogNumberN = (gs_mLogNumberNEnd+gs_mLogNumberNTop)/2;
			g_sendPacketId = ATPLogForSearchAcqId;
			DEBUG_ATP("ATPLogForSearchAcqId ");
			anypal_TransferPacket(ATPLogForSearchAcqId);
		}
		break;
	}
	case ATPLogForSearchAcqId:
	{
		DEBUG_ATP("Response get search log data successful\n");
		Real_Time_Clock NLogDate;
		NLogDate.year = (uint16_t)(gs_packetIn[12]+YEAR_OFFSET);
		NLogDate.month = gs_packetIn[13];
		NLogDate.mday = gs_packetIn[14];
		NLogDate.hour = gs_packetIn[15];
		NLogDate.min = gs_packetIn[16];
		NLogDate.sec = gs_packetIn[17];
		NLogDate.wday=0;
		NLogDate.yday=0;
		if (realtimeclock_Compare2Date(s_mXday, NLogDate) == true)
		{
			gs_mLogNumberNTop = gs_mLogNumberN;
		}
		else
		{
			gs_mLogNumberNEnd = gs_mLogNumberN;
		}
		if ((gs_mLogNumberNEnd - gs_mLogNumberNTop)>=3)
		{
			gs_mLogNumberN = (gs_mLogNumberNEnd+gs_mLogNumberNTop)/2;
			g_sendPacketId = ATPLogForSearchAcqId;
			DEBUG_ATP("ATPLogForSearchAcqId ");
			anypal_TransferPacket(ATPLogForSearchAcqId);
		}
		else
		{
			g_sendPacketId = ATPLognCountId;
			DEBUG_ATP("ATPLognCountId ");
			anypal_TransferPacket(ATPLognCountId);
			anypal_SendGettingSpO2PulseDataStatus(false);
		}
		break;
	}
	case ATPLognCountId:
	{
		DEBUG_ATP("Response get log ncount data successful\n");
		gs_mLogNumberNTop = gs_mLogNumberNTop + gs_mnCount;
		for(uint8_t i=0;i<gs_mnCount;i++)
		{
			Real_Time_Clock NLogDate;
			FH310_LOG_T log;
			NLogDate.year = (uint16_t)(gs_packetIn[12+10*i]+YEAR_OFFSET);
			NLogDate.month = gs_packetIn[13+10*i];
			NLogDate.mday = gs_packetIn[14+10*i];
			NLogDate.hour = gs_packetIn[15+10*i];
			NLogDate.min = gs_packetIn[16+10*i];
			NLogDate.sec = gs_packetIn[17+10*i];
			NLogDate.wday =0;
			NLogDate.yday =0;
			if (realtimeclock_Compare2Date(s_mXday, NLogDate) == false)
			{
				log.YYYY = NLogDate.year;
				log.MM = NLogDate.month;
				log.DD = NLogDate.mday;
				log.hh = NLogDate.hour;
				log.min = NLogDate.min;
				log.ss = NLogDate.sec;
				log.SpO2 = gs_packetIn[18+10*i];
				log.pulse = gs_packetIn[19+10*i];
				log.ID = eAnypalLogId;
				log.flowRateSetting=0;
				logInterface_WriteSpO2Log(log);
			}
		}
		if (gs_mLogNumberNTop < gs_anypalLogQuantity)
		{
			g_sendPacketId = ATPLognCountId;
			DEBUG_ATP("ATPLognCountId ");
			anypal_TransferPacket(ATPLognCountId);
		}
		else
		{
			anypal_GetNextStateAfterGetSpO2Log();
		}
		break;
	}
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeQuantityLogData		                           			*/
/*                                                                                      */
/* Details:	analyze Quantity Log Data response packet							 		*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_mLogNumberNTop			                              	*/
/*                     (O) gs_mLogNumberNEnd                              			   	*/
/*                     (I) gs_anypalLogQuantity		                                   	*/
/*                     (I) gs_packetIn				                                   	*/
/* local valiables   : NONE			                                                  	*/
/*?Note?                                                                       			*/
/*                                                                                      */
/****************************************************************************************/
static void anypal_AnalyzeQuantityLogData(void)
{
	uint8_t *pNumLogN;
	pNumLogN = (uint8_t*)&gs_anypalLogQuantity;
	*pNumLogN = gs_packetIn[18];
	pNumLogN++;
	*pNumLogN = gs_packetIn[19];
	pNumLogN++;
	*pNumLogN = gs_packetIn[20];
	pNumLogN++;
	*pNumLogN = gs_packetIn[21];
	DEBUG_ATP("gs_mLogNumberN: %d\n",gs_anypalLogQuantity);
	if (gs_anypalLogQuantity >0)
	{
		gs_mLogNumberNTop =FIRST_LOG_NUMBER;
		gs_mLogNumberNEnd = gs_anypalLogQuantity;
		g_sendPacketId = ATPLogDatanEndId;
		DEBUG_ATP("ATPLogDatanEndId ");
		anypal_TransferPacket(ATPLogDatanEndId);
	}
	else
	{
		anypal_GetNextStateAfterGetSpO2Log();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_GetNextStateAfterGetCalendar                          			*/
/*                                                                                      */
/* Details:	Get Next State After Get Calendar							 				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_isCalendarUpdate			                              	*/
/*                     (O) gs_isGotCalendar                                           	*/
/*                     (O) g_sendPacketId						                        */
/****************************************************************************************/
static void anypal_GetNextStateAfterGetCalendar(void)
{
	if (gs_isGotSpO2Log == false)
	{
		g_sendPacketId = ATPLogQuantityId;
		DEBUG_ATP("ATPLogQuantityId ");
		anypal_TransferPacket(ATPLogQuantityId);
	}
	else
	{
		anypal_GetNumberOfUnTransmittingLog();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeGetCalendar		                           			*/
/*                                                                                      */
/* Details:	analyze Get Calendar response packet							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_packetIn					                              	*/
/****************************************************************************************/
static void anypal_AnalyzeGetCalendar(void)
{
	gs_ATPClockTime.year = (int16_t)(gs_packetIn[10]+YEAR_OFFSET);
	gs_ATPClockTime.month = (uint8_t)gs_packetIn[11];
	gs_ATPClockTime.mday = (uint8_t)gs_packetIn[12];
	gs_ATPClockTime.hour = (uint8_t)gs_packetIn[13];
	gs_ATPClockTime.min = (uint8_t)gs_packetIn[14];
	gs_ATPClockTime.sec = (uint8_t)gs_packetIn[15];

	guiInterface_RTCSettime(&gs_ATPClockTime, E_ATP_REQUEST_SET_RTC_ID);

	anypal_GetNextStateAfterGetCalendar();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_SetCalendarUpdate			                          			*/
/*                                                                                      */
/* Details:	Get Next State After Get IDNumber							 				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_isGotCalendar                                           	*/
/*                     (O) gs_isCalendarUpdate					                        */
/****************************************************************************************/
void anypal_SetCalendarUpdate(void)
{
	gs_isCalendarUpdate = true;
	gs_isGotCalendar = false;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_GetNextStateAfterGetIDNumber                          			*/
/*                                                                                      */
/* Details:	Get Next State After Get IDNumber							 				*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_isCalendarUpdate			                              	*/
/*                     (O) gs_isGotCalendar                                           	*/
/*                     (O) g_sendPacketId						                        */
/****************************************************************************************/
static void anypal_GetNextStateAfterGetIDNumber(void)
{
	if(gs_isGotCalendar == false)
	{
		gs_isGotCalendar = true;
		if (gs_isCalendarUpdate == true)
		{
			//turn off the synchronization request
			gs_isCalendarUpdate = false;
			g_sendPacketId = ATPSetCalendarId;
			DEBUG_ATP("ATPSetCalendarId ");
			anypal_TransferPacket(ATPSetCalendarId);
		}
		else
		{
			g_sendPacketId = ATPGetCalendarId;
			DEBUG_ATP("ATPGetCalendarId ");
			anypal_TransferPacket(ATPGetCalendarId);
		}
	}
	else
	{
		anypal_GetNextStateAfterGetCalendar();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzePatientNumber		                           			*/
/*                                                                                      */
/* Details:	analyze patient number response packet							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_isGotIDNumber                               			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static void anypal_AnalyzePatientNumber(void)
{
	bool isSameIDNumber = true;
	for (int i=0; i< ATP_PATIENT_ID_LEN; i++)
	{
		if(gs_packetIn[10+i]!=virtualAnyPal.info.patientID[i])
		{
			isSameIDNumber = false;
			break;
		}
	}
	if ((isSameIDNumber == false)||(gs_isGotIDNumber == false))
	{
		memcpy (virtualAnyPal.info.patientID,&gs_packetIn[10],ATP_PATIENT_ID_LEN);
		DEBUG_ATP("\n virtualAnyPal.info.patientID = %d", virtualAnyPal.info.patientID);
		GUI_EVENT_STRUCT event;
		event.id = eAnypalInforEventId;
		event.data.atpInfor = virtualAnyPal.info;
		guiInterface_SendGuiEvent(event);
		gs_isGotIDNumber = true;
	}
	anypal_GetNextStateAfterGetIDNumber();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeDeviceInformation		                      			*/
/*                                                                                      */
/* Details:	analyze Device Information response packet						 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) virtualAnyPal				                              	*/
/*                     (I) gs_packetIn                                     			   	*/
/*                     (O) gs_isAlarmAnypal			                                   	*/
/*                     (O) gs_isCharging			                                   	*/
/*                     (O) gs_isGotIDNumber			                                   	*/
/****************************************************************************************/
static void anypal_AnalyzeDeviceInformation(void)
{
	memcpy(virtualAnyPal.info.version, &gs_packetIn[11],ATP_VERSION_LEN);
	if ((gs_packetIn[18]==ATP_03_DEV_INFO_BYTE)&&(gs_packetIn[19]==NONE_DATA))
	{
		virtualAnyPal.info.type = ATP_3;
		memcpy(virtualAnyPal.info.ComdityCode, ATP_03_COMDITY_CODE,ATP_COMDITY_CODE_LEN);
	}
	else
	{
		virtualAnyPal.info.type = ATP_2;
		memcpy(virtualAnyPal.info.ComdityCode, ATP_02_COMDITY_CODE,ATP_COMDITY_CODE_LEN);
	}
	if((gs_packetIn[20]&ATP_BATTERY_STATUS_BIT)!=0)
	{
		virtualAnyPal.status.isBatteryAbnomal =true;
		DEBUG_ATP("ERR: Anypal Battery Abnomal \n");
		ftdiAnypalHost_SetAnypalState(eAnypalAlarm);
		gs_isAlarmAnypal = true;
	}
	if((gs_packetIn[20]&ATP_RTC_STATUS_BIT)!=0)
	{
		virtualAnyPal.status.isRTCAbnormal =true;
		DEBUG_ATP("ERR: Anypal RTC Abnomal \n");
		ftdiAnypalHost_SetAnypalState(eAnypalAlarm);
		gs_isAlarmAnypal = true;
	}
	virtualAnyPal.status.isChaging = (gs_packetIn[20]&ATP_CHARGING_STATUS_BIT);

	if(gs_isAlarmAnypal == false)
	{
		if(gs_isCharging != virtualAnyPal.status.isChaging)
		{
			//send status to GUI
			if(virtualAnyPal.status.isChaging == false)
			{
				DEBUG_ATP("Stop Charging\n");
			}
			gs_isCharging = virtualAnyPal.status.isChaging;

			GUI_EVENT_STRUCT event;
			event.id = eAnypalStatusEventId;
			event.data.atpStatus = virtualAnyPal.status;
			guiInterface_SendGuiEvent(event);
		}
		if(gs_isGotIDNumber == true)
		{
			anypal_GetNextStateAfterGetIDNumber();
		}
		else
		{
			g_sendPacketId = ATPIdNumberId;
			DEBUG_ATP("ATPIdNumberId ");
			anypal_TransferPacket(ATPIdNumberId);
		}
	}
	else
	{
		//send status to GUI
		anypal_StopAllAnypalTimer();

		GUI_EVENT_STRUCT event;
		event.id = eAnypalStatusEventId;
		event.data.atpStatus = virtualAnyPal.status;
		guiInterface_SendGuiEvent(event);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_AnalyzeCheckSum				                      			*/
/*                                                                                      */
/* Details:	processing when check sum pass									 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t packetType						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : 	NONE		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static void anypal_AnalyzeCheckSum(uint8_t packetType)
{
	if (xTimerStop(gs_AnypalResponseTimer,TIMER_WAITING) != pdPASS) {
		DEBUG_ATP("ERR: Failed to stop gs_AnypalResponeTimer \n");
	}

	switch(packetType)
	{
	case PACKET_ATP_INFO:
		DEBUG_ATP("Response Device Information successful\n");
		anypal_AnalyzeDeviceInformation();
		break;
	case PACKET_GET_PATIENTID:
		DEBUG_ATP("Response Patient Number successful\n");
		anypal_AnalyzePatientNumber();
		break;
	case PACKET_GET_CALENDAR:
		DEBUG_ATP("Response get calendar successful\n");
		anypal_AnalyzeGetCalendar();
		break;
	case PACKET_SET_CALENDAR:
		DEBUG_ATP("Response set calendar successful\n");
		anypal_GetNextStateAfterGetCalendar();
		break;
	case PACKET_QUANTITY_DATA:
		DEBUG_ATP("Response get stored data quantity successful\n");
		anypal_AnalyzeQuantityLogData();
		break;
	case PACKET_GET_LOG_DATA:
		anypal_AnalyzeGetLogData();
		break;
	case PACKET_QUANTITY_DATA_FH:
		DEBUG_ATP("Response get stored data quantity FH successful\n");
		anypal_AnalyzeQuantityLogDataFH();
		break;
	case PACKET_STORAGE_OXYGEN_DATA:
		DEBUG_ATP("Response stored data successful\n");
		if(gs_numberOfFH310Log > gs_nTopFH310Log)
		{
			g_sendPacketId = ATPStorageOfOxygenId;
			DEBUG_ATP("ATPStorageOfOxygenId ");
			anypal_TransferPacket(ATPStorageOfOxygenId);
		}
		else
		{
			ftdiAnypalHost_SetAnypalState(eAnypalSleep);
			anypal_StartTimerAnypalCycleTimer();
		}
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_reTryWhenWrongRespone			                      			*/
/*                                                                                      */
/* Details:	when a wrong response packet come from anypal, the re send previous packet	*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_nextState					                              	*/
/*                     (O) gs_AnypalResponseTimer                         			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void anypal_reTryWhenWrongRespone(void)
{
	if (xTimerStop(gs_AnypalResponseTimer,TIMER_WAITING) != pdPASS)
	{
		DEBUG_ATP("ERR: Failed to stop gs_AnypalResponeTimer \n");
	}
	gs_nextState = ATP_Wait_SignatureF;
	ftdiAnypalHost_SetAnypalState(eAnypalRetranferForTimeOut);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: anypal_ReceivePacketFromATP			                      			*/
/*                                                                                      */
/* Details:	Receive packet for ATP one by one byte							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t receicePacket					  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) gs_nextState					                              	*/
/*                     (O) gs_packetIn                                     			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void anypal_ReceivePacketFromATP(uint8_t receicePacket)
{
	static uint8_t s_packetType = 0xff;
	static uint8_t s_dateDataCount=1;
	static uint16_t inPacketIndex =0;
	if (gs_nextState!=ATP_Wait_SignatureF)
		DEBUG_ATP("%x ",(uint8_t)receicePacket);
	switch(gs_nextState)
	{
	case ATP_Wait_SignatureF:
		if(receicePacket == F_BYTE)
		{
			DEBUG_ATP("\nPacketin: %x ",(uint8_t)receicePacket);
			gs_nextState = ATP_Wait_SignatureD;
			inPacketIndex =0;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			gs_nextState = ATP_Wait_SignatureF;
		}
		break;
	case ATP_Wait_SignatureD:
		if(receicePacket == D_BYTE)
		{
			gs_nextState = ATP_Wait_SignatureH;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_SignatureH:
		if(receicePacket == T_BYTE)
		{
			gs_nextState = ATP_Wait_ModelCode;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_ModelCode:
	{
		switch(receicePacket){
		case MODEL_CODE_ATP_02:
		case MODEL_CODE_ATP_W:
		case MODEL_CODE_ATP_03:
			gs_nextState = ATP_Wait_SequenceNumber;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
			break;
		default:
			anypal_reTryWhenWrongRespone();
			break;
		}
		break;
	}
	case ATP_Wait_SequenceNumber:
		if(receicePacket > NONE_DATA)
		{
			gs_nextState = ATP_Wait_PacketType;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}

		break;

	case ATP_Wait_PacketType:
	{
		switch(receicePacket)
		{
		case PACKET_ATP_INFO:
		case PACKET_GET_CALENDAR:
		case PACKET_GET_PATIENTID:
		case PACKET_SET_CALENDAR:
		case PACKET_QUANTITY_DATA:
		case PACKET_GET_LOG_DATA:
		case PACKET_QUANTITY_DATA_FH:
		case PACKET_STORAGE_OXYGEN_DATA:
			s_packetType = receicePacket;
			gs_nextState = ATP_Wait_Destination;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
			break;
		default:
			anypal_reTryWhenWrongRespone();
			break;
		}
		break;
	}
	case ATP_Wait_Destination:
		if(receicePacket == NONE_DATA)
		{
			gs_nextState = ATP_Wait_DataLengthLower;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_DataLengthLower:
		gs_nextState = ATP_Wait_DataLengthHigh;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;
	case ATP_Wait_DataLengthHigh:
		gs_nextState = ATP_Wait_Status;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;

	case ATP_Wait_Status:
	{
		if(receicePacket == NONE_DATA)
		{
			switch(s_packetType)
			{

			case PACKET_ATP_INFO:
				gs_nextState = ATP_Wait_EquipmentCode;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_GET_PATIENTID:
				gs_nextState = ATP_Wait_IdNumberId;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_GET_CALENDAR:
				gs_nextState = ATP_Wait_GetYearId;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_SET_CALENDAR:
				gs_nextState = ATP_Wait_CheckSum;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_QUANTITY_DATA:
				gs_nextState = ATP_Wait_GetQuantityData;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_GET_LOG_DATA:
				gs_nextState = ATP_Wait_GetNumberOfData;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_QUANTITY_DATA_FH:
				gs_nextState = ATP_Wait_GetQuantityFH310;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			case PACKET_STORAGE_OXYGEN_DATA:
				gs_nextState = ATP_Wait_CheckSum;
				gs_packetIn[inPacketIndex]=receicePacket;
				inPacketIndex++;
				break;
			default:
				break;
			}
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	}

	case ATP_Wait_EquipmentCode:
		if(receicePacket == NONE_DATA)
		{
			gs_nextState = ATP_Wait_MajorNumber;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_MajorNumber:
		if(receicePacket <= MAJOR_NUMBER_MAX)
		{
			gs_nextState = ATP_Wait_MinorNumber;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_MinorNumber:
		if(receicePacket <= MINOR_NUMBER_MAX)
		{
			gs_nextState = ATP_Wait_SerialNumber;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_SerialNumber:
		gs_nextState = ATP_Wait_YearUpper;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;
	case ATP_Wait_YearUpper:
		if(receicePacket <= YEAR_NUMBER_MAX)
		{
			gs_nextState = ATP_Wait_YearLower;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_YearLower:
		if(receicePacket <= YEAR_NUMBER_MAX)
		{
			gs_nextState = ATP_Wait_Month;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_Month:
		if((receicePacket >= JANUARY) &&
				(receicePacket <= DECEMBER))
		{
			gs_nextState = ATP_Wait_Day;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_Day:
		if((receicePacket >= FIRST_DAY_OF_MONTH) &&
				(receicePacket <= LAST_DAY_OF_MONTH))
		{
			gs_nextState = ATP_Wait_DeviceInforLower;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_DeviceInforLower:
		gs_nextState = ATP_Wait_DeviceInforHigh;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;
	case ATP_Wait_DeviceInforHigh:
		gs_nextState = ATP_Wait_Device_Status;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;
	case ATP_Wait_Device_Status:
		gs_nextState = ATP_Wait_CheckSum;
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		break;

	case ATP_Wait_IdNumberId:
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		if(inPacketIndex == RECEIVE_PACKET_ID_NUMBER_LEN)
		{
			gs_nextState = ATP_Wait_CheckSum;
		}
		break;

	case ATP_Wait_GetYearId:
		if(receicePacket <= MINOR_NUMBER_MAX)
		{
			gs_nextState = ATP_Wait_GetMonthId;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetMonthId:
		if((receicePacket >= JANUARY) &&
				(receicePacket <= DECEMBER))
		{
			gs_nextState = ATP_Wait_GetDayId;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetDayId:
		if((receicePacket >= FIRST_DAY_OF_MONTH) &&
				(receicePacket <= LAST_DAY_OF_MONTH))
		{
			gs_nextState = ATP_Wait_GetHour;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetHour:
		if(receicePacket <= MAX_HOUR)
		{
			gs_nextState = ATP_Wait_GetMinute;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetMinute:
		if(receicePacket <= MAX_MIN)
		{
			gs_nextState = ATP_Wait_GetSecond;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetSecond:
		if(receicePacket <= MAX_SEC)
		{
			gs_nextState = ATP_Wait_CheckSum;
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetQuantityData:
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		if(inPacketIndex == RECEIVE_PACKET_GET_QUANTITY_DATA_LEN)
		{
			gs_nextState = ATP_Wait_CheckSum;
		}
		break;

	case ATP_Wait_GetNumberOfData:
		if((receicePacket > NONE_DATA) && (receicePacket <= MAX_TRANSMIT_LOG))
		{
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
			gs_nextState = ATP_Wait_GetIdNumberData;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;

	case ATP_Wait_GetIdNumberData:
		if(receicePacket <= ID_NUMBER_DATA_MAX)
		{
			gs_packetIn[inPacketIndex]=receicePacket;
			inPacketIndex++;
			s_dateDataCount =0;
			gs_nextState = ATP_Wait_GetDateData;
		}
		else
		{
			anypal_reTryWhenWrongRespone();
		}
		break;
	case ATP_Wait_GetDateData:
	{
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		if(s_dateDataCount == DATE_DATA_LEN)
		{
			gs_nextState = ATP_Wait_GetSpO2Data;
		}
		else
		{
			s_dateDataCount++;
		}
		break;
	}

	case ATP_Wait_GetSpO2Data:
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		gs_nextState = ATP_Wait_GetPulseData;
		break;

	case ATP_Wait_GetPulseData:
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		gs_nextState = ATP_Wait_CheckSumData;
		break;

	case ATP_Wait_CheckSumData:
	{

		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;

		if(inPacketIndex < (10*gs_mnCount + 10))
		{
			gs_nextState = ATP_Wait_GetIdNumberData;
		}
		else
		{
			gs_nextState =ATP_Wait_CheckSum;
		}

		break;
	}

	case ATP_Wait_GetQuantityFH310:
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;

		if(inPacketIndex == RECEIVE_PACKET_GET_QUANTITY_DATA_LEN)
		{
			gs_nextState = ATP_Wait_CheckSum;
		}

		break;

	case ATP_Wait_CheckSum:
	{
		DEBUG_ATP("\n");
		gs_packetIn[inPacketIndex]=receicePacket;
		inPacketIndex++;
		uint8_t checkSum=0x00;

		for(int i=0;i<inPacketIndex;i++)
		{
			checkSum = checkSum ^ gs_packetIn[i];
		}
		if (checkSum ==0)
		{
			gs_nextState = ATP_Wait_SignatureF;
			anypal_AnalyzeCheckSum(s_packetType);
		}
		else
		{
			DEBUG_ATP("CheckSum fail\n");
			anypal_reTryWhenWrongRespone();
		}
		break;
	}
	default:
		gs_nextState = ATP_Wait_SignatureF;
		break;
	}
	return;
}



