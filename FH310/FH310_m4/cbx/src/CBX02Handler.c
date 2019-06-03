/********************************************************************/
/*                                                                  */
/* File Name    : CBX02Handler.c                        			*/
/*                                                                  */
/* General      : CBX02 Handler										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/11/22      Linh Nguyen	  	    new file        */
/* #0001       2017/08/30      Linh Nguyen	  	    modify	        */
/*      update to send ATP-03 information	                        */
/* #0002       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode									*/
/*	- change CBX_TurnOnTransferMode function						*/
/*	- Add cbx_CopyLogToEXTRam function								*/
/*  Use the new get serial number function 							*/
/* #0003       2016/09/11      Linh Nguyen	  	    modify          */
/*  Change the design for collect information and log of anypal		*/
/*	Move send packet for cbx to ext ram								*/
/*	Create get real data funciton									*/
/* #0004       2017/09/20      Linh Nguyen	  	    modify          */
/*  Remove unuse code												*/
/*	Send message eCalendarUpdate									*/
/*	Do not use RS232_SendBtInfo to send BT infor					*/
/*	Fix cbx_SendFh310OperationStatus for unit test					*/
/*	Use the new get log function									*/
/* #0005       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/*  Get new anypal infor in UpdatingLogDeviceNo1ID command	     	*/
/*  Use guiInterface_RTCSettime for set calendar			     	*/
/*			2017/09/28       Linh Nguyen	  	    modify          */
/*  #0006 change cbx_SendFh310OperationStatus function for bug #1941*/
/*  #0007 check all command error case for bug #1949				*/
/*			2017/10/12       Linh Nguyen	  	    modify          */
/*  #0009 fix response commad for error case (bug #1949)			*/
/*  #0010 upper CRC for error response (bug #1968)					*/
/*			2017/10/19       Linh Nguyen	  	    modify          */
/*  #0011 Check parameter data format (bug #1949)					*/
/*			2017/11/02       Linh Nguyen	  	    modify          */
/*  #0012 Fix the AD data send to CBX (bug #2012)					*/
/*			2017/11/23       Linh Nguyen	  	    modify          */
/*  #0013 Fix bug 2040												*/
/*			2017/11/27	       Linh Nguyen	  	    modify          */
/*  #0014 Fix bug 2044												*/
/*			2017/11/29	       Linh Nguyen	  	    modify          */
/*  #0015 Fix bug 2044												*/
/*			2017/12/05	       Quyen Ngo	  	    modify          */
/*  #0016 Fix bug 2052												*/
/*			2018/03/14	       Quyen Ngo	  	    modify          */
/*  #0017 Add new reset case because the BT reset sequence			*/
/* specification change												*/
/********************************************************************/
#include "CBX02Handler.h"
#include "string.h"
#include "stringtools.h"
#include "timers.h"
#include "Bluetooth.h"
#include "CompositionMgr.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "LogInterface.h"
#include "RS232.h"
#include "AnypalInterface.h"
#include "ctype.h"

#ifdef DEBUG
#define DEBUG_CBX_EN
#endif

#ifdef DEBUG_CBX_EN
#define DEBUG_CBX(...) printf(__VA_ARGS__)
#else
#define DEBUG_CBX(...)
#endif

#define TRANFER_MODE_DELAY 20/portTICK_PERIOD_MS

#define MACHINE_MAX_LOG								1000
#define CBX_USAGE_MAX_LOG							8000
#define ANYPAL_MAX_LOG								20000 //id:

#define PSA_CYCLE_SCALE_DIV_FROM_MS					10   //ms/10=seconds*100

__BSS(RAM8) static FH310_LOG_T s_machineLog[MACHINE_MAX_LOG];
__BSS(RAM8) static FH310_LOG_T s_CBXLog[CBX_USAGE_MAX_LOG];
__BSS(RAM8) static FH310_LOG_T s_ATPLog[ANYPAL_MAX_LOG];

static uint16_t s_machineLogNumber;
static uint16_t s_CBXLogNumber;
static uint16_t s_ATPLogNumber;

static TimerHandle_t gs_CommunicationErrorTimer;
extern xQueueHandle g_RealDataQueue;

static uint8_t gs_mParameter[PARAMETER_LENGTH];

static uint8_t gs_mParameterIndex = 0;

// Current Log index request
static uint32_t gs_mCurrentLogIndex;

static bool gs_isGettingSpO2Data = false;

static bool s_isPacketFormRS232 = false;

static bool gs_isTransferMode = false;


/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_CopyLogToEXTRam		            		                  		*/
/*                                                                                      */
/* Details:  copy data to external ram													*/
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
/* Using Global Data : (I) uint16_t dest				                              	*/
/*                     (I) FH310_LOG_T *source                            			   	*/
/*                     (I) uint16_t NoLog			                                   	*/
/*                     (I) LogType logType			                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_CopyLogToEXTRam(uint16_t dest, FH310_LOG_T *source, uint16_t NoLog, LogType logType)
{
	switch (logType)
	{
	case eMachineLog:
		memcpy(&s_machineLog[dest], source, NoLog*(sizeof(FH310_LOG_T)));
		break;
	case eServerLog:
		memcpy(&s_CBXLog[dest], source, NoLog*(sizeof(FH310_LOG_T)));
		break;
	case eAnypalLog:
		memcpy(&s_ATPLog[dest], source, NoLog*(sizeof(FH310_LOG_T)));
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: CBX_TurnOnTransferMode				   		                  		*/
/*                                                                                      */
/* Details Turn On Transfer Mode														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NUM_OF_LOG_t NoLog						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) s_machineLog							                        */
/*                     (O) s_CBXLog                                           			*/
/*                     (O) s_ATPLog						                                */
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_TurnOnTransferMode(NUM_OF_LOG_t NoLog)
{
	s_machineLogNumber = NoLog.NoMachineLog;
	s_CBXLogNumber = NoLog.NoCBXLog;
	s_ATPLogNumber = NoLog.NoATPLog;
	gs_isTransferMode = true;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: CBX_TurnOffTransferMode				   		                  		*/
/*                                                                                      */
/* Details Turn Off Transfer Mode														*/
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
/*                     (O) isTransferMode                                 			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_TurnOffTransferMode(void)
{

	gs_isTransferMode = false;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-setting_SetContact					                                		*/
/*                                                                                      */
/* Details  -Set Contact											      				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *bufferContract : buffer contact								*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_isPacketFormRS232		                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              											    	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_isPacketFormRS232(bool isPacketFormRS232)
{
	s_isPacketFormRS232 = isPacketFormRS232;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_TimerCallback                                        				*/
/*                                                                                      */
/* Details: call back function for timer Interrupt							 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) TimerHandle_t xTimer						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_isGettingSpO2Data                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static void cbx_TimerCallback( TimerHandle_t xTimer)
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	case COMMUNICATION_ERROR_TIMER_ID:
		gs_isGettingSpO2Data = false;
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_CreateTimer                                        			*/
/*                                                                                      */
/* Details: create all timer use for Anypal communication					 			*/
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
/*                     (O) gs_CommunicationErrorTimer                     			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
void cbx_CreateTimer(void)
{
	gs_CommunicationErrorTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"CBXComErr",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(COMMUNICATION_ERROR_TIMER),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) COMMUNICATION_ERROR_TIMER_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					cbx_TimerCallback
			);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SetGettingSpO2Data                                				*/
/*                                                                                      */
/* Details:  Set value of gs_isGettingSpO2Data variable									*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool value								  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : none		                                         	               	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_isGettingSpO2Data                           			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_SetGettingSpO2Data(bool value)
{
	gs_isGettingSpO2Data = value;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendErrPacketToCbx                		                  		*/
/*                                                                                      */
/* Details  Send ErrPacket To CBX														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t command							  							*/
/*                                                                                      */
/*             (O) uint8_t ErrStatus													*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
int32_t cbx_SendErrPacketToCbx(uint8_t command, uint8_t ErrStatus)
{
	uint8_t R1,R2;
	///uint8_t Packet[10];
	R1 = command;
	R2 = ErrStatus;

	uint8_t Packet[CBX_ERR_PACKET_LENGTH] = {0};

	Packet[0] = STX_BYTE; // STX
	Packet[1] = R1; // R1
	Packet[2] = R2; // R2
	Packet[3] = ETX_BYTE; //ETX

	// Main packet for calculate CRC
	uint8_t MainPacket[MAIN_ERR_PACKET_LENGTH] = {0};

	for (int i = 0; i < MAIN_ERR_PACKET_LENGTH ; i++)
	{
		MainPacket[i] = Packet[i+1];
	}

	uint16_t temp = util_CompCrcCcitt(0x0000,MainPacket, MAIN_ERR_PACKET_LENGTH);
	sprintf((char*)&Packet[MAIN_ERR_PACKET_LENGTH + 1],"%.4X",temp);
	DEBUG_CBX("ERR: %s\n", Packet);
	if(s_isPacketFormRS232)
	{
		rs232_SendPacketToCbx(Packet, CBX_ERR_PACKET_LENGTH);
	}
	else
	{
		bluetooth_SendPacket(Packet, CBX_ERR_PACKET_LENGTH);
	}
	return CBX_ERR_PACKET_LENGTH;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendVersionInfoDeviceNo1            		                  		*/
/*                                                                                      */
/* Details  Send DeviceNo1 Version Info To CBX											*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) uint8_t *addedInfo													*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_SendVersionInfoDeviceNo1(uint8_t *addedInfo, AnypalInfor ATP)
{
	uint8_t versionNumber[20] = {0};   //"V01-030002015/01/30\0";
	strcat((char*)versionNumber, "V");//"V01-01 001 2016/08/24
	versionNumber[1] = ATP.version[0];
	versionNumber[2] = ATP.version[1];
	strcat((char*)versionNumber, "-");
	versionNumber[4] = ATP.version[2];
	versionNumber[5] = ATP.version[3];
	versionNumber[6] = ATP.version[4];
	versionNumber[7] = ATP.version[5];
	versionNumber[8] = ATP.version[6];
	versionNumber[9] = ATP.version[7];
	versionNumber[10] = ATP.version[8];
	versionNumber[11] = ATP.version[9];
	versionNumber[12] = ATP.version[10];
	strcat((char*)versionNumber, "/");
	versionNumber[14] = ATP.version[11];
	versionNumber[15] = ATP.version[12];
	strcat((char*)versionNumber, "/");
	versionNumber[17] = ATP.version[13];
	versionNumber[18] = ATP.version[14];
	strncat((char*)addedInfo,(char*)versionNumber,strlen((char*)versionNumber));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AddSpaceChar			            		                  		*/
/*                                                                                      */
/* Details  Add Space Char																*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint16_t len								  							*/
/*                                                                                      */
/*             (O) uint8_t *dest														*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
INLINE static void cbx_AddSpaceChar(uint8_t *dest, uint16_t len)
{
	while(strlen((char*)dest)<len)
	{
		strncat((char*)dest," ",strlen((char*)" "));
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendCumulativeHours	            		                  		*/
/*                                                                                      */
/* Details  Send Cumulative Hours To CBX												*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_BtReturnPacketId packetId				  							*/
/*                                                                                      */
/*             (O) uint8_t* addedInfo													*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_SendCumulativeHours(E_BtReturnPacketId packetId, uint8_t* addedInfo)
{
	int8_t Fh310PatientHour[FH310_PATIENT_HOUR_LEN]={0};
	int8_t Fh310UnitHour[FH310_PATIENT_HOUR_LEN]={0};

	CumulativeHourTime time = guiInterface_GetCumulativeHour(patients);
	sprintf((char*)Fh310PatientHour,"%.8x%.2x%.2x\0",time.hour,time.min,time.sec);

	time = guiInterface_GetCumulativeHour(uints);
	sprintf((char*)Fh310UnitHour,"%.8x%.2x%.2x\0",time.hour,time.min,time.sec);

	if (packetId ==Fh310PatientHourID)
	{
		strncat((char*)addedInfo, (char*)Fh310PatientHour,FH310_PATIENT_HOUR_LEN);
	}
	else if (packetId == Fh310UnitHourID)
	{
		strncat((char*)addedInfo, (char*)Fh310UnitHour,FH310_PATIENT_HOUR_LEN);
	}
	else if (packetId == CumulativeOperationID)
	{
		strncat((char*)addedInfo, (char*)Fh310PatientHour,FH310_PATIENT_HOUR_LEN);
		strncat((char*)addedInfo, (char*)Fh310UnitHour,FH310_PATIENT_HOUR_LEN);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendFh310LogData		            		                  		*/
/*                                                                                      */
/* Details  Send Fh310 Log Data To CBX													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t mNumberOfLogRequest				  							*/
/*             (I) E_BtReturnPacketId packetId                                          */
/*             (O) uint8_t* addedInfo													*/
/*                                                                                      */
/* ReturnValue : bool		                         		                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) gs_mCurrentLogIndex			                              	*/
/*                     (O) s_machineLog                                    			   	*/
/*                     (O) s_CBXLog					                                   	*/
/*                     (O) s_ATPLog                                    			   	*/
/*                     (O) s_machineLog                                    			   	*/
/*                                                                                      */
/****************************************************************************************/
static bool cbx_SendFh310LogData(uint32_t mNumberOfLogRequest, E_BtReturnPacketId packetId, uint8_t* addedInfo)
{
	FH310_LOG_T* log = NULL;
	FH310_LOG_T logGet[MAX_LOG_CBX_RQ];
	if(gs_isTransferMode)
	{
		if(mNumberOfLogRequest > MAX_LOG_REQUEST)
		{
			return false;
		}
		if(packetId == Fh310LogDataID)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (s_machineLogNumber+1))
			{
				return false;
			}
			log = &s_machineLog[gs_mCurrentLogIndex-1];
		}
		else if (LogDataFh310UsageLogID == packetId)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (s_CBXLogNumber+1))
			{
				return false;
			}
			log = &s_CBXLog[gs_mCurrentLogIndex-1];

		}
		else if (LogDataAnypalPulseLogID == packetId)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (s_ATPLogNumber+1))
			{
				return false;
			}
			log = &s_ATPLog[gs_mCurrentLogIndex-1];
		}
	}
	else
	{
		if(mNumberOfLogRequest > MAX_LOG_CBX_RQ)
		{
			return false;
		}
		LOG_REQUEST_t cbxGetLog;
		if (packetId == Fh310LogDataID)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (logInterface_GetNumberOfLog(eMachineLog)+1))
			{
				return false;
			}
			cbxGetLog.id = eCBXGetMachineLogId;
		}
		else if (LogDataFh310UsageLogID == packetId)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (logInterface_GetNumberOfLog(eServerLog)+1))
			{
				return false;
			}
			cbxGetLog.id = eCBXGetServerLogId;
		}
		else if (LogDataAnypalPulseLogID == packetId)
		{
			if((gs_mCurrentLogIndex + mNumberOfLogRequest) > (logInterface_GetNumberOfLog(eAnypalLog)+1))
			{
				return false;
			}
			cbxGetLog.id = eCBXGetAnypalLogId;
		}

		cbxGetLog.beginLog = gs_mCurrentLogIndex;
		cbxGetLog.numberOfLog = mNumberOfLogRequest;
		uint8_t numLog;
		logInterface_ReceiveLogForCBX(cbxGetLog, logGet, &numLog);
		log = logGet;
	}

	for (int i=0; i< mNumberOfLogRequest; i++)
	{
		if (LogDataAnypalPulseLogID == packetId)
		{
			sprintf((char*)addedInfo,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d0000%.2x%.2x",
					(log[i].YYYY%100),log[i].MM,log[i].DD,log[i].hh,log[i].min,log[i].ss,log[i].SpO2,log[i].pulse);
		}
		else
		{
			sprintf((char*)addedInfo,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d%.4x%.4x",
					(log[i].YYYY%100),log[i].MM,log[i].DD,log[i].hh,log[i].min,log[i].ss,log[i].ID,(log[i].flowRateSetting*5));
		}
		addedInfo+=LOG_DATA_LEN;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_GetRealData			            		                  		*/
/*                                                                                      */
/* Details  get Real Data To CBX														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) MONITOR_DATA_t *monitorData				  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static bool cbx_GetRealData(MONITOR_DATA_t *monitorData)
{
	GUI_EVENT_STRUCT event;
	event.id = eCBXGetRealDateid;
	guiInterface_SendGuiEvent(event);

	if(!xQueueReceive(g_RealDataQueue, monitorData,QUEUE_WAIT_1000MS))
	{
		DEBUG_CBX("Failed to receive item to queue within 1000ms\n");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendRealData			            		                  		*/
/*                                                                                      */
/* Details  Send Real Data To CBX														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t* addedInfo						  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool				                                                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/****************************************************************************************/
static bool cbx_SendRealData(uint8_t* addedInfo)
{
	int8_t calendar[CALENDAR_LEN]={0};
	Real_Time_Clock clock;
	guiInterface_RTCGettime(&clock);
	sprintf((char*)calendar,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d\0",
			(clock.year%YEAR_SCALE),clock.month,clock.mday,clock.hour,clock.min,clock.sec);
	strncat((char*)addedInfo,(char*)calendar,CALENDAR_LEN);

	MONITOR_DATA_t monitorData;
	if(!cbx_GetRealData(&monitorData))
	{
		return false;
	}

	PSA_MONITOR_PACKET_t realMonitorData = monitorData.PASMonitorData;

	int16_t corresPondingValue = realMonitorData.productankPress;
	if(corresPondingValue< 0)
	{
		corresPondingValue =0;
	}
	uint16_t adConverterValue = realMonitorData.prouctankAdc;
	int8_t producTankAdStr[MONITOR_DATA_LEN] = {0};

	sprintf((char*)producTankAdStr,"%.4x\0",adConverterValue);
	int8_t productTankKpaStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)productTankKpaStr,"%.4x\0",corresPondingValue);

	strncat((char*)addedInfo,(char*)producTankAdStr,MONITOR_DATA_LEN);
	strncat((char*)addedInfo,(char*)productTankKpaStr,MONITOR_DATA_LEN);

	//			// Canula Pressure
	corresPondingValue = realMonitorData.cannulaPress;
	if(corresPondingValue< 0)
	{
		corresPondingValue =0;
	}
	adConverterValue = realMonitorData.canulaAdc;
	int8_t cannulaAdStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)cannulaAdStr,"%.4x\0",adConverterValue);

	int8_t cannulaKpaStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)cannulaKpaStr,"%.4x\0",corresPondingValue);

	strncat((char*)addedInfo,(char*)cannulaAdStr,MONITOR_DATA_LEN);
	strncat((char*)addedInfo,(char*)cannulaKpaStr,MONITOR_DATA_LEN);

	// Oxygen Concentration
	//modify for Ext Communication SRS Rev01 draft1 2017053001
	strcat((char*)addedInfo,"00000000\0");

	// Temperature
	//modify for bug 1707
	corresPondingValue = realMonitorData.temperature/TEMP_SCALE_VALUE;
	if(corresPondingValue< 0)
	{
		corresPondingValue =0;
	}
	adConverterValue = realMonitorData.temperatureAdc;
	int8_t temperatureAdStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)temperatureAdStr,"%.4x\0",adConverterValue);

	int8_t temperatureStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)temperatureStr,"%.4x\0",corresPondingValue);

	strncat((char*)addedInfo,(char*)temperatureAdStr,MONITOR_DATA_LEN);
	strncat((char*)addedInfo,(char*)temperatureStr,MONITOR_DATA_LEN);

	//Illuminance
	strcat((char*)addedInfo,"00000000\0");
	//Cooling Fan1
	strcat((char*)addedInfo,"00000000\0");
	//Cooling Fan 2
	strcat((char*)addedInfo,"00000000\0");
	//Rreserved
	strcat((char*)addedInfo,"00000000\0");

	// CycleTime
	corresPondingValue = (realMonitorData.psaCycleTime)/PSA_CYCLE_SCALE_DIV_FROM_MS;
	int8_t CycleTime[MONITOR_DATA_LEN] = {0};
	sprintf((char*)CycleTime,"%.4x\0",corresPondingValue);
	strncat((char*)addedInfo,(char*)CycleTime,MONITOR_DATA_LEN);

	//			// ValA time
	strcat((char*)addedInfo,"0000\0");

	//			//Val B time
	strcat((char*)addedInfo,"0000\0");

	// Setting Flow rate
	corresPondingValue = FLOW_RATE_SETTING_SCALE* monitorData.flowRateSetting;
	int8_t FlowStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)FlowStr,"%.4x\0",corresPondingValue);
	strncat((char*)addedInfo,(char*)FlowStr,MONITOR_DATA_LEN);

	//			// Alarm Information
	strcat((char*)addedInfo,"00000000\0");

	//			// Operation indormation
	strcat((char*)addedInfo,"0125\0");
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendFh310OperationStatus            		                  		*/
/*                                                                                      */
/* Details  Send Fh310 Operation Status To CBX											*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE										  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Arguments : (I) uint8_t* addedInfo						  							*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static bool cbx_SendFh310OperationStatus(uint8_t* addedInfo)
{
	uint8_t OperationSwitchStatus=0;
	uint8_t OperationStatus=0;
	uint8_t OperationStatusData = 0;
	uint8_t OxygenConcentrationStatus;
	MONITOR_DATA_t monitorData;

	if(!cbx_GetRealData(&monitorData))
	{
		return false;
	}

	PSA_MONITOR_PACKET_t realMonitorData = monitorData.PASMonitorData;
	// Operation Status
	E_ModeSetting currentMode =  monitorData.currentMode;
	if (currentMode == eStandyMode)
	{
		OperationSwitchStatus = 0;
	}
	else
	{
		OperationSwitchStatus =1;
	}

	E_PsaSequence psaSequence = realMonitorData.psaCurSeq;
	if (psaSequence == PSA_STANDBY)
	{
		OperationStatus = 0;
	}
	else if(psaSequence == PSA_START)
	{
		OperationStatus = 1;
	}
	else if (psaSequence == PSA_BASIC || psaSequence == PSA_DECOMPRESS)
	{
		OperationStatus = 2;
	}
	else if(psaSequence == PSA_END)
	{
		OperationStatus = 3;
	}

	int8_t oxySendorStatus = 1;

	E_AlarmId currentAlarm = monitorData.currentAlarm;
	if ((currentAlarm == eO2ConcentrationId)
			|| (currentAlarm == eO2ConcentrationSensorId)
			|| (currentAlarm == eLoweringO2ConcentrationId))
	{
		OxygenConcentrationStatus = 1;
	}
	else
	{
		OxygenConcentrationStatus = 0;
	}
	OperationStatusData = OperationSwitchStatus | (OperationStatus <<1) | (oxySendorStatus << 3)| (OxygenConcentrationStatus << 4);
	addedInfo[0] = CharToAscci(OperationStatusData >> 4);
	addedInfo[1]=  CharToAscci(OperationStatusData & 0x0f);

	// Flow rate
	uint16_t Flow = FLOW_RATE_SETTING_SCALE* monitorData.flowRateSetting;
	int8_t FlowStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)FlowStr,"%.4x\0",Flow);
	strncat((char*)addedInfo,(char*)FlowStr,MONITOR_DATA_LEN);

	// Oxygen Concentration
	uint16_t o2 = realMonitorData.o2Concentrator;
	int8_t oxygenConcentrationStr[MONITOR_DATA_LEN] = {0};
	sprintf((char*)oxygenConcentrationStr, "%.4x",o2);
	strncat((char*)addedInfo,(char*)oxygenConcentrationStr,MONITOR_DATA_LEN);

	//Alarm Number
	strcat((char*)addedInfo,"0000\0");
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_PreSendPacket		                                       		*/
/*                                                                                      */
/* Details:  Create packet data and send to CBX-02										*/
/*       Data structure: STX | R1 | R2 | Added Info | ETX | CRC1 | CRC2 | CRC3 | CRC4	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_BtReturnPacketId packetId				  							*/
/*                                                                                      */
/*             (I) uint32_t mNumberOfLogRequest											*/
/*                                                                                      */
/* ReturnValue : CBX_SENDPACKET_RESULT_t                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_CBXLogNumber				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static CBX_SENDPACKET_RESULT_t cbx_PreSendPacket(uint32_t mNumberOfLogRequest, E_BtReturnPacketId packetId)
{
	static uint32_t addedInfoLength=0;
	__BSS(RAM8) static uint8_t s_packetSend[ADD_INFO_MAX_LEN + SEND_PACKET_OFFSET_LEN] ={0};

	memset(s_packetSend,0,addedInfoLength+SEND_PACKET_OFFSET_LEN);

	addedInfoLength=0;

	uint8_t *addedInfo = &s_packetSend[3] ;

	uint8_t R1 = 0x00;
	uint8_t R2 = 0x00;

	static AnypalInfor ATP;

	switch(packetId)
	{
	case BTInfoID:
	{
		strncat((char*)addedInfo,(char*)bluetooth_GetAddr(),BT_ADDESS_LEN);
		strcat((char*)addedInfo, PIN_CODE);
		strcat((char*)addedInfo, FH310_DEVICE_NAME);
		R1 = C1_REGISTRATION;
		R2 = SP_BYTE;
		break;
	}
	case IdentificationInfoID:
	{
		strcat((char*)addedInfo, FH310_COMMUDITY_CODE);
		strcat((char*)addedInfo, FH310_DEVICE_NAME);
		R1 = C1_DEVICE_IDENTIFICATION_INFORMATION;
		R2 = SP_BYTE;
		ATP = guiInterface_GetAnypalInfo();
		break;
	}
	case IdentificationInfoDeviceNo1ID:
	{
		R1 = C1_DEVICE_IDENTIFICATION_INFORMATION;
		R2 = SP_BYTE;
		if(strlen((char*)ATP.equipmentName) == 0)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		if(!strncmp((char*)ATP.equipmentName,ATP03_DEVICE_NAME,strlen((char*)ATP.equipmentName)))
		{
			strcat((char*)addedInfo, ATP03_COMMUDITY_CODE);
			strcat((char*)addedInfo, ATP03_DEVICE_NAME);
		}
		else if(!strncmp((char*)ATP.equipmentName,ATP02_DEVICE_NAME,strlen((char*)ATP.equipmentName)))
		{
			strcat((char*)addedInfo, ATP02_COMMUDITY_CODE);
			strcat((char*)addedInfo, ATP02_DEVICE_NAME);
		}
		else
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		break;
	}

	case VersionInfoID:
	{
		uint8_t *versionNumber = (uint8_t*)stringtools_GetVersionCBX();
		strncat((char*)addedInfo,(char*)versionNumber,strlen((char*)versionNumber));
		R1 = C1_VERSION_INFORMATION;
		R2 = SP_BYTE;
		break;
	}
	case VersionInfoDeviceNo1ID:
	{
		R1 = C1_VERSION_INFORMATION;
		R2 = SP_BYTE;
		if(strlen((char*)ATP.equipmentName) == 0)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		cbx_SendVersionInfoDeviceNo1(addedInfo, ATP);
		break;
	}

	case IdNumberID:
	{
		int8_t serialNumber[SERIAL_NUMBER_BUFFER_SIZE];
		guiInterface_GetSerialNumber(serialNumber);
		memcpy((char*)addedInfo,serialNumber,SERIAL_NUMBER_BUFFER_SIZE);
		cbx_AddSpaceChar(addedInfo,ID_NUMBER_ADDED_INFOR_LEN);
		R1 = C1_ID_NUMBER;
		R2 = SP_BYTE;
		break;
	}

	case IdNumberDeviceNo1ID:
	{
		R1 = C1_ID_NUMBER;
		R2 = SP_BYTE;
		if(strlen((char*)ATP.equipmentName) == 0)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		memcpy((char*)addedInfo,ATP.numberID,ID_NUMBER_ADDED_INFOR_LEN);
		cbx_AddSpaceChar(addedInfo,ID_NUMBER_ADDED_INFOR_LEN);
		break;
	}

	case CumulativeOperationID:
	case Fh310PatientHourID:
	case Fh310UnitHourID:
	{
		cbx_SendCumulativeHours(packetId, addedInfo);
		R1 = C1_CUMULATIVE_OPERATION_HOURS;
		R2 = SP_BYTE;
		break;
	}

	case UpdatingCheckID:
	{
		int8_t* updatingState = composition_Get(eUpdatingStatus);//"01\0";
		strncat((char*)addedInfo,(char*)updatingState,COMPOSITION_LENG);
		composition_ClearUpdatingStatus();
		R1 = C1_DEVICE_IDENTIFICATION_INFORMATION;
		R2 = SP_BYTE;
		break;
	}

	case DeviceQuantityID:
	{
		int8_t* deviceQuantity = composition_Get(eDeviceQuantity);  // "02\0";
		strncat((char*)addedInfo,(char*)deviceQuantity,COMPOSITION_LENG);
		R1 = C1_DEVICE_IDENTIFICATION_INFORMATION;
		R2 = SP_BYTE;
		break;
	}

	case DeviceQuantityDeviceNo1ID:
	{
		R1 = C1_DEVICE_IDENTIFICATION_INFORMATION;
		R2 = SP_BYTE;
		if(strlen((char*)ATP.equipmentName) == 0)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		strcat((char*)addedInfo,IDENTIFICATION);
		break;
	}
	case CalendarSetupID:
	{
		R1 = C1_CALENDAR;
		R2 = SP_BYTE;
		break;
	}
	case Fh310LogQuantityID:
	{
		composition_SetUpdatingStatus();
		// Log Quantity of Fh310
		uint32_t mFh310LogQuantity;
		if (gs_isTransferMode)
		{
			mFh310LogQuantity = s_machineLogNumber;
		}
		else
		{
			mFh310LogQuantity = logInterface_GetNumberOfLog(eMachineLog);
		}
		sprintf((char*)addedInfo,"%.8x\0",mFh310LogQuantity);
		R1 = C1_LOG_EVEN;
		R2 = SP_BYTE;
		break;
	}
	case Fh310LogDataID:
	{
		R1 = C1_LOG_EVEN;
		R2 = SP_BYTE;
		if(!cbx_SendFh310LogData(mNumberOfLogRequest, packetId, addedInfo))
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		break;
	}

	case DeviceQuantityUpdatingLogID:
	{
		R1 = C1_LOG_EVEN;
		R2 = SP_BYTE;
		memcpy((char*)addedInfo, (char*)composition_Get(eDeviceQuantity),3);
		break;
	}

	case UpdatingLogDeviceNo1ID:
	{
		R1 = C1_LOG_EVEN;
		R2 = SP_BYTE;
		//ATP = guiInterface_GetAnypalInfo();
		if(strlen((char*)ATP.equipmentName) == 0)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		if(!strncmp((char*)ATP.equipmentName,ATP03_DEVICE_NAME,strlen((char*)ATP.equipmentName)))
		{
			strcat((char*)addedInfo, ATP03_DEVICE_NAME);
		}
		else if(!strncmp((char*)ATP.equipmentName,ATP02_DEVICE_NAME,strlen((char*)ATP.equipmentName)))
		{
			strcat((char*)addedInfo, ATP02_DEVICE_NAME);
		}
		else
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}

		memcpy((char*)&addedInfo[16],ATP.numberID,ID_NUMBER_ADDED_INFOR_LEN);
		cbx_AddSpaceChar(addedInfo,UPDATING_LOG_DEVICE_NO1_ADDED_INFOR_LEN);
		strcat((char*)addedInfo,"00\0");
		break;
	}

	case LogQuantityFh310UsageLogID:
	{
		R1 = C1_MEMORY_DATA;
		R2 = SP_BYTE;
		uint32_t mFh310UsageLogQuantity;
		if (gs_isTransferMode)
		{
			mFh310UsageLogQuantity = s_CBXLogNumber;
		}
		else
		{
			mFh310UsageLogQuantity = logInterface_GetNumberOfLog(eServerLog);
		}
		sprintf((char*)addedInfo,"%.8x\0",mFh310UsageLogQuantity);
		break;
	}

	case LogQuantityAnypalPulseLogID:
	{
		R1 = C1_MEMORY_DATA;
		R2 = SP_BYTE;
		uint32_t mAnypalPulseLogQuantity;
		AnypalInfor currentInfo = guiInterface_GetAnypalInfo();

		if (strncmp((char*)currentInfo.numberID,(char*)ATP.numberID,ID_NUMBER_ADDED_INFOR_LEN))
		{
			mAnypalPulseLogQuantity =0;
		}
		else if (gs_isTransferMode)
		{
			mAnypalPulseLogQuantity = s_ATPLogNumber;
		}
		else
		{
			mAnypalPulseLogQuantity = logInterface_GetNumberOfLog(eAnypalLog);
		}
		sprintf((char*)addedInfo,"%.8x\0",mAnypalPulseLogQuantity);
		break;
	}

	case RealDataID:
	{
		R1 = C1_REAL_DATA;
		R2 = SP_BYTE;
		if(!cbx_SendRealData(addedInfo))
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		break;
	}

	case Fh310OperationStatusID:
	{
		R1 = C1_OPERATIONAL_STATUS;
		R2 = SP_BYTE;
		if(!cbx_SendFh310OperationStatus(addedInfo))
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		break;
	}

	case LogQuantityDeviceNo1ID:
	{
		R1 = C1_LOG_QUANTITY_DEVICENO1;
		R2 = SP_BYTE;
		strcat((char*)addedInfo,LOG_QUANTITY_DEVICENO1);
		break;
	}

	case LogDataFh310UsageLogID:
	case LogDataAnypalPulseLogID:
	{
		R1 = C1_MEMORY_DATA;
		R2 = SP_BYTE;
		if(!cbx_SendFh310LogData(mNumberOfLogRequest, packetId, addedInfo))
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result =SEND_MESSAGE_FAIL;
			result.mC1 = R1;
			return result;
		}
		break;
	}

	default:
		break;
	}

	UpperCase((char*)addedInfo);

	addedInfoLength = strlen((char*)addedInfo);
	uint32_t cbxPacketLength = addedInfoLength + SEND_PACKET_OFFSET_LEN;
	uint32_t mainPacketLength = addedInfoLength + 3;

	s_packetSend[0] = STX_BYTE; // STX
	s_packetSend[1] = R1; // R1
	s_packetSend[2] = R2; // R2

	s_packetSend[addedInfoLength + ETX_OFFSET_POSITION] = ETX_BYTE; //ETX

	uint16_t temp = util_CompCrcCcitt(0x0000,&s_packetSend[1], (uint32_t)mainPacketLength);

	sprintf((char*)&s_packetSend[addedInfoLength + CRC_OFFSET_POSITION],"%.4X",temp);

	if(s_isPacketFormRS232)
	{
		if(cbxPacketLength > RS232_UART_SRB_SIZE)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result = BUFFER_FULL;
			result.mC1 = R1;
			return result;
		}
		else
		{
			rs232_SendPacketToCbx(s_packetSend, cbxPacketLength);
		}
	}
	else
	{
		if(cbxPacketLength > BT_UART_SRB_SIZE)
		{
			CBX_SENDPACKET_RESULT_t result;
			result.result = BUFFER_FULL;
			result.mC1 = R1;
			return result;
		}
		else
		{
			bluetooth_SendPacket(s_packetSend, cbxPacketLength);
		}
	}
	CBX_SENDPACKET_RESULT_t result;
	result.result =cbxPacketLength;
	result.mC1 = R1;
	if(addedInfoLength < 120)
	{
		DEBUG_CBX("addedInfo: \"%s\"\n",addedInfo);
	}
	else
	{
		DEBUG_CBX("addedInfo: %d\n",addedInfoLength);
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_SendPacketToCbx		                                       		*/
/*                                                                                      */
/* Details:   send packet data to CBX-02												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_BtReturnPacketId packetId				  							*/
/*                                                                                      */
/*             (O) uint32_t mNumberOfLogRequest											*/
/*                                                                                      */
/* ReturnValue : int16_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local variables   : NONE			                                                  	*/
/*?Note?                                                                              	*/
/*                                                                                      */
/****************************************************************************************/
static int16_t cbx_SendPacketToCbx(uint32_t mNumberOfLogRequest, E_BtReturnPacketId packetId)
{
	CBX_SENDPACKET_RESULT_t result = cbx_PreSendPacket(mNumberOfLogRequest, packetId);
	if(result.result == SEND_MESSAGE_FAIL)
	{
		cbx_SendErrPacketToCbx(result.mC1, ERR_PARAM_RANGE);
	}
	return result.result;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_CheckParameterFormat							               		*/
/*                                                                                      */
/* Details  Check Parameter Format														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *parameter						  							*/
/*                                                                                      */
/*             (I) uint8_t parameterLen													*/
/*                                                                                      */
/****************************************************************************************/
static bool cbx_CheckParameterFormat(uint8_t *parameter, uint8_t parameterLen)
{
	for(int i=0; i < parameterLen; i++)
	{
		if(!isxdigit((int)parameter[i]))
		{
			return false;
		}
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeDeviceIdentificationInformationCommand               		*/
/*                                                                                      */
/* Details  Analyze Device Identification Information Command							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t mC1								  							*/
/*                                                                                      */
/*             (I) uint8_t mC2															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                   			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeVersionInformationCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_VERSION_INFORMATION)
	{
		if(gs_mParameterIndex > strlen(VERSION_INFO))
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_NUM);
		}
		else if(gs_mParameterIndex < strlen(VERSION_INFO))
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_NUM);
		}
		else if(!strcmp((char*)gs_mParameter,VERSION_INFO))
		{
			DEBUG_CBX("VersionInfoID request\n");
			cbx_SendPacketToCbx(DUMMY, VersionInfoID);
		}
		else if(cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeDeviceIdentificationInformationCommand               		*/
/*                                                                                      */
/* Details  Analyze Device Identification Information Command							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t mC1								  							*/
/*                                                                                      */
/*             (I) uint8_t mC2															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                   			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeDeviceIdentificationInformationCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_VERSION_INFORMATION)
	{
		if(gs_mParameterIndex > strlen(IDENTIFICATION))
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_NUM);
		}
		else if(gs_mParameterIndex < strlen(IDENTIFICATION))
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_NUM);
		}
		else if (!strcmp((char*)gs_mParameter, IDENTIFICATION))
		{
			DEBUG_CBX("IdentificationInfoID request\n");
			cbx_SendPacketToCbx(DUMMY, IdentificationInfoID);
		}
		else if (cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_DATA);
		}
	}
	else if (mC2 == C2_UPDATING_CHECK_IDENTIFICATION_INFO)
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_NUM);
		}
		else
		{
			DEBUG_CBX("UpdatingCheckID request\n");
			cbx_SendPacketToCbx(DUMMY, UpdatingCheckID);
		}
	}
	else if (mC2 == C2_DEVICE_QUANTITY)
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else
		{
			DEBUG_CBX("DeviceQuantityID request\n");
			cbx_SendPacketToCbx(DUMMY, DeviceQuantityID);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeCumulativeOperationHoursCommand      		         		*/
/*                                                                                      */
/* Details  Analyze Cumulative	Operation Hours Command									*/
/*        																				*/
/* Arguments : (I) uint8_t mC1								  							*/
/*                                                                                      */
/*             (I) uint8_t mC2															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                   			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeCumulativeOperationHoursCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_CUMULATIVE_OPERATION_HOURS)
	{
		if (gs_mParameterIndex > strlen(PATIENTHOUR))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (gs_mParameterIndex < strlen(PATIENTHOUR))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if(!strcmp((char*)gs_mParameter,PATIENTHOUR))
		{
			DEBUG_CBX("Fh710PatientHourID request\n");
			cbx_SendPacketToCbx(DUMMY, Fh310PatientHourID);
		}
		else if (!strcmp((char*)gs_mParameter,UNITHOUR))
		{
			DEBUG_CBX("Fh710UnitHourID request\n");
			cbx_SendPacketToCbx(DUMMY, Fh310UnitHourID);
		}
		else if(!strcmp((char*)gs_mParameter,CUMULATIVE_OPERATION))
		{
			DEBUG_CBX("CumulativeOperationID request\n");
			cbx_SendPacketToCbx(DUMMY, CumulativeOperationID);
		}
		else if (cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeCalendarCommand      		         						*/
/*                                                                                      */
/* Details  Analyze Calendar Command													*/
/*        																				*/
/* Arguments : (I) uint8_t mC1								  							*/
/*                                                                                      */
/*             (I) uint8_t mC2															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                   			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeCalendarCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_CALENDAR)
	{
		if(gs_mParameterIndex != CALENDAR_PARAMETER_LEN)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if(cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			DEBUG_CBX("DATE: %s\n",gs_mParameter);
			DEBUG_CBX("CalendarSetupID request\n");
			DEBUG_CBX("Calendar set done\n");
			Real_Time_Clock clock;
			clock.year = ((HexCharToInt(gs_mParameter[0]) << 4) | HexCharToInt(gs_mParameter[1])) + 2000;
			clock.month = (HexCharToInt(gs_mParameter[2]) << 4) | HexCharToInt(gs_mParameter[3]);
			clock.mday = (HexCharToInt(gs_mParameter[4]) << 4) | HexCharToInt(gs_mParameter[5]);
			clock.hour = (HexCharToInt(gs_mParameter[6]) << 4) | HexCharToInt(gs_mParameter[7]);
			clock.min = (HexCharToInt(gs_mParameter[8]) << 4) | HexCharToInt(gs_mParameter[9]);
			clock.sec = (HexCharToInt(gs_mParameter[10]) << 4) | HexCharToInt(gs_mParameter[11]);

			if (guiInterface_RTCSettime(&clock, E_CBX_REQUEST_SET_RTC_ID))
			{
				cbx_SendPacketToCbx(DUMMY, CalendarSetupID);
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1, ERR_PARAM_RANGE);
			}
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1, ERR_PARAM_DATA);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeLogEvenCommand					      		         		*/
/*                                                                                      */
/* Details  Analyze Log Even Command													*/
/*        																				*/
/* Arguments : (I) uint8_t mC2								  							*/
/*             (I) uint8_t mC1	                                                        */
/*                                                                                      */
/* Using Global Data : (O) gs_mCurrentLogIndex						                  	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeLogEvenCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_LOG_QUANTITY)
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else
		{
			DEBUG_CBX("Fh310LogQuantityID request\n");
			cbx_SendPacketToCbx(DUMMY, Fh310LogQuantityID);
		}
	}
	else if (mC2 == C2_LOG_DATA)
	{
		if(gs_mParameterIndex > LOG_DATA_PARAMETER_LENG)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if(gs_mParameterIndex < LOG_DATA_PARAMETER_LENG)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if(cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			uint8_t mCurrentLogIndex[CURRENT_LOG_INDEX_LEN];
			gs_mCurrentLogIndex =0;
			for(int i= 0; i<LOG_DATA_PARAMETER_LENG -LOG_INDEX_POSITION; i++)
			{
				mCurrentLogIndex[i] = HexCharToInt(gs_mParameter[LOG_DATA_PARAMETER_LENG-LOG_INDEX_POSITION-1-i]);
				gs_mCurrentLogIndex = gs_mCurrentLogIndex |(mCurrentLogIndex[i]<<(4*i));
			}
			DEBUG_CBX("\n");
			uint32_t mNumberOfLogRequest = HexCharToInt(gs_mParameter[LOG_DATA_PARAMETER_LENG-1]);
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[LOG_DATA_PARAMETER_LENG-2])<<4;
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[LOG_DATA_PARAMETER_LENG-3])<<8;
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[LOG_DATA_PARAMETER_LENG-4])<<12;
			if(mNumberOfLogRequest == 0 || gs_mCurrentLogIndex == 0)
			{
				cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);

			}
			else
			{
				DEBUG_CBX("Fh310LogDataID request: %s %x %d\n",gs_mParameter, gs_mCurrentLogIndex, mNumberOfLogRequest);
				cbx_SendPacketToCbx(mNumberOfLogRequest, Fh310LogDataID);
			}
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
		}
	}
	else if (mC2 == C2_UPDATING_LOG)
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else
		{
			DEBUG_CBX("DeviceQuantityUpdatingLogID request\n");
			cbx_SendPacketToCbx(DUMMY, DeviceQuantityUpdatingLogID);
		}
	}
	else if (mC2 == C2_UPDATING_LOG_DEVICE_NO1)
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else
		{
			DEBUG_CBX("UpdatingLogDeviceNo1ID request\n");
			cbx_SendPacketToCbx(DUMMY, UpdatingLogDeviceNo1ID);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzePassCommand					      		         		*/
/*                                                                                      */
/* Details  Analyze Pass Command														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t mC2								  							*/
/*                                                                                      */
/*             (I) uint8_t mC1															*/
/*                                                                                      */
/* Using Global Data : (I) gs_mParameter                                  			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzePassCommand(uint8_t mC1, uint8_t mC2)
{
	if (mC2 == C2_PASS_COMMAND)
	{
		uint8_t PassC1, PassC2;
		uint8_t PassParameter[PASS_PARAMETER_LEN]={0};
		for (int i = 0; i < gs_mParameterIndex-3; i++)
		{
			PassParameter[i]= gs_mParameter[i+3];
		}
		PassC1 = gs_mParameter[1];
		PassC2 = gs_mParameter[2];
		if (PassC1 == C1_DEVICE_IDENTIFICATION_INFORMATION)
		{
			if (PassC2 == C2_DEVICE_QUANTITY)
			{
				if(strlen((char*)PassParameter)  > 0)
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
				}
				else
				{
					DEBUG_CBX("DeviceQuantityDeviceNo1ID request\n");
					cbx_SendPacketToCbx(DUMMY, DeviceQuantityDeviceNo1ID);
				}
			}
			else if (PassC2 == C2_VERSION_INFORMATION)
			{
				if(strlen((char*)PassParameter) != strlen(VERSION_INFO))
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
				}
				else if(!strcmp((char*)PassParameter,VERSION_INFO))
				{
					DEBUG_CBX("IdentificationInfoDeviceNo1ID request\n");
					cbx_SendPacketToCbx(DUMMY, IdentificationInfoDeviceNo1ID);
				}
				else if(cbx_CheckParameterFormat(PassParameter, strlen((char*)PassParameter)))
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
				}
				else
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
				}
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,ERR_IMPOSSIBLE_EXC);
			}
		}
		else if (PassC1 == C1_ID_NUMBER)
		{
			if (PassC2 == C2_ID_NUMBER)
			{
				if(strlen((char*)PassParameter)  > 0)
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
				}
				else
				{
					DEBUG_CBX("IdNumberDeviceNo1ID request\n");
					cbx_SendPacketToCbx(DUMMY, IdNumberDeviceNo1ID);
				}
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,ERR_IMPOSSIBLE_EXC);
			}
		}
		else if (PassC1 == C1_VERSION_INFORMATION)
		{
			if (PassC2 == C2_VERSION_INFORMATION)
			{
				if(strlen((char*)PassParameter)  > strlen(VERSION_INFO))
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
				}
				else if(strlen((char*)PassParameter) < strlen(VERSION_INFO))
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
				}
				else if(!strcmp((char*)PassParameter,VERSION_INFO))
				{
					DEBUG_CBX("VersionInfoDeviceNo1ID request\n");
					cbx_SendPacketToCbx(DUMMY, VersionInfoDeviceNo1ID);
				}
				else if(cbx_CheckParameterFormat(PassParameter, strlen((char*)PassParameter)))
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
				}
				else
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
				}
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,ERR_IMPOSSIBLE_EXC);
			}
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_IMPOSSIBLE_EXC);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeMemoryDataCommand				      		         		*/
/*                                                                                      */
/* Details  Analyze Memory Data Command													*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t mC2								  							*/
/*                                                                                      */
/*             (I) uint8_t mC1															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                  			   	*/
/*                     (O) gs_isGettingSpO2Data		                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeMemoryDataCommand(uint8_t mC1, uint8_t mC2)
{
	static bool isUpdateLogQuantityDeviceNo1 = false;
	if (mC2 == C2_LOG_QUANTITY)
	{
		if(gs_mParameterIndex > strlen((char*)LOG_DEVICE_NO1))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if(gs_mParameterIndex < strlen((char*)LOG_DEVICE_NO1))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (!strncmp((char*)gs_mParameter,LOG_DEVICE_NO1,strlen((char*)LOG_DEVICE_NO1)))
		{
			DEBUG_CBX("LogQuantityDeviceNo1ID request\n");
			cbx_SendPacketToCbx(DUMMY, LogQuantityDeviceNo1ID);
		}
		else if (!strncmp((char*)gs_mParameter,FH310_USAGE_LOG,strlen(FH310_USAGE_LOG)))
		{
			// Log Quantity Fh-310 usage log for server
			DEBUG_CBX("LogQuantityFh310UsageLogID request\n");
			cbx_SendPacketToCbx(DUMMY, LogQuantityFh310UsageLogID);
		}
		else if (!strncmp((char*)gs_mParameter,ANYPAL_PULSE_LOG,strlen((char*)ANYPAL_PULSE_LOG)))
		{
			if(gs_isGettingSpO2Data == false)
			{
				// Log Quantity Anypal
				DEBUG_CBX("LogQuantityAnypalPulseLogID request\n");
				isUpdateLogQuantityDeviceNo1 = true;
				if (xTimerStop(gs_CommunicationErrorTimer,10) != pdPASS)
				{
					DEBUG_CBX("ERR: Failed to stop gs_CommunicationErrorTimer \n");
				}
				cbx_SendPacketToCbx(DUMMY, LogQuantityAnypalPulseLogID);
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,RESPONSE_DELAY);
				if (xTimerStart(gs_CommunicationErrorTimer,10) != pdPASS)
				{
					DEBUG_CBX("ERR: Failed to start gs_CommunicationErrorTimer \n");
				}
			}
		}
		else if(cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
		}
	}
	else if (mC2 == C2_LOG_DATA)
	{
		if(gs_mParameterIndex > MEM_DATA_PARAMETER_LEN)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
			return;
		}
		else if(gs_mParameterIndex < MEM_DATA_PARAMETER_LEN)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
			return;
		}
		else if(cbx_CheckParameterFormat(gs_mParameter, gs_mParameterIndex))
		{
			uint8_t mCurrentLogIndex[CURRENT_LOG_INDEX_LEN];

			gs_mCurrentLogIndex =0;
			for(int i=0; i<8; i++)
			{
				mCurrentLogIndex[i] = HexCharToInt(gs_mParameter[i+4]);
				gs_mCurrentLogIndex = gs_mCurrentLogIndex |(mCurrentLogIndex[i]<<(4*(7-i)));
			}
			uint32_t mNumberOfLogRequest = HexCharToInt(gs_mParameter[15]);
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[14])<<4;
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[13])<<8;
			mNumberOfLogRequest |= HexCharToInt(gs_mParameter[12])<<12;
			DEBUG_CBX("%s %x, %x\n", gs_mParameter, gs_mCurrentLogIndex, mNumberOfLogRequest);
			if(gs_mCurrentLogIndex == 0 || mNumberOfLogRequest ==0 )
			{
				cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
			}
			else if (!strncmp((char*)gs_mParameter,LOG_DEVICE_NO1,strlen((char*)LOG_DEVICE_NO1)))
			{
				cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
			}
			else if (!strncmp((char*)gs_mParameter,FH310_USAGE_LOG,strlen((char*)FH310_USAGE_LOG)))
			{
				DEBUG_CBX("LogDataFh310UsageLogID request\n");
				cbx_SendPacketToCbx(mNumberOfLogRequest, LogDataFh310UsageLogID);
			}
			else if (!strncmp((char*)gs_mParameter,ANYPAL_PULSE_LOG,strlen((char*)ANYPAL_PULSE_LOG)))
			{
				if(isUpdateLogQuantityDeviceNo1 == false)
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
				}
				else if(gs_isGettingSpO2Data == true)
				{
					cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
				}
				else
				{
					DEBUG_CBX("LogDataAnypalPulseLogID request\n");
					cbx_SendPacketToCbx(mNumberOfLogRequest, LogDataAnypalPulseLogID);
				}
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,ERR_PARAM_RANGE);
			}

		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_DATA);
		}
	}
	else
	{
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzeCheckSum				                      				*/
/*                                                                                      */
/* Details:	processing when check sum pass									 			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t mC1								  							*/
/*                                                                                      */
/*             (I) uint8_t mC2															*/
/*                                                                                      */
/* Using Global Data : (I) gs_mParameter				                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
static void cbx_AnalyzeCheckSum(uint8_t mC1, uint8_t mC2)
{
	switch(mC1)
	{
	case REGISTRATION_TYPE:
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (mC2 == SP_BYTE)
		{
			/** >>#0017
				Add new reset case because the BT reset sequence specification change
			#0017 >>**/
			if(s_isPacketFormRS232)
			{
				bluetooth_Reset();
			}
			cbx_SendPacketToCbx(DUMMY, BTInfoID);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_CMD);
		}
		break;
	}

	case C1_DEVICE_IDENTIFICATION_INFORMATION:
	{
		cbx_AnalyzeDeviceIdentificationInformationCommand(mC1, mC2);
		break;
	}
	case C1_VERSION_INFORMATION:
	{
		cbx_AnalyzeVersionInformationCommand(mC1, mC2);
		break;
	}
	case C1_ID_NUMBER:
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (mC2 == C2_ID_NUMBER)
		{
			DEBUG_CBX("IdNumberID request\n");
			cbx_SendPacketToCbx(DUMMY, IdNumberID);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_CMD);
		}
		break;
	}
	case C1_CUMULATIVE_OPERATION_HOURS:
	{
		cbx_AnalyzeCumulativeOperationHoursCommand(mC1, mC2);
		break;
	}
	case C1_CALENDAR:
	{
		cbx_AnalyzeCalendarCommand(mC1, mC2);
		break;
	}
	case C1_LOG_EVEN:
	{
		cbx_AnalyzeLogEvenCommand(mC1, mC2);
		break;
	}
	case C1_REAL_DATA:
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (mC2 == C2_REAL_DATA)
		{
			DEBUG_CBX("RealDataID request\n");
			cbx_SendPacketToCbx(DUMMY, RealDataID);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_CMD);
		}
		break;
	}
	case C1_PASS_COMMAND://Pass command (Common command)
	{
		cbx_AnalyzePassCommand(mC1, mC2);
		break;
	}
	case C1_OPERATIONAL_STATUS://Operational status
	{
		if(gs_mParameterIndex > 0)
		{
			cbx_SendErrPacketToCbx(mC1,ERR_PARAM_NUM);
		}
		else if (mC2 == C2_OPERATIONAL_STATUS)
		{
			DEBUG_CBX("Fh710OperationStatusID request\n");
			cbx_SendPacketToCbx(DUMMY, Fh310OperationStatusID);
		}
		else
		{
			cbx_SendErrPacketToCbx(mC1,ERR_CMD);
		}
		break;
	}
	case C1_MEMORY_DATA:
	{
		cbx_AnalyzeMemoryDataCommand(mC1, mC2);
		break;
	}
	default:
		cbx_SendErrPacketToCbx(mC1,ERR_CMD);
		break;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_AnalyzePacketFromCbx					      		         		*/
/*                                                                                      */
/* Details  Analyze Packet From Cbx														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t ch								  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) gs_mParameter                                  			   	*/
/*                     (O) gs_mParameterIndex		                                   	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_AnalyzePacketFromCbx(uint8_t ch)
{
	// C1 byte data in packet receive from CBX-02
	static uint8_t mC1;

	// C2 byte data in packet receive from CBX-02
	static uint8_t mC2;

	static uint8_t s_CBXPacketIn[PACKETIN_LENGTH];
	static uint16_t mpPacketIndex = 0;
	static E_CBX02CurrentState NextState = B_Wait_STX;

	switch(NextState)
	{
	case B_Wait_STX:
	{
		if(ch == STX_BYTE)
		{
			NextState = B_Wait_CtrlCode;
			memset(s_CBXPacketIn,0,PACKETIN_LENGTH);
		}
		else
		{
			NextState = B_Wait_STX;
		}
		break;
	}
	case B_Wait_CtrlCode:
	{
		mpPacketIndex = 0;
		s_CBXPacketIn[mpPacketIndex] = ch;
		if (ch == DLE_BYTE)
		{
			NextState = B_Wait_C1;
			mpPacketIndex ++;
		}
		else if ((ch == REGISTRATION_TYPE) || (ch == CTRL_CODE_WAIT_C2))
		{
			mC1 = ch;
			NextState = B_Wait_C2;
			mpPacketIndex ++;
		}
		else
		{
			NextState = B_Wait_STX;
			cbx_SendErrPacketToCbx(ch, ERR_CMD);
		}
		break;
	}
	case B_Wait_C1:
	{
		if ((ch == REGISTRATION_TYPE) || (ch == CTRL_CODE_WAIT_C2))
		{
			NextState = B_Wait_STX;
			cbx_SendErrPacketToCbx(ch, ERR_CMD);
		}
		else
		{
			mC1 = ch;
			s_CBXPacketIn[mpPacketIndex] = ch;
			NextState = B_Wait_C2;
			mpPacketIndex ++;
		}
		break;
	}
	case B_Wait_C2:
	{
		mC2 = ch;
		s_CBXPacketIn[mpPacketIndex] = ch;
		NextState = B_Wait_Parameter;
		gs_mParameterIndex =0;
		memset(gs_mParameter,0,PARAMETER_LENGTH);
		mpPacketIndex ++;
		break;
	}
	case B_Wait_Parameter:
	{
		s_CBXPacketIn[mpPacketIndex] = ch;
		mpPacketIndex ++;
		if (ETX_BYTE != ch)
		{
			gs_mParameter[gs_mParameterIndex]=ch;
			gs_mParameterIndex++;
			if(gs_mParameterIndex > PARAMETER_LENGTH)
			{
				NextState = B_Wait_STX;
				cbx_SendErrPacketToCbx(mC1,ERR_PARAM_LENTH);
				return;
			}
		}
		else
		{
			NextState = B_Wait_CRC;
			DEBUG_CBX("\n\n\n\n");
		}
		break;
	}
	case B_Wait_CRC:
	{
		static int CRCLen =0;

		static uint8_t crc_packet[CRC_SIZE] ={0};
		crc_packet[CRCLen]=HexCharToInt(ch);
		CRCLen++;
		//DEBUG_CBX("%c ",ch);

		if (CRC_LENGTH == CRCLen)
		{
			DEBUG_CBX("\n");

			NextState = B_Wait_STX;

			s_CBXPacketIn[mpPacketIndex] = (crc_packet[0] << 4) | crc_packet[1];

			mpPacketIndex ++;
			s_CBXPacketIn[mpPacketIndex] = (crc_packet[2] << 4) | crc_packet[3];
			//
			mpPacketIndex ++;

			uint16_t CRCode = util_CompCrcCcitt(0x0000, s_CBXPacketIn, mpPacketIndex);
			DEBUG_CBX("CRCode: 0x%x\n",CRCode);
			//update the information
			if(CRCode == 0x0000 || mC1 == C1_CALENDAR ||
					(mC1 == C1_LOG_EVEN && mC2 == C2_LOG_DATA))//check CRC was good
			{
				cbx_AnalyzeCheckSum(mC1, mC2);
			}
			else
			{
				cbx_SendErrPacketToCbx(mC1,ERR_CRC);
			}
			CRCLen = 0;
		}
		break;
	}
	default:
		break;
	}//end switch
	return;
}//end func

/****************************************************************************************/
/*                                                                                      */
/* Function name: cbx_ReceivePacketFromCbx					      		         		*/
/*                                                                                      */
/* Details  Receive Packet From Cbx														*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t packketLen						  							*/
/*                                                                                      */
/*             (O) uint8_t* ch															*/
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
void cbx_ReceivePacketFromCbx(uint8_t* ch, uint8_t packketLen)
{
	for (int i =0; i <packketLen; i++)
	{
		cbx_AnalyzePacketFromCbx(ch[i]);
	}
}
