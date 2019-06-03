/********************************************************************/
/*                                                                  */
/* File Name    : AlarmMrg.c                                    	*/
/*                                                                  */
/* General      : AlarmMrg provide all of methods for alarm		    */
/*                task processing							     	*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/10     Truong Nguyen(MV)  	 new file  	    */
/*                                                                  */
/********************************************************************/
/* include system files */


//#include <AlarmMgr.h>
//#include <AlarmTask.h>
//#include <DevCtrl.h>
//#include <DualCoreCommon.h>
//#include <DevInterface.h>
//#include <queue.h>
//#include "TaskCommon.h"
#include "stdint.h"
#include "stdbool.h"
//#define DEBUG_ALARM_MGR_EN
#ifdef DEBUG_ALARM_MGR_EN
#define DEBUG_ALARM_MGR(...) printf(__VA_ARGS__)
#else
#define DEBUG_ALARM_MGR(...)
#endif

/* constants, macro definition */

#define ALM_TICK_PER_SEC 1000/50

#define ERROR_DIFFPRESS_CONST -24.215


#define ALM_TACHYPNEA_TIMEOUT 3

#define O2_CONCENTRATION_98_PERCENT 9800
#define O2_CONCENTRATION_82_PERCENT 8200
#define O2_CONCENTRATION_86_PERCENT 8600
#define O2_CONCENTRATION_30_PERCENT 3000
#define O2_CONCENTRATION_10_PERCENT 1000


#define FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT 				1<<0
#define FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT 					1<<1
#define FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT 			1<<2
#define FLOWCONTROLLER_CMD_TIMEOUT_ERR_BIT					1<<3
#define FLOWCONTROLLER_COMMUNICATION_NO_RESPONSE_ERR_BIT	1<<7

#define FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT 		1<<0
#define FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT 		1<<1
#define FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT 		1<<2


#define COMP_MAX_RPM							3000
#define DC_OVER_VOLT_ERR_BIT 					1<<0
#define DC_LOW_VOLT_ERR_BIT						1<<1
#define SOFT_OVER_CURRENT_ERR_BIT 				1<<2
#define HARD_OVER_CURRENT_ERR_BIT 				1<<3
#define BOARD_TEMP_ERR_BIT						1<<4
#define MOTOR_TEMP_ERR_BIT						1<<5
#define RPM_ERR_BIT								1<<6
#define MOTOR_STEP_OUT_ERR_BIT					1<<7
#define COMP_CMD_TIME_OUT_BIT					1<<8
#define COMP_RESET_NOTI_BIT						1<<9
#define COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT	1<<15 // special bit, only set by control SW, not Compressor driver

#define COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT 			1<<0
#define COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT			1<<1
#define COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT 			1<<2


#define FLOWRATE_ZERO_ONE_LPM	 	10	// scale by 100
#define FlOWRATE_ZERO_TWO_FIVE_LPM	25	// scale by 100
#define FLOWRATE_ONE_LPM 		 	100// scale by 100
#define FLOWPRESS_ZERO_FOUR_KPA 	40	// scale by 100
#define FLOWPRESS_ONE_KPA           100 // scale by 100
#define FLOWPRESS_TWO_KPA			200// scale by 100



#define TANKPRESS_FOURTYNINE_KPA  		49.0
#define BATTERY_CAPACITY_LOW_LEVEL		20
#define BATTERY_CAPACITY_VERYLOW_LEVEL 	8

#define BATTERY_OVER_TEMP_ALARM_ERR_BIT 				0x1000
#define BATTERY_FULLY_CHARGED_BIT 						1<<5

#define SUPPLYPRESS_SEVENTYTWO_KPA	72.0
#define SUPPLYPRESS_SIXTYTHREE_KPA	63.0
#define NUMBER_OF_FLOWRATE 			11
#define MINIMUM_VACUUM_PRESS		-59.0

#define SUPPLY_PRESS_SENSOR_MAX 	200.0
#define VACUUM_PRESS_SENSOR_MIN		-100.0
#define TANK_PRESS_SENSOR_MAX		100.0

#define O2_SENSOR_ERROR_TIMES		3


#define FC_CHECKSUM_ERROR_TIMES		3
#define FC_CODE_ERROR_TIMES			3
#define FC_DATALENGHT_ERROR_TIMES	3


#define CD_CHECKSUM_ERROR_TIMES		3
#define CD_CODE_ERROR_TIMES			3
#define CD_DATALENGHT_ERROR_TIMES	3

#define	ERROR_ONE_TIMES 	1
#define ERROR_TWO_TIMES		2
#define ERROR_THREE_TIMNES 	3


#define BREATH_RATE_HIGH	50

#define Q_DEBUG

typedef enum
{
	eCarSource,
	eACSource,
	eBatterySource
} E_ChargeSource;

typedef enum
{
	eInactive = 0,
	eActive
} E_AlarmStatus;

typedef enum
{
	eNoneAlarmId,
	eFirsAlarmId = 1,
	eAlarmBreathErr = eFirsAlarmId,
	eAlarmAbnormalOxygenConcentration,
	eAlarmLowOxygenConcentration,
	eAlarmOxygenTankPressure,
	eAlarmBatteryTemperature,
	eAlarmBatteryCharge,
	eAlarmBatteryRemainingCapacityLower,
	eAlarmPowerLostErr,
	eAlarmCompressorSupplyPressure,
	eAlarmCompressorSuctionPressure,
	eAlarmFlowcontrol,
	eAlarmSupplyPressureSensor,
	eAlarmVacuumPressureSensor,
	eAlarmTankPressureSensor,
	eAlarmDifferentialPressureSensorCommunication,
	eAlarmO2Sensor,
	eAlarmFlowControllerCommunicationNoResponse,
	eAlarmFlowControllerPressureSensor,
	eAlarmFlowControllerFlowSensor,
	eAlarmFlowControllerTemperatureSensor,
	eAlarmFlowControllerCommunication,
	eAlarmFlowControllerCommunicationCheckSum,
	eAlarmFlowControllerCommunicationCommand,
	eAlarmFlowControllerCommunicationParameter,
	eAlarmFlowControllerResponseCheckSumError,
	eAlarmFlowControllerResponseCodeError,
	eAlarmFlowControllerResponseDataLengthError,
	eAlarmCompressorCommunicationNoResponse,
	eAlarmCompressorDCOverVoltage,
	eAlarmCompressorDCLowerVoltage,
	eAlarmCompressorSoftwareOverCurrent,
	eAlarmCompressorHardwareOverCurrent,
	eAlarmCompressorBoardTemperature,
	eAlarmCompressorMotorTemperature,
	eAlarmCompressorRotationSpeed,
	eAlarmCompressorStepOut,
	eAlarmCompressorCommunication,
	eAlarmCompressorCommunicationCheckSum,
	eAlarmCompressorCommunicationCommand,
	eAlarmCompressorCommunicationParameter,
	eAlarmCompressorResponseCheckSumError,
	eAlarmCompressorResponseCodeError,
	eAlarmCompressorResponseDataLenghtError,
	eAlarmvLEDDriverCommunication,
	eAlarmAudioCommunication,
	eAlarmSpeaker,
	eAlarmSPIFlashMemory,
	eAlarmBatteryType,
	eAlarmTachypnea,
	eALarmVeryLowBattery,
	eLastAlarmId,
	NumberOfAlarm = eLastAlarmId
} E_AlarmId;

typedef struct {
	E_AlarmId ID;
	E_AlarmStatus previousStatus;
	E_AlarmStatus currentStatus;
//	E_AlarmCommand command;
}ALARM_STAT_T;

typedef struct
{
	uint8_t requestId;
	int16_t requestData;
	int16_t requestTestAlarmType;
	int16_t requestTestAlarmData;
}USB_MSG_TO_DEV;


typedef struct
{
	uint16_t numberOfLog;
}LOG_MSG_TO_DEV;


typedef struct
{
	uint16_t deviceId;
}ALARM_MSG_TO_DEV;


typedef struct
{
	bool isSpeakerConnected;
}AUDIO_MSG_TO_DEV;

typedef union
{
	USB_MSG_TO_DEV 		usbMsg;
	LOG_MSG_TO_DEV 		logMgs;
	ALARM_MSG_TO_DEV	alarmMsg;
	AUDIO_MSG_TO_DEV 	audioMsg;
}DEV_MSG_DATA;

typedef struct{
	uint8_t id;
	DEV_MSG_DATA msgData;
}DEV_EVENT_STRUCT;

enum {
	eFromUsbTask,
	eFromLogTask,
	eFromAudioTask,
	eFromBatteryTask,
	eFromAlarmTask,
};

enum
{
    eNoneCommand,
    eFlowControllerResetCommand,
    eCompressorResetCommand,
};





typedef enum
{
	eSynchronizedMode=0,
	eContinuosMode
} E_ModeSetting;
typedef enum
{
	eCompressorNoErr = 0,
	eCommunicationError,
	eDCOverVoltageError,
	eDCLowVoltageError,
	eSoftwareError,
	eHardwareError,
	eBoardTemperatureError,
	eMotorTemperatureError,
	eRPMError,
	eMotorStepOutError,
	eCompChecksumErr,
	eCompCommandErr,
	eCompParamErr,
	eCompNoResponseErr,
	eCompResponseLengthErr,
	eCompResponseCodeErr,
	eCompResponseChecksumErr,
	eCompResponseParamErr,
	eCompressorReset,
}E_CompressorStatus;
typedef enum
{
	eFlowControllerNoErr,
	eChecksumErr,
	eCommandErr,
	eParamErr,
	eNoResponseErr,
	eResponseLengthErr,
	eResponseCodeErr,
	eResponseCheckSumErr,
	eResponseParamErr,
	eFlowReset,
}E_FlowControllerNoAlarmStatus;

typedef enum{
	ePSALowTemperature,
	ePSANormalTemperature,
	ePSAHighTemperature
}E_PSATemperature;

typedef struct {
	E_ModeSetting operationMode;
	bool isAC100VplugIn; //1 plug in, 0 unplug in
	bool isBatteryConnected; //1 insert, 0 uninstall
	bool isBreathTrigger;// 1 Triger, 0  No trigger

	bool isAudioCommunicationError; //1 disconnection, 0 connection
	bool isDifferentialPressureSensorNoResponse;	// 1 not respond, 0 normal

	uint16_t batteryCapacity;	// remaining percent
	uint16_t batteryStatus; // 1 Error, 0 normal
	float providePressure;	//kpa
	float tankPressure; //kPa
	float vacuumPressure; //kpa

	uint16_t compressorStatus;
	uint8_t	compressorCommunicateStatus;
	E_CompressorStatus compressorResponseStatus;

	uint16_t O2Concentration; // %
	uint16_t flowRateSetting; //x100 L/min
	uint16_t flowRateSensor; // x100 L/min
	uint16_t flowPress;//x100 Kpa

	uint8_t flowControllerStatus;
	uint8_t flowControllerCommunicateStatus;
	E_FlowControllerNoAlarmStatus flowControllerResponseStatus;

	bool isSpeakerError;	//1 error, 0 normal
//	E_CompressorSequence compressorSequence;
	uint8_t breathRate;
	float differentialPressure;
	bool isBatteryCharging;
	bool isInvalidBattery;
	bool isBatteryChargingError;
	E_PSATemperature PSATemperature;
//	double O2AlarmDetectionTimeOffset;
}ALARM_MONITOR_t;


static ALARM_STAT_T stAlarmList[NumberOfAlarm];
static ALARM_MONITOR_t gs_currentAlarmMonitor;



//static int16_t 	gs_previousFlowRateSetting;
//static uint32_t gs_alarmTaskTimer;
static bool gs_isResetFCFlag = false;// This Flag for purpose disable E02 when FC error (02 concentration indicate 20.8%)
/*counter*/

static uint32_t gs_changeFlowRateSettingTimer = 0;

//these values have been scale by 100
static const int16_t gs_flowRateSetting[12] = {10, 25, 50, 75, 100, 125, 150, 175, 200, 250, 300,0};
static const int16_t gs_underFlowRateValue[11] = {0, 5, 30, 55, 80, 105, 130, 155, 180, 230, 280};
static const int16_t gs_upperFlowRateValue[11] = {30, 45, 70, 95, 120, 145, 170, 195, 220, 270, 320};
static const int16_t gs_SupplyPressureMaximum[12] = {40, 45, 50, 54, 63, 63, 63, 68, 68, 72, 72, 0};

/****************************************************************************************/
/*                                                                                      */
/* Funtion name	: alarmMgr_InitAlarm	                                                */
/*                                                                                      */
/* Details	 	:  create and initialize default value for all alarm         			*/
/*                                                                                      */
/* Global valiables   : (O)stAlarmList                                                  */
/*		                                                                             	*/
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
	// Init for unit test
	gs_currentAlarmMonitor.operationMode = eSynchronizedMode;
}

void alarmMgrUnitTest_ReSetUpForBreathErrorTest(void)
{
	gs_currentAlarmMonitor.operationMode = eContinuosMode;
	gs_changeFlowRateSettingTimer = 1;
	stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_UpdateAlarmMonitor						                    */
/*                                                                                      */
/* Details		:       update current  value of all alarm monitor	for device task		*/
/*                   							                                        */
/* Using Global Data : (O) 	currentAlarmMonitor                              			*/
/*					   (O)  gs_previousOperationMode									*/
/*					   (O)  gs_currentOperationMode										*/
/*					   (O)  gs_previousFlowRateSetting									*/
/*					   (O)  gs_isChangeFlowRateSetting									*/
/* ************************************************************************************ */
//void alarmMgr_UpdateAlarmMonitor(void)
//{
//	gs_alarmTaskTimer = alarmTask_GetAlarmTaskTimerValue();
//	gs_previousFlowRateSetting = gs_currentAlarmMonitor.flowRateSetting;
//	devCtrl_GetAlarmMonitorStruct(&gs_currentAlarmMonitor);
//	if (gs_currentAlarmMonitor.flowRateSetting != gs_previousFlowRateSetting)
//	{
//		gs_changeFlowRateSettingTimer = 0;
//		DEBUG_ALARM_MGR("Change flow rate setting --> %ds %d\n", gs_alarmTaskTimer/20, gs_currentAlarmMonitor.flowRateSetting);
//	}
//	else if (gs_changeFlowRateSettingTimer < ALM_TICK_PER_SEC*60*60) //1s * 60*60= 1hour
//	{
//		gs_changeFlowRateSettingTimer++;
//	}
//}



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFCAbnormalOxygenConcentration	   						*/
/*                                                                                      */
/* Details:         																	*/
/*	  1. This function start after Portable warm up(5min).								*/
/*	  2. A reset signal is sent to FC when the state of O2 concentration 				*/
/* 	     less than 30% continues for 60 s or more.										*/
/*	  3. If above condition appear, Check start again after 30s from reset signal to FC */
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration          		                      		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) s_AbnormalOxygenConcentrationAlarmCounter 					*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/


bool alarmMgr_CheckFCAbnormalOxygenConcentration(uint16_t O2Concentration)
{
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	static bool s_isCheckO2Low = true;
	static bool s_isStartCount30S = false;
	static uint32_t s_countTimeO2Low = 0;
	static uint32_t s_countTimeFromFCReset = 0;

	if (s_isCheckO2Low == true)
	{
		if (O2Concentration <= O2_CONCENTRATION_30_PERCENT)
			s_countTimeO2Low++;
		else
			s_countTimeO2Low = 0;
		if(s_countTimeO2Low >= ALM_TICK_PER_SEC*60)
		{
			s_countTimeO2Low = 0;
			s_isCheckO2Low = false;
			s_isStartCount30S = true;
			s_isSendFlowControllerResetCM = true;//Reset FC
			gs_isResetFCFlag = true;
		}
	}
	if(s_isStartCount30S == true)
	{
		s_countTimeFromFCReset++;
		if(s_countTimeFromFCReset >= ALM_TICK_PER_SEC*30)
		{
			s_countTimeFromFCReset = 0;
			s_isStartCount30S = false;
			s_isCheckO2Low = true;
		}
	}

    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//            DEBUGOUT("RESET FLOW CONTROLLER...........\n");
//        }
        s_isSendFlowControllerResetCM = false;
        return true;
    }
    else
    	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckBreathErrorAlarmStatus				    			*/
/*                                                                                      */
/* Details:         check status of Abnormal Oxygen Concentration Alarm					*/
/*                                                                                      */
/* Arguments : (I)  int16_t flowRateSetting          		                      		*/
/*             (I)  bool isBreathTrigger                                                */
/*             (I)	uint16_t flowPress                                                  */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_currentAlarmMonitor.operationMode 						*/
/*                     (I) gs_BreathAlarmSynchronnizedCounter							*/
/*                     (I) s_BreathAlarmContinuousCounter								*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckBreathErrorAlarmStatus( uint16_t flowRateSetting, bool isBreathTrigger, uint16_t flowPress)//E01
{
	static uint32_t s_BreathAlarmSynchronnizedCounter = 0;	//E01
	static uint32_t s_BreathAlarmContinuousCounter = 0;	//E01
	if (gs_currentAlarmMonitor.operationMode == eSynchronizedMode)
	{

		static bool preIsBreathTrigger = false;
		if (isBreathTrigger != preIsBreathTrigger)
		{
			stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
			s_BreathAlarmSynchronnizedCounter = 0;
		}
		else if (stAlarmList[eAlarmBreathErr].currentStatus == eInactive)
		{
			s_BreathAlarmSynchronnizedCounter++;

			if(s_BreathAlarmSynchronnizedCounter >= ALM_TICK_PER_SEC*30)//30s
			{
				stAlarmList[eAlarmBreathErr].currentStatus = eActive;
				stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;//E11
				DEBUG_ALARM_MGR("Breath Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
				s_BreathAlarmSynchronnizedCounter = 0;
			}
		}
		preIsBreathTrigger = isBreathTrigger;
	}

	else if (gs_currentAlarmMonitor.operationMode == eContinuosMode)
	{
		if (gs_changeFlowRateSettingTimer == 0)
		{
			s_BreathAlarmContinuousCounter = 0;
		}

		if(flowRateSetting == FLOWRATE_ZERO_ONE_LPM)//0.1Lpm (scale by 100)
		{
			if (flowPress > FLOWPRESS_ZERO_FOUR_KPA)//0.4KPa (scale by 100)
			{
				if (s_BreathAlarmContinuousCounter >= ALM_TICK_PER_SEC*30)//1s*30= 30s
				{
					stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
					stAlarmList[eAlarmBreathErr].currentStatus = eActive;
					DEBUG_ALARM_MGR("Breath Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
				}
				s_BreathAlarmContinuousCounter++;
			}
			else
			{
				s_BreathAlarmContinuousCounter = 0;
				stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
			}
		}

		else if(flowRateSetting == FlOWRATE_ZERO_TWO_FIVE_LPM)//0.25Lpm (scale by 100)
		{
            if (flowPress > FLOWPRESS_ONE_KPA)//1KPa (scale by 100)
            {
                if (s_BreathAlarmContinuousCounter >= ALM_TICK_PER_SEC*30)//1s*30= 30s
                {
                    stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
                    stAlarmList[eAlarmBreathErr].currentStatus = eActive;
                    DEBUG_ALARM_MGR("Breath Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
                }
                s_BreathAlarmContinuousCounter++;
            }
            else
            {
                s_BreathAlarmContinuousCounter = 0;
                stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
            }
		}
		else
		{
			if (flowPress > FLOWPRESS_TWO_KPA)	//2KPa (scale by 100)
			{
				if (s_BreathAlarmContinuousCounter >= ALM_TICK_PER_SEC*30)//1s*30= 30s
				{
					stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
					stAlarmList[eAlarmBreathErr].currentStatus = eActive;
					DEBUG_ALARM_MGR("Breath Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
				}
				s_BreathAlarmContinuousCounter++;
			}
			else
			{
				s_BreathAlarmContinuousCounter = 0;
				stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
			}
		}
	}
	return stAlarmList[eAlarmBreathErr].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Abnormal Oxygen Concentration Alarm					*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration          		                      		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) s_AbnormalOxygenConcentrationAlarmCounter 					*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(uint16_t flowRateSetting, uint16_t O2Concentration)//E02
{
	static uint32_t s_AbnormalOxygenConcentrationAlarmCounter = 0;	//E02

	if (stAlarmList[eAlarmAbnormalOxygenConcentration].currentStatus == eInactive)
	{
		if((stAlarmList[eAlarmBreathErr].currentStatus == eInactive)
				&&(stAlarmList[eAlarmTachypnea].currentStatus == eInactive))
		{
			if(gs_isResetFCFlag == true)
			{
				gs_isResetFCFlag = false;
				s_AbnormalOxygenConcentrationAlarmCounter = 0;
//				DEBUGOUT("DISABLE E02...........\n");
			}

			if (O2Concentration < O2_CONCENTRATION_82_PERCENT)
			{
				if ((flowRateSetting == FLOWRATE_ZERO_ONE_LPM)||(flowRateSetting == FlOWRATE_ZERO_TWO_FIVE_LPM))
				{
					if (s_AbnormalOxygenConcentrationAlarmCounter >= ALM_TICK_PER_SEC*60*20)//1s*60*20= 20Min
					{
						stAlarmList[eAlarmAbnormalOxygenConcentration].currentStatus = eActive;
						DEBUG_ALARM_MGR("Abnormal Oxygen Concentration Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
					}
				}
				else
				{
					if (s_AbnormalOxygenConcentrationAlarmCounter >= ALM_TICK_PER_SEC*60*3)//1s*60*3= 3Min
					{
						stAlarmList[eAlarmAbnormalOxygenConcentration].currentStatus = eActive;
						DEBUG_ALARM_MGR("Abnormal Oxygen Concentration Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
					}
				}
				s_AbnormalOxygenConcentrationAlarmCounter++;
			}
			else
				s_AbnormalOxygenConcentrationAlarmCounter = 0;
		}
	}
	else if (O2Concentration >= O2_CONCENTRATION_82_PERCENT)
	{
		stAlarmList[eAlarmAbnormalOxygenConcentration].currentStatus = eInactive;
		s_AbnormalOxygenConcentrationAlarmCounter = 0;
	}
	return stAlarmList[eAlarmAbnormalOxygenConcentration].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckLowOxygenConcentrationAlarmStatus	   					*/
/*                                                                                      */
/* Details:         check status of Low Oxygen Concentration Alarm     					*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration          		                      		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) s_LowOxygenConcentrationAlarmCounter 						*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckLowOxygenConcentrationAlarmStatus(uint16_t flowRateSetting, uint16_t O2Concentration)//E03
{
	static uint32_t s_LowOxygenConcentrationAlarmCounter = 0;	//E03
	if (stAlarmList[eAlarmLowOxygenConcentration].currentStatus == eInactive)
	{
		if((stAlarmList[eAlarmBreathErr].currentStatus == eInactive)
				&&(stAlarmList[eAlarmTachypnea].currentStatus == eInactive))
		{
			if ((O2Concentration >= O2_CONCENTRATION_82_PERCENT) && (O2Concentration < O2_CONCENTRATION_86_PERCENT))
			{
				if ((flowRateSetting == FLOWRATE_ZERO_ONE_LPM)||(flowRateSetting == FlOWRATE_ZERO_TWO_FIVE_LPM))
				{
					if (s_LowOxygenConcentrationAlarmCounter >= ALM_TICK_PER_SEC*60*20)//1s*60*20= 20Min
					{
						stAlarmList[eAlarmLowOxygenConcentration].currentStatus = eActive;
						DEBUG_ALARM_MGR("Low Oxygen Concentration Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
					}
				}
				else
				{
					if (s_LowOxygenConcentrationAlarmCounter >= ALM_TICK_PER_SEC*60*3)//1s*60*3= 3Min
					{
						stAlarmList[eAlarmLowOxygenConcentration].currentStatus = eActive;
						DEBUG_ALARM_MGR("Low Oxygen Concentration Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
					}
				}
				s_LowOxygenConcentrationAlarmCounter++;
			}
			else if (O2Concentration >= O2_CONCENTRATION_86_PERCENT)
				s_LowOxygenConcentrationAlarmCounter = 0;
		}

	}
	else if (O2Concentration >= O2_CONCENTRATION_86_PERCENT)
	{
		stAlarmList[eAlarmLowOxygenConcentration].currentStatus = eInactive;
		s_LowOxygenConcentrationAlarmCounter = 0;
	}
	return stAlarmList[eAlarmLowOxygenConcentration].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_OxygenTankPressureErrorAlarmStatus				   			*/
/*                                                                                      */
/* Details:         check status of Oxygen Tank Pressure Error Alarm   					*/
/*                                                                                      */
/* Arguments : (I)  float tankPressure						        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_OxygenTankPressureErrorAlarmStatus(float currentTankPressure)//E04
{
	static uint32_t s_OxygenTankPressureErrorAlarmCounter = 0;	//E04
	static float maximumTankPressure = 0;
	if (stAlarmList[eAlarmOxygenTankPressure].currentStatus == eInactive)
	{
		s_OxygenTankPressureErrorAlarmCounter++;
		if (maximumTankPressure < currentTankPressure)
		{
			maximumTankPressure = currentTankPressure;
		}
		if(s_OxygenTankPressureErrorAlarmCounter > ALM_TICK_PER_SEC*30)//30s
		{
			s_OxygenTankPressureErrorAlarmCounter = 0;
			if (maximumTankPressure  < TANKPRESS_FOURTYNINE_KPA)	// 0.5kg/cm2 ~ 49 KPa
			{
				stAlarmList[eAlarmOxygenTankPressure].currentStatus = eActive;
				DEBUG_ALARM_MGR("Oxygen Tank Pressure Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			maximumTankPressure = 0;
		}
	}
	return stAlarmList[eAlarmOxygenTankPressure].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckBatteryTemperatureErrorAlarmStatus	   				*/
/*                                                                                      */
/* Details:         check status of Battery Temperature Alarm    						*/
/*                                                                                      */
/* Arguments : (I)  bool batteryTemperatureError				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckBatteryTemperatureErrorAlarmStatus(bool isBatteryInsert, uint16_t batteryStatus)//E05
{
	static uint16_t countTimeBatteryOverTemp = 0;
	if (stAlarmList[eAlarmBatteryTemperature].currentStatus == eInactive)
	{
		if ((isBatteryInsert)&&(batteryStatus & BATTERY_OVER_TEMP_ALARM_ERR_BIT))
		{
			//DEBUGOUT("Test time %d %d\n",countTimeBatteryOverTemp,batteryStatus);
			countTimeBatteryOverTemp++;
			if(countTimeBatteryOverTemp>=ALM_TICK_PER_SEC*12)//12s
			{
				//DEBUGOUT("Test %d\n",batteryStatus);
				stAlarmList[eAlarmBatteryTemperature].currentStatus = eActive;
				DEBUG_ALARM_MGR("Battery Temperature Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
		}
		else
			countTimeBatteryOverTemp = 0;
	}
	return stAlarmList[eAlarmBatteryTemperature].currentStatus;
}


void alarmMgr_ReSetupForCheckBatteryChargeErrorAlarmStatus()
{
	stAlarmList[eAlarmBatteryCharge].currentStatus = eInactive;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckBatteryChargeErrorAlarmStatus	   						*/
/*                                                                                      */
/* Details:         check status Battery Charge Error Alarm     						*/
/*                                                                                      */
/* Arguments : (I)  bool bool isAC100VplugIn          	                	     		*/
/* 			   (I)  bool isBatteryInsert          		                 	     		*/
/* 			   (I)  bool isBatteryCharging         		                 	     		*/
/*             (I)  uint16_t batteryStatus               		              			*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_BatteryChargeErrorAlarmCounter 							*/
/*                     (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckBatteryChargeErrorAlarmStatus(bool isAC100VplugIn, bool isBatteryInsert,
														bool isBatteryError, bool isBatteryCharging,
														uint16_t batteryStatus)//E06
{
	static uint32_t batteryStartChargingTimer = 0;
	static uint8_t countTimeBatterryError = 0;
	if ((stAlarmList[eAlarmBatteryCharge].currentStatus == eInactive)
			&&(stAlarmList[eAlarmBatteryType].currentStatus == eInactive))
	{
		if ((isBatteryInsert)&&(isAC100VplugIn))
		{
			if (batteryStatus & BATTERY_FULLY_CHARGED_BIT)
			{
				batteryStartChargingTimer = 0;
			}
			else
			{
				if(isBatteryCharging)
					batteryStartChargingTimer=0;
				else
					batteryStartChargingTimer++;
				if((batteryStartChargingTimer > ALM_TICK_PER_SEC*60*30)/*30Mins*/)
				{
					stAlarmList[eAlarmBatteryCharge].currentStatus = eActive;
					DEBUG_ALARM_MGR("Battery Charge Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
				}
			}
		}
		if((isBatteryError == true)&&(isAC100VplugIn))
		{
			countTimeBatterryError++;
			if(countTimeBatterryError > ALM_TICK_PER_SEC*10)
			{
				stAlarmList[eAlarmBatteryCharge].currentStatus = eActive;
				DEBUG_ALARM_MGR("Battery Charge Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
		}
		else
			countTimeBatterryError = 0;
	}
	return stAlarmList[eAlarmBatteryCharge].currentStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Battery Remaining Capacity Lower Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  uint8_t currentBatteryCapacity				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/

E_ChargeSource mockDevInterface_GetChargeSource( E_ChargeSource source)
{
	return source;
}

E_AlarmStatus alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(uint8_t currentBatteryCapacity,
																	bool isBatteryInsert,
																	bool isACPlugIn,
																	E_ChargeSource source)//E07
{
	static uint32_t batteryVeryLowCounter = 0;
	static uint32_t batteryLowCounter = 0;

	if (stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus == eInactive)
	{
		if ((currentBatteryCapacity <= BATTERY_CAPACITY_LOW_LEVEL)
				&&(currentBatteryCapacity >= BATTERY_CAPACITY_VERYLOW_LEVEL)
				&&(isBatteryInsert == true)
				&&(isACPlugIn == false))
			batteryLowCounter++;
		else
			batteryLowCounter = 0;
		if(batteryLowCounter >= ALM_TICK_PER_SEC *2)//2s
		{
			stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eActive;
			DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	else if ((isACPlugIn && (isBatteryInsert == true))
			||(currentBatteryCapacity > BATTERY_CAPACITY_LOW_LEVEL))
	{
		stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eInactive;
		DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm InActive --> %ds\n", gs_alarmTaskTimer/20);
	}
	else if (isBatteryInsert == false)
	{
		stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eInactive;
		DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm InActive --> %ds\n", gs_alarmTaskTimer/20);
	}

	if((mockDevInterface_GetChargeSource(source) == eBatterySource)
			&&(isBatteryInsert == true)
			&&(stAlarmList[eAlarmBatteryType].currentStatus == eInactive))
	{
		if((currentBatteryCapacity < BATTERY_CAPACITY_VERYLOW_LEVEL)&&(currentBatteryCapacity > 0))
			batteryVeryLowCounter++;
		else
			batteryVeryLowCounter = 0;
		if(batteryVeryLowCounter >= ALM_TICK_PER_SEC *10)//10s
			stAlarmList[eALarmVeryLowBattery].currentStatus = eActive;
	}
	return stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus;
}

E_AlarmStatus alarmMgr_CheckBatteryRemainingCapacityVelyLowAlarmStatus(uint8_t currentBatteryCapacity,bool isBatteryInsert, bool isACPlugIn, E_ChargeSource source)//E07
{
	static uint32_t batteryVeryLowCounter = 0;
	static uint32_t batteryLowCounter = 0;

	if (stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus == eInactive)
	{
		if ((currentBatteryCapacity <= BATTERY_CAPACITY_LOW_LEVEL)
				&&(currentBatteryCapacity >= BATTERY_CAPACITY_VERYLOW_LEVEL)
				&&(isBatteryInsert==true)
				&&(isACPlugIn==false))
			batteryLowCounter++;
		else
			batteryLowCounter = 0;
		if(batteryLowCounter >= ALM_TICK_PER_SEC *2)//2s
		{
			stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eActive;
			DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	else if ((isACPlugIn&&(isBatteryInsert == true))
			||(currentBatteryCapacity > BATTERY_CAPACITY_LOW_LEVEL))
	{
		stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eInactive;
		DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm InActive --> %ds\n", gs_alarmTaskTimer/20);
	}
	else if (isBatteryInsert == false)
	{
		stAlarmList[eAlarmBatteryRemainingCapacityLower].currentStatus = eInactive;
		DEBUG_ALARM_MGR(" Battery Remaining Capacity Lower Alarm InActive --> %ds\n", gs_alarmTaskTimer/20);
	}

	if((mockDevInterface_GetChargeSource(source) == eBatterySource)
			&&(isBatteryInsert == true)
			&&(stAlarmList[eAlarmBatteryType].currentStatus == eInactive))
	{
		if((currentBatteryCapacity < BATTERY_CAPACITY_VERYLOW_LEVEL)&&(currentBatteryCapacity > 0))
			batteryVeryLowCounter++;
		else
			batteryVeryLowCounter = 0;
		if(batteryVeryLowCounter >= ALM_TICK_PER_SEC *10)//10s
			stAlarmList[eALarmVeryLowBattery].currentStatus = eActive;
	}
	return stAlarmList[eALarmVeryLowBattery].currentStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Compressor Supply Pressure Error Alarm   			*/
/*                                                                                      */
/* Arguments : (I)  float currentSupplyPressure					        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/

void alarmMgr_SetupCheckCompressorSupplyPressureErrorAlarmStatus(E_ModeSetting mode)
{
	stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eInactive;
	gs_currentAlarmMonitor.operationMode = mode;
}
E_AlarmStatus alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(uint16_t flowRateSetting,
                                                                   float currentSupplyPressure,
                                                                   E_PSATemperature PSATemperature)//E09
{
	static uint32_t s_CompressorSupplyPressureErrorAlarmCounter = 0;	//E09
	static float maximumSupplyPressure = 0;
	if ((stAlarmList[eAlarmCompressorSupplyPressure].currentStatus == eInactive)
			&&(stAlarmList[eAlarmBreathErr].currentStatus == eInactive))
	{
		if (gs_currentAlarmMonitor.operationMode == eContinuosMode)
		{
			s_CompressorSupplyPressureErrorAlarmCounter++;
			if (maximumSupplyPressure < currentSupplyPressure)
			{
				maximumSupplyPressure = currentSupplyPressure;
			}
			if(s_CompressorSupplyPressureErrorAlarmCounter > ALM_TICK_PER_SEC*30)//30s
			{
				s_CompressorSupplyPressureErrorAlarmCounter = 0;
				if (flowRateSetting == FLOWRATE_ONE_LPM) //1Lpm
				{
					if(PSATemperature == ePSAHighTemperature)
					{
						if (maximumSupplyPressure < SUPPLYPRESS_SIXTYTHREE_KPA)
						{
							stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eActive;
							DEBUG_ALARM_MGR("Compressor Supply Pressure Error Alarm Active --> %d\n", gs_alarmTaskTimer/20);
						}
					}
					else
					{
						if (maximumSupplyPressure < SUPPLYPRESS_SEVENTYTWO_KPA)
						{
							stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eActive;
							DEBUG_ALARM_MGR("Compressor Supply Pressure Error Alarm Active --> %d\n", gs_alarmTaskTimer/20);
						}
					}

				}
				else
				{
					if (maximumSupplyPressure < SUPPLYPRESS_SEVENTYTWO_KPA)
					{
						stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eActive;
						DEBUG_ALARM_MGR("Compressor Supply Pressure Error Alarm Active --> %d\n", gs_alarmTaskTimer/20);
					}
				}
				maximumSupplyPressure = 0;
			}
		}
		else if (gs_currentAlarmMonitor.operationMode == eSynchronizedMode)
		{
			uint8_t flowRateIndex;
			for(flowRateIndex = 0; flowRateIndex < NUMBER_OF_FLOWRATE; flowRateIndex++)
			{
				if(flowRateSetting == gs_flowRateSetting[flowRateIndex])
				{
					break;
				}
			}
			s_CompressorSupplyPressureErrorAlarmCounter++;
			if (maximumSupplyPressure < currentSupplyPressure)
			{
				maximumSupplyPressure = currentSupplyPressure;
			}
			if(s_CompressorSupplyPressureErrorAlarmCounter > ALM_TICK_PER_SEC*30)//30s
			{
				s_CompressorSupplyPressureErrorAlarmCounter = 0;
				//DEBUG_ALARM_MGR("Current Supply Pressure --> %d\n", (uint16_t)currentSupplyPressure);
				//DEBUG_ALARM_MGR("Maximum Supply Pressure --> %d\n", (uint16_t)maximumSupplyPressure);
				if (maximumSupplyPressure < gs_SupplyPressureMaximum[flowRateIndex])
				{
					stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eActive;
					DEBUG_ALARM_MGR("Compressor Supply Pressure Error Alarm Active --> %d\n", gs_alarmTaskTimer/20);
				}
				maximumSupplyPressure = 0;
			}
		}
	}
	else if (stAlarmList[eAlarmBreathErr].currentStatus == eActive)
	{
		stAlarmList[eAlarmCompressorSupplyPressure].currentStatus = eInactive;
	}
	return stAlarmList[eAlarmCompressorSupplyPressure].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Compressor Suction Pressure Error Alarm   			*/
/*                                                                                      */
/* Arguments : (I)  float currentSupplyPressure						      				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(float currentVacuumPressure)//E10
{
	static uint32_t s_CompressorSuctionPressureErrorAlarmCounter = 0;	//E10
	static float minimumVacuumPressure = 0;

	if((stAlarmList[eAlarmCompressorSuctionPressure].currentStatus == eInactive)
			&&(stAlarmList[eAlarmBreathErr].currentStatus == eInactive))
	{
		s_CompressorSuctionPressureErrorAlarmCounter++;
		if (minimumVacuumPressure > currentVacuumPressure)// (Because Vacuum Pressure is negative)
		{
			minimumVacuumPressure = currentVacuumPressure;
		}
		if(s_CompressorSuctionPressureErrorAlarmCounter > ALM_TICK_PER_SEC*30)//30s
		{
			s_CompressorSuctionPressureErrorAlarmCounter = 0;
			if (minimumVacuumPressure > MINIMUM_VACUUM_PRESS)
			{
				stAlarmList[eAlarmCompressorSuctionPressure].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Vacuum Pressure Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			minimumVacuumPressure = 0;
		}
	}
	else if (stAlarmList[eAlarmBreathErr].currentStatus == eActive)
	{
		stAlarmList[eAlarmCompressorSuctionPressure].currentStatus = eInactive;
	}
	return stAlarmList[eAlarmCompressorSuctionPressure].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControlErrorAlarmStatus	   						*/
/*                                                                                      */
/* Details:         check status of Flow Control Error Alarm   							*/
/*                                                                                      */
/* Arguments : (I)  float supplyPressure						        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
void alarmMgr_SetupForCheckFlowControlErrorAlarmStatus()
{
	gs_currentAlarmMonitor.operationMode = eContinuosMode;
	stAlarmList[eAlarmBreathErr].currentStatus = eInactive;
	stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
	gs_changeFlowRateSettingTimer = 60*ALM_TICK_PER_SEC;
}

E_AlarmStatus alarmMgr_CheckFlowControlErrorAlarmStatus(uint16_t flowRateSetting, uint16_t flowRateSensor)//E11
{
	static uint32_t s_FlowControlErrorAlarmCounter = 0;	//E11
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if(gs_currentAlarmMonitor.operationMode == eContinuosMode)
	{
		uint8_t flowRateIndex;
		if ((stAlarmList[eAlarmBreathErr].currentStatus == eInactive)/*&&(flowRateSetting <= 100)*/)
		{
			for(flowRateIndex = 0; flowRateIndex < NUMBER_OF_FLOWRATE; flowRateIndex++)
			{
				if(flowRateSetting == gs_flowRateSetting[flowRateIndex])
				{
					break;
				}
			}
			if (gs_changeFlowRateSettingTimer == 0)
			{
				s_FlowControlErrorAlarmCounter = 0;
			}

			if (gs_changeFlowRateSettingTimer >= 60*ALM_TICK_PER_SEC)//60S
			{
				if ((flowRateSensor < gs_underFlowRateValue[flowRateIndex])
						||(flowRateSensor > gs_upperFlowRateValue[flowRateIndex])
						||((flowRateSetting == 10)&&(flowRateSensor == 0)))
				{
					if (stAlarmList[eAlarmFlowcontrol].currentStatus == eInactive)
					{
						if (s_FlowControlErrorAlarmCounter >= 40* ALM_TICK_PER_SEC)//40S
						{
		                    s_isSendFlowControllerResetCM = true;
							stAlarmList[eAlarmFlowcontrol].currentStatus = eActive;
							DEBUG_ALARM_MGR("Flow Rate Alarm Active --> %ds\n",gs_alarmTaskTimer/20);
						}
						s_FlowControlErrorAlarmCounter ++;
					}
				}
				else
				{
					s_FlowControlErrorAlarmCounter = 0;
					stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
				}
			}
		}
	    else
	    {
	        //do nothing
	    }
	    if (s_isSendFlowControllerResetCM == true)
	    {
//	        deviceQueue.id = eFromAlarmTask;
//	        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//	        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//	        {
//	            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//	        }
//	        else
//	        {
//	            s_isSendFlowControllerResetCM = false;
//	        }
	    }
	}
	else
	{
		stAlarmList[eAlarmFlowcontrol].currentStatus = eInactive;
	}
	return stAlarmList[eAlarmFlowcontrol].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckSupplyPressureSensorErrorAlarmStatus	   				*/
/*                                                                                      */
/* Details:         check status of Supply Pressure Sensor Error Alarm   				*/
/*                                                                                      */
/* Arguments : (I)  float supplyPressure						        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckSupplyPressureSensorErrorAlarmStatus(float supplyPressure)//E12
{
	if (stAlarmList[eAlarmSupplyPressureSensor].currentStatus == eInactive)
	{
		if (supplyPressure > SUPPLY_PRESS_SENSOR_MAX)	// > 3Volt
		{
			stAlarmList[eAlarmSupplyPressureSensor].currentStatus = eActive;
			DEBUG_ALARM_MGR("Supply Pressure Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
	return stAlarmList[eAlarmSupplyPressureSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckVacuumPressureSensorErrorAlarmStatus	   				*/
/*                                                                                      */
/* Details:         check status of Vacuum Pressure Sensor Error Alarm   				*/
/*                                                                                      */
/* Arguments : (I)  float vacuumPressure						        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckVacuumPressureSensorErrorAlarmStatus(float vacuumPressure)//E13
{
	if (stAlarmList[eAlarmVacuumPressureSensor].currentStatus == eInactive)
	{
		if (vacuumPressure < VACUUM_PRESS_SENSOR_MIN)	//	> 3Volt
		{
			stAlarmList[eAlarmVacuumPressureSensor].currentStatus = eActive;
			DEBUG_ALARM_MGR("Vacuum Pressure Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
	return stAlarmList[eAlarmVacuumPressureSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckTankPressureSensorErrorAlarmStatus	   				*/
/*                                                                                      */
/* Details:         check status of Tank Pressure Sensor Error Alarm   					*/
/*                                                                                      */
/* Arguments : (I)  float tankPressure							        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckTankPressureSensorErrorAlarmStatus(float tankPressure)//E14
{
	if (stAlarmList[eAlarmTankPressureSensor].currentStatus == eInactive)
	{
		if (tankPressure > TANK_PRESS_SENSOR_MAX)	// > 100kPa
		{
			stAlarmList[eAlarmTankPressureSensor].currentStatus = eActive;
			DEBUG_ALARM_MGR("Tank Pressure Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	else
	{
	    //do nothing
	}
	return stAlarmList[eAlarmTankPressureSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus*/
/*                                                                                      */
/* Details:         check Flow Controller Communication Error Alarm   					*/
/*                                                                                      */
/* Arguments : (I)  bool isFlowControllerCommunicationError		        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                     (I) g_isResetFlowController                                      */
/*                     (I) s_FlowControllerCommunicationErrorAlarmCounter              */
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus(double differentialPressure)//E15
{
	static uint32_t s_DifferentialPressureSensorCommunicationErrorAlarmCounter = 0;//E15

	if (stAlarmList[eAlarmDifferentialPressureSensorCommunication].currentStatus == eInactive)
	{
		if (differentialPressure == ERROR_DIFFPRESS_CONST)
		{
			s_DifferentialPressureSensorCommunicationErrorAlarmCounter++;
		}
		else
		{
			s_DifferentialPressureSensorCommunicationErrorAlarmCounter = 0;
		}

		if (s_DifferentialPressureSensorCommunicationErrorAlarmCounter > ALM_TICK_PER_SEC*60)//60s
		{
			stAlarmList[eAlarmDifferentialPressureSensorCommunication].currentStatus = eActive;
			DEBUG_ALARM_MGR("Tank Pressure Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }

	return stAlarmList[eAlarmDifferentialPressureSensorCommunication].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckOxygenSensorErrorAlarmStatus	   						*/
/*                                                                                      */
/* Details:         check status of Oxygen Sensor Error Alarm     						*/
/*                                                                                      */
/* Arguments : (I)  int16_t O2Concentration          		                      		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_OxygenSensorErrorAlarmCounter 							*/
/*                     (I) stAlarmList													*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckOxygenSensorErrorAlarmStatus(uint16_t O2Concentration)//E16
{
	static uint32_t s_OxygenSensorErrorAlarmCounter = 0;	//E16
	static uint8_t CountTimesOxygenSensorError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
	static uint16_t countTime10sError;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;

	if (stAlarmList[eAlarmO2Sensor].currentStatus == eInactive)
	{
		if ((O2Concentration > O2_CONCENTRATION_98_PERCENT) || (O2Concentration < O2_CONCENTRATION_10_PERCENT))
		{
			countTime10sError++;
			if (countTime10sError >= ALM_TICK_PER_SEC*10)//1s*10 = 10s
			{
				countTime10sError = 0;
				CountTimesOxygenSensorError++;
				if (CountTimesOxygenSensorError < ERROR_THREE_TIMNES)
				{
		            s_isSendFlowControllerResetCM = true;
				}
			}
		}
		else
		{
			countTime10sError = 0;
		}

		if (CountTimesOxygenSensorError == ERROR_ONE_TIMES)
		{
			s_OxygenSensorErrorAlarmCounter++;
		}
		else if (CountTimesOxygenSensorError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_OxygenSensorErrorAlarmCounter;
				s_OxygenSensorErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_OxygenSensorErrorAlarmCounter++;
		}
		else if (CountTimesOxygenSensorError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_OxygenSensorErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmO2Sensor].currentStatus = eActive;
				DEBUG_ALARM_MGR("Oxygen Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_OxygenSensorErrorAlarmCounter;
				s_OxygenSensorErrorAlarmCounter = 0;
				CountTimesOxygenSensorError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
	return stAlarmList[eAlarmO2Sensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus	*/
/*                                                                                      */
/* Details:         check Flow Controller Communication No Response Error Alarm   		*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                     (I) g_isResetFlowController                                      */
/*                     (I) s_FlowControllerCommunicationErrorAlarmCounter              */
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus(uint8_t flowControllerStatus)//E17
{
	static uint32_t s_FlowControllerCommunicationErrorAlarmCounter = 0;	//E17
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t flowControllerCommunicationError = flowControllerStatus & FLOWCONTROLLER_COMMUNICATION_NO_RESPONSE_ERR_BIT;
	if (flowControllerCommunicationError)
		s_FlowControllerCommunicationErrorAlarmCounter++;
	else
	{
		s_FlowControllerCommunicationErrorAlarmCounter = 0;
		stAlarmList[eAlarmFlowControllerCommunicationNoResponse].currentStatus = eInactive;
	}
	if	(s_FlowControllerCommunicationErrorAlarmCounter >= ALM_TICK_PER_SEC*60)//60s
	{
		s_FlowControllerCommunicationErrorAlarmCounter = 0;
		if(flowControllerCommunicationError)
		{
            s_isSendFlowControllerResetCM = true;
			stAlarmList[eAlarmFlowControllerCommunicationNoResponse].currentStatus = eActive;
			DEBUG_ALARM_MGR("Flow Controller Communication No Response Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
	return stAlarmList[eAlarmFlowControllerCommunicationNoResponse].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus 			*/
/*                                                                                      */
/* Details:         check status of Flow Controller Pressure Sensor Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus(uint8_t flowControllerStatus)//E18
{
	static uint32_t s_FlowControllerPressureSensorErrorAlarmCounter = 0;	//E18
	static uint8_t countTimesPressureSensorError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t pressureSensorErr = flowControllerStatus & FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerPressureSensor].currentStatus == eInactive)
	{
		if (pressureSensorErr  == FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT)
		{
			countTimesPressureSensorError++;
			if (countTimesPressureSensorError < ERROR_THREE_TIMNES)
			{
			    s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesPressureSensorError == ERROR_ONE_TIMES)
		{
			s_FlowControllerPressureSensorErrorAlarmCounter++;
		}
		else if (countTimesPressureSensorError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerPressureSensorErrorAlarmCounter;
				s_FlowControllerPressureSensorErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerPressureSensorErrorAlarmCounter++;
		}
		else if (countTimesPressureSensorError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerPressureSensorErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerPressureSensor].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Pressure Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerPressureSensorErrorAlarmCounter;
				s_FlowControllerPressureSensorErrorAlarmCounter = 0;
				countTimesPressureSensorError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerPressureSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Tank Pressure Sensor Error Alarm   					*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus(uint8_t flowControllerStatus)//E19
{
	static uint32_t s_FlowControllerFlowSensorErrorAlarmCounter = 0;	//E19
	static uint8_t countTimesFlowSensorError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t flowSensorErr = flowControllerStatus & FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerFlowSensor].currentStatus == eInactive)
	{
		if (flowSensorErr  == FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT)
		{
			countTimesFlowSensorError++;
			if (countTimesFlowSensorError < ERROR_THREE_TIMNES)
			{
			    s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesFlowSensorError == ERROR_ONE_TIMES)
		{
			s_FlowControllerFlowSensorErrorAlarmCounter++;
		}
		else if (countTimesFlowSensorError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerFlowSensorErrorAlarmCounter;
				s_FlowControllerFlowSensorErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerFlowSensorErrorAlarmCounter++;
		}
		else if (countTimesFlowSensorError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerFlowSensorErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerFlowSensor].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Flow Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerFlowSensorErrorAlarmCounter;
				s_FlowControllerFlowSensorErrorAlarmCounter = 0;
				countTimesFlowSensorError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerFlowSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus 		*/
/*                                                                                      */
/* Details:         check status of Flow Controller Temperature Sensor Alarm    		*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus(uint8_t flowControllerStatus)//E20
{
	static uint32_t s_FlowControllerTemperatureSensorErrorAlarmCounter = 0;	//E20
	static uint8_t countTimesTemperatureSensorError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t temperatureSensorErr = flowControllerStatus & FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerTemperatureSensor].currentStatus == eInactive)
	{
		if (temperatureSensorErr  == FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT)
		{
			countTimesTemperatureSensorError++;
			if (countTimesTemperatureSensorError < ERROR_THREE_TIMNES)
			{
	            s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesTemperatureSensorError == ERROR_ONE_TIMES)
		{
			s_FlowControllerTemperatureSensorErrorAlarmCounter++;
		}
		else if (countTimesTemperatureSensorError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerTemperatureSensorErrorAlarmCounter;
				s_FlowControllerTemperatureSensorErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerTemperatureSensorErrorAlarmCounter++;
		}
		else if (countTimesTemperatureSensorError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerTemperatureSensorErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerTemperatureSensor].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Temperature Sensor Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerTemperatureSensorErrorAlarmCounter;
				s_FlowControllerTemperatureSensorErrorAlarmCounter = 0;
				countTimesTemperatureSensorError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerTemperatureSensor].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationErrorAlarmStatus			*/
/*                                                                                      */
/* Details:         check Flow Controller Communication  Error Alarm   					*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus				        				*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                     (I) g_isResetFlowController                                      */
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerCommunicationErrorAlarmStatus(uint8_t flowControllerStatus)//E21
{
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if(stAlarmList[eAlarmFlowControllerCommunication].currentStatus == eInactive)
	{
		if	(flowControllerStatus & FLOWCONTROLLER_CMD_TIMEOUT_ERR_BIT )
		{
		    s_isSendFlowControllerResetCM = true;
			stAlarmList[eAlarmFlowControllerCommunication].currentStatus = eActive;
			DEBUG_ALARM_MGR("Flow Controller Communication Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerCommunication].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller Communication CheckSum Alarm    	*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus(uint8_t flowControllerComunicationStatus)//E22
{
	static uint32_t s_FlowControllerCommunicationCheckSumErrorAlarmCounter = 0;	//E22
	static uint8_t countTimesCommunicationCheckSumError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationCheckSumErr = flowControllerComunicationStatus & FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerCommunicationCheckSum].currentStatus == eInactive)
	{
		if (communicationCheckSumErr  == FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationCheckSumError++;
			if (countTimesCommunicationCheckSumError < ERROR_THREE_TIMNES)
			{
                s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesCommunicationCheckSumError == ERROR_ONE_TIMES)
		{
			s_FlowControllerCommunicationCheckSumErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCheckSumError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationCheckSumErrorAlarmCounter;
				s_FlowControllerCommunicationCheckSumErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerCommunicationCheckSumErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCheckSumError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerCommunicationCheckSumErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerCommunicationCheckSum].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Communication CheckSum Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationCheckSumErrorAlarmCounter;
				s_FlowControllerCommunicationCheckSumErrorAlarmCounter = 0;
				countTimesCommunicationCheckSumError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerCommunicationCheckSum].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller Communication Command Alarm   	 	*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus(uint8_t flowControllerComunicationStatus)//E23
{
	static uint32_t s_FlowControllerCommunicationCommandErrorAlarmCounter = 0;		//E23
	static uint8_t countTimesCommunicationCommandError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationCommandErr = flowControllerComunicationStatus & FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerCommunicationCommand].currentStatus == eInactive)
	{
		if (communicationCommandErr  == FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationCommandError++;
			if (countTimesCommunicationCommandError < ERROR_THREE_TIMNES)
			{
                s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesCommunicationCommandError == ERROR_ONE_TIMES)
		{
			s_FlowControllerCommunicationCommandErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCommandError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationCommandErrorAlarmCounter;
				s_FlowControllerCommunicationCommandErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerCommunicationCommandErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCommandError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerCommunicationCommandErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerCommunicationCommand].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Communication Command Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationCommandErrorAlarmCounter;
				s_FlowControllerCommunicationCommandErrorAlarmCounter = 0;
				countTimesCommunicationCommandError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerCommunicationCommand].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller Communication Parameter Alarm  	 	*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus(uint8_t flowControllerComunicationStatus)//E24
{
	static uint32_t s_FlowControllerCommunicationParameterErrorAlarmCounter = 0;	//E24
	static uint8_t countTimesCommunicationParameterError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationParameterErr = flowControllerComunicationStatus & FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmFlowControllerCommunicationParameter].currentStatus == eInactive)
	{
		if (communicationParameterErr  == FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationParameterError++;
			if (countTimesCommunicationParameterError < ERROR_THREE_TIMNES)
			{
			    s_isSendFlowControllerResetCM = true;
			}
		}
		if (countTimesCommunicationParameterError == ERROR_ONE_TIMES)
		{
			s_FlowControllerCommunicationParameterErrorAlarmCounter++;
		}
		else if (countTimesCommunicationParameterError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationParameterErrorAlarmCounter;
				s_FlowControllerCommunicationParameterErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_FlowControllerCommunicationParameterErrorAlarmCounter++;
		}
		else if (countTimesCommunicationParameterError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_FlowControllerCommunicationParameterErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmFlowControllerCommunicationParameter].currentStatus = eActive;
				DEBUG_ALARM_MGR("Flow Controller Communication Parameter Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_FlowControllerCommunicationParameterErrorAlarmCounter;
				s_FlowControllerCommunicationParameterErrorAlarmCounter = 0;
				countTimesCommunicationParameterError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerCommunicationParameter].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus	 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller	Response CheckSum Error Alarm  	 	*/
/*                                                                                      */
/* Arguments : (I) E_FlowControllerNoAlarmStatus flowControllerResponseStatus   		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus(E_FlowControllerNoAlarmStatus flowControllerResponseStatus)//E25
{
	static uint8_t CountTimesCheckSumError = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if (stAlarmList[eAlarmFlowControllerResponseCheckSumError].currentStatus == eInactive)
	{
		if (flowControllerResponseStatus  == eResponseCheckSumErr)
			CountTimesCheckSumError++;
		else
			CountTimesCheckSumError = 0;
		if(CountTimesCheckSumError == FC_CHECKSUM_ERROR_TIMES)
		{
		    s_isSendFlowControllerResetCM = true;
			stAlarmList[eAlarmFlowControllerResponseCheckSumError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Flow Controller Response CheckSum Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerResponseCheckSumError].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus		 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller	Response Command Code Error Alarm  	*/
/*                                                                                      */
/* Arguments : (I) E_FlowControllerNoAlarmStatus flowControllerResponseStatus   		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus(E_FlowControllerNoAlarmStatus flowControllerResponseStatus)//E26
{
	static uint8_t CountTimesCodeError = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if (stAlarmList[eAlarmFlowControllerResponseCodeError].currentStatus == eInactive)
	{
		if (flowControllerResponseStatus  == eResponseCodeErr)
			CountTimesCodeError++;
		else
			CountTimesCodeError = 0;
		if(CountTimesCodeError == FC_CODE_ERROR_TIMES)
		{
            s_isSendFlowControllerResetCM = true;
			stAlarmList[eAlarmFlowControllerResponseCodeError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Flow Controller Response Code Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerResponseCodeError].currentStatus ;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus	 	*/
/*                                                                                      */
/* Details:         check status of Flow Controller	Response Data Length Error Alarm   	*/
/*                                                                                      */
/* Arguments : (I) E_FlowControllerNoAlarmStatus flowControllerResponseStatus   		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus(E_FlowControllerNoAlarmStatus flowControllerResponseStatus)//E27
{
	static uint8_t CountTimesDataLengthError = 0;
    static bool s_isSendFlowControllerResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if (stAlarmList[eAlarmFlowControllerResponseDataLengthError].currentStatus == eInactive)
	{
		if (flowControllerResponseStatus  == eResponseLengthErr)
			CountTimesDataLengthError++;
		else
			CountTimesDataLengthError = 0;
		if(CountTimesDataLengthError == FC_DATALENGHT_ERROR_TIMES)
		{
		    s_isSendFlowControllerResetCM = true;
			stAlarmList[eAlarmFlowControllerResponseDataLengthError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Flow Controller Response Data Length Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendFlowControllerResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eFlowControllerResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//            s_isSendFlowControllerResetCM = false;
//        }
    }
    return stAlarmList[eAlarmFlowControllerResponseDataLengthError].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus	   	*/
/*                                                                                      */
/* Details:         check Compressor Communication Error Alarm (No Response)  			*/
/*                                                                                      */
/* Arguments : (I)  bool isCompressorCommunicationError		        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                     (I) g_isResetFlowController                                      */
/*                     (I) s_FlowControllerCommunicationErrorAlarmCounter              */
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus(uint16_t compressorStatus)//E28
{
	static uint32_t s_CompressorCommunicationErrorAlarmCounter = 0;	//E28
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorCommunicationError = compressorStatus & COMP_COMMUNICATION_NO_RESPONSE_ERR_BIT;
	if (compressorCommunicationError)
		s_CompressorCommunicationErrorAlarmCounter++;
	else
	{
		s_CompressorCommunicationErrorAlarmCounter = 0;
		stAlarmList[eAlarmCompressorCommunicationNoResponse].currentStatus = eInactive;
	}
	if	(s_CompressorCommunicationErrorAlarmCounter >= ALM_TICK_PER_SEC*60)//60s
	{
		s_CompressorCommunicationErrorAlarmCounter = 0;
		if(compressorCommunicationError)
		{
            s_isSendCompressorResetCM = true;
			stAlarmList[eAlarmCompressorCommunicationNoResponse].currentStatus = eActive;
			DEBUG_ALARM_MGR("Compressor Communication No Response Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorCommunicationNoResponse].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus	       		*/
/*                                                                                      */
/* Details:         check status of Compressor DC Over Voltage Alarm     				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus(uint16_t compressorStatus)//E29
{
	static uint32_t s_CompressorDCOverVoltageErrorAlarmCounter = 0;	//E29
	static uint8_t countTimesCompressorDCOverVoltageError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorDCOverVoltageError = compressorStatus & DC_OVER_VOLT_ERR_BIT;
	if (stAlarmList[eAlarmCompressorDCOverVoltage].currentStatus == eInactive)
	{
		if (compressorDCOverVoltageError  == DC_OVER_VOLT_ERR_BIT)
		{
			countTimesCompressorDCOverVoltageError++;
			if (countTimesCompressorDCOverVoltageError < ERROR_THREE_TIMNES)
			{
                s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorDCOverVoltageError == ERROR_ONE_TIMES)
		{
			s_CompressorDCOverVoltageErrorAlarmCounter++;
		}
		else if (countTimesCompressorDCOverVoltageError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorDCOverVoltageErrorAlarmCounter;
				s_CompressorDCOverVoltageErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorDCOverVoltageErrorAlarmCounter++;
		}
		else if (countTimesCompressorDCOverVoltageError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorDCOverVoltageErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorDCOverVoltage].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor DC Over Voltage Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorDCOverVoltageErrorAlarmCounter;
				s_CompressorDCOverVoltageErrorAlarmCounter = 0;
				countTimesCompressorDCOverVoltageError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorDCOverVoltage].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus	   			*/
/*                                                                                      */
/* Details:         check status of Compressor DC Lower Voltage Alarm     				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus(uint16_t compressorStatus)//E30
{
	static uint32_t s_CompressorDCLowerVoltageErrorAlarmCounter = 0;	//E30
	static uint8_t countTimesCompressorDClowerVoltageError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorDCLowerVoltageError = compressorStatus & DC_LOW_VOLT_ERR_BIT;
	if (stAlarmList[eAlarmCompressorDCLowerVoltage].currentStatus == eInactive)
	{
		if (compressorDCLowerVoltageError  == DC_LOW_VOLT_ERR_BIT)
		{
			countTimesCompressorDClowerVoltageError++;
			if (countTimesCompressorDClowerVoltageError < ERROR_THREE_TIMNES)
			{
                s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorDClowerVoltageError == ERROR_ONE_TIMES)
		{
			s_CompressorDCLowerVoltageErrorAlarmCounter++;
		}
		else if (countTimesCompressorDClowerVoltageError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorDCLowerVoltageErrorAlarmCounter;
				s_CompressorDCLowerVoltageErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorDCLowerVoltageErrorAlarmCounter++;
		}
		else if (countTimesCompressorDClowerVoltageError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorDCLowerVoltageErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorDCLowerVoltage].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor DC Lower Voltage Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorDCLowerVoltageErrorAlarmCounter;
				s_CompressorDCLowerVoltageErrorAlarmCounter = 0;
				countTimesCompressorDClowerVoltageError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorDCLowerVoltage].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus	   		*/
/*                                                                                      */
/* Details:         check status of Compressor Software Over Current Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus(uint16_t compressorStatus)//E31
{
	static uint32_t s_CompressorSoftwareOverCurrentErrorAlarmCounter = 0;	//E31
	static uint8_t countTimesCompressorSoftwareOverCurrentError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorSoftwareOverCurrentError = compressorStatus & SOFT_OVER_CURRENT_ERR_BIT;
	if (stAlarmList[eAlarmCompressorSoftwareOverCurrent].currentStatus == eInactive)
	{
		if (compressorSoftwareOverCurrentError  == SOFT_OVER_CURRENT_ERR_BIT)
		{
			countTimesCompressorSoftwareOverCurrentError++;
			if (countTimesCompressorSoftwareOverCurrentError < ERROR_THREE_TIMNES)
			{
                s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorSoftwareOverCurrentError == ERROR_ONE_TIMES)
		{
			s_CompressorSoftwareOverCurrentErrorAlarmCounter++;
		}
		else if (countTimesCompressorSoftwareOverCurrentError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorSoftwareOverCurrentErrorAlarmCounter;
				s_CompressorSoftwareOverCurrentErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorSoftwareOverCurrentErrorAlarmCounter++;
		}
		else if (countTimesCompressorSoftwareOverCurrentError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorSoftwareOverCurrentErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorSoftwareOverCurrent].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Software Over Current Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorSoftwareOverCurrentErrorAlarmCounter;
				s_CompressorSoftwareOverCurrentErrorAlarmCounter = 0;
				countTimesCompressorSoftwareOverCurrentError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorSoftwareOverCurrent].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus	       	*/
/*                                                                                      */
/* Details:         check status of Compressor Hardware Over Current Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  uint8_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus(uint16_t compressorStatus)//E32
{
	static uint32_t s_CompressorHardwareOverCurrentErrorAlarmCounter = 0;	//E32
	static uint8_t countTimesCompressorHardwareOverCurrentError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorHardwareOverCurrentError = compressorStatus & HARD_OVER_CURRENT_ERR_BIT;
	if (stAlarmList[eAlarmCompressorHardwareOverCurrent].currentStatus == eInactive)
	{
		if (compressorHardwareOverCurrentError  == HARD_OVER_CURRENT_ERR_BIT)
		{
			countTimesCompressorHardwareOverCurrentError++;
			if (countTimesCompressorHardwareOverCurrentError < ERROR_THREE_TIMNES)
			{
                s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorHardwareOverCurrentError == ERROR_ONE_TIMES)
		{
			s_CompressorHardwareOverCurrentErrorAlarmCounter++;
		}
		else if (countTimesCompressorHardwareOverCurrentError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorHardwareOverCurrentErrorAlarmCounter;
				s_CompressorHardwareOverCurrentErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorHardwareOverCurrentErrorAlarmCounter++;
		}
		else if (countTimesCompressorHardwareOverCurrentError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorHardwareOverCurrentErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorHardwareOverCurrent].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Hardware Over Current Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorHardwareOverCurrentErrorAlarmCounter;
				s_CompressorHardwareOverCurrentErrorAlarmCounter = 0;
				countTimesCompressorHardwareOverCurrentError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorHardwareOverCurrent].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus	   		*/
/*                                                                                      */
/* Details:         check status of Compressor Board Temperature Alarm    				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus(uint16_t compressorStatus)//E33
{
	static uint32_t s_CompressorBoardTemperatureErrorAlarmCounter = 0;	//E33
	static uint8_t countTimesCompressorBoardTemperatureError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorBoardTemperatureError = compressorStatus & BOARD_TEMP_ERR_BIT;
	if (stAlarmList[eAlarmCompressorBoardTemperature].currentStatus == eInactive)
	{
		if (compressorBoardTemperatureError  == BOARD_TEMP_ERR_BIT)
		{
			countTimesCompressorBoardTemperatureError++;
			if (countTimesCompressorBoardTemperatureError < ERROR_THREE_TIMNES)
			{
                s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorBoardTemperatureError == ERROR_ONE_TIMES)
		{
			s_CompressorBoardTemperatureErrorAlarmCounter++;
		}
		else if (countTimesCompressorBoardTemperatureError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorBoardTemperatureErrorAlarmCounter;
				s_CompressorBoardTemperatureErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorBoardTemperatureErrorAlarmCounter++;
		}
		else if (countTimesCompressorBoardTemperatureError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorBoardTemperatureErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorBoardTemperature].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Board Temperature Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorBoardTemperatureErrorAlarmCounter;
				s_CompressorBoardTemperatureErrorAlarmCounter = 0;
				countTimesCompressorBoardTemperatureError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorBoardTemperature].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus	       	*/
/*                                                                                      */
/* Details:         check status of Compressor Motor Temperature Alarm    				*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus(uint16_t compressorStatus)//E34
{
	static uint32_t s_CompressorMotorTemperatureErrorAlarmCounter = 0;	//E34
	static uint8_t countTimesCompressorMotorTemperatureError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorMotorTemperatureError = compressorStatus & MOTOR_TEMP_ERR_BIT;
	if (stAlarmList[eAlarmCompressorMotorTemperature].currentStatus == eInactive)
	{
		if (compressorMotorTemperatureError  == MOTOR_TEMP_ERR_BIT)
		{
			countTimesCompressorMotorTemperatureError++;
			if (countTimesCompressorMotorTemperatureError < ERROR_THREE_TIMNES)
			{
				s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorMotorTemperatureError == ERROR_ONE_TIMES)
		{
			s_CompressorMotorTemperatureErrorAlarmCounter++;
		}
		else if (countTimesCompressorMotorTemperatureError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorMotorTemperatureErrorAlarmCounter;
				s_CompressorMotorTemperatureErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorMotorTemperatureErrorAlarmCounter++;
		}
		else if (countTimesCompressorMotorTemperatureError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorMotorTemperatureErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorMotorTemperature].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Motor Temperature Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorMotorTemperatureErrorAlarmCounter;
				s_CompressorMotorTemperatureErrorAlarmCounter = 0;
				countTimesCompressorMotorTemperatureError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorMotorTemperature].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus	       		*/
/*                                                                                      */
/* Details:         check status of Compressor Rotation Speed Error Alarm    			*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus(uint16_t compressorStatus)//E35
{
	static uint32_t s_CompressorRotationSpeedErrorAlarmCounter = 0;	//E35
	static uint8_t countTimesCompressorRotationSpeedError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorRotationSpeedError = compressorStatus & RPM_ERR_BIT;
	if (stAlarmList[eAlarmCompressorRotationSpeed].currentStatus == eInactive)
	{
		if (compressorRotationSpeedError  == RPM_ERR_BIT)
		{
			countTimesCompressorRotationSpeedError++;
			if (countTimesCompressorRotationSpeedError < ERROR_THREE_TIMNES)
			{
				s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCompressorRotationSpeedError == ERROR_ONE_TIMES)
		{
			s_CompressorRotationSpeedErrorAlarmCounter++;
		}
		else if (countTimesCompressorRotationSpeedError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorRotationSpeedErrorAlarmCounter;
				s_CompressorRotationSpeedErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorRotationSpeedErrorAlarmCounter++;
		}
		else if (countTimesCompressorRotationSpeedError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorRotationSpeedErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorRotationSpeed].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Rotation Speed Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorRotationSpeedErrorAlarmCounter;
				s_CompressorRotationSpeedErrorAlarmCounter = 0;
				countTimesCompressorRotationSpeedError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorRotationSpeed].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorStepOutErrorAlarmStatus			       		*/
/*                                                                                      */
/* Details:         check status of Compressor Step Out Error Alarm    					*/
/*                                                                                      */
/* Arguments : (I)  uint8_t compressorStatus        									*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList  												*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorStepOutErrorAlarmStatus(uint16_t compressorStatus)//E36
{
	static uint32_t s_CompressorStepOutErrorAlarmCounter = 0;	//E36
	static uint8_t CountTimesCompressorStepOutError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint16_t compressorStepOutError = compressorStatus & MOTOR_STEP_OUT_ERR_BIT;

	if (stAlarmList[eAlarmCompressorStepOut].currentStatus == eInactive)
	{
		if (compressorStepOutError  == MOTOR_STEP_OUT_ERR_BIT)
		{
			CountTimesCompressorStepOutError++;
			if(CountTimesCompressorStepOutError < ERROR_THREE_TIMNES)
			{
			    s_isSendCompressorResetCM = true;
			}
		}
		if (CountTimesCompressorStepOutError == ERROR_ONE_TIMES)
		{
			s_CompressorStepOutErrorAlarmCounter++;
		}
		else if (CountTimesCompressorStepOutError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorStepOutErrorAlarmCounter;
				s_CompressorStepOutErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorStepOutErrorAlarmCounter++;
		}
		else if (CountTimesCompressorStepOutError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorStepOutErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorStepOut].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Step Out Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorStepOutErrorAlarmCounter;
				s_CompressorStepOutErrorAlarmCounter = 0;
				CountTimesCompressorStepOutError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorStepOut].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorCommunicationErrorAlarmStatus	 		  	*/
/*                                                                                      */
/* Details:         check Compressor Communication Error Alarm (No Response)  			*/
/*                                                                                      */
/* Arguments : (I)  uint16_t compressorStatus				        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                     (I) g_isResetCompressor		                                    */
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorCommunicationErrorAlarmStatus(uint16_t compressorStatus)//E37
{
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if	(stAlarmList[eAlarmCompressorCommunication].currentStatus == eInactive)
	{
		if(compressorStatus & COMP_CMD_TIME_OUT_BIT)
		{
		    s_isSendCompressorResetCM = true;
			stAlarmList[eAlarmCompressorCommunication].currentStatus = eActive;
			DEBUG_ALARM_MGR("Compressor Communication Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorCommunication].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus 		*/
/*                                                                                      */
/* Details:         check status of Compressor Communication CheckSum Alarm   		 	*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus(uint8_t compressorComunicationStatus)//E38
{
	static uint32_t s_CompressorCommunicationCheckSumErrorAlarmCounter = 0;	//E38
	static uint8_t countTimesCommunicationCheckSumError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationCheckSumErr = compressorComunicationStatus & COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmCompressorCommunicationCheckSum].currentStatus == eInactive)
	{
		if (communicationCheckSumErr  == COMPRESSOR_CHECKSUM_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationCheckSumError++;
			if (countTimesCommunicationCheckSumError < ERROR_THREE_TIMNES)
			{
			    s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCommunicationCheckSumError == ERROR_ONE_TIMES)
		{
			s_CompressorCommunicationCheckSumErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCheckSumError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationCheckSumErrorAlarmCounter;
				s_CompressorCommunicationCheckSumErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorCommunicationCheckSumErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCheckSumError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorCommunicationCheckSumErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorCommunicationCheckSum].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Communication CheckSum Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationCheckSumErrorAlarmCounter;
				s_CompressorCommunicationCheckSumErrorAlarmCounter = 0;
				countTimesCommunicationCheckSumError = ERROR_TWO_TIMES;
			}
		}
	}
    else
    {
        //do nothing
    }
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorCommunicationCheckSum].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus 		*/
/*                                                                                      */
/* Details:         check status of Compressor Communication Command Alarm   	 		*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus(uint8_t compressorComunicationStatus)//E39
{
	static uint32_t s_CompressorCommunicationCommandErrorAlarmCounter = 0;		//E39
	static uint8_t countTimesCommunicationCommandError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationCommandErr = compressorComunicationStatus & COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmCompressorCommunicationCommand].currentStatus == eInactive)
	{
		if (communicationCommandErr  == COMPRESSOR_COMMAND_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationCommandError++;
			if (countTimesCommunicationCommandError < ERROR_THREE_TIMNES)
			{
			    s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCommunicationCommandError == ERROR_ONE_TIMES)
		{
			s_CompressorCommunicationCommandErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCommandError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationCommandErrorAlarmCounter;
				s_CompressorCommunicationCommandErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorCommunicationCommandErrorAlarmCounter++;
		}
		else if (countTimesCommunicationCommandError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorCommunicationCommandErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorCommunicationCommand].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Communication Command Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationCommandErrorAlarmCounter;
				s_CompressorCommunicationCommandErrorAlarmCounter = 0;
				countTimesCommunicationCommandError = ERROR_TWO_TIMES;
			}
		}
	}
	else
	{
	    //do nothing
	}
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorCommunicationCommand].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CompressorCommunicationParameterErrorAlarmStatus 			*/
/*                                                                                      */
/* Details:         check status of Flow Controller Communication Parameter Alarm  	 	*/
/*                                                                                      */
/* Arguments : (I)  uint8_t flowControllerStatus        								*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorCommunicationParameterErrorAlarmStatus(uint8_t compressorComunicationStatus)//E40
{
	static uint32_t s_CompressorCommunicationParameterErrorAlarmCounter = 0;	//E40
	static uint8_t countTimesCommunicationParameterError = 0;
	static uint32_t timeBetweenOfTwiceErrorContinuous = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	uint8_t communicationParameterErr = compressorComunicationStatus & COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT;
	if (stAlarmList[eAlarmCompressorCommunicationParameter].currentStatus == eInactive)
	{
		if (communicationParameterErr  == COMPRESSOR_PARAMETER_COMMUNICATION_ERR_BIT)
		{
			countTimesCommunicationParameterError++;
			if (countTimesCommunicationParameterError < ERROR_THREE_TIMNES)
			{
			    s_isSendCompressorResetCM = true;
			}
		}
		if (countTimesCommunicationParameterError == ERROR_ONE_TIMES)
		{
			s_CompressorCommunicationParameterErrorAlarmCounter++;
		}
		else if (countTimesCommunicationParameterError == ERROR_TWO_TIMES)
		{
			static bool isError2st = true;
			if(isError2st == true)
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationParameterErrorAlarmCounter;
				s_CompressorCommunicationParameterErrorAlarmCounter = 0;
				isError2st = false;
			}
			s_CompressorCommunicationParameterErrorAlarmCounter++;
		}
		else if (countTimesCommunicationParameterError == ERROR_THREE_TIMNES)
		{
			if(timeBetweenOfTwiceErrorContinuous + s_CompressorCommunicationParameterErrorAlarmCounter < ALM_TICK_PER_SEC*60)
			{
				stAlarmList[eAlarmCompressorCommunicationParameter].currentStatus = eActive;
				DEBUG_ALARM_MGR("Compressor Communication Parameter Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
			}
			else
			{
				timeBetweenOfTwiceErrorContinuous = s_CompressorCommunicationParameterErrorAlarmCounter;
				s_CompressorCommunicationParameterErrorAlarmCounter = 0;
				countTimesCommunicationParameterError = ERROR_TWO_TIMES;
			}
		}
	}
	else
	{
	    //do nothing
	}
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorCommunicationParameter].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus		 	*/
/*                                                                                      */
/* Details:         check status of Compressor Response CheckSum Error Alarm  	 		*/
/*                                                                                      */
/* Arguments : (I) E_CompressorStatus CompressorResponseStatus   						*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus(E_CompressorStatus compressorResponseStatus)//E41
{
	static uint8_t CountTimesCompressorCheckSumError = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if (stAlarmList[eAlarmCompressorResponseCheckSumError].currentStatus == eInactive)
	{
		if (compressorResponseStatus  == eCompResponseChecksumErr)
			CountTimesCompressorCheckSumError++;
		else
			CountTimesCompressorCheckSumError = 0;
		if(CountTimesCompressorCheckSumError == CD_CHECKSUM_ERROR_TIMES)
		{
		    s_isSendCompressorResetCM = true;
			stAlarmList[eAlarmCompressorResponseCheckSumError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Compressor Response CheckSum Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	else
	{
	    //do nothing
	}
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorResponseCheckSumError].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus			 	*/
/*                                                                                      */
/* Details:         check status of Compressor	Response Command Code Error Alarm  		*/
/*                                                                                      */
/* Arguments : (I) E_CompressorStatus compressorResponseStatus					   		*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus(E_CompressorStatus compressorResponseStatus)//E42
{
	static uint8_t CountTimesCompressorCodeError = 0;
	static bool s_isSendCompressorResetCM = false;
//	DEV_EVENT_STRUCT deviceQueue;

	if (stAlarmList[eAlarmCompressorResponseCodeError].currentStatus == eInactive)
	{
		if (compressorResponseStatus  == eCompResponseCodeErr)
			CountTimesCompressorCodeError++;
		else
			CountTimesCompressorCodeError = 0;
		if(CountTimesCompressorCodeError == CD_CODE_ERROR_TIMES)
		{
		    s_isSendCompressorResetCM = true;
			stAlarmList[eAlarmCompressorResponseCodeError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Compressor Response Code Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
        }
	}
	else
	{
	    // do nothing
	}
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorResponseCodeError].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus		 	*/
/*                                                                                      */
/* Details:         check status of Compressor	Response Data Length Error Alarm   		*/
/*                                                                                      */
/* Arguments : (I) E_CompressorStatus compressorResponseStatus   						*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus(E_CompressorStatus compressorResponseStatus)//E43
{
	static uint8_t CountTimesCompressorDataLengthError = 0;
    static bool s_isSendCompressorResetCM = false;
//    DEV_EVENT_STRUCT deviceQueue;
	if (stAlarmList[eAlarmCompressorResponseDataLenghtError].currentStatus == eInactive)
	{
		if (compressorResponseStatus  == eCompResponseLengthErr)
			CountTimesCompressorDataLengthError++;
		else
			CountTimesCompressorDataLengthError = 0;
		if(CountTimesCompressorDataLengthError == CD_DATALENGHT_ERROR_TIMES)
		{
		    s_isSendCompressorResetCM = true;
			stAlarmList[eAlarmCompressorResponseDataLenghtError].currentStatus = eActive;
			DEBUG_ALARM_MGR("Compressor Response Data Length Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);

		}
	}
	else
	{
	    //do nothing
	}
    if (s_isSendCompressorResetCM == true)
    {
//        deviceQueue.id = eFromAlarmTask;
//        deviceQueue.msgData.alarmMsg.deviceId = eCompressorResetCommand;
//        if(!devTask_SendToDeviceQueue(deviceQueue,10))
//        {
//            DEBUG_TASK("Failed to send Item to deviceQueue from AlarmTask");
//        }
//        else
//        {
//        	s_isSendCompressorResetCM = false;
//        }
    }
    return stAlarmList[eAlarmCompressorResponseDataLenghtError].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckAudioCommunicateErrorAlarmStatus	       				*/
/*                                                                                      */
/* Details:         check status of Audio Communicate Alarm    							*/
/*                                                                                      */
/* Arguments : (I)  bool audioCommunication				        						*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckAudioCommunicateErrorAlarmStatus(bool audioCommunication)//E45
{
	if (stAlarmList[eAlarmAudioCommunication].currentStatus == eInactive)
	{
		if (audioCommunication == true)
		{
			stAlarmList[eAlarmAudioCommunication].currentStatus = eActive;
			DEBUG_ALARM_MGR("Audio Communicate Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	return stAlarmList[eAlarmAudioCommunication].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckSpeakerAlarmStatus	        						*/
/*                                                                                      */
/* Details:         check status of Speaker Alarm    									*/
/*                                                                                      */
/* Arguments : (I)  bool speakerCommunication				        					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckSpeakerErrorAlarmStatus(bool isSpeakerConnected)//E46
{
	if (stAlarmList[eAlarmSpeaker].currentStatus == eInactive)
	{
		if (isSpeakerConnected == false)
		{
			stAlarmList[eAlarmSpeaker].currentStatus = eActive;
			DEBUG_ALARM_MGR("Speaker Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	return stAlarmList[eAlarmSpeaker].currentStatus;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckBatteryTypeAlarmStatus	        					*/
/*                                                    	                                */
/* Details:         check Type Battery    												*/
/*                                                                                      */
/* Arguments : (I)  bool isValidBattery				        							*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/
E_AlarmStatus alarmMgr_CheckBatteryTypeAlarmStatus(bool isBatteryConnected, bool isInvalidBattery)//E48
{
	static uint16_t countTimeBatteryTypeError = 0;

	if (isBatteryConnected == true)
	{
		if (stAlarmList[eAlarmBatteryType].currentStatus == eInactive)
		{
			if (isInvalidBattery == true)
			{
				countTimeBatteryTypeError++;
				if(countTimeBatteryTypeError >= ALM_TICK_PER_SEC*10)//10s
				{
					countTimeBatteryTypeError = 0;
					stAlarmList[eAlarmBatteryType].currentStatus = eActive;
					DEBUG_ALARM_MGR("Battery Type Error Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
				}
			}
		}
		else if(isInvalidBattery == false)
		{
			countTimeBatteryTypeError = 0;
			stAlarmList[eAlarmBatteryType].currentStatus = eInactive;
			DEBUG_ALARM_MGR("Battery Type Error Alarm eInactive --> %ds\n", gs_alarmTaskTimer/20);
		}
	}
	else
		countTimeBatteryTypeError = 0;
	return stAlarmList[eAlarmBatteryType].currentStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMgr_CheckTachypneaErrorAlarmStatus	        					*/
/*                                                                                      */
/* Details:         check Breath rate Alarm    											*/
/*                                                                                      */
/* Arguments : (I)  uint8_t breathRate				       			 					*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) stAlarmList													*/
/*                                                                                      */
/****************************************************************************************/

void alarmMgr_SetupForCheckTachypneaErrorAlarmStatus()
{
	gs_currentAlarmMonitor.operationMode = eSynchronizedMode;
}
E_AlarmStatus alarmMgr_CheckTachypneaErrorAlarmStatus(uint8_t breathRate, bool isBreathTrigger)//E49
{
	static uint32_t s_TachypneaErrorAlarmCounter;//E49
	if(gs_currentAlarmMonitor.operationMode == eSynchronizedMode)
	{
		if (stAlarmList[eAlarmTachypnea].currentStatus == eInactive)
		{
			static uint32_t timeOutNoBreath;
			static bool preIsBreathTrigger = false;
			if (isBreathTrigger != preIsBreathTrigger)
			{
				timeOutNoBreath = 0;
			}
			else
			{
				timeOutNoBreath++;
			}
			preIsBreathTrigger = isBreathTrigger;
			if (breathRate > BREATH_RATE_HIGH)
			{
				if(timeOutNoBreath < ALM_TACHYPNEA_TIMEOUT*ALM_TICK_PER_SEC)//3s
				{
					if (s_TachypneaErrorAlarmCounter >= ALM_TICK_PER_SEC*60)//1s*60= 60s
					{
						stAlarmList[eAlarmTachypnea].currentStatus = eActive;
						DEBUG_ALARM_MGR("Tachypnea Alarm Active --> %ds\n", gs_alarmTaskTimer/20);
					}
				}
				s_TachypneaErrorAlarmCounter++;
			}
			else
				s_TachypneaErrorAlarmCounter = 0;
		}
		else if (breathRate <= BREATH_RATE_HIGH)
		{
			stAlarmList[eAlarmTachypnea].currentStatus = eInactive;
			s_TachypneaErrorAlarmCounter = 0;
		}
	}
	else
	{
		stAlarmList[eAlarmTachypnea].currentStatus = eInactive;
	}
	return stAlarmList[eAlarmTachypnea].currentStatus;
}


///****************************************************************************************/
///*                                                                                      */
///* Function name: 	alarmMgr_CheckAllAlarm				 								*/
///*                                                                                      */
///* Details:         check status for all of alarm depend on operation mode				*/
///* Using Global Data : (I) stAlarmList													*/
///*                                                                                      */
///****************************************************************************************/
////void alarmMgr_CheckAllAlarm(void)
////{
////	alarmMgr_CheckBreathErrorAlarmStatus(gs_currentAlarmMonitor.flowRateSetting, gs_currentAlarmMonitor.isBreathTrigger,
////			gs_currentAlarmMonitor.flowPress);//E01
////	if (gs_alarmTaskTimer >= ALM_TICK_PER_SEC*60*5)//1s*60*5 = 5Mins
////	{
////		alarmMgr_CheckFCAbnormalOxygenConcentration(gs_currentAlarmMonitor.O2Concentration);
////		alarmMgr_CheckAbnormalOxygenConcentrationAlarmStatus(gs_currentAlarmMonitor.flowRateSetting,
////															 gs_currentAlarmMonitor.O2Concentration); //E02
////		alarmMgr_CheckLowOxygenConcentrationAlarmStatus(gs_currentAlarmMonitor.flowRateSetting,
////														gs_currentAlarmMonitor.O2Concentration);	//E03
////		alarmMgr_OxygenTankPressureErrorAlarmStatus(gs_currentAlarmMonitor.tankPressure);	//E04
////		alarmMgr_CheckCompressorSupplyPressureErrorAlarmStatus(gs_currentAlarmMonitor.flowRateSetting,
////				gs_currentAlarmMonitor.providePressure,
////				gs_currentAlarmMonitor.PSATemperature);	//E09
////		alarmMgr_CheckCompressorSuctionPressureErrorAlarmStatus(gs_currentAlarmMonitor.vacuumPressure);	//E10
////		alarmMgr_CheckFlowControlErrorAlarmStatus(gs_currentAlarmMonitor.flowRateSetting, gs_currentAlarmMonitor.flowRateSensor);//E11
////	}
////
////	alarmMgr_CheckBatteryTemperatureErrorAlarmStatus(gs_currentAlarmMonitor.isBatteryConnected,
////													 gs_currentAlarmMonitor.batteryStatus); //E05
////	alarmMgr_CheckBatteryChargeErrorAlarmStatus(gs_currentAlarmMonitor.isAC100VplugIn,
////												gs_currentAlarmMonitor.isBatteryConnected,
////												gs_currentAlarmMonitor.isBatteryChargingError,
////												gs_currentAlarmMonitor.isBatteryCharging,
////												gs_currentAlarmMonitor.batteryStatus);	//E06
////
////	if (gs_alarmTaskTimer >= ALM_TICK_PER_SEC*10)//10s
////	{
////		alarmMgr_CheckBatteryRemainingCapacityLowerAlarmStatus(gs_currentAlarmMonitor.batteryCapacity
////				,gs_currentAlarmMonitor.isBatteryConnected
////				,gs_currentAlarmMonitor.isAC100VplugIn);	//E07
////		alarmMgr_CheckSpeakerErrorAlarmStatus(gs_currentAlarmMonitor.isSpeakerError);	//E46
////	}
////
////	alarmMgr_CheckSupplyPressureSensorErrorAlarmStatus(gs_currentAlarmMonitor.providePressure);	//E12
////	alarmMgr_CheckVacuumPressureSensorErrorAlarmStatus(gs_currentAlarmMonitor.vacuumPressure);	//E13
////	alarmMgr_CheckTankPressureSensorErrorAlarmStatus(gs_currentAlarmMonitor.tankPressure);		//E14
////	alarmMgr_CheckDifferentialPressureSensorCommunicationErrorAlarmStatus(gs_currentAlarmMonitor.differentialPressure);	//E15
////	alarmMgr_CheckOxygenSensorErrorAlarmStatus(gs_currentAlarmMonitor.O2Concentration);	//E16
////	alarmMgr_CheckFlowControllerCommunicationNoResponseErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerStatus);//E17
////	alarmMgr_CheckFlowControllerPressureSensorErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerStatus);	//E18
////	alarmMgr_CheckFlowControllerFlowSensorErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerStatus);		//E19
////	alarmMgr_CheckFlowControllerTemperatureSensorErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerStatus);	//E20
////	alarmMgr_CheckFlowControllerCommunicationErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerStatus);		//E21
////	alarmMgr_CheckFlowControllerCommunicationChecksumErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerCommunicateStatus);	//E22
////	alarmMgr_CheckFlowControllerCommunicationCommandErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerCommunicateStatus);	//E23
////	alarmMgr_CheckFlowControllerCommunicationParameterErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerCommunicateStatus);//E24
////	alarmMgr_CheckFlowControllerResponseCheckSumErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerResponseStatus);	//E25
////	alarmMgr_CheckFlowControllerResponseCodeErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerResponseStatus);		//E26
////	alarmMgr_CheckFlowControllerResponseDataLengthErrorAlarmStatus(gs_currentAlarmMonitor.flowControllerResponseStatus);//E27
////	alarmMgr_CheckCompressorCommunicationNoResponseErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);//E28
////	alarmMgr_CheckCompressorDCOverVoltageErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E29
////	alarmMgr_CheckCompressorDCLowerVoltageErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);//E30
////	alarmMgr_CheckCompressorSoftwareOverCurrentErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E31
////	alarmMgr_CheckCompressorHardwareOverCurrentErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E32
////	alarmMgr_CheckCompressorBoardTemperatureErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E33
////	alarmMgr_CheckCompressorMotorTemperatureErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E34
////	alarmMgr_CheckCompressorRotationSpeedErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);		//E35
////	alarmMgr_CheckCompressorStepOutErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);	//E36
////	alarmMgr_CheckCompressorCommunicationErrorAlarmStatus(gs_currentAlarmMonitor.compressorStatus);//E37
////	alarmMgr_CheckCompressorCommunicationChecksumErrorAlarmStatus(gs_currentAlarmMonitor.compressorCommunicateStatus);	//E38
////	alarmMgr_CheckCompressorCommunicationCommandErrorAlarmStatus(gs_currentAlarmMonitor.compressorCommunicateStatus);	//E39
////	alarmMgr_CheckCompressorCommunicationParameterErrorAlarmStatus(gs_currentAlarmMonitor.compressorCommunicateStatus);	//E40
////	alarmMgr_CheckCompressorResponseCheckSumErrorAlarmStatus(gs_currentAlarmMonitor.compressorResponseStatus);	//E41
////	alarmMgr_CheckCompressorResponseCodeErrorAlarmStatus(gs_currentAlarmMonitor.compressorResponseStatus);		//E42
////	alarmMgr_CheckCompressorResponseDataLengthErrorAlarmStatus(gs_currentAlarmMonitor.compressorResponseStatus);//E43
////	alarmMgr_CheckAudioCommunicateErrorAlarmStatus(gs_currentAlarmMonitor.isAudioCommunicationError);	//E45
////	if (gs_alarmTaskTimer >= ALM_TICK_PER_SEC*5)//5s
////	{
////		alarmMgr_CheckBatteryTypeAlarmStatus(gs_currentAlarmMonitor.isBatteryConnected,gs_currentAlarmMonitor.isInvalidBattery);//E48
////	}
////	alarmMgr_CheckTachypneaErrorAlarmStatus(gs_currentAlarmMonitor.breathRate, gs_currentAlarmMonitor.isBreathTrigger);//E49
////}
//
///****************************************************************************************/
///*                                                                                      */
///* Function name: 	alarmMgr_UpdateAllAlarm				 								*/
///*                                                                                      */
///* Details:         update status for all of alarm 										*/
///* Using Global Data : (I) stAlarmList													*/
///*                                                                                      */
///****************************************************************************************/
////void alarmMgr_UpdateAllAlarm(void)
////{
////	ALARM_STAT_T alarmQueueSend;
////	for(int i = eFirsAlarmId; i< eLastAlarmId ; i++)
////	{
////		if (stAlarmList[i].previousStatus != stAlarmList[i].currentStatus)
////		{
////		    alarmQueueSend = stAlarmList[i];
////			if(!devTask_SendToAlarmQueue(alarmQueueSend, 10))
////			{
////				DEBUG_ALARM_MGR("SEND ALARM FAIL ! \n ");
////			}
////			else
////			{
////			    stAlarmList[i].previousStatus = stAlarmList[i].currentStatus;
////			}
////		}
////	}
////}
//
//
///* end of files */
