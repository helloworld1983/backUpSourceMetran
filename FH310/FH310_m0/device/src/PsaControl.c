/********************************************************************/
/*                                                                  */
/* File Name    : PsaControl.c		                                */
/*                                                                  */
/* General       : This module contain function for init and handle
 * 				   psa device based on psa parameter				*/
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            Jun 30, 2016      Quyen Ngo(MV)  		new file        */
/* #0001	  2017/09/01		Quyen Ngo(MV)		modify			*/
/*		-remove assert												*/
/*		-remove psa_GetPsaState	function							*/
/*		-Does not check pressure to decide go to PSA BASIC or 		*/
/*		DECOMRESS sequence, after 3S from PSA start, always goto	*/
/*		basic sequence and start check for switch to DECOMPRESS 	*/
/*		1 cycle later (modify occure on psa_CheckEventFag funtion)	*/
/* #0002      2017/09/08		Quyen Ngo(MV)		modify			*/
/*		-change call to new latch valve request function			*/
/* #0003	  2017/09/15		Quyen Ngo			modify			*/
/*		-change psa_SetMode function to correct the way of using	*/
/*		 argument(Bug 1848)											*/
/* #0004	  2017/11/02		Quyen Ngo			modify			*/
/*		Resend Request PSA param msg when IPC queue is full			*/
/* #0005	  2017/12/07		Quyen Ngo			modify			*/
/*		Clear s_time2sCount	when start new psa cycle (bug 2053)		*/
/* #0006	  2017/12/07		Quyen Ngo			modify			*/
/*		Allow change compressor RPM when current RPM not 0 and 100  */
/*      (bug 2054)													*/
/********************************************************************/

#include <string.h>

#include "PsaControl.h"
#include "TaskCommon.h"
#include "ValveControl.h"
#include "timers.h"
#include "PwmController.h"
#include "FlowController.h"
#include "DualCoreCommon.h"
#include "Compressor.h"
#include "PsaSelectMgr.h"
#include "LatchValve.h"
#include "AlarmLed.h"
#include "FilterCleaningModule.h"
#include "IpcMsg.h"
#include "IpcTask.h"
#include "EncoderInterface.h"

#ifdef DEBUG
#define Q_DEBUG
#define DEBUG_PSA_EN
#endif

#ifdef DEBUG_PSA_EN
#define DEBUG_PSA(...) printf(__VA_ARGS__)
#else
#define DEBUG_PSA(...)
#endif


#define PSA_DECOMPRESS_THRESHOLD 20
#define PSA_DECOMPRESS_EXIT_THRESHOLD -10
#define SCALE_TO_MS 100 /* psa_time_recevive = PSA GUI time setting (S) *10 => psa_time_recevive * 100 = MS */
#define TIME_2S_COUNT 2000/DEVICE_TASK_DELAY_TIME /*device task call psa_check_pressure every 50ms => 50ms*40=2S*/
#define SOL1_ON_OFFSET_ID 1
#define SOL2_ON_OFFSET_ID 2
#define SOL3_ON_OFFSET_ID 3
#define SOL4_ON_OFFSET_ID 4
#define SOL5_ON_OFFSETA_ID 5
#define SOL1_OFF_OFFSET_ID 6
#define SOL2_OFF_OFFSET_ID 7
#define SOL3_OFF_OFFSET_ID 8
#define SOL4_OFF_OFFSET_ID 9
#define SOL5_ON_OFFSETB_ID 10
#define SOL5_OPENA_ID 11
#define SOL5_OPENB_ID 12
#define MAX_PSA_ID 13
#define PSA_START_3S_ID 14
#define PSA_START_23S_ID 15
#define PSA_END_10S_FIRST_ID 16
#define PSA_END_20S_LAST_ID 17
#define SOL_END_OPEN_A_ID 18
#define SOL_END_OPEN_B_ID 19
#define PSA_START_20S_ID 20

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

#define PSA_3S_TICKS_TIME 	3000
#define PSA_23S_TICKS_TIME 	23000
#define PSA_10S_TICKS_TIME 	10000
#define PSA_20S_TICKS_TIME 	20000

/* handles to the created timers. */
// psa valve control timer
static TimerHandle_t gs_sol1OnOffsetTimer;
static TimerHandle_t gs_sol1OffOffsetTimer;
static TimerHandle_t gs_sol2OnOffsetTimer;
static TimerHandle_t gs_sol2OffOffsetTimer;
static TimerHandle_t gs_sol3OnOffsetTimer;
static TimerHandle_t gs_sol3OffOffsetTimer;
static TimerHandle_t gs_sol4OnOffsetTimer;
static TimerHandle_t gs_sol4OffOffsetTimer;
static TimerHandle_t gs_sol5OnOffsetATimer;
static TimerHandle_t gs_sol5OnOffsetBTimer;
static TimerHandle_t gs_sol5OpenATimer;
static TimerHandle_t gs_sol5OpenBTimer;
static TimerHandle_t gs_maxPsaTimer;
// psa start sequence timer
static TimerHandle_t gs_psaStart3STimer;
static TimerHandle_t gs_psaStart23STimer;
static TimerHandle_t gs_psaStart20STimer;
// psa end sequence timer
static TimerHandle_t gs_psaEnd10SFirstTimer;
static TimerHandle_t gs_psaEnd20SLastTimer;
static TimerHandle_t gs_solEndOpenATimer;

// private psa parameter items
static uint16_t gs_sol1OnOffsetTime = 1000;
static uint16_t gs_sol1OffOffsetTime = 1000;
static uint16_t gs_sol2OnOffsetTime = 1000;
static uint16_t gs_sol2OffOffsetTime = 1000;
static uint16_t gs_sol3OnOffsetTime = 1000;
static uint16_t gs_sol3OffOffsetTime = 1000;
static uint16_t gs_sol4OnOffsetTime = 1000;
static uint16_t gs_sol4OffOffsetTime = 1000;
static uint16_t gs_sol5OnOffsetATime = 100;
static uint16_t gs_sol5OnOffsetBTime = 100;
static uint16_t gs_sol5OpenATime = 1000;
static uint16_t gs_sol5OpenBTime = 1000;
static uint16_t gs_solEndOpenATime = 2000;
static uint16_t gs_maxPsaTime = 30000;
static uint16_t gs_fcOutputVal = 300;
static uint16_t gs_coolingFanDuty = 100;
static uint16_t gs_compressorRpm = 2600;
static uint16_t gs_psaMaxPressure = 30;

static IPC_MSG_DATA_t gs_ipcRequestParamData;

static float gs_pTankCurrentPressureVal = 0.0;


static bool gs_sol1OnOffsetTimerFlag = false;
static bool gs_sol1OffOffsetTimerFlag = false;
static bool gs_sol2OnOffsetTimerFlag = false;
static bool gs_sol2OffOffsetTimerFlag = false;
static bool gs_sol3OnOffsetTimerFlag = false;
static bool gs_sol3OffOffsetTimerFlag = false;
static bool gs_sol4OnOffsetTimerFlag = false;
static bool gs_sol4OffOffsetTimerFlag = false;
static bool gs_sol5OnOffsetATimerFlag = false;
static bool gs_sol5OnOffsetBTimerFlag = false;
static bool gs_sol5OpenATimerFlag = false;
static bool gs_sol5OpenBTimerFlag = false;
static bool gs_solEndOpenATimerFlag = false;
static bool gs_maxPsaTimerFlag = false;
static bool gs_psaStart3STimerFlag = false;
static bool gs_psaStart23STimerFlag = false;
static bool gs_psaStart20STimerFlag = false;
static bool gs_psaEnd10SFirstTimerFlag = false;
static bool gs_psaEnd20SLastTimerFlag = false;

static bool gs_sol1OnOffsetTimerUpdate = false;
static bool gs_sol1OffOffsetTimerUpdate = false;
static bool gs_sol2OnOffsetTimerUpdate = false;
static bool gs_sol2OffOffsetTimerUpdate = false;
static bool gs_sol3OnOffsetTimerUpdate = false;
static bool gs_sol3OffOffsetTimerUpdate = false;
static bool gs_sol4OnOffsetTimerUpdate = false;
static bool gs_sol4OffOffsetTimerUpdate = false;
static bool gs_sol5OnOffsetATimerUpdate = false;
static bool gs_sol5OnOffsetBTimerUpdate = false;
static bool gs_sol5OpenATimerUpdate = false;
static bool gs_sol5OpenBTimerUpdate = false;
static bool gs_solEndOpenATimerUpdate = false;
static bool gs_maxPsaTimerUpdate = false;

static E_PsaPhase gs_psaCurrentPhase = PHASE_B;
static E_PsaSequence gs_psaCurentSeq = PSA_STANDBY;
static E_PsaSate s_psaState = PSA_STANDBY_STATE; //just use for timing, not involve with valve control
static E_PsaMode s_psaMode = PSA_END_MODE;

static uint16_t s_psaCycleTimeCnt = 0;
static uint16_t s_psaCycleTime = 0;

static bool s_isProceesCompressorRestartCheck = false;
static bool s_isPressureOverPcompStlow   = false;
static uint16_t s_compRestart30sCnt = 0;

static E_ValStatus s_V1BeforeDepressStat = VAL_OFF;
static E_ValStatus s_V2BeforeDepressStat = VAL_OFF;
static E_ValStatus s_V3BeforeDepressStat = VAL_OFF;
static E_ValStatus s_V4BeforeDepressStat = VAL_OFF;

//For handle 2s condition before previous PSA trigger
static uint16_t s_time2sCount = 0;

// Check event flag for control val
static void psa_CheckEventFlag(void) ;
// Check psa pressure
static void psa_CheckPressure(float productTankPress);

static void psa_GotoDecompressSeq(void);

/* Define a callback function that will be used by multiple timer
 instances.  The callback function does nothing but count the number
 of times the associated timer expires, and stop the timer once the
 timer has expired 10 times.  The count is saved as the ID of the
 timer. */
static void psa_TimerCallback( TimerHandle_t xTimer )
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	// Psa start sequence
	case PSA_START_3S_ID:
		if(gs_psaCurentSeq == PSA_START)
		{
			gs_psaStart3STimerFlag = true;
		}
		break;
	case PSA_START_23S_ID:
		if((gs_psaCurentSeq == PSA_BASIC) | (gs_psaCurentSeq == PSA_DECOMPRESS))
		{
			gs_psaStart23STimerFlag = true;
		}
		break;
	case PSA_START_20S_ID:
		if((gs_psaCurentSeq == PSA_BASIC) | (gs_psaCurentSeq == PSA_DECOMPRESS))
		{
			gs_psaStart20STimerFlag = true;
		}
		break;
		// Psa end sequence
	case PSA_END_10S_FIRST_ID:
		if(gs_psaCurentSeq == PSA_END)
		{
			gs_psaEnd10SFirstTimerFlag = true;
		}
		break;
	case PSA_END_20S_LAST_ID:
		if(gs_psaCurentSeq == PSA_END)
		{
			gs_psaEnd20SLastTimerFlag = true;
		}
		break;
	case SOL1_ON_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol1OnOffsetTimerFlag = true;
		}
		break;
	case SOL2_ON_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol2OnOffsetTimerFlag = true;
		}
		break;
	case SOL3_ON_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol3OnOffsetTimerFlag = true;
		}
		break;
	case SOL4_ON_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol4OnOffsetTimerFlag = true;
		}
		break;
	case SOL1_OFF_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol1OffOffsetTimerFlag = true;
		}
		break;
	case SOL2_OFF_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol2OffOffsetTimerFlag = true;
		}
		break;
	case SOL3_OFF_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol3OffOffsetTimerFlag = true;
		}
		break;
	case SOL4_OFF_OFFSET_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol4OffOffsetTimerFlag = true;
		}
		break;
	case SOL5_ON_OFFSETA_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol5OnOffsetATimerFlag = true;
		}
		break;
	case SOL5_ON_OFFSETB_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol5OnOffsetBTimerFlag = true;
		}
		break;
	case SOL5_OPENA_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol5OpenATimerFlag = true;
		}
		break;
	case SOL5_OPENB_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_sol5OpenBTimerFlag = true;
		}
		break;
	case MAX_PSA_ID:
		if(gs_psaCurentSeq == PSA_BASIC)
		{
			gs_maxPsaTimerFlag = true;
		}
		break;
	case SOL_END_OPEN_A_ID:
		if(gs_psaCurentSeq == PSA_END)
		{
			gs_solEndOpenATimerFlag = true;
		}
		break;
	default:
		DEBUGOUT("Psa invalid timer id\n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_ProcessPendingIpcMsg												*/
/*                                                                                      */
/* Details:  Resend request psa paramter messsage if previous send is failed			*/
/*																						*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t gs_fcReSendCnt 					         		    */
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void psa_ProcessPendingIpcMsg(void)
{
	if(gs_ipcRequestParamData.data_id == eRequestPsaParamAtStart)
	{
		if (ipc_PushMsg(IPCEX_ID_GUI, gs_ipcRequestParamData) != QUEUE_INSERT) {
			DEBUGOUT("M0 Push data Err: %d\n",gs_ipcRequestParamData.data_id);
		}
		else
		{
			gs_ipcRequestParamData.data_id = eNoDeviceEventId;
			gs_ipcRequestParamData.data_value = 0x00;
		}
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: start_psa_val_timer
//
//    Processing: This function is used only for start psa val control timer on psa parameter table
//					include sol1,2,3,4,5 time on-off offset and max psa timer
//
//    Input Parameters: TimerHandle_t xTimer, uint16_t time,bool* update
//
//    Output Parameters: None
//
//    Return Values: bool status
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
static bool psa_StartValveTimer(TimerHandle_t xTimer,
		uint16_t time,
		bool* update)
{
	bool status = true;
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	if(*update == true) {
		*update = false;
		switch (timerId) {
		case SOL1_ON_OFFSET_ID:
			if(gs_sol1OnOffsetTime == 0)
			{
				gs_sol1OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL2_ON_OFFSET_ID:
			if(gs_sol2OnOffsetTime == 0)
			{
				gs_sol2OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL3_ON_OFFSET_ID:
			if(gs_sol3OnOffsetTime == 0)
			{
				gs_sol3OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL4_ON_OFFSET_ID:
			if(gs_sol4OnOffsetTime == 0)
			{
				gs_sol4OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL1_OFF_OFFSET_ID:
			if(gs_sol1OffOffsetTime == 0)
			{
				gs_sol1OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL2_OFF_OFFSET_ID:
			if(gs_sol2OffOffsetTime == 0)
			{
				gs_sol2OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL3_OFF_OFFSET_ID:
			if(gs_sol3OffOffsetTime == 0)
			{
				gs_sol3OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL4_OFF_OFFSET_ID:
			if(gs_sol4OffOffsetTime == 0)
			{
				gs_sol4OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_ON_OFFSETA_ID:
			if(gs_sol5OnOffsetATime == 0)
			{
				gs_sol5OnOffsetATimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_ON_OFFSETB_ID:
			if(gs_sol5OnOffsetBTime == 0)
			{
				gs_sol5OnOffsetBTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_OPENA_ID:
			//			DEBUG_PSA("S5 oA c: %d \n",time);
			//Stop and clear sol5 open B timer flag
			if(xTimerStop(gs_sol5OpenBTimer,100) != pdPASS)
			{
				return false;
			}
			else
			{
				gs_sol5OpenBTimerFlag = false;
			}
			if(gs_sol5OpenATime == 0)
			{
				gs_sol5OpenATimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_OPENB_ID:
			//			DEBUG_PSA("S5 oB c: %d \n",time);
			//Stop and clear sol5 open A timer flag
			if(xTimerStop(gs_sol5OpenATimer,100) != pdPASS)
			{
				return false;
			}
			else
			{
				gs_sol5OpenATimerFlag = false;
			}
			if(gs_sol5OpenBTime == 0)
			{
				gs_sol5OpenBTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case MAX_PSA_ID:
			if(gs_maxPsaTime == 0)
			{
				gs_maxPsaTimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL_END_OPEN_A_ID:
			if(gs_solEndOpenATime == 0)
			{
				gs_solEndOpenATimerFlag = true;
			}
			else
			{
				if(xTimerChangePeriod(xTimer, pdMS_TO_TICKS(time),100) != pdPASS) {
					status = false;
				}
			}
			break;
		default:
			DEBUGOUT("Invalid timer id\n");
			break;
		}
	}
	else {
		switch (timerId) {
		case SOL1_ON_OFFSET_ID:
			if(gs_sol1OnOffsetTime == 0)
			{
				gs_sol1OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL2_ON_OFFSET_ID:
			if(gs_sol2OnOffsetTime == 0)
			{
				gs_sol2OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL3_ON_OFFSET_ID:
			if(gs_sol3OnOffsetTime == 0)
			{
				gs_sol3OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL4_ON_OFFSET_ID:
			if(gs_sol4OnOffsetTime == 0)
			{
				gs_sol4OnOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL1_OFF_OFFSET_ID:
			if(gs_sol1OffOffsetTime == 0)
			{
				gs_sol1OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL2_OFF_OFFSET_ID:
			if(gs_sol2OffOffsetTime == 0)
			{
				gs_sol2OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL3_OFF_OFFSET_ID:
			if(gs_sol3OffOffsetTime == 0)
			{
				gs_sol3OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL4_OFF_OFFSET_ID:
			if(gs_sol4OffOffsetTime == 0)
			{
				gs_sol4OffOffsetTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_ON_OFFSETA_ID:
			if(gs_sol5OnOffsetATime == 0)
			{
				gs_sol5OnOffsetATimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_ON_OFFSETB_ID:
			if(gs_sol5OnOffsetBTime == 0)
			{
				gs_sol5OnOffsetBTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_OPENA_ID:
			//			DEBUG_PSA("S5 oA start: %d \n",time);
			//Stop and clear sol5 open B timer flag
			if(xTimerStop(gs_sol5OpenBTimer,100) != pdPASS)
			{
				return false; //Stop open B timer before start open A timer
			}
			else
			{
				gs_sol5OpenBTimerFlag = false;
			}
			if(gs_sol5OpenATime == 0)
			{
				gs_sol5OpenATimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL5_OPENB_ID:
			//			DEBUG_PSA("S5 oB start: %d \n",time);
			//Stop and clear sol5 open A timer flag
			if(xTimerStop(gs_sol5OpenATimer,100) != pdPASS)
			{
				return false; //Cannot Stop open A timer before start open B timer
			}
			else
			{
				gs_sol5OpenATimerFlag = false;
			}
			if(gs_sol5OpenBTime == 0)
			{
				gs_sol5OpenBTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case MAX_PSA_ID:
			if(gs_maxPsaTime == 0)
			{
				gs_maxPsaTimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		case SOL_END_OPEN_A_ID:
			if(gs_solEndOpenATime == 0)
			{
				gs_solEndOpenATimerFlag = true;
			}
			else
			{
				if(xTimerStart(xTimer,100) != pdPASS) {
					status = false;
				}
			}
			break;
		default:
			DEBUGOUT("Invalid timer id\n");
			break;
		}
	}
	return status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psa_CheckForRestartCompressor
//
//    Detail: Check productank pressure for restart compressor
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
static void psa_CheckForRestartCompressor(void)
{
	if (s_isProceesCompressorRestartCheck == true)
	{
		if(gs_pTankCurrentPressureVal > P_COMP_STLOW)
		{
			s_isPressureOverPcompStlow = true;
		}
		s_compRestart30sCnt++;
		if(s_compRestart30sCnt >= PSA_COMP_RESTART_30S_TIME)
		{
			s_isProceesCompressorRestartCheck = false;
			s_compRestart30sCnt = 0;
			if(s_isPressureOverPcompStlow == false)
			{
				compressor_Restart();
				s_psaState = PSA_RESTART_STATE;
			}
			else
			{
				s_psaState = PSA_BASIC_STATE;
				s_psaMode = PSA_NORMAL_MODE;
				DEBUGOUT("psa normal pressure \n");
			}
		}
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psa_RequestParameterAtStart
//
//    Detail: Send reqest to GUI to request Psa parameter at psa start sequence
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
static void psa_SendRequestParameterAtStart(E_PsaSelectName psaType)
{
	gs_ipcRequestParamData.data_id = eRequestPsaParamAtStart;
	gs_ipcRequestParamData.data_value = (uint16_t)psaType;
	memset(&gs_ipcRequestParamData.stAlarmStatPacket, 0, sizeof(gs_ipcRequestParamData.stAlarmStatPacket));
	memset(&gs_ipcRequestParamData.stPsaParamPacket, 0, sizeof(gs_ipcRequestParamData.stPsaParamPacket));
	memset(&gs_ipcRequestParamData.stPsaMonitorPacket, 0, sizeof(gs_ipcRequestParamData.stPsaMonitorPacket));
	memset(&gs_ipcRequestParamData.version, 0, sizeof(gs_ipcRequestParamData.version));

	if (ipc_PushMsg(IPCEX_ID_GUI, gs_ipcRequestParamData) != QUEUE_INSERT) {
		DEBUGOUT("M0 Push data Err: %d\n",gs_ipcRequestParamData.data_id);
	}
	else
	{
		gs_ipcRequestParamData.data_id = eNoDeviceEventId;
		gs_ipcRequestParamData.data_value = 0x00;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  	psa_StartPsaCycle															*/
/*                                                                                      */
/* Details：  start new psa cycle by open/close valve and start SollXXXTimer depend on
 * 			curren psa phase				        								    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 	gs_psaCurrentPhase, gs_solXXXTimer, gs_solXXXTime,
 * 						gs_solXXXTimerUpdate							              	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void psa_StartPsaCycle(void) {
	//For handle 2s condition before previous PSA trigger
	s_time2sCount = 0;
	s_psaCycleTime = DEVICE_TASK_DELAY_TIME * s_psaCycleTimeCnt;
	s_psaCycleTimeCnt = 0;
	if (gs_psaCurrentPhase == PHASE_A) {
		DEBUG_PSA("P_A");
		gs_psaCurrentPhase = PHASE_B;
		valve_closeVal1();
		valve_closeVal4();
		valve_openVal2();
		valve_openVal3();
		if(psa_StartValveTimer(gs_sol5OnOffsetATimer, gs_sol5OnOffsetATime, &gs_sol5OnOffsetATimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol5 on offset A timer \n");
		}
		if(psa_StartValveTimer(gs_sol1OnOffsetTimer, gs_sol1OnOffsetTime, &gs_sol1OnOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol1 on offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol2OffOffsetTimer, gs_sol2OffOffsetTime, &gs_sol2OffOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol2 off offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol3OffOffsetTimer, gs_sol3OffOffsetTime, &gs_sol3OffOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol3 off offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol4OnOffsetTimer, gs_sol4OnOffsetTime, &gs_sol4OnOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol4 on offset timer \n");
		}
		if(psa_StartValveTimer(gs_maxPsaTimer, gs_maxPsaTime, &gs_maxPsaTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Max PSA timer \n");
		}
	}
	else if (gs_psaCurrentPhase == PHASE_B) {
		DEBUG_PSA("P_B");
		gs_psaCurrentPhase = PHASE_A;
		valve_openVal1();
		valve_openVal4();
		valve_closeVal2();
		valve_closeVal3();
		if(psa_StartValveTimer(gs_sol5OnOffsetBTimer, gs_sol5OnOffsetBTime, &gs_sol5OnOffsetBTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol5 on offset B timer \n");
		}
		if(psa_StartValveTimer(gs_sol1OffOffsetTimer, gs_sol1OffOffsetTime, &gs_sol1OffOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol1 off offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol2OnOffsetTimer, gs_sol2OnOffsetTime, &gs_sol2OnOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol2 on offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol3OnOffsetTimer, gs_sol3OnOffsetTime, &gs_sol3OnOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol3 on offset timer \n");
		}
		if(psa_StartValveTimer(gs_sol4OffOffsetTimer, gs_sol4OffOffsetTime, &gs_sol4OffOffsetTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Sol4 off offset timer \n");
		}
		if(psa_StartValveTimer(gs_maxPsaTimer, gs_maxPsaTime, &gs_maxPsaTimerUpdate) != true) {
			DEBUGOUT("ERR: Failed to start Max PSA timer \n");
		}
	}
	return;
}

//*****************************************************************************/
//    Operation Name: psa_UpdateParameter()
//
//    Processing:  update new parameters value
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Global data use: gs_solXXXTime, gs_solXXXTimerUpdate, gs_fcOutputVal
//					   gs_coolingFanDuty, gs_compressorRpm,  gs_psaMaxPressure
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void psa_UpdateParameter(PSA_PARAM_PACKET_t stPsaParam) {
	// TODO: scale value
#ifdef Q_DEBUG
	DEBUGOUT("update psa parameter \n");
#endif
	if (gs_sol1OnOffsetTime != stPsaParam.solenoid1OnOffsetTime * SCALE_TO_MS){
		gs_sol1OnOffsetTime = stPsaParam.solenoid1OnOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol1_on_offset_time: %d \n",gs_sol1OnOffsetTime);
#endif
		gs_sol1OnOffsetTimerUpdate = true;
	}
	if (gs_sol1OffOffsetTime != stPsaParam.solenoid1OffOffsetTime * SCALE_TO_MS){
		gs_sol1OffOffsetTime = stPsaParam.solenoid1OffOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol1_off_offset_time: %d \n",gs_sol1OffOffsetTime);
#endif
		gs_sol1OffOffsetTimerUpdate = true;
	}
	if (gs_sol2OnOffsetTime != stPsaParam.solenoid2OnOffsetTime * SCALE_TO_MS){
		gs_sol2OnOffsetTime = stPsaParam.solenoid2OnOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol2_on_offset_time: %d \n",gs_sol2OnOffsetTime);
#endif
		gs_sol2OnOffsetTimerUpdate = true;
	}
	if (gs_sol2OffOffsetTime != stPsaParam.solenoid2OffOffsetTime * SCALE_TO_MS){
		gs_sol2OffOffsetTime = stPsaParam.solenoid2OffOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol2_off_offset_time: %d \n",gs_sol2OffOffsetTime);
#endif
		gs_sol2OffOffsetTimerUpdate = true;
	}
	if (gs_sol3OnOffsetTime != stPsaParam.solenoid3OnOffsetTime * SCALE_TO_MS){
		gs_sol3OnOffsetTime = stPsaParam.solenoid3OnOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol3_on_offset_time: %d \n",gs_sol3OnOffsetTime);
#endif
		gs_sol3OnOffsetTimerUpdate = true;
	}
	if (gs_sol3OffOffsetTime != stPsaParam.solenoid3OffOffsetTime * SCALE_TO_MS){
		gs_sol3OffOffsetTime = stPsaParam.solenoid3OffOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol3_off_offset_time: %d \n",gs_sol3OffOffsetTime);
#endif
		gs_sol3OffOffsetTimerUpdate = true;
	}
	if (gs_sol4OnOffsetTime != stPsaParam.solenoid4OnOffsetTime * SCALE_TO_MS){
		gs_sol4OnOffsetTime = stPsaParam.solenoid4OnOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol4_on_offset_time: %d \n",gs_sol4OnOffsetTime);
#endif
		gs_sol4OnOffsetTimerUpdate = true;
	}
	if (gs_sol4OffOffsetTime != stPsaParam.solenoid4OffOffsetTime * SCALE_TO_MS){
		gs_sol4OffOffsetTime = stPsaParam.solenoid4OffOffsetTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol4_off_offset_time: %d \n",gs_sol4OffOffsetTime);
#endif
		gs_sol4OffOffsetTimerUpdate = true;
	}
	if (gs_sol5OnOffsetATime != stPsaParam.solenoid5OnOffsetTimeA * SCALE_TO_MS){
		gs_sol5OnOffsetATime = stPsaParam.solenoid5OnOffsetTimeA * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_on_offsetA_time: %d \n",gs_sol5OnOffsetATime);
#endif
		gs_sol5OnOffsetATimerUpdate = true;
	}
	if (gs_sol5OnOffsetBTime != stPsaParam.solenoid5OnOffsetTimeB * SCALE_TO_MS){
		gs_sol5OnOffsetBTime = stPsaParam.solenoid5OnOffsetTimeB * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_on_offsetB_time: %d \n",gs_sol5OnOffsetBTime);
#endif
		gs_sol5OnOffsetBTimerUpdate = true;
	}
	if (gs_sol5OpenATime != stPsaParam.solenoid5OpenTimeA * SCALE_TO_MS){
		gs_sol5OpenATime = stPsaParam.solenoid5OpenTimeA * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_openA_time: %d \n",gs_sol5OpenATime);
#endif
		gs_sol5OpenATimerUpdate = true;
	}
	if (gs_sol5OpenBTime != stPsaParam.solenoid5OpenTimeB * SCALE_TO_MS){
		gs_sol5OpenBTime = stPsaParam.solenoid5OpenTimeB * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_openB_time: %d \n",gs_sol5OpenBTime);
#endif
		gs_sol5OpenBTimerUpdate = true;
	}
	if (gs_maxPsaTime != stPsaParam.MaxPsa * 1000){ /*Max PSA time is not *10 in GUI setting */
		gs_maxPsaTime = stPsaParam.MaxPsa * 1000;
#ifdef Q_DEBUG
		DEBUGOUT("Max_psa_time: %d \n",gs_maxPsaTime);
#endif
		gs_maxPsaTimerUpdate = true;
	}

	if (gs_solEndOpenATime != stPsaParam.solenoidEndOpenTime * SCALE_TO_MS){
		gs_solEndOpenATime = stPsaParam.solenoidEndOpenTime * SCALE_TO_MS;
#ifdef Q_DEBUG
		DEBUGOUT("gs_solEndOpenATime: %d \n",gs_solEndOpenATime);
#endif
		gs_solEndOpenATimerUpdate = true;
	}
	gs_fcOutputVal = stPsaParam.FCoutput;
	gs_coolingFanDuty = stPsaParam.coolingFan;
	gs_compressorRpm = stPsaParam.compressor;
	gs_psaMaxPressure = stPsaParam.psaPressure;
	compressor_UpdatePsaParamRpm(gs_compressorRpm);
#ifdef Q_DEBUG
	DEBUGOUT("PSA_max_pressure: %d \n",gs_psaMaxPressure);
#endif
	return;
}

//*****************************************************************************/
//    Operation Name: psa_ActivePsaParameterAtStart()
//
//    Processing:  active psa parameter at start
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Global variable use: gs_coolingFanDuty, gs_fcOutputVal, gs_fcOutputVal
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void psa_ActivePsaParameterAtStart(PSA_PARAM_PACKET_t stPsaParam) {
	psa_UpdateParameter(stPsaParam);
#ifdef Q_DEBUG
	DEBUGOUT("active_psa_parameter_at_start \n");
#endif
	//TODO: Control psa device
	//Compressor: 0 rpm
	compressor_ReqestSetRpm(0);
	//Cooling fan: max speed
	coolingFan_Setduty(100);
	//FCOutput: setting_val
	flowCtrl_RequestSetFlow(gs_fcOutputVal);
	return;
}

//*****************************************************************************/
//    Operation Name: psa_ActiveNewPsaParameter()
//
//    Processing:  active new psa parameter. change device operation immediately
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Global variable use: gs_coolingFanDuty, gs_fcOutputVal, gs_fcOutputVal
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void psa_ActiveNewPsaParameter(PSA_PARAM_PACKET_t stPsaParam) {
#ifdef Q_DEBUG
	DEBUGOUT("active psa parameter \n");
#endif
	psa_UpdateParameter(stPsaParam);
	//TODO:
	// Setup new FCoutput value
	flowCtrl_RequestSetFlow(gs_fcOutputVal);
	if(compressor_GetCurentSeq() == COMP_NORMAL_SEQ)
	{
		// Setup new compressor rpm value
		compressor_ReqestSetRpm(gs_compressorRpm);
	}
	DEBUGOUT("s_psaState: %d\n",s_psaState);
	if((s_psaState != PSA_START_STATE) && (s_psaState != PSA_PASS_3S_STATE))
	{
		// Setup new cooling fan duty value
		DEBUGOUT("Set duty cooling fan \n");
		coolingFan_Setduty(gs_coolingFanDuty);
	}
	return;
}

//*****************************************************************************/
//    Operation Name: psa_CheckEventFlag()
//
//    Processing:  chek event flag and control valve
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Global variables use: gs_psaCurentSeq, gs_solXXXTimer, gs_solXXXTimerFlag, gs_psaXXXTimer, gs_psaXXXTimerFlag
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
static void psa_CheckEventFlag(void) {
	switch (gs_psaCurentSeq) {
	case PSA_START:
		//Clear another psa sequence flag: END, BASIC
		gs_sol1OnOffsetTimerFlag = false;
		gs_sol1OffOffsetTimerFlag = false;
		gs_sol2OnOffsetTimerFlag = false;
		gs_sol2OffOffsetTimerFlag = false;
		gs_sol3OnOffsetTimerFlag = false;
		gs_sol3OffOffsetTimerFlag = false;
		gs_sol4OnOffsetTimerFlag = false;
		gs_sol4OffOffsetTimerFlag = false;
		gs_sol5OnOffsetATimerFlag = false;
		gs_sol5OnOffsetBTimerFlag = false;
		gs_sol5OpenATimerFlag = false;
		gs_sol5OpenBTimerFlag = false;
		gs_maxPsaTimerFlag = false;
		gs_psaEnd10SFirstTimerFlag = false;
		gs_psaEnd20SLastTimerFlag = false;

		gs_psaStart23STimerFlag = false; // handle in PSA basic sequence or PSA decompress
		gs_psaStart20STimerFlag = false; // handle in PSA basic sequence or PSA decompress
		if (gs_psaStart3STimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA START 3S TIMEOUT \n");
#endif
			gs_psaStart3STimerFlag = false;
			s_psaState = PSA_PASS_3S_STATE;
			//TODO
			compressor_Start();
			//start psa basic sequence
			gs_psaCurentSeq = PSA_BASIC;
			gs_psaCurrentPhase = PHASE_B;
			psa_StartPsaCycle();
		}
		break;
	case PSA_END:
		//Clear another psa sequence flag: START, BASIC
		gs_sol1OnOffsetTimerFlag = false;
		gs_sol1OffOffsetTimerFlag = false;
		gs_sol2OnOffsetTimerFlag = false;
		gs_sol2OffOffsetTimerFlag = false;
		gs_sol3OnOffsetTimerFlag = false;
		gs_sol3OffOffsetTimerFlag = false;
		gs_sol4OnOffsetTimerFlag = false;
		gs_sol4OffOffsetTimerFlag = false;
		gs_sol5OnOffsetATimerFlag = false;
		gs_sol5OnOffsetBTimerFlag = false;
		gs_sol5OpenATimerFlag = false;
		gs_sol5OpenBTimerFlag = false;
		gs_maxPsaTimerFlag = false;
		gs_psaStart3STimerFlag = false;
		gs_psaStart23STimerFlag = false;
		gs_psaStart20STimerFlag = false;
		if (gs_psaEnd10SFirstTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA END 10S 1st TIMEOUT \n");
#endif
			gs_psaEnd10SFirstTimerFlag = false;
			valve_closeVal1();
			valve_closeVal2();
			valve_closeVal3();
			valve_closeVal4();
		}

		if (gs_psaEnd20SLastTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA END 20S last TIMEOUT \n");
#endif
			gs_psaEnd20SLastTimerFlag = false;
			//Set coolingfan speed to 0 rpm
			coolingFan_Setduty(0);
			gs_psaCurentSeq = PSA_STANDBY;
			s_psaState = PSA_STANDBY_STATE;
		}
		//handle Sol at PSA END open A,B sequence flag
		if (gs_solEndOpenATimerFlag ==true)
		{
			DEBUG_PSA("SOL END OPEN A TIMEOUT \n");
			gs_solEndOpenATimerFlag = false;
			valve_closeVal1();
			valve_closeVal2();
			valve_closeVal3();
			valve_closeVal4();
		}
		break;

	case PSA_BASIC:
		//Clear another psa sequence flag: START, END
		gs_psaStart3STimerFlag = false;
		gs_psaEnd10SFirstTimerFlag = false;
		gs_psaEnd20SLastTimerFlag = false;

		if (gs_psaStart23STimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA START 23S TIMEOUT \n");
#endif
			gs_psaStart23STimerFlag = false;
			s_psaState = PSA_PASS_23S_STATE;
		}

		if (gs_psaStart20STimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA START 20S TIMEOUT \n");
#endif
			gs_psaStart20STimerFlag = false;
			s_psaState = PSA_PASS_20S_STATE;
			//Cooling fan set settig value
			coolingFan_Setduty(gs_coolingFanDuty);
		}

		if (gs_sol1OnOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol1_on_offset_timer_flag \n");
#endif
			gs_sol1OnOffsetTimerFlag = false;
			valve_openVal1();
		}

		if (gs_sol1OffOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol1_off_offset_timer_flag \n");
#endif
			gs_sol1OffOffsetTimerFlag = false;
			valve_closeVal1();
		}

		if (gs_sol2OnOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol2_on_offset_timer_flag \n");
#endif
			gs_sol2OnOffsetTimerFlag = false;
			valve_openVal2();
		}
		if (gs_sol2OffOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol2_off_offset_timer_flag \n");
#endif
			gs_sol2OffOffsetTimerFlag = false;
			valve_closeVal2();
		}

		if (gs_sol3OnOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol3_on_offset_timer_flag \n");
#endif
			gs_sol3OnOffsetTimerFlag = false;
			valve_openVal3();
		}
		if (gs_sol3OffOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol3_off_offset_timer_flag \n");
#endif
			gs_sol3OffOffsetTimerFlag = false;
			valve_closeVal3();
		}

		if (gs_sol4OnOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol4_on_offset_timer_flag \n");
#endif
			gs_sol4OnOffsetTimerFlag = false;
			valve_openVal4();
		}
		if (gs_sol4OffOffsetTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol4_off_offset_timer_flag \n");
#endif
			gs_sol4OffOffsetTimerFlag = false;
			valve_closeVal4();
		}

		if (gs_sol5OnOffsetATimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol5_on_offsetA_timer_flag \n");
#endif
			gs_sol5OnOffsetATimerFlag = false;
			if (psa_StartValveTimer(gs_sol5OpenATimer, gs_sol5OpenATime, &gs_sol5OpenATimerUpdate) != true) {
				DEBUGOUT("ERR: Failed to start Sol5 open A timer \n");
			}
			//Avoid open sol5 short time when Sol5 time = 0
			if(gs_sol5OpenATime != 0)
			{
				valve_openVal5();
			}
		}
		if (gs_sol5OnOffsetBTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol5_on_offsetB_timer_flag \n");
#endif
			gs_sol5OnOffsetBTimerFlag = false;
			if (psa_StartValveTimer(gs_sol5OpenBTimer, gs_sol5OpenBTime, &gs_sol5OpenBTimerUpdate) != true) {
				DEBUGOUT("ERR: Failed to start Sol5 open B timer \n");
			}
			//Avoid open sol5 short time when Sol5 time = 0
			if(gs_sol5OpenBTime != 0)
			{
				valve_openVal5();
			}
		}

		if (gs_sol5OpenATimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol5_openA_timer_flag \n");
#endif
			gs_sol5OpenATimerFlag = false;
			valve_closeVal5();
		}
		if (gs_sol5OpenBTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Sol5_openB_timer_flag \n");
#endif
			gs_sol5OpenBTimerFlag = false;
			valve_closeVal5();
		}

		if (gs_maxPsaTimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("Max_psa_timer_flag \n");
#endif
			gs_maxPsaTimerFlag = false;
			psa_StartPsaCycle();
		}
		break;
	case PSA_DECOMPRESS:
		gs_sol1OnOffsetTimerFlag = false;
		gs_sol1OffOffsetTimerFlag = false;
		gs_sol2OnOffsetTimerFlag = false;
		gs_sol2OffOffsetTimerFlag = false;
		gs_sol3OnOffsetTimerFlag = false;
		gs_sol3OffOffsetTimerFlag = false;
		gs_sol4OnOffsetTimerFlag = false;
		gs_sol4OffOffsetTimerFlag = false;
		gs_sol5OnOffsetATimerFlag = false;
		gs_sol5OnOffsetBTimerFlag = false;
		gs_sol5OpenATimerFlag = false;
		gs_sol5OpenBTimerFlag = false;
		gs_maxPsaTimerFlag = false;
		break;
	default:
		break;
	}
	return;
}

//*****************************************************************************/
//    Operation Name: psa_CreateTimer()
//
//    Processing:  create all timer use for psa control
//
//    Input Parameters: NONE
//
//    Output Parameters: NONE
//
//    Return Values:  void
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  N/A
//
//    Requirements:  N/A
//
//*****************************************************************************/
void psa_CreateTimer(void) {
	gs_psaStart3STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"PsaStar3S",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(PSA_3S_TICKS_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) PSA_START_3S_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_psaStart23STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"PsaStar23",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(PSA_23S_TICKS_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) PSA_START_23S_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_psaStart20STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"PsaStar20",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(PSA_20S_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_20S_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);

	gs_psaEnd10SFirstTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"PsaEnd10F",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(PSA_10S_TICKS_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) PSA_END_10S_FIRST_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_psaEnd20SLastTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"PsaEnd20L",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(PSA_20S_TICKS_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) PSA_END_20S_LAST_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_sol1OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
	                      kernel. */
					"S1OnOfsTm",
					/* The timer period in ticks, must be
	                      greater than 0. */
					pdMS_TO_TICKS(gs_sol1OnOffsetTime),
					/* The timers will auto-reload themselves
	                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
	                      number of times the timer has expired, which
	                      is initialised to 0. */
					( void * ) SOL1_ON_OFFSET_ID,
					/* Each timer calls the same callback when
	                      it expires. */
					psa_TimerCallback
			);

	gs_sol1OffOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
	                      kernel. */
					"S1OfOfsTm",
					/* The timer period in ticks, must be
	                      greater than 0. */
					pdMS_TO_TICKS(gs_sol1OffOffsetTime),
					/* The timers will auto-reload themselves
	                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
	                      number of times the timer has expired, which
	                      is initialised to 0. */
					( void * ) SOL1_OFF_OFFSET_ID,
					/* Each timer calls the same callback when
	                      it expires. */
					psa_TimerCallback
			);

	gs_sol2OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
	                      kernel. */
					"S2OnOfsTm",
					/* The timer period in ticks, must be
	                      greater than 0. */
					pdMS_TO_TICKS(gs_sol2OnOffsetTime),
					/* The timers will auto-reload themselves
	                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
	                      number of times the timer has expired, which
	                      is initialised to 0. */
					( void * ) SOL2_ON_OFFSET_ID,
					/* Each timer calls the same callback when
	                      it expires. */
					psa_TimerCallback
			);

	gs_sol2OffOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"S2OfOfsTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(gs_sol2OffOffsetTime),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) SOL2_OFF_OFFSET_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_sol3OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"S3OnOfsTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(gs_sol3OnOffsetTime),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) SOL3_ON_OFFSET_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					psa_TimerCallback
			);

	gs_sol3OffOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"S3OfOfsTm",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol3OffOffsetTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) SOL3_OFF_OFFSET_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);

	gs_sol4OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"S4OnOfsTm",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol4OnOffsetTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) SOL4_ON_OFFSET_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);

	gs_sol4OffOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"S4OfOfsTm",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol4OffOffsetTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL4_OFF_OFFSET_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_sol5OnOffsetATimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"S5OnOfsA",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol5OnOffsetATime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL5_ON_OFFSETA_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_sol5OnOffsetBTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"S5OnOfsB",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol5OnOffsetBTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL5_ON_OFFSETB_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_sol5OpenATimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"S5OpenA",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol5OpenATime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL5_OPENA_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_sol5OpenBTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"S5OpenB",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol5OpenBTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL5_OPENB_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_solEndOpenATimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"SolEndOpA",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_solEndOpenATime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) SOL_END_OPEN_A_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);

	gs_maxPsaTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"MaxPsaTm",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_maxPsaTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) MAX_PSA_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  	psa_GotoDecompressSeq														*/
/*                                                                                      */
/* Details：  Go to psa decompress  sequence by open valve and stop Max psatimer			*/
/*                                                                                      */
/* Arguments : 		 					  												*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : s_VXBeforeDepressStat, gs_maxPsaTimer							*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                    				*/
/*【Note】                                                                            												    */
/*                                                                                      */
/****************************************************************************************/
static void psa_GotoDecompressSeq(void)
{
	s_V1BeforeDepressStat = valve_getStatus(VAL1);
	s_V2BeforeDepressStat = valve_getStatus(VAL2);
	s_V3BeforeDepressStat = valve_getStatus(VAL3);
	s_V4BeforeDepressStat = valve_getStatus(VAL4);
	DEBUG_PSA("DecompressSeq %d %d %d %d \n ",s_V1BeforeDepressStat,s_V2BeforeDepressStat,s_V3BeforeDepressStat,s_V4BeforeDepressStat);
	valve_openVal1();
	valve_openVal2();
	valve_openVal3();
	valve_openVal4();
	valve_openVal5();
	if(xTimerStop(gs_maxPsaTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop maxPsa Timer \n");
	}
	if(xTimerStop(gs_sol1OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol1OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol1OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol1OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol2OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol2OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol2OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol2OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol3OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol3OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol3OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol3OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol4OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol4OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol4OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol4OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol5OnOffsetATimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OnOffsetATimer Timer \n");
	}
	if(xTimerStop(gs_sol5OnOffsetBTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OnOffsetBTimer Timer \n");
	}

	if(xTimerStop(gs_sol5OpenATimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OpenATimer Timer \n");
	}
	if(xTimerStop(gs_sol5OpenBTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OpenBTimer Timer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  	psa_ExitDecompressSeq														*/
/*                                                                                      */
/* Details：  Exit psa decompress sequence and jump to psa basic by recovder val status	*/
/* 			sequence.						        								    */
/*                                                                                      */
/* Arguments : 		 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : s_VXBeforeDepressStat							             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 								                                    */
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void psa_ExitDecompressSeq(void)
{
	DEBUG_PSA("Exit decompress %d %d %d %d \n ",s_V1BeforeDepressStat,s_V2BeforeDepressStat,s_V3BeforeDepressStat,s_V4BeforeDepressStat);
	//Recover val status
	if (s_V1BeforeDepressStat == VAL_ON)
	{
		valve_openVal1();
	}
	else
	{
		valve_closeVal1();
	}

	if (s_V2BeforeDepressStat == VAL_ON)
	{
		valve_openVal2();
	}
	else
	{
		valve_closeVal2();
	}

	if (s_V3BeforeDepressStat == VAL_ON)
	{
		valve_openVal3();
	}
	else
	{
		valve_closeVal3();
	}

	if (s_V4BeforeDepressStat == VAL_ON)
	{
		valve_openVal4();
	}
	else
	{
		valve_closeVal4();
	}

	valve_closeVal5(); //Always close valve 5

	if(psa_StartValveTimer(gs_maxPsaTimer, gs_maxPsaTime, &gs_maxPsaTimerUpdate) != true) {
		DEBUGOUT("ERR: Failed to start Max PSA timer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  	psa_CheckPressure															*/
/*                                                                                      */
/* Details：  Check prductank pressure for start psa cycle or switch to PSA decompress
 * 			sequence.						        								    */
/*                                                                                      */
/* Arguments : float productTankPress			 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : gs_psaCurentSeq, gs_psaMaxPressure, gs_time2sCount             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : s_time2sCount				                                    */
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void psa_CheckPressure(float productTankPress) {
	gs_pTankCurrentPressureVal = productTankPress;
	psa_CheckForRestartCompressor();
	switch (gs_psaCurentSeq) {
	case PSA_BASIC:
		s_time2sCount++;
		if(productTankPress >= gs_psaMaxPressure + PSA_DECOMPRESS_THRESHOLD)
		{
			//Save valve status before jump to decompress sequence
			gs_psaCurentSeq = PSA_DECOMPRESS;
			psa_GotoDecompressSeq();
			//TODO clear psa_basic timer flag
			// don't need clear here, done clear in psa_check_event_flag function
		}
		else if (productTankPress >= gs_psaMaxPressure)
		{
			if(s_time2sCount >= TIME_2S_COUNT)
			{
				DEBUG_PSA("Product tank %f \n",productTankPress);
				psa_StartPsaCycle();
			}
		}
		break;
	case PSA_DECOMPRESS:
		if (gs_psaStart23STimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA START 23S TIMEOUT \n");
#endif
			gs_psaStart23STimerFlag = false;
			s_psaState = PSA_PASS_23S_STATE;
		}

		if (gs_psaStart20STimerFlag == true) {
#ifdef Q_DEBUG
			DEBUGOUT("PSA START 20S TIMEOUT \n");
#endif
			gs_psaStart20STimerFlag = false;
			s_psaState = PSA_PASS_20S_STATE;
			//Cooling fan set settig value
			coolingFan_Setduty(gs_coolingFanDuty);
		}

		if(productTankPress <= gs_psaMaxPressure + PSA_DECOMPRESS_EXIT_THRESHOLD)
		{
			gs_psaCurentSeq = PSA_BASIC;
			psa_ExitDecompressSeq();
		}
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psa_Handle
//
//    Detail: psa_StartCheckCompRestartSeq
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O) s_isProceesCompressorRestartCheck
//						  (O) s_compRestart30sCnt
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psa_StartCheckCompRestartSeq(void)
{
	s_isPressureOverPcompStlow = false;
	s_isProceesCompressorRestartCheck = true;
	s_compRestart30sCnt = 0;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	psa_Handle															*/
/*                                                                                      */
/* Details： handle psa control															*/
/*                                                                                      */
/* Arguments : None			 					  										*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 	(O) uint16_t s_psaCycleTimeCnt 	 								*/
/*						(O) E_PsaSate s_psaState 										*/
/*						(O) E_PsaMode s_psaMode											*/
/*						(O) bool gs_psaStart3STimerFlag									*/
/*						(O) bool gs_psaStart23STimerFlag								*/
/*						(O) bool gs_psaStart20STimerFlag								*/
/* 						(O) TimerHandle_t gs_psaStart3STimer							*/
/*						(O) TimerHandle_t gs_psaStart23STimer							*/
/*						(O) TimerHandle_t gs_psaStart20STimer							*/
/*                    					                                            	*/
/*                     								                                 	*/
/* 													                                    */
/*【Note】                                                                            												    */
/*                                                                                      */
/****************************************************************************************/
void psa_Handle(float productTankPress)
{
	s_psaCycleTimeCnt++;
	psa_ProcessPendingIpcMsg();
	psa_CheckPressure(productTankPress);
	psa_CheckEventFlag();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	psa_EnterStartSequence												*/
/*                                                                                      */
/* Details：  Enter to psa start sequence by control valve and start psa start timer		*/
/*                                                                                      */
/* Arguments : None			 					  										*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 	(O) E_PsaSequence gs_psaCurentSeq 								*/
/*						(O) E_PsaSate s_psaState 										*/
/*						(O) E_PsaMode s_psaMode											*/
/*						(O) bool gs_psaStart3STimerFlag									*/
/*						(O) bool gs_psaStart23STimerFlag								*/
/*						(O) bool gs_psaStart20STimerFlag								*/
/* 						(O) TimerHandle_t gs_psaStart3STimer							*/
/*						(O) TimerHandle_t gs_psaStart23STimer							*/
/*						(O) TimerHandle_t gs_psaStart20STimer							*/
/*                    					                                            	*/
/*                     								                                 	*/
/* 													                                    */
/*【Note】                                                                            												    */
/*                                                                                      */
/****************************************************************************************/
void psa_EnterStartSequence(void) {
	compressor_Reset();
	flowCtrl_Reset();
	//switch O2 source to internal
	latchValve_RequestOpenAndClose();
	s_psaCycleTimeCnt = 0;
	E_PsaSelectName psaSelectTypeAtStart = psaSelect_GetConditionAtStart();
	psa_SendRequestParameterAtStart(psaSelectTypeAtStart);
	//TODO
	// Stop all PSA End timer
	gs_psaCurentSeq = PSA_START;
	s_psaState = PSA_START_STATE;
	s_psaMode = PSA_START_MODE;
	valve_openVal1();
	valve_openVal2();
	valve_openVal3();
	valve_openVal4();
	valve_closeVal5();
	//clear psa start timer flag
	gs_psaStart3STimerFlag = false;
	gs_psaStart23STimerFlag = false;
	gs_psaStart20STimerFlag = false;

	if (xTimerStart(gs_psaStart3STimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to start PSA start 3S timer \n");
	}
	if(xTimerStart(gs_psaStart23STimer,100) != pdPASS) {
		DEBUGOUT("ERR: Failed to start PSA start 23s time \n");
	}

	if(xTimerStart(gs_psaStart20STimer,100) != pdPASS) {
		DEBUGOUT("ERR: Failed to start PSA start 20s time \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_RequestEnterStartSequence										*/
/*                                                                                      */
/* Details:  Request enter to psa start sequence										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 																		*/
/*                                                                                      */
/* Using Global Data : (I)																*/
/*																						*/
/*																						*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void psa_RequestEnterStartSequence(void) {
	filter_Start();
	encoderInterface_SetEnable(true);
	psa_EnterStartSequence();
	return;
}
/****************************************************************************************/
/*                                                                                      */
/* General：  	psa_EnterEndSequence														*/
/*                                                                                      */
/* Details：  Enter to psa end sequence by control valve and psa devices and start psa end*/
/* 		    timer																		*/
/*                                                                                      */
/* Arguments : None			 					  										*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : (O) bool s_isProceesCompressorRestartCheck						*/
/*					   (O) E_PsaSequence gs_psaCurentSeq								*/
/*                     (O) E_PsaSate s_psaState                                        	*/
/*					   (O) E_PsaMode s_psaMode											*/
/*                     (O) bool gs_psaEnd10SFirstTimerFlag                          	*/
/*					   (O) bool gs_psaEnd20SLastTimerFlag								*/
/*					   (O) TimerHandle_t gs_maxPsaTimer									*/
/*					   (O) TimerHandle_t gs_sol1OnOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol1OffOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol2OnOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol2OffOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol3OnOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol3OffOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol4OnOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol4OffOffsetTimer							*/
/*					   (O) TimerHandle_t gs_sol5OnOffsetATimer							*/
/*					   (O) TimerHandle_t gs_sol5OnOffsetBTimer							*/
/*					   (O) TimerHandle_t gs_sol5OpenATimer								*/
/*					   (O) TimerHandle_t gs_sol5OpenBTimer								*/
/*					   (O) TimerHandle_t gs_psaEnd10SFirstTimer							*/
/*					   (O) TimerHandle_t gs_psaEnd20SLastTimer							*/
/*					   (O) TimerHandle_t gs_solEndOpenATimer							*/
/*					   (I) uint16_t gs_solEndOpenATime									*/
/*						unit: ms														*/
/*						range:0~20000 													*/
/*					   (I) bool* gs_solEndOpenATimerUpdate								*/
/* 													                                    */
/*【Note】                                                                            											   	    */
/*                                                                                      */
/****************************************************************************************/
void psa_EnterEndSequence(void) {
	filter_Stop();
	encoderInterface_SetEnable(false);

	s_isProceesCompressorRestartCheck = false;
	psaSelect_StopCheckCondition();
	gs_psaCurentSeq = PSA_END;
	s_psaState = PSA_END_STATE;
	s_psaMode = PSA_END_MODE;

	//TODO
	//Copressor set 0 rpm
	//FC output set 0 rpm
	flowCtrl_Stop();
	compressor_Stop();
	compressor_ClearAlarmStatus();
	flowCtrl_ClearAlarmStatus();
	valve_openVal1();
	valve_openVal2();
	valve_openVal3();
	valve_openVal4();
	valve_closeVal5();
	// Clear PSA End timer flag
	gs_psaEnd10SFirstTimerFlag = false;
	gs_psaEnd20SLastTimerFlag = false;

	//Stop PSA basic timer
	if(xTimerStop(gs_maxPsaTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop maxPsa Timer \n");
	}
	if(xTimerStop(gs_sol1OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol1OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol1OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol1OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol2OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol2OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol2OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol2OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol3OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol3OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol3OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol3OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol4OnOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol4OnOffsetTimer Timer \n");
	}
	if(xTimerStop(gs_sol4OffOffsetTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol4OffOffsetTimer Timer \n");
	}

	if(xTimerStop(gs_sol5OnOffsetATimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OnOffsetATimer Timer \n");
	}
	if(xTimerStop(gs_sol5OnOffsetBTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OnOffsetBTimer Timer \n");
	}

	if(xTimerStop(gs_sol5OpenATimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OpenATimer Timer \n");
	}
	if(xTimerStop(gs_sol5OpenBTimer,100) != pdPASS)
	{
		DEBUGOUT("Cannot stop gs_sol5OpenBTimer Timer \n");
	}
	//done stop PSA basic timer

	if (xTimerStart(gs_psaEnd10SFirstTimer,0) != pdPASS) {
		DEBUGOUT("ERR: Failed to start PSA end 10S timer \n");
	}
	if(xTimerStart(gs_psaEnd20SLastTimer,100) != pdPASS) {
		DEBUGOUT("ERR: Failed to start PSA end 20s last timer \n");
	}
	if(psa_StartValveTimer(gs_solEndOpenATimer, gs_solEndOpenATime, &gs_solEndOpenATimerUpdate) != true) {
		DEBUGOUT("ERR: Failed to start Sol End open A timer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_ChangeParamSetting												*/
/*                                                                                      */
/* Details:  handle user change psaparameter setting individually in PSA parameter table*/
/*                                                                                      */
/* Arguments :	IPC_MSG_DATA_t msg									 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 		uint32_t 								   						*/
/*						unit: ms														*/
/*						range: 0~60000													*/
/*                                                                                      */
/* Using Global Data : (O)	uint16_t gs_sol1OnOffsetTime 								*/
/*					   (O)	uint16_t gs_sol1OffOffsetTime 								*/
/*					   (O)  uint16_t gs_sol2OnOffsetTime								*/
/*					   (O)  uint16_t gs_sol2OffOffsetTime 								*/
/*					   (O)	uint16_t gs_sol3OnOffsetTime 								*/
/*					   (O)  uint16_t gs_sol3OffOffsetTime 								*/
/*					   (O)  uint16_t gs_sol4OnOffsetTime								*/
/*					   (O)  uint16_t gs_sol4OffOffsetTime								*/
/*					   (O)  uint16_t gs_sol5OnOffsetATime 								*/
/*					   (O)  uint16_t gs_sol5OnOffsetBTime								*/
/*					   (O)  uint16_t gs_sol5OpenATime									*/
/*					   (O)	uint16_t gs_sol5OpenBTime									*/
/*					   (O)  uint16_t gs_solEndOpenATime 								*/
/*					   (O)	uint16_t gs_maxPsaTime 										*/
/*					   (O)  uint16_t gs_fcOutputVal 									*/
/*					   (O)  uint16_t gs_coolingFanDuty									*/
/*					   (O)	uint16_t gs_compressorRpm 									*/
/*					   (O)  uint16_t gs_psaMaxPressure 									*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void psa_ChangeParamSetting(IPC_MSG_DATA_t msg)
{
	switch (msg.data_id) {
	case eMotorCompressorChangeId:
		//TODO change compressor speed
		// check range of msg.data_value
		gs_compressorRpm = msg.data_value;
		compressor_ReqestSetRpm(gs_compressorRpm);
#ifdef Q_DEBUG
		DEBUGOUT("compressor_rpm: %d \n", gs_compressorRpm);
#endif
		break;
	case eOxygenFlowChangeId:
		//TODO FC output
		// check range of msg.data_value
		gs_fcOutputVal = msg.data_value;
		flowCtrl_RequestSetFlow(gs_fcOutputVal);
#ifdef Q_DEBUG
		DEBUGOUT("FCOutput_val: %d \n", gs_fcOutputVal);
#endif
		break;
	case eCoolingFanChangeId:
		gs_coolingFanDuty = msg.data_value;
		coolingFan_Setduty(gs_coolingFanDuty);
#ifdef Q_DEBUG
		DEBUGOUT("cooling_fan_duty: %d \n", gs_coolingFanDuty);
#endif
		break;
	case ePSAPressureChangeId:
		gs_psaMaxPressure = msg.data_value;
#ifdef Q_DEBUG
		DEBUGOUT("PSA_max_pressure: %d \n", gs_psaMaxPressure);
#endif
		break;
	case eSolenoid1OnOffSetTimeChangeId:
		gs_sol1OnOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol1OnOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol1_on_offset_time: %d \n", gs_sol1OnOffsetTime);
#endif
		break;
	case eSolenoid1OffOffSetTimeChangeId:
		gs_sol1OffOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol1OffOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol1_off_offset_time: %d \n", gs_sol1OffOffsetTime);
#endif
		break;
	case eSolenoid2OnOffSetTimeChangeId:
		gs_sol2OnOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol2OnOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol2_on_offset_time: %d \n", gs_sol2OnOffsetTime);
#endif
		break;
	case eSolenoid2OffOffSetTimeChangeId:
		gs_sol2OffOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol2OffOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol2_off_offset_time: %d \n", gs_sol2OffOffsetTime);
#endif
		break;
	case eSolenoid3OnOffSetTimeChangeId:
		gs_sol3OnOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol3OnOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol3_on_offset_time: %d \n", gs_sol3OnOffsetTime);
#endif
		break;
	case eSolenoid3OffOffSetTimeChangeId:
		gs_sol3OffOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol3OffOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol3_off_offset_time: %d \n", gs_sol3OffOffsetTime);
#endif
		break;
	case eSolenoid4OnOffSetTimeChangeId:
		gs_sol4OnOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol4OnOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol4_on_offset_time: %d \n", gs_sol4OnOffsetTime);
#endif
		break;
	case eSolenoid4OffOffSetTimeChangeId:
		gs_sol4OffOffsetTime = msg.data_value * SCALE_TO_MS;
		gs_sol4OffOffsetTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol4_off_offset_time: %d \n", gs_sol4OffOffsetTime);
#endif
		break;
	case eSolenoid5OnOffSetTimeAChangeId:
		gs_sol5OnOffsetATime = msg.data_value * SCALE_TO_MS;
		gs_sol5OnOffsetATimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_on_offsetA_time: %d \n", gs_sol5OnOffsetATime);
#endif
		break;
	case eSolenoid5OnOffSetTimeBChangeId:
		gs_sol5OnOffsetBTime = msg.data_value * SCALE_TO_MS;
		gs_sol5OnOffsetBTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_on_offsetB_time: %d \n", gs_sol5OnOffsetBTime);
#endif
		break;
	case eSol5TimeAChangeId:
		gs_sol5OpenATime = msg.data_value * SCALE_TO_MS;
		gs_sol5OpenATimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_openA_time: %d \n", gs_sol5OpenATime);
#endif
		break;
	case eSol5TimeBChangeId:
		gs_sol5OpenBTime = msg.data_value * SCALE_TO_MS;
		gs_sol5OpenBTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Sol5_openB_time: %d \n", gs_sol5OpenBTime);
#endif
		break;
		//TODO handle sol end open A Change ID
	case eSolEndOpenChangeId:
		gs_solEndOpenATime = msg.data_value * SCALE_TO_MS;
		gs_solEndOpenATimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("gs_solEndOpenATime: %d \n", gs_solEndOpenATime);
#endif
		break;
	case eMaxPSATimeChangeId:
		gs_maxPsaTime = msg.data_value * 1000;
		gs_maxPsaTimerUpdate = true;
#ifdef Q_DEBUG
		DEBUGOUT("Max_psa_time: %d \n", gs_maxPsaTime);
#endif
		break;
	default:
		DEBUGOUT("Invalid psa param id\n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_GetCycleTime														*/
/*                                                                                      */
/* Details:  Get psa cycle time															*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 		uint32_t 								   						*/
/*						unit: ms														*/
/*						range: 0~60000													*/
/*                                                                                      */
/* Using Global Data : (I)uint32_t s_psaCycleTime										*/
/*						unit: ms														*/
/*						range: 0~60000													*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t psa_GetCycleTime()
{
	return s_psaCycleTime;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_GetPsaSeq														*/
/*                                                                                      */
/* Details:  Get psa current sequence													*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	E_PsaSequence										   				*/
/*                                                                                      */
/* Using Global Data : (I)E_PsaSequence gs_psaCurentSeq 								*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
E_PsaSequence psa_GetPsaSeq(void)
{
	return gs_psaCurentSeq;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_FinishCompressorRestart											*/
/*                                                                                      */
/* Details:  Finish compressor restart sequence, change psa state to basic				*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_PsaSate s_psaState											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void psa_FinishCompressorRestart(void)
{
	s_psaState = PSA_BASIC_STATE;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_SetMode															*/
/*                                                                                      */
/* Details:  Only call this whens set psa mode to PSA_NORMAL_MODE in PSA_START_MODE case*/
/*                                                                                      */
/* Arguments :		(I)	E_PsaMode mode								 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_PsaMode s_psaMode											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void psa_SetMode(E_PsaMode mode)
{
	s_psaMode = mode;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  psa_GetMode															*/
/*                                                                                      */
/* Details:  Get psa mode																*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	E_PsaMode												   			*/
/*                                                                                      */
/* Using Global Data : (I) E_PsaMode s_psaMode											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
E_PsaMode psa_GetMode(void)
{
	return s_psaMode;
}

