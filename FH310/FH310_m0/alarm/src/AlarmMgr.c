/********************************************************************/
/*                                                                  */
/* File Name    : AlarmMrg.c                                    	*/
/*                                                                  */
/* General      : AlarmMrg provide all of methods for alarm		    */
/*                task processing							     	*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/09     Linh Nguyen(MV)  	    new file        */
/* #0001       2017/08/25	  Linh Nguyen(MV)	    modify          */
/*       remove check NVRAM in alarmMgr_ClearAllAlarm()				*/
/*		 remove function alarmMgr_CheckSpeakerAlarmStatus()         */
/* #0002       2017/09/01     Linh Nguyen(MV)       modify			*/
/*		 stop increase time when it is full	         				*/
/* #0003	   2017/09/01	  Quyen Ngo(MV)			modify			*/
/*		 add prefix FC to Err status of Flow controller				*/
/* #0004       2016/09/05      Linh Nguyen	  	    modify          */
/*	remove detection the flow controller err in the flow controller */
/*	communicates alarm												*/
/* #0005       2016/11/03      Linh Nguyen	  	    modify          */
/*	Fix wrong compressor alarm ID (bug #2014) 						*/
/* 		       2017/11/17      Linh Nguyen	  	    modify          */
/*	#0006 Remove unuse value (coveriy)		 						*/
/*	#0007 Fix bug minor cannla alarm does not clear (bug 2028)		*/
/*	#0008 Fix bug wrong timing of flowrate alarm (bug 2029)			*/
/* 		       2017/11/20      Linh Nguyen	  	    modify          */
/*	#0009 Add {} (bug 2035)		 									*/
/* 		       2017/12/13      Linh Nguyen	  	    modify          */
/*	#0010 Add {} (coding rule)	 									*/
/********************************************************************/
/* include system files */

#include <AlarmMgr.h>
#include <AlarmTask.h>
#include <board_api.h>
#include <DualCoreCommon.h>
#include <IpcTask.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include "DeviceInterface.h"
#include "FlowController.h"
#include "Compressor.h"
#include "IpcMsg.h"

/* constants, macro definition */
#define SCALE_VALUE 100.0

/* definitions of structures */
#define DEBUG_ALARM_EN

#ifdef DEBUG_ALARM_EN
#define DEBUG_ALARM(...) printf(__VA_ARGS__)
#else
#define DEBUG_ALARM(...)
#endif

/*static global variable*/
static ALARM_STAT_T stAlarmList[NumberOfAlarm];
static ALARM_MONITOR_t gs_currentAlarmMonitor;

static int16_t gs_previousFlowRateSetting;

/*counter*/
static uint32_t gs_temperatureAlarmCounter =0;
static uint32_t gs_highPressureAlarmCounter =0;
static uint32_t gs_lowPressureAlarmCounter =0;
static uint32_t gs_changeFlowRateSettingTimer =0;
static uint32_t gs_coolingFanAlarmCounter =0;
static uint32_t gs_O2ConcentrationAlarmCounter =0;
static uint32_t gs_loweringO2ConcentrationAlarmCounter =0;
static uint32_t gs_O2ConcentrationSensorCounter =0;
static uint32_t gs_cannulaAlarmUnder27sCounter =0;
static uint32_t gs_cannulaMore27sAlarmCounter =0;
static uint32_t gs_compressorOutletTemperatureAlarmCounter =0;
static uint32_t gs_risingCompressorOutletTemperatureAlarmCounter =0;
static uint32_t gs_flowRateAlarmCounter =0;
static uint32_t gs_externalFlowPathpressureAlarmCounter=0;

//these values have been scale by 100
static const int16_t gs_flowRateSetting[FR_MAX_VALUE]={25, 50, 75, 100, 125, 150, 175, 200, 250, 300};
//threshold value of cannunla pressure
static const int16_t gs_underFlowRateValue[FR_MAX_VALUE] = {5, 30, 55, 80, 105, 130, 155, 180, 225, 270};
static const int16_t gs_upperFlowRateValue[FR_MAX_VALUE] = {45, 75, 95, 120, 145, 170, 195, 220, 275, 330};
/* external decralation */
//uint32_t alarmTaskTimer =0; //timer to counter running time
//E_ModeSetting previousOperationMode;
//E_ModeSetting currentOperationMode;
/* Body of functions */

/****************************************************************************************/
/*                                                                                      */
/* Funtion name	: alarmMgr_InitAlarm	                                                */
/*                                                                                      */
/* Details	 	:  create and initialize default value for all alarm         			*/
/*                                                                                      */
/* Global valiables   : (O)stAlarmList                                                  */
/*?Note?                                                                             	*/
/*                                                                                      */
/****************************************************************************************/
void alarmMgr_InitAlarm(void)
{
	for(int i = eFirsAlarmId; i< eLastAlarmId ; i++)
	{
		stAlarmList[i].ID = (E_AlarmId)i;
		stAlarmList[i].previousStatus = eInactive;
		stAlarmList[i].currentStatus = eInactive;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_UpdateAlarmMonitor					 	                    */
/*                                                                                      */
/* Details		:       update current  value of all alarm monitor	for device task		*/
/*                   							                                        */
/* Using Global Data : (O) 	gs_currentAlarmMonitor                             			*/
/*					   (O)  gs_previousFlowRateSetting									*/
/*					   (O)  g_currentOperationMode										*/
/*					   (O)  g_previousOperationMode										*/
/*					   (O)  gs_isChangeFlowRateSetting									*/
/****************************************************************************************/
void alarmMgr_UpdateAlarmMonitor(void)
{
	gs_previousFlowRateSetting = gs_currentAlarmMonitor.flowRateSetting;
	gs_currentAlarmMonitor = devInterface_GetAlarmMonitor();
	g_previousOperationMode = g_currentOperationMode;
	g_currentOperationMode = gs_currentAlarmMonitor.operationMode;
	if (gs_currentAlarmMonitor.flowRateSetting != gs_previousFlowRateSetting)
	{
		gs_changeFlowRateSettingTimer =0;
		DEBUG_ALARM("Change flow rate setting --> %ds %d\n", g_alarmTaskTimer/20, gs_currentAlarmMonitor.flowRateSetting);
	}
	else if(gs_changeFlowRateSettingTimer != UINT32_MAX)
	{
		gs_changeFlowRateSettingTimer++;
	}
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_ClearAllAlarm			                                    */
/*                                                                                      */
/* Details		: 	sets the current status and previous status of all alarms to     	*/
/*                  default value (eInActive) except Anypal alarm.           			*/
/*                                                                                      */
/* Using Global Data : (O) stAlarmList	                                              	*/
/*                     (O) gs_temperatureAlarmCounter                      				*/
/*                     (O) gs_highPressureAlarmCounter                      			*/
/*                     (O) gs_lowPressureAlarmCounter                      				*/
/*                     (O) gs_changeFlowRateSettingTimer                      			*/
/*                     (O) gs_coolingFanAlarmCounter                      				*/
/*                     (O) gs_O2ConcentrationAlarmCounter                      			*/
/*                     (O) gs_O2ConcentrationSensorCounter                      		*/
/*                     (O) gs_cannulaAlarmUnder27sCounter                      			*/
/*                     (O) gs_cannulaMore27sAlarmCounter                      			*/
/*                     (O) gs_compressorOutletTemperatureAlarmCounter                 	*/
/*                     (O) gs_risingCompressorOutletTemperatureAlarmCounter          	*/
/*                     (O) gs_flowRateAlarmCounter                     					*/
/*                     (O) gs_externalFlowPathpressureAlarmCounter                      */
/*                                                                                      */
/****************************************************************************************/
void alarmMgr_ClearAllAlarm(void)
{
	for(int i = eFirsAlarmId; i< eLastAlarmId ; i++)
	{
		stAlarmList[i].previousStatus = eInactive;
		stAlarmList[i].currentStatus = eInactive;
	}

	gs_temperatureAlarmCounter =0;
	gs_highPressureAlarmCounter =0;
	gs_lowPressureAlarmCounter =0;
	gs_changeFlowRateSettingTimer =0;
	gs_coolingFanAlarmCounter =0;
	gs_O2ConcentrationAlarmCounter =0;
	gs_loweringO2ConcentrationAlarmCounter =0;
	gs_O2ConcentrationSensorCounter =0;
	gs_cannulaAlarmUnder27sCounter =0;
	gs_cannulaMore27sAlarmCounter =0;
	gs_compressorOutletTemperatureAlarmCounter =0;
	gs_risingCompressorOutletTemperatureAlarmCounter =0;
	gs_flowRateAlarmCounter =0;
	gs_externalFlowPathpressureAlarmCounter=0;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckTemperatureAlarmStatus	                 			*/
/*                                                                                      */
/* Details:         check status of Temperature Alarm     								*/
/*                                                                                      */
/* Arguments : (I)  - float currentTemperature          		                      	*/
/*                  - range : -30 - 150                                                 */
/*                  - unit 	: deg C                                                     */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_temperatureAlarmCounter 									*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckTemperatureAlarmStatus(float currentTemperature)
{
	if (stAlarmList[eTemperatureId].currentStatus == eInactive)
	{

		if ((currentTemperature >= HIGH_UNIT_INTERNAL_TEMPERATURE) ||
				(currentTemperature <= LOW_UNIT_INTERNAL_TEMPERATURE))
		{
			if (gs_temperatureAlarmCounter == TEMPERATURE_ALARM_TIMEOUT)//10s
			{
				stAlarmList[eTemperatureId].currentStatus = eActive;
				DEBUG_ALARM("Temperature Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_temperatureAlarmCounter++;
		}
		else
		{
			gs_temperatureAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckHighPressureAlarmStatus	                 			*/
/*                                                                                      */
/* Details:         check status of High Pressure Alarm     							*/
/*                                                                                      */
/* Arguments : (I)  float currentTemperature          		                      		*/
/*                  - range : 0 - 200                                                   */
/*                  - unit 	: kPa	                                                    */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_highPressureAlarmCounter 									*/
/*                     (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckHighPressureAlarmStatus(float currentPressure)
{
	if (stAlarmList[eHighPressureId].currentStatus == eInactive)
	{
		if ((currentPressure >= PRODUCT_TANK_HIGH_PRESSURE) &&
				(g_alarmTaskTimer >= START_HIGH_PRESSURE_ALARM_DETECT))//10s
		{
			if (gs_highPressureAlarmCounter == HIGH_PRESSURE_ALARM_TIMEOUT)
			{
				stAlarmList[eHighPressureId].currentStatus = eActive;
				DEBUG_ALARM("High Pressure Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_highPressureAlarmCounter++;
		}
		else
		{
			gs_highPressureAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckLowPressureAlarmStatus	                 			*/
/*                                                                                      */
/* Details:         check status of Low Pressure Alarm    								*/
/*                                                                                      */
/* Arguments : (I)  float currentPressure          		                      			*/
/*                  - range : 0 - 200                                                   */
/*                  - unit 	: kPa	                                                    */
/*			   (I)E_PsaSequence psaSequence												*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_lowPressureAlarmCounter  									*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckLowPressureAlarmStatus(float currentPressure,
		E_PsaMode psaMode)
{
	if (stAlarmList[eLowPressureId].currentStatus == eInactive)
	{
		if ((currentPressure <= PRODUCT_TANK_LOW_PRESSURE)&&(psaMode == PSA_NORMAL_MODE))
		{
			if ((gs_lowPressureAlarmCounter >= LOW_PRESSURE_ALARM_TIMEOUT))//30S
			{
				stAlarmList[eLowPressureId].currentStatus = eActive;
				DEBUG_ALARM("Low Pressure Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_lowPressureAlarmCounter++;
		}
		else
		{
			gs_lowPressureAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorDCOverVoltageAlarmStatus	                */
/*                                                                                      */
/* Details:         check status of Compressor DC Over Voltage Alarm					*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorDCOverVoltageAlarmStatus(uint16_t compressorStatus)
{
	uint16_t compressorErr = compressorStatus & COMP_ABNORMAL_DC_VOLT_BIT;
	if (stAlarmList[eCompressorDCOverVoltageId].currentStatus == eInactive)
	{
		if (compressorErr == COMP_ABNORMAL_DC_VOLT_BIT)
		{
			stAlarmList[eCompressorDCOverVoltageId].currentStatus = eActive;
			DEBUG_ALARM("Compressor DC Over Voltage Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorSoftwareOverCurrentAlarmStatus	            */
/*                                                                                      */
/* Details:         check status of Compressor Software Over Current Alarm				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorSoftwareOverCurrentAlarmStatus(uint16_t compressorStatus)
{
	uint16_t compressorErr = compressorStatus & COMP_ABNORMAL_SW_OVERCURRENT_BIT;
	if (stAlarmList[eCompressorSoftwareOverCurrentId].currentStatus == eInactive)
	{
		if (compressorErr == COMP_ABNORMAL_SW_OVERCURRENT_BIT)
		{
			stAlarmList[eCompressorSoftwareOverCurrentId].currentStatus = eActive;
			DEBUG_ALARM("Compressor Software Over Current Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorBoardTemperatureAlarmStatus	                */
/*                                                                                      */
/* Details:         check status of Compressor Board Temperature Alarm     				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorBoardTemperatureAlarmStatus(uint16_t compressorStatus)
{
	uint16_t compressorErr = compressorStatus & COMP_ABNORMAL_TEMP_ERR_BIT;
	if (stAlarmList[eCompressorBoardTemperatureId].currentStatus == eInactive)
	{
		if (compressorErr == COMP_ABNORMAL_TEMP_ERR_BIT)
		{
			stAlarmList[eCompressorBoardTemperatureId].currentStatus = eActive;
			DEBUG_ALARM("Compressor Board Temperature Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorRPMAlarmStatus	           					*/
/*                                                                                      */
/* Details:         check status of Compressor RPM  Alarm     							*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorRPMAlarmStatus(uint16_t compressorStatus)
{
	uint16_t compressorErr = compressorStatus & COMP_ROTAION_SPEED_ERR_BIT;
	if (stAlarmList[eCompressorRPMId].currentStatus == eInactive)
	{
		if (compressorErr == COMP_ROTAION_SPEED_ERR_BIT)
		{
			stAlarmList[eCompressorRPMId].currentStatus = eActive;
			DEBUG_ALARM("Compressor Motor Temperature Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorHardwareOverCurrentAlarmStatus	   		    */
/*                                                                                      */
/* Details:         check status of  Compressor hardware Over Current Alarm				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorHardwareOverCurrentAlarmStatus(uint16_t compressorStatus)
{
	uint16_t compressorErr = compressorStatus & COMP_ABNORMAL_HW_OVERCURRENT_BIT;
	if (stAlarmList[eCompressorHardwareOverCurrent].currentStatus == eInactive)
	{
		if (compressorErr == COMP_ABNORMAL_HW_OVERCURRENT_BIT)
		{
			stAlarmList[eCompressorHardwareOverCurrent].currentStatus = eActive;
			DEBUG_ALARM("Compressor hardware Over Current Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCoolingFanAlarmStatus	                 				*/
/*                                                                                      */
/* Details:         check status of Cooling Fan Alarm    								*/
/*                                                                                      */
/* Arguments : (I)  bool coolingFanRotationState				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_coolingFanAlarmCounter  									*/
/*                     (I) stAlarmList													*/
/*                     (I) gs_startCheckCoolingFanAlarm                                 */
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCoolingFanAlarmStatus(bool coolingFanRotationState)
{
	if (stAlarmList[eCoolingFanId].currentStatus == eInactive)
	{
		if (coolingFanRotationState == true)
		{
			if (gs_coolingFanAlarmCounter >= COOLING_FAN_ALARM_TIMEOUT)
			{
				stAlarmList[eCoolingFanId].currentStatus = eActive;
				DEBUG_ALARM("Cooling Fan Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_coolingFanAlarmCounter++;
		}
		else
		{
			gs_coolingFanAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckO2ConcentrationAlarmStatus	                 		*/
/*                                                                                      */
/* Details:         check status of O2 Concentration Alarm    							*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration				        						*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_O2ConcentrationAlarmCounter  								*/
/*                     (I) stAlarmList													*/
/*                     (I) g_alarmTaskTimer												*/
/*                     (I) gs_changeFlowRateSettingTimer								*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckO2ConcentrationAlarmStatus(int16_t O2Concentration)
{
	if (stAlarmList[eO2ConcentrationId].currentStatus == eInactive)
	{
		if (((float)(O2Concentration/SCALE_VALUE) < O2_CONCENTRATION_75_PERCENT)
				&& (g_alarmTaskTimer > START_O2_CONCENTRATION_ALARM_DETECT) // 1S*20*60 = 20MIN
				&& (gs_changeFlowRateSettingTimer > O2_CONCENTRATION_ALARM_CHANGE_FR)) // 1S*20*60 = 20MIN
		{
			if (gs_O2ConcentrationAlarmCounter >= O2_CONCENTRATION_ALARM_TIMEOUT) //1*30*60 = 3MIN
			{
				stAlarmList[eO2ConcentrationId].currentStatus = eActive;
				DEBUG_ALARM("O2 Concentration Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_O2ConcentrationAlarmCounter++;
		}
		else
		{
			gs_O2ConcentrationAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCannulaUnder27sAlarmStatus		               		*/
/*                                                                                      */
/* Details:         check status of Cannula Under 27s Alarm    							*/
/*                                                                                      */
/* Arguments : (I)  float cannulaPressure				        						*/
/*                  - range : 0 - 100                                                   */
/*                  - unit 	: kPa	                                                    */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_cannulaAlarmUnder27sCounter		  						*/
/*                     (I) stAlarmList													*/
/* local variables   : (I) flowRateIndex             			              			*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCannulaUnder27sAlarmStatus(float cannulaPressure)
{
	if (gs_changeFlowRateSettingTimer == 0)
	{
		gs_cannulaAlarmUnder27sCounter =0;
	}
	if (stAlarmList[eCannulaUnder27sId].currentStatus == eInactive)
	{
		if (cannulaPressure >= CANNULA_HIGHT_PRESSURE)
		{
			if ((gs_cannulaAlarmUnder27sCounter >= CANNULA_UNDER_27S_ALARM_TIMEOUT) //3S
					&& (gs_cannulaAlarmUnder27sCounter <= CANNULA_MORE_27S_ALARM_TIMEOUT))//27S
			{
				stAlarmList[eCannulaUnder27sId].currentStatus = eActive;
				DEBUG_ALARM("Cannula Under 27s Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_cannulaAlarmUnder27sCounter++;
		}
		else
		{
			gs_cannulaAlarmUnder27sCounter =0;
		}
	}
	else if (stAlarmList[eCannulaUnder27sId].currentStatus == eActive)
	{
		if ((cannulaPressure < CANNULA_HIGHT_PRESSURE) ||
				(gs_changeFlowRateSettingTimer == 0))
		{
			stAlarmList[eCannulaUnder27sId].currentStatus = eInactive;
			DEBUG_ALARM("Cannula Under 27s Alarm INActive--> %ds\n", g_alarmTaskTimer/20);
		}
		gs_cannulaAlarmUnder27sCounter =0;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCannulaMore27sAlarmStatus			               		*/
/*                                                                                      */
/* Details:         check status of Cannula more 27s Alarm    							*/
/*                                                                                      */
/* Arguments : (I)  float cannulaPressure				        						*/
/*                  - range : 0 - 200                                                   */
/*                  - unit 	: kPa	                                                    */
/*             (I)  int16_t flowRateSetting             	                            */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_cannulaMore27sAlarmCounter		  						*/
/*                     (I) stAlarmList													*/
/* local variables   : (I) flowRateIndex             			              			*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCannulaMore27sAlarmStatus(float cannulaPressure)
{

	if (gs_changeFlowRateSettingTimer == 0)
	{
		gs_cannulaMore27sAlarmCounter =0;
	}

	if (stAlarmList[eCannulaMore27sId].currentStatus == eInactive)
	{
		if (cannulaPressure >= CANNULA_HIGHT_PRESSURE)
		{
			if (gs_cannulaMore27sAlarmCounter > CANNULA_MORE_27S_ALARM_TIMEOUT)
			{
				stAlarmList[eCannulaMore27sId].currentStatus = eActive;
				DEBUG_ALARM("Cannula more 27s Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_cannulaMore27sAlarmCounter++;
		}
		else
		{
			gs_cannulaMore27sAlarmCounter =0;
		}
	}
	else if (stAlarmList[eCannulaMore27sId].currentStatus == eActive)
	{
		if ((cannulaPressure < CANNULA_HIGHT_PRESSURE) ||
				(gs_changeFlowRateSettingTimer == 0))
		{
			stAlarmList[eCannulaMore27sId].currentStatus = eInactive;
			gs_cannulaMore27sAlarmCounter =0;
			DEBUG_ALARM("Cannula more 27s Alarm INActive--> %ds\n", g_alarmTaskTimer/20);
		}
		gs_cannulaMore27sAlarmCounter++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckLoweringO2ConcentrationAlarmStatus               		*/
/*                                                                                      */
/* Details:         check status of Lowering O2 Concentration Alarm    					*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration				        						*/
/*                  - range : 0 - 100                                                   */
/*                  - unit 	: %	                                                    	*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_loweringO2ConcentrationAlarmCounter  						*/
/*                     (I) stAlarmList													*/
/*                     (I) g_alarmTaskTimer												*/
/*                     (I) gs_changeFlowRateSettingTimer								*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckLoweringO2ConcentrationAlarmStatus (int16_t O2Concentration)
{
	if (stAlarmList[eLoweringO2ConcentrationId].currentStatus == eInactive)
	{
		if (((float)(O2Concentration/SCALE_VALUE) <= O2_CONCENTRATION_82_PERCENT)
				&& (g_alarmTaskTimer > START_LOWERING_O2_ALARM_DETECT) //20*60*1S = 20MIN
				&& (gs_changeFlowRateSettingTimer > LOWERING_O2_ALARM_CHANGE_FR)) //20*60*1S = 20MIN
		{
			if (gs_loweringO2ConcentrationAlarmCounter >= LOWERING_O2_ALARM_TIMEOUT)//3*60*1S = 3MIN
			{
				stAlarmList[eLoweringO2ConcentrationId].currentStatus = eActive;
				DEBUG_ALARM("Lowering O2 Concentration Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_loweringO2ConcentrationAlarmCounter++;
		}
		else
		{
			gs_loweringO2ConcentrationAlarmCounter =0;
		}
	}
	else if (stAlarmList[eLoweringO2ConcentrationId].currentStatus == eActive)
	{
		if ((O2Concentration/SCALE_VALUE) > O2_CONCENTRATION_82_PERCENT)
		{
			if (gs_loweringO2ConcentrationAlarmCounter >= STOP_LOWERING_O2_ALARM_TIMEOUT)//10S
			{
				stAlarmList[eLoweringO2ConcentrationId].currentStatus = eInactive;
				DEBUG_ALARM("Lowering O2 Concentration Alarm INActive--> %ds\n", g_alarmTaskTimer/20);
			}
			gs_loweringO2ConcentrationAlarmCounter++;
		}
		else
		{
			gs_loweringO2ConcentrationAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckO2ConcentrationSensorAlarmStatus               		*/
/*                                                                                      */
/* Details:         check status of O2 Concentration Sensor Alarm    					*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration				        						*/
/*                  - range : 0 - 200                                                   */
/*                  - unit 	: kPa	                                                    */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_O2ConcentrationSensorCounter  							*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckO2ConcentrationSensorAlarmStatus(int16_t O2Concentration)
{
	if (stAlarmList[eO2ConcentrationSensorId].currentStatus == eInactive)
	{
		if (((float)(O2Concentration/SCALE_VALUE) > O2_CONCENTRATION_98_PERCENT)
				|| ((O2Concentration/SCALE_VALUE) < O2_CONCENTRATION_10_PERCENT))
		{
			if (gs_O2ConcentrationSensorCounter >= O2_CONCENTRATION_SENSOR_ALARM_TIMEOUT)
			{
				stAlarmList[eO2ConcentrationSensorId].currentStatus = eActive;
				DEBUG_ALARM("O2 Concentration Sensor Alarm Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_O2ConcentrationSensorCounter++;
		}
		else
		{
			gs_O2ConcentrationSensorCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowRateAlarmStatus			                		*/
/*                                                                                      */
/* Details:         check status of Flow Rate Alarm    									*/
/*                                                                                      */
/* Arguments : (I)  int16_t flowRateSensor				        						*/
/*                  - range : 0 -999	                                                */
/*                  - unit 	: (L/min) *100                                              */
/*             (I)  int16_t flowRateSetting		                                        */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_flowRateAlarmCounter  									*/
/*                     (I) stAlarmList													*/
/*                     (I) g_alarmTaskTimer												*/
/*                     (I) gs_changeFlowRateSettingTimer								*/
/* local variables   : (I) flowRateIndex             			              			*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowRateAlarmStatus(int16_t flowRateSensor,
		int16_t flowRateSetting)
{
	uint8_t flowRateIndex;
	if ((stAlarmList[eCannulaUnder27sId].currentStatus == eInactive)&&
			(stAlarmList[eCannulaMore27sId].currentStatus == eInactive))
	{
		for(flowRateIndex =0; flowRateIndex <FR_MAX_VALUE; flowRateIndex++)
		{
			if(flowRateSetting == gs_flowRateSetting[flowRateIndex])
			{
				break;
			}
		}

		if (gs_changeFlowRateSettingTimer == 0)
		{
			gs_flowRateAlarmCounter  =0;
		}

		if ((gs_changeFlowRateSettingTimer >= FLOW_RATE_ALARM_CHANGE_FR)//60S
				&& (g_alarmTaskTimer >= START_FLOW_RATE_ALARM_DETECT))//60S
		{
			if (stAlarmList[eFlowRateId].currentStatus == eInactive)
			{
				if ((flowRateSensor < gs_underFlowRateValue[flowRateIndex])||
						flowRateSensor > gs_upperFlowRateValue[flowRateIndex])
				{
					if (gs_flowRateAlarmCounter  >= FLOW_RATE_ALARM_TIMEOUT)//40S
					{
						stAlarmList[eFlowRateId].currentStatus = eActive;
						DEBUG_ALARM("Flow Rate Alarm Active --> %ds\n", g_alarmTaskTimer/20);
					}
					gs_flowRateAlarmCounter ++;
				}
				else
				{
					gs_flowRateAlarmCounter =0;
				}
			}
		}
	}
	else
	{
		gs_flowRateAlarmCounter =0;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFilterCleaningAlarmStatus        						*/
/*                                                                                      */
/* Details:         check status of Filter Cleaning Alarm    							*/
/*                                                                                      */
/* Arguments : (I)  bool isPhotointerrupterSignalOk				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFilterCleaningAlarmStatus(bool isPhotointerrupterSignalOk)
{
	if (stAlarmList[eFilterCleaningId].currentStatus == eInactive)
	{
		if (isPhotointerrupterSignalOk == true)
		{
			stAlarmList[eFilterCleaningId].currentStatus = eActive;
			DEBUG_ALARM("Filter Cleaning Alarm Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorOutletTemperatureAlarmStatus      			*/
/*                                                                                      */
/* Details:         check status of Compressor Outlet Temperature Alarm   				*/
/*                                                                                      */
/* Arguments : (I)  float compressorOutletTemperature          		                	*/
/*                  - range : -30 - 150                                                 */
/*                  - unit 	: deg C                                                     */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_compressorOutletTemperatureAlarmCounter 					*/
/*                     (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckCompressorOutletTemperatureAlarmStatus(float compressorOutletTemperature)
{
	if (stAlarmList[eCompressorOutletTemperatureId].currentStatus == eInactive)
	{
		if ((compressorOutletTemperature >= COMPRESSOR_OUTLET_HIGH_TEMPERATURE)||
				compressorOutletTemperature <= COMPRESSOR_OUTLET_LOW_TEMPERATURE)
		{
			if (gs_compressorOutletTemperatureAlarmCounter == COMPRESSOR_OUTLET_ALARM_TIMEOUT)//5S
			{
				stAlarmList[eCompressorOutletTemperatureId].currentStatus = eActive;
				DEBUG_ALARM("Compressor Outlet Temperature Communication Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_compressorOutletTemperatureAlarmCounter++;
		}
		else
		{
			gs_compressorOutletTemperatureAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckRisingCompressorOutletTemperatureAlarmStatus 			*/
/*                                                                                      */
/* Details:         check status of Rising Compressor Outlet Temperature Alarm   		*/
/*                                                                                      */
/* Arguments : (I)  float compressorOutletTemperature          		                	*/
/*                  - range : -30 - 150                                                 */
/*                  - unit 	: deg C                                                     */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_risingCompressorOutletTemperatureAlarmCounter				*/
/*                     (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckRisingCompressorOutletTemperatureAlarmStatus(float compressorOutletTemperature)
{
	if (stAlarmList[eRisingCompressorOutletTemperatureId].currentStatus == eInactive)
	{
		if (compressorOutletTemperature >= RISSING_COMPRESSOR_OUTLET_HIGH_TEMPERATURE)
		{
			if (gs_risingCompressorOutletTemperatureAlarmCounter == RISING_COMPRESSOR_OUTLET_ALARM_TIMEOUT)//5S
			{
				stAlarmList[eRisingCompressorOutletTemperatureId].currentStatus = eActive;
				DEBUG_ALARM("Rising Compressor Outlet Temperature Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_risingCompressorOutletTemperatureAlarmCounter++;
		}
		else
		{
			gs_risingCompressorOutletTemperatureAlarmCounter =0;
		}
	}
	else if (stAlarmList[eRisingCompressorOutletTemperatureId].currentStatus == eActive)
	{
		if (compressorOutletTemperature < RISSING_COMPRESSOR_OUTLET_HIGH_TEMPERATURE)
		{
			stAlarmList[eRisingCompressorOutletTemperatureId].currentStatus = eInactive;
			DEBUG_ALARM("Rising Compressor Outlet Temperature INActive --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationAlarmStatus 				*/
/*                                                                                      */
/* Details:         check status of Flow Controller Communication Alarm    				*/
/*                                                                                      */
/* Arguments : (I)  E_FlowControllerStatus flowControllerStatus        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowControllerCommunicationAlarmStatus(uint8_t flowControllerStatus)
{
	uint8_t flowControlErr = flowControllerStatus & FC_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eFlowControllerCommunicationId].currentStatus == eInactive)
	{
		if (flowControlErr == FC_COMMUNICATION_ERR_BIT)
		{
			stAlarmList[eFlowControllerCommunicationId].currentStatus = eActive;
			DEBUG_ALARM("Flow Controller Communication Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerPressureSensorAlarmStatus 				*/
/*                                                                                      */
/* Details:         check status of Flow Controller Pressure Sensor Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  E_FlowControllerStatus flowControllerStatus        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowControllerPressureSensorAlarmStatus(uint8_t flowControllerStatus)
{
	uint8_t flowControlErr = flowControllerStatus & FC_PRESSURE_SENSOR_ERR_BIT;
	if (stAlarmList[eFlowControllerPressureSensorId].currentStatus == eInactive)
	{
		if (flowControlErr  == FC_PRESSURE_SENSOR_ERR_BIT)
		{
			stAlarmList[eFlowControllerPressureSensorId].currentStatus = eActive;
			DEBUG_ALARM("Flow Controller Pressure Sensor Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerUltrasonicSensorAlarmStatus 			*/
/*                                                                                      */
/* Details:         check status of Flow Controller Ultrasonic Sensor Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  E_FlowControllerStatus flowControllerStatus        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowControllerUltrasonicSensorAlarmStatus(uint8_t flowControllerStatus)
{
	uint8_t flowControlErr = flowControllerStatus & FC_ULTRASONIC_SENSOR_ERR_BIT;
	if (stAlarmList[eFlowControllerUltrasonicSensorId].currentStatus == eInactive)
	{
		if (flowControlErr  == FC_ULTRASONIC_SENSOR_ERR_BIT)
		{
			stAlarmList[eFlowControllerUltrasonicSensorId].currentStatus = eActive;
			DEBUG_ALARM("Flow Controller Ultrasonic Sensor Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerTemperatureSensorAlarmStatus 			*/
/*                                                                                      */
/* Details:         check status of Flow Controller Temperature Sensor Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  E_FlowControllerStatus flowControllerStatus        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowControllerTemperatureSensorAlarmStatus(uint8_t flowControllerStatus)
{
	uint8_t flowControlErr = flowControllerStatus & FC_TEMPERATURE_SENSOR_ERR_BIT;
	if (stAlarmList[eFlowControllerTemperatureSensorId].currentStatus == eInactive)
	{
		if (flowControlErr  == FC_TEMPERATURE_SENSOR_ERR_BIT)
		{
			stAlarmList[eFlowControllerTemperatureSensorId].currentStatus = eActive;
			DEBUG_ALARM("Flow Controller Temperature Sensor Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommandTimeOutAlarmStatus 				*/
/*                                                                                      */
/* Details:         check status of Flow Controller Command Time Out Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  E_FlowControllerStatus flowControllerStatus        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckFlowControllerCommandTimeOutAlarmStatus(uint8_t flowControllerStatus)
{
	uint8_t flowControlErr = flowControllerStatus & FC_CMD_TIMEOUT_ERR_BIT;
	if (stAlarmList[eFlowControllerCommandTimeOutId].currentStatus == eInactive)
	{
		if (flowControlErr  == FC_CMD_TIMEOUT_ERR_BIT)
		{
			stAlarmList[eFlowControllerCommandTimeOutId].currentStatus = eActive;
			DEBUG_ALARM("Flow Controller Command Time Out Alarm   Active --> %ds\n", g_alarmTaskTimer/20);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckExternalFlowPathpressureAlarmStatus	      			*/
/*                                                                                      */
/* Details:         check status of External Flow Path Pressure Alarm 					*/
/*                                                                                      */
/* Arguments : (I)  float externalFlowPathPressure       		                		*/
/*                  - range : 0 - 200                                                   */
/*                  - unit 	: kPa	                                                    */
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_externalFlowPathpressureAlarmCounter			 			*/
/*                     (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMgr_CheckExternalFlowPathpressureAlarmStatus(float externalFlowPathPressure)
{
	if (stAlarmList[eExternalFlowPathpressureId].currentStatus == eInactive)
	{
		if (externalFlowPathPressure >= EXTERNAL_FLOW_PATH_HIGH_PRESSURE)
		{
			if (gs_externalFlowPathpressureAlarmCounter ==EXTERNAL_FLOW_PATH_ALARM_TIMEOUT) //1S
			{
				stAlarmList[eExternalFlowPathpressureId].currentStatus = eActive;
				DEBUG_ALARM("External Flow Path Pressure Active --> %ds\n", g_alarmTaskTimer/20);
			}
			gs_externalFlowPathpressureAlarmCounter++;
		}
		else
		{
			gs_externalFlowPathpressureAlarmCounter =0;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_SendAlarmToGUI				 								*/
/*                                                                                      */
/* Details:         send the alarm message to GUI task on M4 core 						*/
/*                                                                                      */
/* Arguments : (I)  ALARM_STAT_T alarmStat		        								*/
/*                                                                                      */
/****************************************************************************************/
static bool alarmMgr_SendAlarmToGUI(ALARM_STAT_T alarmStat)
{
	bool result = false;
	IPC_MSG_DATA_t data;
	data.data_id = eAlarmMGSId;
	data.data_value = 0;
	data.stAlarmStatPacket = alarmStat;

	if(ipc_PushMsg(IPCEX_ID_GUI, data) == QUEUE_INSERT)
	{
		DEBUG_ALARM("id - status: %d - %d\n",alarmStat.ID, alarmStat.currentStatus);
		DEBUG_ALARM("Operation mode: %d\n", gs_currentAlarmMonitor.operationMode);
		result = true;
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_SendAlarmStat					 							*/
/*                                                                                      */
/* Details:         send the alarm message to Device task and GUI task					*/
/*                                                                                      */
/* Arguments : (I)  ALARM_STAT_T alarmStat		        								*/
/*                                                                                      */
/****************************************************************************************/
static bool alarmMgr_SendAlarmStat(ALARM_STAT_T alarmStat)
{
	return alarmMgr_SendAlarmToGUI(alarmStat);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckAllAlarm				 								*/
/*                                                                                      */
/* Details:         check status for all of alarm depend on operation mode				*/
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
void alarmMgr_CheckAllAlarm(void)
{
	switch (gs_currentAlarmMonitor.operationMode)
	{
	case eServiceMode:
	case ePatientMode:
	{
		alarmMgr_CheckCompressorDCOverVoltageAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorSoftwareOverCurrentAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorHardwareOverCurrentAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorBoardTemperatureAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorRPMAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckFlowControllerCommunicationAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerPressureSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerUltrasonicSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerTemperatureSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerCommandTimeOutAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckTemperatureAlarmStatus(
				gs_currentAlarmMonitor.unitInternalTemperature);

		alarmMgr_CheckHighPressureAlarmStatus(
				gs_currentAlarmMonitor.productTankPressure);

		alarmMgr_CheckLowPressureAlarmStatus(
				gs_currentAlarmMonitor.productTankPressure,
				gs_currentAlarmMonitor.psaMode);

		alarmMgr_CheckCoolingFanAlarmStatus(
				gs_currentAlarmMonitor.coolingFanNotRotationState);

		alarmMgr_CheckO2ConcentrationAlarmStatus(
				gs_currentAlarmMonitor.O2Concentration);

		alarmMgr_CheckCannulaUnder27sAlarmStatus(
				gs_currentAlarmMonitor.cannulaPressure);

		alarmMgr_CheckCannulaMore27sAlarmStatus(
				gs_currentAlarmMonitor.cannulaPressure);

		alarmMgr_CheckLoweringO2ConcentrationAlarmStatus(
				gs_currentAlarmMonitor.O2Concentration);

		alarmMgr_CheckO2ConcentrationSensorAlarmStatus(
				gs_currentAlarmMonitor.O2Concentration);

		alarmMgr_CheckFlowRateAlarmStatus(
				gs_currentAlarmMonitor.flowRateSensor,
				gs_currentAlarmMonitor.flowRateSetting);

		alarmMgr_CheckFilterCleaningAlarmStatus(
				gs_currentAlarmMonitor.isFilterErr);

		alarmMgr_CheckCompressorOutletTemperatureAlarmStatus(
				gs_currentAlarmMonitor.compressorOutletTemperature);

		alarmMgr_CheckRisingCompressorOutletTemperatureAlarmStatus(
				gs_currentAlarmMonitor.compressorOutletTemperature);

		alarmMgr_CheckExternalFlowPathpressureAlarmStatus(
				gs_currentAlarmMonitor.externalFlowPathPressure);
		break;
	}
	case eStandyMode:
	{
		alarmMgr_CheckCompressorDCOverVoltageAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorSoftwareOverCurrentAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorHardwareOverCurrentAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorBoardTemperatureAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckCompressorRPMAlarmStatus(
				gs_currentAlarmMonitor.compressorStatus);

		alarmMgr_CheckFlowControllerCommunicationAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerPressureSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerUltrasonicSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerTemperatureSensorAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);

		alarmMgr_CheckFlowControllerCommandTimeOutAlarmStatus(
				gs_currentAlarmMonitor.flowControllerStatus);
		break;
	}
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_UpdateAllAlarm				 								*/
/*                                                                                      */
/* Details:         update status for all of alarm 										*/
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
void alarmMgr_UpdateAllAlarm(void)
{
	for(int i = eFirsAlarmId; i< eLastAlarmId ; i++)
	{
		if (stAlarmList[i].previousStatus != stAlarmList[i].currentStatus)
		{
			if (alarmMgr_SendAlarmStat(stAlarmList[i]))
			{
				stAlarmList[i].previousStatus = stAlarmList[i].currentStatus;
			}
			else
			{
				return;
			}
		}
	}
	return;
}
/* end of files */
