/********************************************************************/
/*                                                                  */
/* File Name    : DeviceController.c                              	*/
/*                                                                  */
/* General       : This file contain the function for init and handle
 * device operation, handle setting sent from GUI and update the
 * monitor information
 * 				   						                  			*/
/*                 	 						                    	*/
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            2016/09/27        Quyen Ngo(MV)  	new file         	*/
/* 0001		  08/28/2017		Quyen Ngo(MV)	remove assign 		*/
/*			  		for some items on alarm monitor struct not use. */
/* #0002	  2017/09/01	    Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/*																	*/
/* #0003       2016/09/05      Linh Nguyen	  	    modify          */
/*		-remove isResettingFlowControler signal for alarm task		*/
/*																	*/
/* #0004	  2017/09/07		Quyen Ngo   		modify			*/
/*		-remove assign to isAC100Vplugin, isBatteryLow			    */
/*		-Change the name of some global static var to gs_ prefix	*/
/*		(Bug 1812)													*/
/*		-Add set get function for access gs_currentFlowrate and use */
/*		them instead of access directly	(Bug 1814)					*/
/*		-change call to new latchve request function				*/
/*																	*/
/* #0005	  2017/09/12		Quyen Ngo			modify			*/
/*		-remove assign to isOperationSwPush							*/
/* #0006	  2017/11/02		Quyen Ngo			modify			*/
/*		Add product tank, canual, temperature AD value in Psa 		*/
/*		monitorpacket (Bug 2012)									*/
/* #0007	  2017/11/02		Quyen Ngo			modify			*/
/*		Add latchValve_ProcessPendingIpcMsg() to handle the case	*/
/*		IPC queue full when send LatchValveCloseFinish msg			*/
/* #0008	  2017/11/03		Viet Le			    modify			*/
/*      Edit latch valve true meaning of it (bug 2013) 				*/
/* #0009	  2017/11/10		Quyen Ngo			modify			*/
/*      Add sequence to update value of O2cylinder status before 	*/
/*		before send first monitor info to GUI (Bug 2023)			*/
/********************************************************************/

/* include system files */
#include "string.h"

/* include user files */
#include <adc.h>
#include "DeviceController.h"
#include "OperationswHandle.h"
#include "PwmController.h"
#include "PressureSensor.h"
#include "PsaControl.h"
#include "ValveControl.h"
#include "TemperatureSensor.h"
#include "ModeHandle.h"
#include "FlowController.h"
#include "AlarmLed.h"
#include "BatteryHandling.h"
#include "TaskCommon.h"
#include "DeviceInterface.h"
#include "Compressor.h"
#include "PsaSelectMgr.h"
#include "backlight.h"
#include "coolingfan.h"
#include "PowerMgr.h"
#include "OxyCylinder.h"
#include "LatchValve.h"
#include "IlluminanceSensor.h"
#include "FilterCleaningModule.h"
#include "IpcMsg.h"

/* definitions of structures */

#ifdef DEBUG
//#define Q_DEBUG
//#define DEBUG_DEVCTRL_EN
#endif

#ifdef DEBUG_DEVCTRL_EN
#define DEBUG_DEVCTRL(...) printf(__VA_ARGS__)
#else
#define DEBUG_DEVCTRL(...)
#endif

/* constants, macro definition */
#define UPDATE_MONITOR_VALUE_TIME_CNT 								100/DEVICE_TASK_DELAY_TIME //50ms*4
#define SEND_MONITOR_INTERVAL_TIME_CNT 								700/DEVICE_TASK_DELAY_TIME //50ms*20
#define MAX_FLOWRATE_SET_SCALE100									10*100 //Max 10LPM

static float gs_productankPress = 0.0;
static uint16_t gs_productankAdc = 0;
static float gs_compPosPress = 0.0;
static float gs_cannulaPress = 0.0;
static uint16_t gs_canulaAdc = 0;
static float gs_compNegPress = 0.0;
static float gs_extFlowPathPress = 0.0;
static float gs_temperature = 0.0;
static uint16_t gs_temperatureAdc = 0;
static float gs_compTemperature = 0.0;
static uint16_t gs_o2Concentrator = 0;
static uint16_t gs_flow = 0;
static uint16_t gs_compessorRpm = 0;
static uint16_t gs_fanDuty = 0;
static E_ValStatus gs_val1Status = VAL_OFF;
static E_ValStatus gs_val2Status = VAL_OFF;
static E_ValStatus gs_val3Status = VAL_OFF;
static E_ValStatus gs_val4Status = VAL_OFF;
static E_ValStatus gs_val5Status = VAL_OFF;
static uint16_t gs_psaCycleTime = 0;
static E_PsaSelectName gs_psaParamSelect = NORM_TEMP_NORM_02;
static E_PsaPortSelect gs_psaPortSelect = DEFAULT_SELECT;
static E_O2CylinStatus gs_o2CylindDetectStatus = O2_CYLIN_NOTATTACH;
static float gs_illuminanceVoltVal = 0.0;
static bool gs_coolingFanErrState = false;
static bool gs_filterPhotoErrStatus = false;
static E_PsaSequence gs_devPsaSeq = PSA_STANDBY;
static uint16_t gs_currentFlowRate = 0;
static ALARM_MONITOR_t gs_stAlarmMonitor;

/* Body of functions */

/****************************************************************************************/
/*                                                                                      */
/* Function name：  devCtrl_SetCurrentFlow													*/
/*                                                                                      */
/* Details： Update current flowrate value setting which using by another function   		*/
/*                                                                                      */
/* Arguments : uint16_t flowRate				 					  					*/
/*			   unit:	flowLPM*100														*/
/*			   range: 	0~10*100														*/
/*                                                                                      */
/* Macros:	   MAX_FLOWRATE_SET_SCALE100												*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 	(O) uint16_t gs_currentFlowRate			                      	*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
static void devCtrl_SetCurrentFlow(uint16_t flowRate)
{
	if(flowRate <= MAX_FLOWRATE_SET_SCALE100)
	{
		gs_currentFlowRate = flowRate;
	}
	else
	{
		DEBUGOUT("Invalid flowrate setting \n");
	}
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name：  devCtrl_GetCurrentFlow													*/
/*                                                                                      */
/* Details： Get current flow rate setting										   		*/
/*                                                                                      */
/* Arguments : 																			*/
/* Macros:	   MAX_FLOWRATE_SET_SCALE100												*/
/* ReturnValue : uint16_t 				 					  							*/
/*			   unit:	flowLPM*100														*/
/*			   range: 	0~10*100														*/
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : 	(I) uint16_t gs_currentFlowRate				                   	*/
/*			   unit:	flowLPM*100														*/
/*			   range: 	0~10*100														*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
uint16_t devCtrl_GetCurrentFlow(void)
{
	return gs_currentFlowRate;
}
/****************************************************************************************/
/*                                                                                      */
/* Funciton name：  Init all specific device module				                        */
/*                                                                                      */
/* Details：   Call init function of specific devices module               				*/
/*                                                                                      */
/* Arguments : None						 					  							*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 																	*/
/* local valiables   : 			                                                    	*/
/*【Note】                                                                             											    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Init(void)
{
	psa_CreateTimer();
	aled_CreateTimer();
	latchValve_CreateTimer();
	//Port INIT
	gpio_Init();

	//Specific device init and config
	battery_Init();
	power_Init();
	valve_Init();
	operationsw_config();
	aled_Init();
	latchValve_Init();
	filter_Init();
	//FC
	flowCtrl_InitCommunication();
	compressor_InitCommunication();
	//PWM for control cooling fan and back light
	pwm_Init();
	coolingFan_Init();
	backlight_Init();
	pwm_Start();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  Decode msg_data receive from GUI task and handle it					*/
/*                                                                                      */
/* Details：  This function process the ipcMsgData. Depend on ipcMsgData id and value,     */
/* the handle_setting function call right function for control device operation			*/
/*  depend on the message send from M4 core.        								    */
/*                                                                                      */
/* Arguments : IPC_MSG_DATA_t ipcMsgData		 					  					*/
/*                                                                                      */
/* Macros:	   VAL_CLOSE																*/
/*			   TURN_MOTOR_OFF															*/
/*			   TURN_LED_ON																*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 									                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void devCtrl_HandleGuiMsg(IPC_MSG_DATA_t ipcMsgData)
{
	switch (ipcMsgData.data_id) {
	case eUintCumulativeHourId:
		filter_UpdateCumulativeHour(ipcMsgData.data_value);
		break;
	case eFlowControllerTest1Id:
		flowCtrl_SendTestCmd(eFlowControllerTest1Id,ipcMsgData.data_value);
		break;
	case eFlowControllerTest2Id:
		flowCtrl_SendTestCmd(eFlowControllerTest2Id,ipcMsgData.data_value);
		break;
	case eO2ConcentrationTestChangeId:
		flowCtrl_SendTestO2Cmd(ipcMsgData.data_value);
		break;
	case eFlowRateTestChangeId:
		flowCtrl_SendTestFlowCmd(ipcMsgData.data_value);
		break;
	case eWatchDogM0ResetChangeId:
		DEBUGOUT("eWatchDogM0ResetChangeId \n");
		ipc_ResetM0WatdogTest();
		break;
	case ePlayLedHighAlarmChangeId:
		aled_PlayHigh();
		break;
	case ePlayLedMediumAlarmChangeId:
		aled_PlayMedium();
		break;
	case eStopLedAlarmChangeID:
		DEBUGOUT("\neStopLedAlarmChangeID");
		aled_StopHigh();
		aled_StopMedium();
		break;
	case eAlarmMGSId:
		devInterface_AlarmProcessGuiRequest(ipcMsgData.stAlarmStatPacket);
		break;
	case eCoolingFanFunctiontestChangeId:
		coolingFan_Setduty(ipcMsgData.data_value);
#ifdef Q_DEBUG
		DEBUGOUT("eCoolingFanFunctiontestChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eMotorRPMFunctionTestChangeId:
		compressor_ReqestSetRpm(ipcMsgData.data_value);
#ifdef Q_DEBUG
		DEBUGOUT("eMotorRPMFunctionTestChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnValve1ChangeId:
		if (ipcMsgData.data_value == VAL_CLOSE) {
			valve_closeVal1();
		} else {
			valve_openVal1();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnValve1ChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnValve2ChangeId:
		if (ipcMsgData.data_value == VAL_CLOSE) {
			valve_closeVal2();
		} else {
			valve_openVal2();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnValve2ChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnValve3ChangeId:
		if (ipcMsgData.data_value == VAL_CLOSE) {
			valve_closeVal3();
		} else {
			valve_openVal3();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnValve3ChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnValve4ChangeId:
		if (ipcMsgData.data_value == VAL_CLOSE) {
			valve_closeVal4();
		} else {
			valve_openVal4();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnValve4ChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnValve5ChangeId:
		if (ipcMsgData.data_value == VAL_CLOSE) {
			valve_closeVal5();
		} else {
			valve_openVal5();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnValve5ChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eDeviceBackupSystemChangeId:
		if (ipcMsgData.data_value == eLatchValveClose) {
			latchValve_RequestClose();
		} else if(ipcMsgData.data_value == eLatchValveOpen) {
			latchValve_RequestOpen();
		}
		else {
			latchValve_RequestOpenAndClose();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eDeviceBackupSystemChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnMotorCompressorChangeId:
		if(ipcMsgData.data_value == TURN_MOTOR_OFF)
		{
			compressor_Stop();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnMotorCompressorChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnMotorCoolingFANChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eTurnMotorCoolingFANChangeId = %d\n", ipcMsgData.data_value);
#endif
		if(ipcMsgData.data_value == TURN_MOTOR_OFF)
		{
			coolingFan_Setduty(0);
		}
		break;
	case eTurnMotorFilterCleaningChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eTurnMotorFilterCleaningChangeId = %d\n", ipcMsgData.data_value);
#endif
		if(ipcMsgData.data_value == TURN_MOTOR_OFF)
		{
			filter_Off();
		}
		else
		{
			filter_On();
		}
		break;
	case eTurnAlarmRedLEDChangeId:
		if (ipcMsgData.data_value == TURN_LED_ON)
		{
			aled_RedOn();
		}
		else
		{
			aled_RedOff();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnAlarmRedLEDChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnAlarmYellowLEDChangeId:
		if (ipcMsgData.data_value == TURN_LED_ON)
		{
			aled_YellowOn();
		}
		else
		{
			aled_YellowOff();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnAlarmYellowLEDChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eTurnAlarmPowerLEDChangeId:
		if (ipcMsgData.data_value == TURN_LED_ON)
		{
			aled_PowerOn();
		}
		else
		{
			aled_PowerOff();
		}
#ifdef Q_DEBUG
		DEBUGOUT("eTurnAlarmPowerLEDChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case ePSAPressureChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("ePSAPressureChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid1OnOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid1OnOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid1OffOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid1OffOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid2OnOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid2OnOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid2OffOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid2OffOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid3OnOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid3OnOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid3OffOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid3OffOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid4OnOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid4OnOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid4OffOffSetTimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid4OffOffSetTimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid5OnOffSetTimeAChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid5OnOffSetTimeAChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSolenoid5OnOffSetTimeBChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSolenoid5OnOffSetTimeBChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSol5TimeAChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSol5TimeAChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eSol5TimeBChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eSol5TimeBChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eCoolingFanChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eCoolingFanChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eMotorCompressorChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eMotorCompressorChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eOxygenFlowChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eOxygenFlowChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eMaxPSATimeChangeId:
		psa_ChangeParamSetting(ipcMsgData);
#ifdef Q_DEBUG
		DEBUGOUT("eMaxPSATimeChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eMaxTemperatureChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eMaxTemperatureChangeId = %d\n", ipcMsgData.data_value);
#endif
		psaSelect_ChangeHighTempThreshold(ipcMsgData.data_value);
		break;
	case eMinTemperatureChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eMinTemperatureChangeId = %d\n", ipcMsgData.data_value);
#endif
		psaSelect_ChangeLowTempThreshold(ipcMsgData.data_value);
		break;
	case eLimitOxygenConcentratorChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eLimitOxygenConcentratorChangeId = %d\n", ipcMsgData.data_value);
#endif
		psaSelect_ChangeO2Threshold(ipcMsgData.data_value);
		break;
	case eInitPacketPSAParaChangeId:
		psa_ActivePsaParameterAtStart(ipcMsgData.stPsaParamPacket);
		devCtrl_SetCurrentFlow(ipcMsgData.stPsaParamPacket.flowRateSetting);
#ifdef Q_DEBUG
		DEBUGOUT("psaPressure = %d\n", ipcMsgData.stPsaParamPacket.psaPressure);
		DEBUGOUT("solenoid1OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid1OnOffsetTime);
		DEBUGOUT("solenoid1OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid1OffOffsetTime);
		DEBUGOUT("solenoid2OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid2OnOffsetTime);
		DEBUGOUT("solenoid2OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid2OffOffsetTime);
		DEBUGOUT("solenoid3OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid3OnOffsetTime);
		DEBUGOUT("solenoid3OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid3OffOffsetTime);
		DEBUGOUT("solenoid4OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid4OnOffsetTime);
		DEBUGOUT("solenoid4OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid4OffOffsetTime);
		DEBUGOUT("solenoid5OnOffsetTimeA = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OnOffsetTimeA);
		DEBUGOUT("solenoid5OnOffsetTimeB = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OnOffsetTimeB);
		DEBUGOUT("solenoid5OpenTimeA = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OpenTimeA);
		DEBUGOUT("solenoid5OpenTimeB = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OpenTimeB);
		DEBUGOUT("coolingFan = %d\n", ipcMsgData.stPsaParamPacket.coolingFan);
		DEBUGOUT("compressor = %d\n", ipcMsgData.stPsaParamPacket.compressor);
		DEBUGOUT("FCoutput = %d\n", ipcMsgData.stPsaParamPacket.FCoutput);
		DEBUGOUT("MaxPsa = %d\n", ipcMsgData.stPsaParamPacket.MaxPsa);
#endif
		break;
	case ePacketPSAParaChangeId:
		if(ipcMsgData.stPsaParamPacket.flowRateSetting != devCtrl_GetCurrentFlow())
		{
			psaSelect_StartCheckO2Condition();
			devCtrl_SetCurrentFlow(ipcMsgData.stPsaParamPacket.flowRateSetting);
		}
		psa_ActiveNewPsaParameter(ipcMsgData.stPsaParamPacket);
#ifdef Q_DEBUG
		DEBUGOUT("psaPressure = %d\n", ipcMsgData.stPsaParamPacket.psaPressure);
		DEBUGOUT("solenoid1OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid1OnOffsetTime);
		DEBUGOUT("solenoid1OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid1OffOffsetTime);
		DEBUGOUT("solenoid2OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid2OnOffsetTime);
		DEBUGOUT("solenoid2OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid2OffOffsetTime);
		DEBUGOUT("solenoid3OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid3OnOffsetTime);
		DEBUGOUT("solenoid3OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid3OffOffsetTime);
		DEBUGOUT("solenoid4OnOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid4OnOffsetTime);
		DEBUGOUT("solenoid4OffOffsetTime = %d\n", ipcMsgData.stPsaParamPacket.solenoid4OffOffsetTime);
		DEBUGOUT("solenoid5OnOffsetTimeA = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OnOffsetTimeA);
		DEBUGOUT("solenoid5OnOffsetTimeB = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OnOffsetTimeB);
		DEBUGOUT("solenoid5OpenTimeA = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OpenTimeA);
		DEBUGOUT("solenoid5OpenTimeB = %d\n", ipcMsgData.stPsaParamPacket.solenoid5OpenTimeB);
		DEBUGOUT("coolingFan = %d\n", ipcMsgData.stPsaParamPacket.coolingFan);
		DEBUGOUT("compressor = %d\n", ipcMsgData.stPsaParamPacket.compressor);
		DEBUGOUT("FCoutput = %d\n", ipcMsgData.stPsaParamPacket.FCoutput);
		DEBUGOUT("MaxPsa = %d\n", ipcMsgData.stPsaParamPacket.MaxPsa);
#endif
		break;
	case eEnterStandbyModeChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eEnterStandbyModeChangeId = %d\n", ipcMsgData.data_value);
#endif
		mode_GotoStandby( (E_ModeSetting) ipcMsgData.data_value); //cast data_value to E_ModeSetting
		break;
	case eEnterPatientModeChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eEnterPatientModeChangeId = %d\n", ipcMsgData.data_value);
#endif
		mode_GotoPatient( (E_ModeSetting) ipcMsgData.data_value); //cast data_value to E_ModeSetting
		break;
	case eEnterServiceModeChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eEnterServiceModeChangeId = %d\n", ipcMsgData.data_value);
#endif
		mode_GotoService( (E_ModeSetting) ipcMsgData.data_value); //cast data_value to E_ModeSetting
		break;
	case eEnterAlarmModeChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eEnterAlarmModeChangeId = %d\n", ipcMsgData.data_value);
#endif
		mode_gotoAlarm( (E_ModeSetting) ipcMsgData.data_value); //cast data_value to E_ModeSetting
		break;
	case eEnterAlarmExternalFlowPathModeChangeId:
		mode_gotoAlarmExtFlow((E_ModeSetting) ipcMsgData.data_value);
		break;
	case eEnterAlarmNotO2CylinderModeChangeId:
		mode_gotoAlarmNoOxyCylinder((E_ModeSetting) ipcMsgData.data_value);
		break;
	case eTurnBackLightChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eTurnBackLightChangeId = %d\n", ipcMsgData.data_value);
#endif
		break;
	case eBacklightChangeId:
#ifdef Q_DEBUG
		DEBUGOUT("eBacklightChangeId = %d\n", ipcMsgData.data_value);
#endif
		backlight_SetStep(ipcMsgData.data_value);
		break;
	default:
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  Decode msg_data receive from GUI task and handle it					*/
/*                                                                                      */
/* Details：  Countinuous checking device status and handle operation						*/
/* Continuous checking operaion switch status											*/
/*																						*/
/*                                                                                      */
/* Arguments : None	 					  												*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 	None							                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 	None			                                               	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_Handle(void) {
	latchValve_ProcessPendingIpcMsg();
	power_Handle();
	battery_Handle();
	flowCtrl_Handle();
	operationsw_Handle();
	compressor_Handle();
	psaSelect_Handling(gs_temperature,gs_o2Concentrator);
	// Psa control handle
	psa_Handle(gs_productankPress);
	//TODO Pass step for back light
	backlight_Handle();
	filter_Handle();
	oxyCylinder_Handle(gs_extFlowPathPress);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_UpdateMonitorInfo											*/
/*                                                                                      */
/* Details：  Update monitor data				        								    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* Macors :		UPDATE_MONITOR_VALUE_TIME_CNT											*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :																	*/
/* 					 (O) float gs_productankPress										*/
/*					 (O) float gs_compPosPress 											*/
/*					 (O) float gs_cannulaPress 											*/
/*					 (O) float gs_compNegPress 											*/
/*					 (O) float gs_extFlowPathPress 										*/
/*					 (O) float gs_temperature											*/
/*					 (O) float gs_compTemperature										*/
/*					 (O) uint16_t gs_o2Concentrator										*/
/*					 (O) uint16_t gs_flow												*/
/* 					 (O) uint16_t gs_compessorRpm										*/
/*					 (O) uint16_t gs_fanDuty											*/
/*					 (O) ValStatus gs_val1Status										*/
/*    				 (O) ValStatus gs_val2Status										*/
/*					 (O) ValStatus gs_val3Status										*/
/*					 (O) ValStatus gs_val4Status										*/
/*					 (O) ValStatus gs_val5Status 										*/
/*					 (O) uint16_t gs_psaCycleTime 										*/
/*					 (O) PsaSelectName gs_psaParamSelect								*/
/*					 (O) E_PsaPortSelect s_psaPortSelect								*/
/*					 (O) E_O2CylinStatus s_o2CylindDetectStatus							*/
/*					 (O) float s_illuminanceVoltVal 									*/
/*					 (O) bool s_coolingFanErrState										*/
/*					 (O) bool s_filterPhotoErrStatus 									*/
/*					 (O) E_PsaSequence s_devPsaSeq										*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											  	    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateMonitorInfo(void)
{
	static int8_t s_updateTimeCnt = UPDATE_MONITOR_VALUE_TIME_CNT;
	ADC_SENSOR_t stAdcSensor;
	s_updateTimeCnt++;
	stAdcSensor = pressure_ReadProductTank();
	gs_productankPress = stAdcSensor.sensorVal;
	gs_productankAdc = stAdcSensor.AdVal;
	if (s_updateTimeCnt >= UPDATE_MONITOR_VALUE_TIME_CNT)
	{
		s_updateTimeCnt = 0;
		gs_compPosPress = pressure_ReadCompressorPos();
		stAdcSensor = pressure_ReadCannula();
		gs_cannulaPress = stAdcSensor.sensorVal;
		gs_canulaAdc = stAdcSensor.AdVal;
		gs_compNegPress = pressure_ReadCompressorNeg();
		gs_extFlowPathPress = pressure_ReadExtFlowPath();
		stAdcSensor = temperature_ReadValueOfInternal();
		gs_temperature = stAdcSensor.sensorVal;
		gs_temperatureAdc = stAdcSensor.AdVal;
		gs_compTemperature = temperature_ReadValueOfCompressor();
		gs_compessorRpm = compressor_getRpm();
		gs_o2Concentrator = flowCtrl_GetO2Val();
		gs_flow = flowCtrl_GetFlowVal();
		gs_fanDuty = coolingFan_getDuty();
		gs_val1Status = valve_getStatus(VAL1);
		gs_val2Status = valve_getStatus(VAL2);
		gs_val3Status = valve_getStatus(VAL3);
		gs_val4Status = valve_getStatus(VAL4);
		gs_val5Status = valve_getStatus(VAL5);
		gs_psaCycleTime = psa_GetCycleTime();
		gs_psaParamSelect = psaSelect_GetConditionType();
		gs_psaPortSelect = psaSelect_CheckPort();
		gs_o2CylindDetectStatus = oxyCylinder_GetStatus();
		gs_illuminanceVoltVal = illuminance_ReadVoltValue();
		gs_coolingFanErrState = coolingFan_GerErr();
		gs_filterPhotoErrStatus = filter_getErrPinState();
		gs_devPsaSeq = psa_GetPsaSeq();
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_UpdateAlarmMonitorStruct									*/
/*                                                                                      */
/* Details： Update alarm monitor struct value			        					    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* Macors :		UPDATE_MONITOR_VALUE_TIME_CNT											*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :																	*/
/* 					 (O) ALARM_MONITOR_t gs_stAlarmMonitor								*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   : 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_UpdateAlarmMonitorStruct(void)
{
	static uint8_t updateTimeCnt = 0;
	updateTimeCnt++;
	if(updateTimeCnt >= UPDATE_MONITOR_VALUE_TIME_CNT)
	{
		updateTimeCnt = 0;
		if(xSemaphoreTake(g_alarmMonitorMutex, 10) == pdTRUE)//Wait 10 ticks
		{

			gs_stAlarmMonitor.operationMode = mode_GetCurrentMode();
			gs_stAlarmMonitor.unitInternalTemperature = gs_temperature;
			gs_stAlarmMonitor.productTankPressure = gs_productankPress;
			gs_stAlarmMonitor.compressorStatus = compressor_getErrStatus();
			gs_stAlarmMonitor.compressorSequence = compressor_GetCurentSeq();
			gs_stAlarmMonitor.coolingFanNotRotationState = gs_coolingFanErrState;
			gs_stAlarmMonitor.O2Concentration = gs_o2Concentrator;
			gs_stAlarmMonitor.flowRateSetting = devCtrl_GetCurrentFlow();
			gs_stAlarmMonitor.cannulaPressure = gs_cannulaPress;
			gs_stAlarmMonitor.flowRateSensor = gs_flow;
			gs_stAlarmMonitor.isFilterErr = filter_getErrStatus();
			gs_stAlarmMonitor.compressorOutletTemperature = gs_compTemperature;
			gs_stAlarmMonitor.flowControllerStatus = flowCtrl_getErrStatus();
			gs_stAlarmMonitor.externalFlowPathPressure = gs_extFlowPathPress;
			gs_stAlarmMonitor.psaSequence = gs_devPsaSeq;
			gs_stAlarmMonitor.psaMode = psa_GetMode();
			xSemaphoreGive(g_alarmMonitorMutex);
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_SendFirstMonitorInfoToGui									*/
/*                                                                                      */
/* Details：  Send first monitor data to GUI after handshake		        			    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* Macors :		UPDATE_MONITOR_VALUE_TIME_CNT											*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :																	*/
/* 					 (I) float gs_productankPress										*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_compPosPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_cannulaPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~100														*/
/*					 (I) float gs_compNegPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_extFlowPathPress 										*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_temperature											*/
/*					  -unit: degC														*/
/*					  -range: -50~280													*/
/*					 (I) float gs_compTemperature										*/
/*					  -unit: degC														*/
/*					  -range: -30~150													*/
/*					 (I) uint16_t gs_o2Concentrator										*/
/*					  -unit: %*100														*/
/*					  -range: 0~10000													*/
/*					 (I) uint16_t gs_flow												*/
/*					  -unit: LPM*100													*/
/*					  -range: 0~1000													*/
/* 					 (I) uint16_t gs_compessorRpm										*/
/*					  -unit: %															*/
/*					  -range: 0~100														*/
/*					 (I) uint16_t gs_fanDuty											*/
/*					  -unit: %															*/
/*					  -range: 0~100														*/
/*					 (I) ValStatus gs_val1Status										*/
/*    				 (I) ValStatus gs_val2Status										*/
/*					 (I) ValStatus gs_val3Status										*/
/*					 (I) ValStatus gs_val4Status										*/
/*					 (I) ValStatus gs_val5Status 										*/
/*					 (I) uint16_t gs_psaCycleTime 										*/
/*					  -unit: ms															*/
/*					  -range: 0~60000													*/
/*					 (I) PsaSelectName gs_psaParamSelect								*/
/*					 (I) E_PsaPortSelect s_psaPortSelect								*/
/*					 (I) E_O2CylinStatus s_o2CylindDetectStatus							*/
/*					 (I) float s_illuminanceVoltVal 									*/
/*					 (I) bool s_coolingFanErrState										*/
/*					 (I) bool s_filterPhotoErrStatus 									*/
/*					 (I) E_PsaSequence s_devPsaSeq										*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											  	    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_SendFirstMonitorInfoToGui(void)
{
	oxyCylinder_Handle(gs_extFlowPathPress);
	IPC_MSG_DATA_t data;
	data.data_id = 	eSendFirstMonitorInfoToGuiId;
	data.data_value = 0;
	memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
	memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
	memset(&data.version, 0, sizeof(data.version));
	data.stPsaMonitorPacket.productankPress = (int16_t)(gs_productankPress *100);
	data.stPsaMonitorPacket.prouctankAdc = gs_productankAdc;
	data.stPsaMonitorPacket.compPosPress = (int16_t)(gs_compPosPress * 100);
	data.stPsaMonitorPacket.cannulaPress = (int16_t)(gs_cannulaPress * 100);
	data.stPsaMonitorPacket.canulaAdc = gs_canulaAdc;
	data.stPsaMonitorPacket.compNegPress = (int16_t)(gs_compNegPress * 100);
	data.stPsaMonitorPacket.extFlowPathPress = (int16_t)(gs_extFlowPathPress * 100);
	data.stPsaMonitorPacket.compTemperature = (int16_t)(gs_compTemperature * 100);
	data.stPsaMonitorPacket.temperature = (int16_t)(gs_temperature * 100);
	data.stPsaMonitorPacket.temperatureAdc = gs_temperatureAdc;
	data.stPsaMonitorPacket.flowMeasure = gs_flow;
	data.stPsaMonitorPacket.compessorRpm = gs_compessorRpm;
	data.stPsaMonitorPacket.fanDuty = gs_fanDuty;
	data.stPsaMonitorPacket.coolingFanErrState = gs_coolingFanErrState;
	data.stPsaMonitorPacket.o2Concentrator = gs_o2Concentrator;
	data.stPsaMonitorPacket.val1Status = gs_val1Status;
	data.stPsaMonitorPacket.val2Status = gs_val2Status;
	data.stPsaMonitorPacket.val3Status = gs_val3Status;
	data.stPsaMonitorPacket.val4Status = gs_val4Status;
	data.stPsaMonitorPacket.val5Status = gs_val5Status;
	data.stPsaMonitorPacket.psaCycleTime = gs_psaCycleTime;
	data.stPsaMonitorPacket.psaParamSelect = gs_psaParamSelect;
	data.stPsaMonitorPacket.psaPortSelect = gs_psaPortSelect;
	data.stPsaMonitorPacket.illuminanceVoltVal = (int16_t)(gs_illuminanceVoltVal*100);
	data.stPsaMonitorPacket.oxyCylinderStatus =  oxyCylinder_GetStatus();
	data.stPsaMonitorPacket.filterPhotoErrStatus = gs_filterPhotoErrStatus;
	data.stPsaMonitorPacket.psaCurSeq = gs_devPsaSeq;

	if (ipc_PushMsg(IPCEX_ID_GUI, data) == QUEUE_INSERT) {
		DEBUG_DEVCTRL("productankPress: %f\n",gs_productankPress);
		DEBUG_DEVCTRL("compPosPress: %f\n",gs_compPosPress);
		DEBUG_DEVCTRL("cannulaPress: %f\n",gs_cannulaPress);
		DEBUG_DEVCTRL("compNegPress: %f\n",gs_compNegPress);
		DEBUG_DEVCTRL("extFlowPathPress: %f\n",gs_extFlowPathPress);
		DEBUG_DEVCTRL("compTemperature: %f\n",gs_compTemperature);
		DEBUG_DEVCTRL("temperature: %f\f",gs_temperature);
		DEBUG_DEVCTRL("flowMeasure: %d\n",gs_flow);
		DEBUG_DEVCTRL("compessorRpm: %d\n",gs_compessorRpm);
		DEBUG_DEVCTRL("fanDuty: %d\n",gs_fanDuty);
		DEBUG_DEVCTRL("o2Concentrator: %d\n",gs_o2Concentrator);
		DEBUG_DEVCTRL("val1Status: %d\n",gs_val1Status);
		DEBUG_DEVCTRL("val2Status: %d\n",gs_val2Status);
		DEBUG_DEVCTRL("val3Status: %d\n",gs_val3Status);
		DEBUG_DEVCTRL("val4Status: %d\n",gs_val4Status);
		DEBUG_DEVCTRL("val5Status: %d\n",gs_val5Status);
		DEBUG_DEVCTRL("psaCycleTime: %d\n",gs_psaCycleTime);
		DEBUG_DEVCTRL("psaParamSelect: %d\n",gs_psaParamSelect);
		DEBUG_DEVCTRL("psaPortSelect: %d\n",gs_psaPortSelect);
		DEBUG_DEVCTRL("illuvoltval: %d\n",data.stPsaMonitorPacket.illuminanceVoltVal);
		DEBUG_DEVCTRL("cylind: %d\n",data.stPsaMonitorPacket.oxyCylinderStatus);
		DEBUG_DEVCTRL("filter err: %d\n",data.stPsaMonitorPacket.filterPhotoErrStatus);
		DEBUG_DEVCTRL("psa seq: %d\n",data.stPsaMonitorPacket.psaCurSeq );
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_SendMonitorInfoToGui										*/
/*                                                                                      */
/* Details：  Send monitor data to GUI every 700ms					        			    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* Macors :		SEND_MONITOR_INTERVAL_TIME_CNT											*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :																	*/
/* 					 (I) float gs_productankPress										*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_compPosPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_cannulaPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~100														*/
/*					 (I) float gs_compNegPress 											*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_extFlowPathPress 										*/
/*					  -unit: kpa														*/
/*					  -range: 0~222.218													*/
/*					 (I) float gs_temperature											*/
/*					  -unit: degC														*/
/*					  -range: -50~280													*/
/*					 (I) float gs_compTemperature										*/
/*					  -unit: degC														*/
/*					  -range: -30~150													*/
/*					 (I) uint16_t gs_o2Concentrator										*/
/*					  -unit: %*100														*/
/*					  -range: 0~10000													*/
/*					 (I) uint16_t gs_flow												*/
/*					  -unit: LPM*100													*/
/*					  -range: 0~1000													*/
/* 					 (I) uint16_t gs_compessorRpm										*/
/*					  -unit: %															*/
/*					  -range: 0~100														*/
/*					 (I) uint16_t gs_fanDuty											*/
/*					  -unit: %															*/
/*					  -range: 0~100														*/
/*					 (I) ValStatus gs_val1Status										*/
/*    				 (I) ValStatus gs_val2Status										*/
/*					 (I) ValStatus gs_val3Status										*/
/*					 (I) ValStatus gs_val4Status										*/
/*					 (I) ValStatus gs_val5Status 										*/
/*					 (I) uint16_t gs_psaCycleTime 										*/
/*					  -unit: ms															*/
/*					  -range: 0~60000													*/
/*					 (I) PsaSelectName gs_psaParamSelect								*/
/*					 (I) E_PsaPortSelect s_psaPortSelect								*/
/*					 (I) E_O2CylinStatus s_o2CylindDetectStatus							*/
/*					 (I) float s_illuminanceVoltVal 									*/
/*					 (I) bool s_coolingFanErrState										*/
/*					 (I) bool s_filterPhotoErrStatus 									*/
/*					 (I) E_PsaSequence s_devPsaSeq										*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											  	    */
/*                                                                                      */
/****************************************************************************************/
void devCtrl_SendMonitorInfoToGui(void)
{
	static uint8_t timeCnt;
	timeCnt++;
	if (timeCnt >= SEND_MONITOR_INTERVAL_TIME_CNT)
	{
		timeCnt = 0;
		IPC_MSG_DATA_t data;
		data.data_id = ePsaMonitorInfo;
		data.data_value = 0;
		memset(&data.stAlarmStatPacket, 0, sizeof(data.stAlarmStatPacket));
		memset(&data.stPsaParamPacket, 0, sizeof(data.stPsaParamPacket));
		memset(&data.version, 0, sizeof(data.version));
		data.stPsaMonitorPacket.productankPress = (int16_t)(gs_productankPress *100);
		data.stPsaMonitorPacket.prouctankAdc = gs_productankAdc;
		data.stPsaMonitorPacket.compPosPress = (int16_t)(gs_compPosPress * 100);
		data.stPsaMonitorPacket.cannulaPress = (int16_t)(gs_cannulaPress * 100);
		data.stPsaMonitorPacket.canulaAdc = gs_canulaAdc;
		data.stPsaMonitorPacket.compNegPress = (int16_t)(gs_compNegPress * 100);
		data.stPsaMonitorPacket.extFlowPathPress = (int16_t)(gs_extFlowPathPress * 100);
		data.stPsaMonitorPacket.compTemperature = (int16_t)(gs_compTemperature * 100);
		data.stPsaMonitorPacket.temperature = (int16_t)(gs_temperature * 100);
		data.stPsaMonitorPacket.temperatureAdc = gs_temperatureAdc;
		data.stPsaMonitorPacket.flowMeasure = gs_flow;
		data.stPsaMonitorPacket.compessorRpm = gs_compessorRpm;
		data.stPsaMonitorPacket.fanDuty = gs_fanDuty;
		data.stPsaMonitorPacket.coolingFanErrState = gs_coolingFanErrState;
		data.stPsaMonitorPacket.o2Concentrator = gs_o2Concentrator;
		data.stPsaMonitorPacket.val1Status = gs_val1Status;
		data.stPsaMonitorPacket.val2Status = gs_val2Status;
		data.stPsaMonitorPacket.val3Status = gs_val3Status;
		data.stPsaMonitorPacket.val4Status = gs_val4Status;
		data.stPsaMonitorPacket.val5Status = gs_val5Status;
		data.stPsaMonitorPacket.psaCycleTime = gs_psaCycleTime;
		data.stPsaMonitorPacket.psaParamSelect = gs_psaParamSelect;
		data.stPsaMonitorPacket.psaPortSelect = gs_psaPortSelect;
		data.stPsaMonitorPacket.illuminanceVoltVal = (int16_t)(gs_illuminanceVoltVal*100);
		data.stPsaMonitorPacket.oxyCylinderStatus = gs_o2CylindDetectStatus;
		data.stPsaMonitorPacket.filterPhotoErrStatus = gs_filterPhotoErrStatus;
		data.stPsaMonitorPacket.psaCurSeq = gs_devPsaSeq;

		if (ipc_PushMsg(IPCEX_ID_GUI, data) == QUEUE_INSERT) {
			DEBUG_DEVCTRL("productankPress: %f\n",gs_productankPress);
			DEBUG_DEVCTRL("compPosPress: %f\n",gs_compPosPress);
			DEBUG_DEVCTRL("cannulaPress: %f\n",gs_cannulaPress);
			DEBUG_DEVCTRL("compNegPress: %f\n",gs_compNegPress);
			DEBUG_DEVCTRL("extFlowPathPress: %f\n",gs_extFlowPathPress);
			DEBUG_DEVCTRL("compTemperature: %f\n",gs_compTemperature);
			DEBUG_DEVCTRL("temperature: %f\f",gs_temperature);
			DEBUG_DEVCTRL("flowMeasure: %d\n",gs_flow);
			DEBUG_DEVCTRL("compessorRpm: %d\n",gs_compessorRpm);
			DEBUG_DEVCTRL("fanDuty: %d\n",gs_fanDuty);
			DEBUG_DEVCTRL("o2Concentrator: %d\n",gs_o2Concentrator);
			DEBUG_DEVCTRL("val1Status: %d\n",gs_val1Status);
			DEBUG_DEVCTRL("val2Status: %d\n",gs_val2Status);
			DEBUG_DEVCTRL("val3Status: %d\n",gs_val3Status);
			DEBUG_DEVCTRL("val4Status: %d\n",gs_val4Status);
			DEBUG_DEVCTRL("val5Status: %d\n",gs_val5Status);
			DEBUG_DEVCTRL("psaCycleTime: %d\n",gs_psaCycleTime);
			DEBUG_DEVCTRL("psaParamSelect: %d\n",gs_psaParamSelect);
			DEBUG_DEVCTRL("psaPortSelect: %d\n",gs_psaPortSelect);
			DEBUG_DEVCTRL("illuvoltval: %d\n",data.stPsaMonitorPacket.illuminanceVoltVal);
			DEBUG_DEVCTRL("cylind: %d\n",data.stPsaMonitorPacket.oxyCylinderStatus);
			DEBUG_DEVCTRL("filter err: %d\n",data.stPsaMonitorPacket.filterPhotoErrStatus);
			DEBUG_DEVCTRL("psa seq: %d\n",data.stPsaMonitorPacket.psaCurSeq );
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_GetAlarmMonitorStruct										*/
/*                                                                                      */
/* Details：  Get alarm monitor struct								        			    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* Macors :		SEND_MONITOR_INTERVAL_TIME_CNT											*/
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :																	*/
/* 					 (I) ALARM_MONITOR_t gs_stAlarmMonitor								*/
/*					 (O) SemaphoreHandle_t g_alarmMonitorMutex							*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											  	    */
/*                                                                                      */
/****************************************************************************************/
ALARM_MONITOR_t devCtrl_GetAlarmMonitorStruct(void)
{
	ALARM_MONITOR_t stAlarmMonitor;
	if(xSemaphoreTake(g_alarmMonitorMutex, 10) == pdTRUE) //Wait 10 ticks
	{
		stAlarmMonitor = gs_stAlarmMonitor;
		xSemaphoreGive(g_alarmMonitorMutex);
	}
	return stAlarmMonitor;
}

/* end of files */
