/********************************************************************/
/*                                                                  */
/* File Name    : BreathTrigger.c                                   */
/*                                                                  */
/* General       : This module contain function to handle breath	*/
/* 				   trigger											*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             April 18, 2017  Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include <FlowCtrl.h>
#include <PanelLed.h>
#include <TaskCommon.h>
#include "Setting.h"
#include "BreathTrigger.h"
#include "PressureSensor.h"
#include "Fan.h"

enum
{
	eIdleStage,
	eInhaleStage,
	eExhaleStage
};
#define DEFAULT_BREATH_RATE 20.0
#define MIN_BREATH_RATE 10
#define CMH2O_TO_PA 98.0665
#define PA_TO_CMH2O 1/CMH2O_TO_PA
#define NUMBER_OF_SAMPLE 3
#define ONE_MINUTE 60000.0

#define UPDATE_BPM_LOOP_CNT 60000/DEVICE_TASK_DELAY_TIME
#define NOISE_CANCEL_LOOP_CNT 400/DEVICE_TASK_DELAY_TIME


//#define DEBUG_PRESSURE_EN
#ifdef DEBUG_PRESSURE_EN
#define DEBUG_PRESSURE(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRESSURE(...)
#endif
//#define DEBUG_SYNCHRONIZE_EN
#ifdef DEBUG_SYNCHRONIZE_EN
#define DEBUG_SYNCHRONIZE(...) printf(__VA_ARGS__)
#else
#define DEBUG_SYNCHRONIZE(...)
#endif


const static float gs_triggerValueLevel1 = (-0.06+0.0104)*CMH2O_TO_PA;//-0.06
const static float gs_triggerValueLevel2 = (-0.08+0.0128)*CMH2O_TO_PA;//-0.08
const static float gs_triggerValueLevel3 = (-0.16+0.0205)*CMH2O_TO_PA;//-0.16
//const static float gs_exhalePressureLow = 0.025*CMH2O_TO_PA;//0.01 * CMH2O_TO_PA;
//const static float gs_exhalePressureHigh = 0.01 * CMH2O_TO_PA;
//const static float gs_detectStartExhale = (0.27/3) * CMH2O_TO_PA;


static bool gs_isInhale = false;
extern const uint16_t g_FlowRateTable[eFlowRateLast];
static float gs_currentTriggerValue=0;
static float gs_currentTriggerLevel1Value = 0;
static float gs_triggerPressure = 0;
static bool gs_isStop = false;
static uint8_t gs_breathRate = DEFAULT_BREATH_RATE;
static uint8_t gs_testSynchronizeModeBPM = 0;
static bool gs_isResetDetectBreathMode = false;
static bool gs_isResetTestSynchronizeMode = false;


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_initDiff										  			*/
/*                                                                                      */
/* Details:    Initialize differential pressure by set resolution of Differential		*/
/*  		sensor and set global variable												*/
/* Arguments:	None																	*/
/*																						*/
/* ReturnValue : None				                                                    */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_InitTrigger(void)
{
	pressure_ChangeResolutionDiff();
	gs_currentTriggerLevel1Value = gs_triggerValueLevel1 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_IsInhale										  			*/
/*                                                                                      */
/* Details:   Check whether user is inhaling											*/
/*                                                                                      */
/* Arguments:	None																	*/
/* ReturnValue : bool				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool breathTrigger_IsInhale()
{
	return gs_isInhale;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	breathTrigger_UpdateTriggerSensitivityLevel						  	*/
/*                                                                                      */
/* Details:   Set trigger sensitivity level												*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_UpdateTriggerSensitivityLevel()
{
	E_ID_TriggerSensitivity level = (E_ID_TriggerSensitivity)setting_Get(eTriggerSensitivitySettingID);
	E_FanVoltage fanVoltage = fan_GetFanVoltage();
	switch (fanVoltage)
	{
		case OFF:
			if(level == eTriggerSensitivityLevel1)
			{
				gs_currentTriggerValue = gs_triggerValueLevel1 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA;
			}
			else
			{
				if(level==eTriggerSensitivityLevel2)
				{
					gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA;
				}
				else
				{
					if(level==eTriggerSensitivityLevel3)
					{
						gs_currentTriggerValue = gs_triggerValueLevel3 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA;
					}
					else
					{
						gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA;
					}
				}
			}
			break;
		case OPERATE_5V:
			if(level == eTriggerSensitivityLevel1)
			{
				gs_currentTriggerValue = gs_triggerValueLevel1 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanLowID)*CMH2O_TO_PA;
			}
			else
			{
				if(level==eTriggerSensitivityLevel2)
				{
					gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanLowID)*CMH2O_TO_PA;
				}
				else
				{
					if(level==eTriggerSensitivityLevel3)
					{
						gs_currentTriggerValue = gs_triggerValueLevel3 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanLowID)*CMH2O_TO_PA;
					}
					else
					{
						gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanLowID)*CMH2O_TO_PA;
					}
				}
			}
			break;
		case OPERATE_9V:
			if(level == eTriggerSensitivityLevel1)
			{
				gs_currentTriggerValue = gs_triggerValueLevel1 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanMidID)*CMH2O_TO_PA;
			}
			else
			{
				if(level==eTriggerSensitivityLevel2)
				{
					gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanMidID)*CMH2O_TO_PA;
				}
				else
				{
					if(level==eTriggerSensitivityLevel3)
					{
						gs_currentTriggerValue = gs_triggerValueLevel3 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanMidID)*CMH2O_TO_PA;
					}
					else
					{
						gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanMidID)*CMH2O_TO_PA;
					}
				}
			}
			break;
		case OPERATE_12V:
			if(level == eTriggerSensitivityLevel1)
			{
				gs_currentTriggerValue = gs_triggerValueLevel1 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanHighID)*CMH2O_TO_PA;
			}
			else
			{
				if(level==eTriggerSensitivityLevel2)
				{
					gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanHighID)*CMH2O_TO_PA;
				}
				else
				{
					if(level==eTriggerSensitivityLevel3)
					{
						gs_currentTriggerValue = gs_triggerValueLevel3 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanHighID)*CMH2O_TO_PA;
					}
					else
					{
						gs_currentTriggerValue = gs_triggerValueLevel2 + (float)setting_OffsetGet(eTriggerSensitivityOffsetID)*CMH2O_TO_PA
														+ (float)setting_OffsetGet(eTriggerSensitivityOffsetFanHighID)*CMH2O_TO_PA;
					}
				}
			}
			break;
		default:
			break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_TestSynchronizeMode							  			*/
/*                                                                                      */
/* Details:   Handle test synchronize mode function										*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*				(O) None																*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
static void breathTrigger_TestSynchronizeMode()
{
	static uint16_t s_cnt_Time = 0;
	static uint16_t s_cnt_OpenTime = 0;
	uint16_t FCopenTime=0;
	gs_breathRate=gs_testSynchronizeModeBPM;
	if(gs_isResetTestSynchronizeMode==true)
	{
		s_cnt_Time=0;
		s_cnt_OpenTime=0;
		gs_isResetTestSynchronizeMode = false;
	}
	FCopenTime = ((((double)g_FlowRateTable[setting_Get(eSynchronizedFlowRateSettingID)]*100.0)/3.0)/(double)gs_testSynchronizeModeBPM)*1;
	FCopenTime += FCopenTime*(setting_OffsetGet(eDischargeVolumeOffsetID)/100.0);
	uint16_t cntTimeBetweenBreath = (ONE_MINUTE/gs_testSynchronizeModeBPM)/DEVICE_TASK_DELAY_TIME;
	if(s_cnt_Time >= cntTimeBetweenBreath)
	{
		s_cnt_Time = 0;
	}
	if(s_cnt_Time == 0)
	{
		DEBUG_SYNCHRONIZE("Test open %d\n",FCopenTime);
		gs_isInhale = true;
		if(flowCtrl_isGetVersionDone()==true)
		{
			flowCtrl_SendOpenAndCloseSettingCmd(&FCopenTime);
		}
		panelLed_LedDisplay(eSynchronizedModeOpen);
		s_cnt_OpenTime=1;
	}
	if(s_cnt_OpenTime>0)
	{
		s_cnt_OpenTime++;
		if(s_cnt_OpenTime >= (FCopenTime/DEVICE_TASK_DELAY_TIME))
		{
			gs_isInhale = false;
			panelLed_LedDisplay(eSynchronizedModeClose);
			s_cnt_OpenTime=0;
		}
	}
	s_cnt_Time++;

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_leftShiftSamples								  			*/
/*                                                                                      */
/* Details:   Left shift all items in array												*/
/*                                                                                      */
/* Arguments:	(I) uint16_t* sample													*/
/*				(O) uint16_t* sample													*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static void breathTrigger_leftShiftSamples(uint16_t* sample)
{
	for(int i=0;i<(NUMBER_OF_SAMPLE-1);i++)
	{
		sample[i]=sample[i+1];
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_clearSamples									  			*/
/*                                                                                      */
/* Details:   Set all items in array to 0												*/
/*                                                                                      */
/* Arguments:	(I) uint16_t* sample													*/
/*				(O) uint16_t* sample													*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static void breathTrigger_clearSamples(uint16_t* sample)
{
	for(int i=0;i<(NUMBER_OF_SAMPLE);i++)
	{
		sample[i]=0;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_DetectBreathMode								  			*/
/*                                                                                      */
/* Details:  	Handle synchronize mode function (Detect inhalation, force the flow 	*/
/*              controller to open, display LED panel)                                  */
/* Arguments:	(I) float cannulaPress													*/
/*				(O) None																*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
static void breathTrigger_DetectBreathMode(float differPressure)
{
	static E_FanVoltage currentFanVoltage = OFF;
	static E_FanVoltage prevFanVoltage = OFF;
	static uint16_t s_cntTimeOut =0;
	static uint16_t s_cnt_OpenTime=0;
	static uint16_t s_cnt_Exhale=0;
	static uint16_t s_currentStage = eIdleStage;
	uint16_t FCopenTime=0;
	static uint16_t s_periodBetweenBreath[NUMBER_OF_SAMPLE]={0};
	static uint16_t s_cnt =0;
	static uint8_t s_i = 0;
	static bool s_isStartCalculatePeriod = false;

	currentFanVoltage = fan_GetFanVoltage();
	if(currentFanVoltage != prevFanVoltage)
	{
		DEBUGOUT("currentFanVoltage..............%d\n",currentFanVoltage);
		prevFanVoltage = currentFanVoltage;
		breathTrigger_UpdateTriggerSensitivityLevel();
	}
	if(gs_isResetDetectBreathMode == true)
	{
		gs_breathRate = DEFAULT_BREATH_RATE;
		s_cnt_OpenTime=0;
		s_cnt_Exhale=0;
		s_currentStage = eIdleStage;
		gs_isResetDetectBreathMode = false;
		gs_isInhale = false;
		s_isStartCalculatePeriod=false;
		breathTrigger_clearSamples(s_periodBetweenBreath);
		s_i=0;
		s_isStartCalculatePeriod=false;
		s_cntTimeOut=0;
	}
	if(s_isStartCalculatePeriod==true)
	{
		s_cnt++;
	}
	else
	{
		s_cnt=0;
	}
	if(s_i == (NUMBER_OF_SAMPLE-1))
	{
		float totalTime = 0;
		for(int i=0;i<NUMBER_OF_SAMPLE;i++)
		{
			totalTime+=s_periodBetweenBreath[i];
		}
		gs_breathRate = ONE_MINUTE/((totalTime*(float)DEVICE_TASK_DELAY_TIME)/NUMBER_OF_SAMPLE);
		if(gs_breathRate<MIN_BREATH_RATE)
		{
			gs_breathRate = DEFAULT_BREATH_RATE;
		}
	}
	else
	{
		gs_breathRate = DEFAULT_BREATH_RATE;
	}
	FCopenTime = ((((double)g_FlowRateTable[setting_Get(eSynchronizedFlowRateSettingID)]*100.0)/3.0)/(double)gs_breathRate)*1;
	FCopenTime += FCopenTime*(setting_OffsetGet(eDischargeVolumeOffsetID)/100.0);
	switch(s_currentStage)
	{
	case eIdleStage:
		if((differPressure<=gs_currentTriggerValue))
		{
			s_cntTimeOut++;
			gs_triggerPressure = differPressure;
			if(s_isStartCalculatePeriod ==false)
			{
				s_isStartCalculatePeriod=true;
			}
			else
			{
				if(s_i == (NUMBER_OF_SAMPLE-1))
				{
					breathTrigger_leftShiftSamples(s_periodBetweenBreath);
				}
				s_periodBetweenBreath[s_i]=s_cnt;
				if(s_i<(NUMBER_OF_SAMPLE-1))
				{
					s_i++;
				}
				else
				{
					s_i=(NUMBER_OF_SAMPLE-1);
				}
				s_cnt = 0;
			}
			gs_isInhale=true;
			DEBUG_SYNCHRONIZE("Open %d\n",FCopenTime);
			if(flowCtrl_isGetVersionDone()==true)
			{
				flowCtrl_SendOpenAndCloseSettingCmd(&FCopenTime);
			}
			panelLed_LedDisplay(eSynchronizedModeOpen);
			s_cnt_OpenTime=0;
			s_cnt_Exhale=0;
			s_currentStage=eInhaleStage;
		}
		break;
	case eInhaleStage:
		s_cnt_OpenTime++;
		if((s_cnt_OpenTime >=(FCopenTime/DEVICE_TASK_DELAY_TIME))/*||((differPressure<=(0.14*CMH2O_TO_PA)))*/)
		{
			gs_isInhale=false;
			s_currentStage = eExhaleStage;
			panelLed_LedDisplay(eSynchronizedModeClose);
		}
		break;
	case eExhaleStage:
		s_cnt_Exhale++;
		if(s_cnt_Exhale >= NOISE_CANCEL_LOOP_CNT)
		{
			if(differPressure > gs_currentTriggerLevel1Value)
			{
				s_currentStage = eIdleStage;
			}
		}
		break;
	default:
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_Handle											  			*/
/*                                                                                      */
/* Details:   pressure handle function is call for checking whether user is inhaling to */
/*           make the flow controller open every 20ms									*/
/*            calculate breath rate								                        */
/* Arguments:	(I) float cannualaPress													*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_Handle(float cannulaPress)
{
	if(gs_isStop==false)
	{
		if(setting_Get(eModeSettingID)==eSynchronizedMode)
		{
			if(gs_testSynchronizeModeBPM==0)
				breathTrigger_DetectBreathMode(cannulaPress);
			else
				breathTrigger_TestSynchronizeMode();
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_GetTriggerPressure								  			*/
/*                                                                                      */
/* Details:   get latest trigger pressure												*/
/*                                                                                      */
/* Arguments:	(I) NONE																*/
/* ReturnValue : Float				                                                    */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
float breathTrigger_GetTriggerPressure()
{
	return gs_triggerPressure;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_Stop											  			*/
/*                                                                                      */
/* Details:   Stop detect breath 														*/
/*                                                                                      */
/* Arguments:	(I) NONE																*/
/* ReturnValue : NONE				                                                    */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_Stop()
{
	gs_isStop = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_GetBreathRate									  			*/
/*                                                                                      */
/* Details:   Get BPM			 														*/
/*                                                                                      */
/* Arguments:	(I) NONE																*/
/* ReturnValue : uint8_t			                                                    */
/*                                                                                      */
/****************************************************************************************/
uint8_t breathTrigger_GetBreathRate()
{
	return gs_breathRate;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_SetTestSynchronizeMode							  			*/
/*                                                                                      */
/* Details:   Set BPM of Synchronize Test Mote											*/
/*                                                                                      */
/* Arguments:	(I) uint8_t breathPerMinute												*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_SetTestSynchronizeMode(uint8_t breathPerMinute)
{
	gs_testSynchronizeModeBPM = breathPerMinute;
	if(gs_testSynchronizeModeBPM == 0)
	{
		gs_isResetDetectBreathMode = true;

	}
	else
	{
		gs_isResetTestSynchronizeMode = true;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ResetDetectBreathMode							  			*/
/*                                                                                      */
/* Details:   Reset all counter in Detect Breath Mode									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*				(O) None																*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_ResetDetectBreathMode()
{
	gs_isResetDetectBreathMode = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	pressure_ResetTestSynchronizeMode						  			*/
/*                                                                                      */
/* Details:   Reset all counter in Test Synchronize Mode								*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*				(O) None																*/
/* ReturnValue : void				                                                    */
/*                                                                                      */
/****************************************************************************************/
void breathTrigger_ResetTestSynchronizeMode()
{
	gs_isResetTestSynchronizeMode = true;
}




