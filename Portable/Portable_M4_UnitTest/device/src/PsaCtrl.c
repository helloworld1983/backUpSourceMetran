/********************************************************************/
/*                                                                  */
/* File Name    : PsaCtrl.c			                                */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   psa device based on psa parameter				*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            Jun 30, 2016      Thao Ha(MV)	  		new file        */
/*                                                                  */
/********************************************************************/
#include "Compressor.h"
#include "timers.h"
#include <assert.h>
#include <AudioMgr.h>
#include <DualCoreCommon.h>
#include <FlowCtrl.h>
#include <PanelLed.h>
#include <PsaCtrl.h>
#include <string.h>
#include <TaskCommon.h>
#include <ValveCtrl.h>
#include "Setting.h"
#include "Fan.h"
#include "LogMgr.h"
#include "LogTask.h"

//#define DEBUG_PSACONTROL_EN
#ifdef DEBUG_PSACONTROL_EN
#define DEBUG_PSACONTROL(...) printf(__VA_ARGS__)
#else
#define DEBUG_PSACONTROL(...)
#endif

static const uint16_t gs_psaOneZeroZeroContinuousParameterLow[ePSATableLastItem]={1550,eFlowRateOneZeroZero,80,1200,500,20};
static const uint16_t gs_psaZeroSevenFiveContinuousParameterLow[ePSATableLastItem]={1300,eFlowRateZeroSevenFive,80,800,300,15};
static const uint16_t gs_psaZeroFiveZeroContinuousParameterLow[ePSATableLastItem]={1000,eFlowRateZeroFiveZero,80,800,300,20};
static const uint16_t gs_psaZeroTwoFiveContinuousParameterLow[ePSATableLastItem]={1000,eFlowRateZeroTwoFive,80,1000,100,20};
static const uint16_t gs_psaZeroOneZeroContinuousParameterLow[ePSATableLastItem]={1150,eFlowRateZeroOneZero,80,1500,1500,20};// day change 15-sept

static const uint16_t gs_psaOneZeroZeroContinuousParameterNormal[ePSATableLastItem]={1450,eFlowRateOneZeroZero,80,800,300,15};
static const uint16_t gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableLastItem]={1300,eFlowRateZeroSevenFive,80,800,300,15};
static const uint16_t gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableLastItem]={1000,eFlowRateZeroFiveZero,80,800,300,20};
static const uint16_t gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableLastItem]={1000,eFlowRateZeroTwoFive,80,1000,100,20};
static const uint16_t gs_psaZeroOneZeroContinuousParameterNormal[ePSATableLastItem]={1150,eFlowRateZeroOneZero,80,1500,1500,20};//day change 15-sept

static const uint16_t gs_psaOneZeroZeroContinuousParameterHigh[ePSATableLastItem]={1550,eFlowRateOneZeroZero,70,800,300,15};
static const uint16_t gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableLastItem]={1300,eFlowRateZeroSevenFive,80,800,300,15};
static const uint16_t gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableLastItem]={1000,eFlowRateZeroFiveZero,80,800,300,20};
static const uint16_t gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableLastItem]={1000,eFlowRateZeroTwoFive,80,1000,100,20};
static const uint16_t gs_psaZeroOneZeroContinuousParameterHigh[ePSATableLastItem]={1150,eFlowRateZeroOneZero,80,1500,1500,20};//day change 15-sept

static const uint16_t gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,70,800,300,17};
static const uint16_t gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableLastItem]={1250,eFlowRateZeroZeroZero,70,700,300,17};

static const uint16_t gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,70,800,300,17};
static const uint16_t gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableLastItem]={1250,eFlowRateZeroZeroZero,70,700,300,17};

static const uint16_t gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableLastItem]={1450,eFlowRateZeroZeroZero,80,800,300,17};
static const uint16_t gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,75,800,300,17};
static const uint16_t gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableLastItem]={1350,eFlowRateZeroZeroZero,70,800,300,17};
static const uint16_t gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableLastItem]={1250,eFlowRateZeroZeroZero,70,700,300,17};

static TimerHandle_t gs_psaStartMaxPVSATimer;
static TimerHandle_t gs_psaStartSol2OpenATimer;
static TimerHandle_t gs_psaStartSol2OpenBTimer;
static TimerHandle_t gs_psaStartSol1OnOffsetTimer;
static TimerHandle_t gs_psaStartSol3OnOffsetTimer;
static TimerHandle_t gs_psaStart2STimer;
static TimerHandle_t gs_psaStart3STimer;
static TimerHandle_t gs_psaStart5MTimer;

static TimerHandle_t gs_psaStartSol1DelayOpenTimer;
static TimerHandle_t gs_psaStartSol3DelayOpenTimer;

static bool gs_isPSAWork = false;

#define PSA_START_MAXPVSA_ID 14
#define PSA_START_SOL1ONOFFSET_ID 17
#define PSA_START_SOL3ONOFFSET_ID 18
#define PSA_START_SOL2OPENA_ID 19
#define PSA_START_SOL2OPENB_ID 20
#define PSA_START_2S_ID 21
#define PSA_START_3S_ID 22
#define PSA_START_5M_ID 23

#define PSA_START_SOL1DELAYOPEN_ID 24
#define PSA_START_SOL3DELAYOPEN_ID 25


#define PSA_2S_TICKS_TIME 	2000
#define PSA_3S_TICKS_TIME 	3000
#define PSA_5M_TICKS_TIME 	300000
#define PSA_LOW_TEMPERATURE 35
#define PSA_HIGH_TEMPERATURE 60 //42
static E_PSATemperature gs_currentTemperature=ePSANormalTemperature;
static bool gs_isMaxPVSATimerChangePeriod = false;
static bool gs_isSol1OnOffsetTimerChangePeriod = false;
static bool gs_isSol3OnOffsetTimerChangePeriod = false;
static bool gs_isSol2OpenATimerChangePeriod = false;
static bool gs_isSol2OpenBTimerChangePeriod = false;
static bool gs_isSol1DelayOpenTimeChangePeriod = false;
static bool gs_isSol3DelayOpenTimeChangePeriod = false;

static bool gs_psaStartMaxPVSATimerFlag=false;
static bool gs_psaStart2STimerFlag=true;
static bool gs_psaStart3STimerFlag=false;
static bool gs_psaStart5MTimerFlag=false;

//static bool gs_checkVal1orVal3Close = false;

extern const uint16_t g_FlowRateTable[eFlowRateLast];
static uint16_t gs_currentFlowRate = eFlowRateZeroOneZero;
static uint16_t gs_maxPVSATicksTime = 15000;
static uint16_t gs_sol2OpenATicksTime = 800;
static uint16_t gs_sol2OpenBTicksTime = 800;
static uint16_t gs_sol1OnOffsetTicksTime = 300;
static uint16_t gs_sol3OnOffsetTicksTime = 300;

static uint16_t gs_sol1DelayOpenTime = 100;
static uint16_t gs_sol3DelayOpenTime = 100;

static bool gs_isSwitch=false;
static uint16_t gs_maxPressure = 80;
static bool gs_isPSAParameterSet=false;
static bool gs_isStop=false;
#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif
extern uint16_t g_compessorRpm;
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_GetCurrentPSATemperature			     						*/
/*                                                                                      */
/* Details:     Get current temperture state of PSA function                            */
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : E_PSATemperature    	                                                */
/*                                                                                      */
/****************************************************************************************/
E_PSATemperature psa_GetCurrentPSATemperature()
{
	return gs_currentTemperature;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_TimerCallback     	    										*/
/*                                                                                      */
/* Details:     Handle time out of all timer in PsaControl module						*/
/*                                                                                      */
/* Arguments:	TimerHandle_t xTimer													*/
/*                                                                                      */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void psa_TimerCallback( TimerHandle_t xTimer )
{

	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId)
	{
	case PSA_START_MAXPVSA_ID:
		gs_psaStartMaxPVSATimerFlag=true;
		if(gs_isMaxPVSATimerChangePeriod==true)
		{
			DEBUG_PSACONTROL("Change Max PVSA Period %d \n",gs_maxPVSATicksTime);
			gs_isMaxPVSATimerChangePeriod=false;
			xTimerChangePeriod(gs_psaStartMaxPVSATimer, pdMS_TO_TICKS(gs_maxPVSATicksTime),100);
		}
		break;
	case PSA_START_SOL1ONOFFSET_ID:
		valve_closeVal3();
		if(gs_isSol1OnOffsetTimerChangePeriod==true)
		{
			DEBUG_PSACONTROL("Change Sol 1 On Offset Timer %d\n",gs_sol1OnOffsetTicksTime);
			gs_isSol1OnOffsetTimerChangePeriod=false;
			xTimerChangePeriod(gs_psaStartSol1OnOffsetTimer, pdMS_TO_TICKS(gs_sol1OnOffsetTicksTime),100);
		}
		if (xTimerStart(gs_psaStartSol1DelayOpenTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 2 Open A timer \n");
		}
		break;
	case PSA_START_SOL3ONOFFSET_ID:

		valve_closeVal1();
		if(gs_isSol3OnOffsetTimerChangePeriod==true)
		{
			DEBUG_PSACONTROL("Change Sol 3 On Offset Timer %d\n",gs_sol3OnOffsetTicksTime);
			gs_isSol3OnOffsetTimerChangePeriod=false;
			xTimerChangePeriod(gs_psaStartSol3OnOffsetTimer, pdMS_TO_TICKS(gs_sol3OnOffsetTicksTime),100);
		}
		if (xTimerStart(gs_psaStartSol3DelayOpenTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 2 Open A timer \n");
		}
		break;
	case PSA_START_SOL1DELAYOPEN_ID:
		valve_openVal1();
		if(gs_isSol1DelayOpenTimeChangePeriod == true)
		{
			DEBUG_PSACONTROL("Change Sol 1 On Offset Timer %d\n",gs_sol1DelayOpenTime);
			gs_isSol1DelayOpenTimeChangePeriod = false;
			xTimerChangePeriod(gs_psaStartSol1DelayOpenTimer, pdMS_TO_TICKS(gs_sol1DelayOpenTime),100);
		}
		break;
	case PSA_START_SOL3DELAYOPEN_ID:
		valve_openVal3();
		if(gs_isSol3DelayOpenTimeChangePeriod == true)
		{
			DEBUG_PSACONTROL("Change Sol 3 On Offset Timer %d\n",gs_sol3DelayOpenTime);
			gs_isSol3DelayOpenTimeChangePeriod = false;
			xTimerChangePeriod(gs_psaStartSol3DelayOpenTimer, pdMS_TO_TICKS(gs_sol3DelayOpenTime),100);
		}
		break;
	case PSA_START_SOL2OPENA_ID:
		valve_closeVal2();
		if(gs_isSol2OpenATimerChangePeriod==true)
		{
			DEBUG_PSACONTROL("Change Sol 2 Open Time A %d\n",gs_sol2OpenATicksTime);
			gs_isSol2OpenATimerChangePeriod=false;
			xTimerChangePeriod(gs_psaStartSol2OpenATimer, pdMS_TO_TICKS(gs_sol2OpenATicksTime),100);
		}
		break;
	case PSA_START_SOL2OPENB_ID:
		valve_closeVal2();
		if(gs_isSol2OpenBTimerChangePeriod==true)
		{
			DEBUG_PSACONTROL("Change Sol 2 Open Time B %d\n",gs_sol2OpenBTicksTime);
			gs_isSol2OpenBTimerChangePeriod=false;
			xTimerChangePeriod(gs_psaStartSol2OpenBTimer, pdMS_TO_TICKS(gs_sol2OpenBTicksTime),100);
		}
		break;
	case PSA_START_2S_ID:
		gs_psaStart2STimerFlag=true;
		break;
	case PSA_START_3S_ID:
		gs_psaStart3STimerFlag=true;
		break;
	case PSA_START_5M_ID:
		gs_psaStart5MTimerFlag=true;
		break;
	default:
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_Switch		     	    										*/
/*                                                                                      */
/* Details:     switch valve															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None                                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void psa_Switch()
{
	if(valve_getStatus(VAL1)==VAL_ON)
	{
		if (xTimerStart(gs_psaStartSol3OnOffsetTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 3 On OFfset timer \n");
		}
		valve_openVal2();
		if (xTimerStart(gs_psaStartSol2OpenATimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 2 Open A timer \n");
		}
	}
	else
	{
		if (xTimerStart(gs_psaStartSol1OnOffsetTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 1 On OFfset timer \n");
		}
		valve_openVal2();
		if (xTimerStart(gs_psaStartSol2OpenATimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Sol 2 Open A timer \n");
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_CheckPressure     	    										*/
/*                                                                                      */
/* Details:     Check the condition to switch valve										*/
/*                                                                                      */
/* Arguments:	(I) float productTankPress												*/
/*              (O) None                                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void psa_CheckPressure(float productTankPress)
{
	if(productTankPress>=gs_maxPressure && gs_psaStart2STimerFlag==true)
	{
		gs_isSwitch=true;
		gs_psaStartMaxPVSATimerFlag=false;
		if (xTimerStart(gs_psaStartMaxPVSATimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start PSA start Max PVSA timer \n");
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_SetCurrentTemperature    										*/
/*                                                                                      */
/* Details:     Set current Compressor board temperature								*/
/*                                                                                      */
/* Arguments:	(I) uint16_t compBoardTemp												*/
/*              (O) None                                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
static void psa_SetCurrentTemperature(uint16_t compBoardTemp)
{
	if(compBoardTemp!=0)
	{
		E_PSATemperature temperature = ePSALowTemperature;
		if(compBoardTemp <= PSA_LOW_TEMPERATURE)
		{
			temperature = ePSALowTemperature;
		}
		else
			if(compBoardTemp > PSA_HIGH_TEMPERATURE)
			{
				temperature = ePSAHighTemperature;
			}
			else
			{
				temperature = ePSANormalTemperature;
			}
		if(temperature != gs_currentTemperature)
		{
			DEBUG_PSACONTROL("Change PSA based on Temperature %d from %d \n",temperature,gs_currentTemperature);
			gs_currentTemperature = temperature;
			if(setting_Get(eModeSettingID) == eContinuosMode)
			{
				psa_select(setting_Get(eContinuousFLowRateSettingID),eContinuosMode);
			}
			else
			{
				psa_select(setting_Get(eSynchronizedFlowRateSettingID),eSynchronizedMode);
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_Handle				    										*/
/*                                                                                      */
/* Details:     This function is called every 20ms to handle psa operation			 	*/
/*                                                                                      */
/* Arguments:	(I) float productTankPress												*/
/*              (I) uint16_t compBoardTemp                                              */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_Handle(float productTankPress,uint16_t compBoardTemp)
{
	static bool s_isStartPSASequence = true;
	if(gs_isStop==false)
	{
		if((flowCtrl_isGetVersionDone()==true) && (compressor_isGetVersionDone()==true) && (gs_isPSAParameterSet==true))
		{
			gs_isPSAWork=true;
		}
		if(gs_isPSAWork==true)
		{
			if((s_isStartPSASequence==true))
			{
				psa_Start();
				s_isStartPSASequence=false;
			}
			psa_SetCurrentTemperature(compBoardTemp);
			psa_CheckPressure(productTankPress);
			if(gs_psaStartMaxPVSATimerFlag==true)
			{
				gs_psaStartMaxPVSATimerFlag=false;
				gs_isSwitch=true;
				if (xTimerStart(gs_psaStartMaxPVSATimer,10) != pdPASS)
				{
					DEBUG_TASK("ERR: Failed to start PSA start Max PVSA timer \n");
				}
			}
			if(gs_isSwitch==true)
			{
				psa_Switch();
				gs_psaStart2STimerFlag=false;
				if (xTimerStart(gs_psaStart2STimer,10) != pdPASS)
				{
					DEBUG_TASK("ERR: Failed to start PSA start 2S timer \n");
				}
				gs_isSwitch=false;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_Start				    										*/
/*                                                                                      */
/* Details:     This function is called when psa function run first time			 	*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_Start()
{
	gs_sol1DelayOpenTime = setting_Get(eSol1DelayOpenTime);
	if (gs_sol1DelayOpenTime == 0)
	{
		gs_sol1DelayOpenTime = 1;
	}
	gs_sol3DelayOpenTime = setting_Get(eSol3DelayOpenTime);
	if (gs_sol3DelayOpenTime == 0)
	{
		gs_sol3DelayOpenTime = 1;
	}
	psa_CreateTimer();
	compressor_ReqestSetRpm(g_compessorRpm);
	flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
	valve_closeVal1();
	valve_openVal3();
	if (xTimerStart(gs_psaStartMaxPVSATimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start PSA start Max PVSA timer \n");
	}
	if (xTimerStart(gs_psaStart3STimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start PSA start 3S Timer \n");
	}
	if (xTimerStart(gs_psaStart5MTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start PSA start 5Min Timer \n");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_CreateTimer			    										*/
/*                                                                                      */
/* Details:     Create all timer for Key_panel module								 	*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_CreateTimer()
{
	gs_psaStartMaxPVSATimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start Max PVSA timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_maxPVSATicksTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_MAXPVSA_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol1OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"Psa Start Sol 1 On Offset timer",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol1OnOffsetTicksTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) PSA_START_SOL1ONOFFSET_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol3OnOffsetTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"Psa Start Sol 3 On Offset timer",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(gs_sol3OnOffsetTicksTime),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) PSA_START_SOL3ONOFFSET_ID,
					/* Each timer calls the same callback when
				                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol2OpenATimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start Sol 2 Open A timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol2OpenATicksTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_SOL2OPENA_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol2OpenBTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start Sol 2 Open B timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol2OpenBTicksTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_SOL2OPENB_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStart2STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start 2S timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(PSA_2S_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_2S_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStart3STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start 3S timer",
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
	gs_psaStart5MTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start 5Min timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(PSA_5M_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_5M_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol1DelayOpenTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start Sol 1 Delay Open ",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol1DelayOpenTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_SOL1DELAYOPEN_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
	gs_psaStartSol3DelayOpenTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Psa Start Sol 3 Delay Open ",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(gs_sol3DelayOpenTime),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) PSA_START_SOL3DELAYOPEN_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					psa_TimerCallback
			);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setMaxPressure		    										*/
/*                                                                                      */
/* Details:     Set max pressure for valve switching								 	*/
/*                                                                                      */
/* Arguments:	(I) uint16_t pressure													*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setMaxPressure(uint16_t pressure)
{
	gs_maxPressure = pressure;
	DEBUG_PSACONTROL("Set Max Pressure %d \n",gs_maxPressure);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setPVSATime			    										*/
/*                                                                                      */
/* Details:     Set max PVSA time for valve switching								 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setPVSATime(double time)
{
	gs_maxPVSATicksTime = time*1000;
	gs_isMaxPVSATimerChangePeriod = true;
	DEBUG_PSACONTROL("Set PVSA Time %d \n",gs_maxPVSATicksTime);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol2OpenATime	    										*/
/*                                                                                      */
/* Details:     Set valve 2 Open Time A												 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol2OpenATime(double time)
{
	gs_sol2OpenATicksTime=time;
	gs_isSol2OpenATimerChangePeriod = true;
	DEBUG_PSACONTROL("Set Sol 2 Open Time A %d\n",gs_sol2OpenATicksTime);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol2OpenBTime	    										*/
/*                                                                                      */
/* Details:     Set valve 2 Open Time B												 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol2OpenBTime(double time)
{
	gs_sol2OpenBTicksTime=time;
	gs_isSol2OpenBTimerChangePeriod = true;
	DEBUG_PSACONTROL("Set Sol 2 Open Time B %d\n",gs_sol2OpenBTicksTime);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol13DelayOpenTime	    										*/
/*                                                                                      */
/* Details:     Set valve 2 Open Time B												 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol1DelayOpenTime(double time)
{
	gs_sol1DelayOpenTime = time;
	if(time == 0)
	{
		gs_sol1DelayOpenTime = 1;
	}
	gs_isSol1DelayOpenTimeChangePeriod = true;
//	DEBUGOUT("Set Sol 1 Open Time  %d\n",gs_sol1DelayOpenTime);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol13DelayOpenTime	    										*/
/*                                                                                      */
/* Details:     Set valve 2 Open Time B												 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol3DelayOpenTime(double time)
{
	gs_sol3DelayOpenTime = time;
	if(time == 0)
	{
		gs_sol3DelayOpenTime = 1;
	}
	gs_isSol3DelayOpenTimeChangePeriod = true;
//	DEBUGOUT("Set Sol 3 Open Time  %d\n",gs_sol3DelayOpenTime);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol1OnOffsetTime	    										*/
/*                                                                                      */
/* Details:     Set valve 1 On Offset Time											 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol1OnOffsetTime(double time)
{

	gs_sol1OnOffsetTicksTime = time;
	if(time == 0)
	{
		gs_sol1OnOffsetTicksTime = 1;
	}
	gs_isSol1OnOffsetTimerChangePeriod = true;
	DEBUG_PSACONTROL("Set Sol 1 On Offset Time %d\n",gs_sol1OnOffsetTicksTime);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_setSol3OnOffsetTime	    										*/
/*                                                                                      */
/* Details:     Set valve 3 On Offset Time											 	*/
/*                                                                                      */
/* Arguments:	(I) double time															*/
/*              (O) None				                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_setSol3OnOffsetTime(double time)
{
	gs_sol3OnOffsetTicksTime=time;
	if(time == 0)
	{
		gs_sol3OnOffsetTicksTime = 1;
	}
	gs_isSol3OnOffsetTimerChangePeriod = true;
	DEBUG_PSACONTROL("Set Sol 3 On Offset Time %d\n",gs_sol3OnOffsetTicksTime);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_select				    										*/
/*                                                                                      */
/* Details:     Select PVSA table based on request flow, current mode and current 	 	*/
/*              compressor board temperature                                            */
/* Arguments:	(I) uint8_t flowRequest													*/
/*              (I) E_ModeSetting mode	                                                */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_select(uint8_t flowRequest,E_ModeSetting mode)
{
	POR_LOG_T log ={'\0'};
	log.EventOrAlarm = eEvent;
	setting_GetCurrentSetting(&log.oldValue);
	setting_GetCurrentSetting(&log.newValue);
	E_logQueueReceive logQueue;
	logQueue.id = eRequestWriteLog;

	if(gs_currentTemperature==ePSALowTemperature)
	{
		log.logCode=ePsaParameterSetLowNormalO2ConcentrationLogId;
		if(mode == eContinuosMode)
		{
			switch(flowRequest)
			{
			case eFlowRateZeroOneZero:
				DEBUG_PSACONTROL("Change to eFlowRateZeroOneZero Continuous Low Temperture\n ");
				gs_currentFlowRate=gs_psaZeroOneZeroContinuousParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaZeroOneZeroContinuousParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaZeroOneZeroContinuousParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaZeroOneZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaZeroOneZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaZeroOneZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaZeroOneZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaZeroOneZeroContinuousParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateZeroTwoFive:
				DEBUG_PSACONTROL("Change to eFlowRateZeroTwoFive Continuous Low Temperture\n ");
				gs_currentFlowRate=gs_psaZeroTwoFiveContinuousParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaZeroTwoFiveContinuousParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaZeroTwoFiveContinuousParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaZeroTwoFiveContinuousParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaZeroTwoFiveContinuousParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaZeroTwoFiveContinuousParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateZeroFiveZero:
				DEBUG_PSACONTROL("Change to eFlowRateZeroFiveZero Continuous Low Temperture\n ");
				gs_currentFlowRate=gs_psaZeroFiveZeroContinuousParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaZeroFiveZeroContinuousParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaZeroFiveZeroContinuousParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaZeroFiveZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaZeroFiveZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaZeroFiveZeroContinuousParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateZeroSevenFive:
				DEBUG_PSACONTROL("Change to eFlowRateZeroSevenFive Continuous Low Temperture\n ");
				gs_currentFlowRate=gs_psaZeroSevenFiveContinuousParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaZeroSevenFiveContinuousParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaZeroSevenFiveContinuousParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaZeroSevenFiveContinuousParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaZeroSevenFiveContinuousParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaZeroSevenFiveContinuousParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateOneZeroZero:
				DEBUG_PSACONTROL("Change to eFlowRateOneZeroZero Continuous Low Temperture\n ");
				gs_currentFlowRate=gs_psaOneZeroZeroContinuousParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaOneZeroZeroContinuousParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaOneZeroZeroContinuousParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaOneZeroZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaOneZeroZeroContinuousParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaOneZeroZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaOneZeroZeroContinuousParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaOneZeroZeroContinuousParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			default:
				break;
			}
		}
		else
		{
			switch(flowRequest)
			{
			case eFlowRateOneTwoFive:
				DEBUG_PSACONTROL("Change to eFlowRateOneTwoFive Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaOneTwoFiveSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateOneFiveZero:
				DEBUG_PSACONTROL("Change to eFlowRateOneFiveZero Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaOneFiveZeroSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateOneSevenFive:
				DEBUG_PSACONTROL("Change to eFlowRateOneSevenFive Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaOneSevenFiveSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateTwoZeroZero:
				DEBUG_PSACONTROL("Change to eFlowRateTwoZeroZero Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaTwoZeroZeroSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateTwoFiveZero:
				DEBUG_PSACONTROL("Change to eFlowRateTwoFiveZero Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaTwoFiveZeroSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			case eFlowRateThreeZeroZero:
				DEBUG_PSACONTROL("Change to eFlowRateThreeZeroZero Sync Low Temperture\n ");
				gs_currentFlowRate=gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableFCOutput];
				psa_setMaxPressure(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATablePVSAPressure]);
				psa_setPVSATime(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableMaxPVSATime]);
				psa_setSol1OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol3OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableSol1AndSol3OffsetTime]);
				psa_setSol2OpenATime(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				psa_setSol2OpenBTime(gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableSol2OpenTime]);
				g_compessorRpm= gs_psaThreeZeroZeroSynchronizeParameterLow[ePSATableCompressorRPM];
				if(gs_isPSAWork==true)
				{
					compressor_ReqestSetRpm(g_compessorRpm);
					flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
				}
				gs_isPSAParameterSet=true;
				break;
			default:
				break;
			}
		}
	}
	else
		if(gs_currentTemperature==ePSANormalTemperature)
		{
			log.logCode=ePsaParameterSetNormalNormalO2ConcentrationLogId;
			if(mode == eContinuosMode)
			{
				switch(flowRequest)
				{
				case eFlowRateZeroOneZero:
					DEBUG_PSACONTROL("Change to eFlowRateZeroOneZero Continuous Normal Temperature\n ");
					gs_currentFlowRate=gs_psaZeroOneZeroContinuousParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaZeroOneZeroContinuousParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaZeroOneZeroContinuousParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaZeroOneZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaZeroOneZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaZeroOneZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaZeroOneZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaZeroOneZeroContinuousParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateZeroTwoFive:
					DEBUG_PSACONTROL("Change to eFlowRateZeroTwoFive Continuous Normal Temperature\n ");
					gs_currentFlowRate=gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaZeroTwoFiveContinuousParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateZeroFiveZero:
					DEBUG_PSACONTROL("Change to eFlowRateZeroFiveZero Continuous Normal Temperature\n ");
					gs_currentFlowRate=gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaZeroFiveZeroContinuousParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateZeroSevenFive:
					DEBUG_PSACONTROL("Change to eFlowRateZeroSevenFive Continuous Normal Temperature\n ");
					gs_currentFlowRate=gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaZeroSevenFiveContinuousParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateOneZeroZero:
					DEBUG_PSACONTROL("Change to eFlowRateOneZeroZero Continuous Normal Temperature\n ");
					gs_currentFlowRate=gs_psaOneZeroZeroContinuousParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaOneZeroZeroContinuousParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaOneZeroZeroContinuousParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaOneZeroZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaOneZeroZeroContinuousParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaOneZeroZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaOneZeroZeroContinuousParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaOneZeroZeroContinuousParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				default:
					break;
				}
			}
			else
			{
				switch(flowRequest)
				{
				case eFlowRateOneTwoFive:
					DEBUG_PSACONTROL("Change to eFlowRateOneTwoFive Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaOneTwoFiveSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateOneFiveZero:
					DEBUG_PSACONTROL("Change to eFlowRateOneFiveZero Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaOneFiveZeroSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateOneSevenFive:
					DEBUG_PSACONTROL("Change to eFlowRateOneSevenFive Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaOneSevenFiveSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateTwoZeroZero:
					DEBUG_PSACONTROL("Change to eFlowRateTwoZeroZero Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaTwoZeroZeroSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateTwoFiveZero:
					DEBUG_PSACONTROL("Change to eFlowRateTwoFiveZero Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaTwoFiveZeroSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				case eFlowRateThreeZeroZero:
					DEBUG_PSACONTROL("Change to eFlowRateThreeZeroZero Sync Normal Temperature\n ");
					gs_currentFlowRate=gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableFCOutput];
					psa_setMaxPressure(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATablePVSAPressure]);
					psa_setPVSATime(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableMaxPVSATime]);
					psa_setSol1OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol3OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableSol1AndSol3OffsetTime]);
					psa_setSol2OpenATime(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					psa_setSol2OpenBTime(gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableSol2OpenTime]);
					g_compessorRpm= gs_psaThreeZeroZeroSynchronizeParameterNormal[ePSATableCompressorRPM];
					if(gs_isPSAWork==true)
					{
						compressor_ReqestSetRpm(g_compessorRpm);
						flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
					}
					gs_isPSAParameterSet=true;
					break;
				default:
					break;
				}
			}
		}
		else
			if(gs_currentTemperature==ePSAHighTemperature)
			{
				log.logCode=ePsaParameterSetHighNormalO2ConcentrationLogId;
				if(mode == eContinuosMode)
				{
					switch(flowRequest)
					{
					case eFlowRateZeroOneZero:
						DEBUG_PSACONTROL("Change to eFlowRateZeroOneZero Continuous High Temperature\n ");
						gs_currentFlowRate=gs_psaZeroOneZeroContinuousParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaZeroOneZeroContinuousParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaZeroOneZeroContinuousParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaZeroOneZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaZeroOneZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaZeroOneZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaZeroOneZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaZeroOneZeroContinuousParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateZeroTwoFive:
						DEBUG_PSACONTROL("Change to eFlowRateZeroTwoFive Continuous High Temperature\n ");
						gs_currentFlowRate=gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaZeroTwoFiveContinuousParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateZeroFiveZero:
						DEBUG_PSACONTROL("Change to eFlowRateZeroFiveZero Continuous High Temperature\n ");
						gs_currentFlowRate=gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaZeroFiveZeroContinuousParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateZeroSevenFive:
						DEBUG_PSACONTROL("Change to eFlowRateZeroSevenFive Continuous High Temperature\n ");
						gs_currentFlowRate=gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaZeroSevenFiveContinuousParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateOneZeroZero:
						DEBUG_PSACONTROL("Change to eFlowRateOneZeroZero Continuous High Temperature\n ");
						gs_currentFlowRate=gs_psaOneZeroZeroContinuousParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaOneZeroZeroContinuousParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaOneZeroZeroContinuousParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaOneZeroZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaOneZeroZeroContinuousParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaOneZeroZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaOneZeroZeroContinuousParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaOneZeroZeroContinuousParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					default:
						break;
					}
				}
				else
				{
					switch(flowRequest)
					{
					case eFlowRateOneTwoFive:
						DEBUG_PSACONTROL("Change to eFlowRateOneTwoFive Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaOneTwoFiveSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateOneFiveZero:
						DEBUG_PSACONTROL("Change to eFlowRateOneFiveZero Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaOneFiveZeroSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateOneSevenFive:
						DEBUG_PSACONTROL("Change to eFlowRateOneSevenFive Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaOneSevenFiveSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateTwoZeroZero:
						DEBUG_PSACONTROL("Change to eFlowRateTwoZeroZero Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaTwoZeroZeroSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateTwoFiveZero:
						DEBUG_PSACONTROL("Change to eFlowRateTwoFiveZero Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaTwoFiveZeroSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					case eFlowRateThreeZeroZero:
						DEBUG_PSACONTROL("Change to eFlowRateThreeZeroZero Sync High Temperature\n ");
						gs_currentFlowRate=gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableFCOutput];
						psa_setMaxPressure(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATablePVSAPressure]);
						psa_setPVSATime(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableMaxPVSATime]);
						psa_setSol1OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol3OnOffsetTime(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableSol1AndSol3OffsetTime]);
						psa_setSol2OpenATime(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						psa_setSol2OpenBTime(gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableSol2OpenTime]);
						g_compessorRpm= gs_psaThreeZeroZeroSynchronizeParameterHigh[ePSATableCompressorRPM];
						if(gs_isPSAWork==true)
						{
							compressor_ReqestSetRpm(g_compessorRpm);
							flowCtrl_RequestSetFlow(g_FlowRateTable[gs_currentFlowRate]);
						}
						gs_isPSAParameterSet=true;
						break;
					default:
						break;
					}
				}
			}
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from mode_ChangeMode");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_isThreeSecondFinish	    										*/
/*                                                                                      */
/* Details:     Check whether PSA has functioned for 3 second                           */
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool psa_isThreeSecondFinish(void)
{
	return gs_psaStart3STimerFlag;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_isWarmUpFinish	    				     						*/
/*                                                                                      */
/* Details:     Check whether Warm up process is finished                               */
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : bool			    	                                                */
/*                                                                                      */
/****************************************************************************************/
bool psa_isWarmUpFinish(void)
{
	return gs_psaStart5MTimerFlag;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	psa_Stop			    				     						*/
/*                                                                                      */
/* Details:     Stop PSA function						                                */
/* Arguments:	(I) None																*/
/*              (O) None	        			                                        */
/* ReturnValue : void			    	                                                */
/*                                                                                      */
/****************************************************************************************/
void psa_Stop(void)
{
	gs_isStop=true;
}
