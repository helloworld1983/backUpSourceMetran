/*
 * DeviceInterface.c
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */
#include <controller/CMVController.h>
#include <controller/ExhController.h>
#include <controller/FlowController.h>
#include <controller/HFOController.h>
#include <controller/HFOServo.h>
#include <controller/NCPAPController.h>
#include <controller/VCVController.h>
#include <deviceTask/DeviceInterface.h>
#include <ipcTask/IpcInterface.h>
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "device/DigitalIO.h"
#include "device/ActivePressureSensor.h"
#include "device/ADCSensor.h"
#include "device/RS485Sensor.h"
#include "device/I2CSensor.h"
#include "device/MotorController.h"

#define DEVICE_QUEUE_WAIT_TIME 0
#define DEV_RT_QUEUE_LENGTH 8
#define DEV_CM_QUEUE_LENGTH 40
#define TEST_COMMUNICATION
static QueueHandle_t s_devTskSettingQueue;
static QueueHandle_t s_devTskCommandQueue;
static SettingA53ToM4 gs_settingA53ToM4;
static RealTimeM4ToA53 gs_realTimeM4ToA53;
static bool gs_isA53AppReady = false;
static bool gs_isA53OSReady = false;
/**
 * Get setting struct sent from A53 application.
 * This struct contains several settings from different modes
 * PCV, VCV, HFO, ... and other required information.
 * @return SettingA53ToM4* gs_settingA53ToM4
 */
SettingA53ToM4* devIf_GetSetting(void)
{
	return &gs_settingA53ToM4;
}
/**
 * Move all motor to desired position specified by desiredMotorPositionstruct
 * @param motorPositionStruct this struct contains desired motor position of each motor
 */
static void devIf_MoveMotorToDesiredStep(desiredMotorPositionStruct motorPositionStruct)
{
	if(motorPositionStruct.desiredAirPosition > -1)
	{
		MotorController_MoveToStepPositionOld(eAirMotor,motorPositionStruct.desiredAirPosition);
	}
	if(motorPositionStruct.desiredExhPosition > -1)
	{
		MotorController_MoveToStepPositionOld(eExhMotor,motorPositionStruct.desiredExhPosition);
	}
	if(motorPositionStruct.desiredHFOPosition > -1)
	{
		MotorController_MoveToStepPositionOld(eHFOMotor,motorPositionStruct.desiredHFOPosition);
	}
	if(motorPositionStruct.desiredO2Position > -1)
	{
		MotorController_MoveToStepPositionOld(eO2Motor,motorPositionStruct.desiredO2Position);
	}
}
/**
 * Send setting message to device task queue
 * @param msg setting message
 * @return True if the sending process is successful, otherwise
 * 			return False
 */
bool devIf_SendSettingMsg(Setting* msg)
{
	if(xQueueSend(s_devTskSettingQueue, msg,DEVICE_QUEUE_WAIT_TIME) == pdFALSE)
	{
		//		PRINTF("\n devif_sendMsg Failure send");
		return false;
	}
	return true;
}
/**
 * Send command message to device task queue
 * @param msg setting message
 * @return True if the sending process is successful, otherwise
 * 			return False
 */
bool devIf_SendCommandMsg(Command* msg)
{
	if(xQueueSend(s_devTskCommandQueue, msg,DEVICE_QUEUE_WAIT_TIME) == pdFALSE)
	{
		//		PRINTF("\n devif_sendMsg Failure send");
		return false;
	}
	return true;
}
/**
 * Check whether A53 Application is ready
 * @return True if A53 Application is ready, otherwise return False
 */
bool devIf_IsA53AppReady()
{
	return gs_isA53AppReady;
}
/**
 * Check whether A53 OS is ready
 * @return True if A53 OS is ready, otherwise return False
 */
bool devIf_IsA53OSReady()
{
	return gs_isA53OSReady;
}
/**
 * Handle queue message (setting and command message) sent by ipc task
 */
void devIf_HandleMsgQueueRecv(void)
{
	static Command s_ipcMsgCommandData;
	static Setting s_ipcMsgSettingData;
	while(xQueueReceive(s_devTskSettingQueue, &s_ipcMsgSettingData, 0) == pdTRUE)
	{
		gs_settingA53ToM4.hfo = s_ipcMsgSettingData.hfo;
		gs_settingA53ToM4.pcv = s_ipcMsgSettingData.pcv;
		gs_settingA53ToM4.vcv = s_ipcMsgSettingData.vcv;
#ifdef TEST_COMMUNICATION
		PRINTF("hfo Freq %d Stroke %d\n",gs_settingA53ToM4.hfo.frequency,gs_settingA53ToM4.hfo.strokeVolume);
#endif
	}
	while(xQueueReceive(s_devTskCommandQueue, &s_ipcMsgCommandData, 0) == pdTRUE)
	{
		switch(s_ipcMsgCommandData.id)
		{
		case eSetImpedanceLineState:
#ifdef TEST_COMMUNICATION
			PRINTF("Set Impedance Line State %d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetState(eImpedanceLine,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetImpedanceLineStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("Set Impedance Line State Direct %d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(eImpedanceLine,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetImpedanceLineDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("Set Impedance Line Desired State %d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eImpedanceLine,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetSafetyValveDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("Set Safety Valve Desired State %d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eSafetyValve,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetSafetyValveStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("Set Safety Valve State Direct %d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(eSafetyValve,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eCMVServoEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eCMVServoEnable\n");
#else
			CMVController_Enable();
#endif
			break;
		case eCMVServoDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eCMVServoDisable\n");
#else
			CMVController_Disable();
#endif
			break;
		case eSetCMVBiasFlow:
			//None Use
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCMVBiasFlow\n");
#endif
			break;
		case eSetCMVInhalationTarget:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCMVInhalationTarget targetPressure:%d riseTime:%d\n",s_ipcMsgCommandData.data.inhTarget.targetPressure,s_ipcMsgCommandData.data.inhTarget.riseTime);
#else
			CMVController_SetInhalationTarget(s_ipcMsgCommandData.data.inhTarget.targetPressure,
					s_ipcMsgCommandData.data.inhTarget.riseTime);
#endif
			break;
		case eSetCMVExhlationTarget:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCMVExhlationTarget targetPress:%d\n",s_ipcMsgCommandData.data.targetPress);
#else
			CMVController_SetExhalationTarget(s_ipcMsgCommandData.data.targetPress);
#endif
			break;
		case eCMVResetIMVStaticVariables:
#ifdef TEST_COMMUNICATION
			PRINTF("eCMVResetIMVStaticVariables\n");
#else
			CMVController_ResetIMVStaticVariables();
#endif
			break;
		case eVCVServoEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVServoEnable\n");
#else
			VCVController_Enable();
#endif
			break;
		case eVCVServoDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVServoDisable\n");
#else
			VCVController_Disable();
#endif
			break;
		case eVCVSetSpecificPhase:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVSetSpecificPhase specificPhaseId:%d\n",(E_SpecificPhaseId)s_ipcMsgCommandData.data.specificPhaseId);
#else
			VCVController_SetSepecificPhase((E_SpecificPhaseId)s_ipcMsgCommandData.data.specificPhaseId);
#endif
			break;
		case eVCVSetBiasFlow:
			//None Use
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVSetBiasFlow\n");
#endif
			break;
		case eVCVSetExhalationTarget:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVSetExhalationTarget targetPress:%d\n",s_ipcMsgCommandData.data.targetPress);
#else
			VCVController_SetExhalationTarget(s_ipcMsgCommandData.data.targetPress);
#endif
			break;
		case eVCVSetInhalationTarget:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVSetInhalationTarget targetPressure:%d riseTime:%d\n",s_ipcMsgCommandData.data.inhTarget.targetPressure,s_ipcMsgCommandData.data.inhTarget.riseTime);
#else
			VCVController_SetInhalationTarget(s_ipcMsgCommandData.data.inhTarget.targetPressure,
					s_ipcMsgCommandData.data.inhTarget.riseTime);
#endif
			break;
		case eVCVResetIMVStaticVariables:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVResetIMVStaticVariables\n");
#else
			VCVController_ResetIMVStaticVariables();
#endif
			break;
		case eVCVSetDesiredFlow:
#ifdef TEST_COMMUNICATION
			PRINTF("eVCVSetDesiredFlow desiredAirFlow:%d desiredO2Flow:%d\n",s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow,s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow);
#else

			VCVController_SetDesiredFlow(s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow,
					s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow);
#endif
			break;
		case eCommunicateProxySensor:
			//None Use
#ifdef TEST_COMMUNICATION
			PRINTF("eCommunicateProxySensor\n");
#endif
			break;
		case eSetPrimaryActivePress:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetPrimaryActivePress primary:%d\n",s_ipcMsgCommandData.data.primary);
#else
			ActivePressureSensor_SetPrimary(s_ipcMsgCommandData.data.primary);
#endif
			break;
		case eSetProxyFlowResetLineStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetProxyFlowResetLineStateDirect state:%d\n",s_ipcMsgCommandData.data.state);

#else
			DigitalIO_SetStateDirect(eProxyFlowResetLine,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetProxyFlowResetLineDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetProxyFlowResetLineDesiredState state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eProxyFlowResetLine,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetExhPressZeroDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetExhPressZeroDesiredState state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eExhPressureZero,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetExhPressZeroStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetExhPressZeroStateDirect state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(eExhPressureZero,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetInhPressZeroDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetInhPressZeroDesiredState state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eInhPressureZero,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetInhPressZeroStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetInhPressZeroStateDirect state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(eInhPressureZero,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetPurgeFlowStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetPurgeFlowStateDirect state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(ePurgeFlow,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetPurgeFlowDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetPurgeFlowDesiredState state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(ePurgeFlow,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetOilPumpStateDirect:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetOilPumpStateDirect state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetStateDirect(eOilPump,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eSetOilPumpDesiredState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetOilPumpDesiredState state:%d\n",s_ipcMsgCommandData.data.state);
#else
			DigitalIO_SetDesiredState(eOilPump,s_ipcMsgCommandData.data.state);
#endif
			break;
		case eAirFlowControllerEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eAirFlowControllerEnable\n");
#else
			FlowController_SetEnable(eAirFlowController,true);
#endif
			break;
		case eAirFlowControllerDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eAirFlowControllerDisable\n");
#else
			FlowController_SetEnable(eAirFlowController,false);
#endif
			break;
		case eO2FlowControllerEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eO2FlowControllerEnable\n");
#else
			FlowController_SetEnable(eO2FlowController,true);
#endif
			break;
		case eO2FlowControllerDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eO2FlowControllerDisable\n");
#else
			FlowController_SetEnable(eO2FlowController,false);
#endif
			break;
		case eExhControllerEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eExhControllerEnable\n");
#else
			ExhController_Enable();
#endif
			break;
		case eExhControllerDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eExhControllerDisable\n");
#else
			ExhController_Disable();
#endif
			break;
		case eHfoRequestDoSVCtr:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoRequestDoSVCtr\n");
#else
			HFOServo_RequestDoSVCtr();
#endif
			break;
		case eHfoRequestDoAmpCtr:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoRequestDoAmpCtr\n");
#else
			HFOServo_RequestDoAmpCtr();
#endif
			break;
		case eStopHfoDoAmpCtr:
#ifdef TEST_COMMUNICATION
			PRINTF("eStopHfoDoAmpCtr\n");
#else
			HFOServo_StopDoAmpCtr();
#endif
			break;
		case eStopHfoDoSVCtr:
#ifdef TEST_COMMUNICATION
			PRINTF("eStopHfoDoSVCtr\n");
#else
			HFOServo_StopDoAmpCtr();
#endif
			break;
		case eHfoSetExhValveLimit:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoSetExhValveLimit exhValveLimit:%d\n",s_ipcMsgCommandData.data.exhValveLimit);
#else
			HFOController_SetExhValveLimit(s_ipcMsgCommandData.data.exhValveLimit);
#endif
			break;
		case eHfoValveRampToPosition:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoValveRampToPosition position:%d\n",s_ipcMsgCommandData.data.position);
#else
			HFOServo_HFOMotorMoveToStepPosition(s_ipcMsgCommandData.data.position);
#endif
			break;
		case eHfoServoEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoServoEnable\n");
#else
			HFOServo_Enable();
#endif
			break;
		case eHfoServoDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoServoDisable\n");
#else
			HFOServo_Disable();
#endif
			break;
		case eHfoControllerEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoControllerEnable\n");
#else
			HFOController_Enable();
#endif
			break;
		case eHfoControllerDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eHfoControllerDisable\n");
#else
			HFOController_Diable();
#endif
			break;
		case eExhValveRampToPosition:
#ifdef TEST_COMMUNICATION
			PRINTF("eExhValveRampToPosition position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eExhMotor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eO2ValveRampToPosition:
#ifdef TEST_COMMUNICATION
			PRINTF("eO2ValveRampToPosition position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eO2Motor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eAirValveRampToPosition:
#ifdef TEST_COMMUNICATION
			PRINTF("eAirValveRampToPosition position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eAirMotor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eResetHFOStaticVariables:
#ifdef TEST_COMMUNICATION
			PRINTF("eResetHFOStaticVariables\n");
#else
			HFOController_ResetHFOStaticVariables();
#endif
			break;
		case eClearHfoServoStaticVariables:
#ifdef TEST_COMMUNICATION
			PRINTF("eClearHfoServoStaticVariables\n");
#else
			HFOServo_ClearStaticVariable();
#endif
			break;
		case eResetHfoServoPIDStatic:
#ifdef TEST_COMMUNICATION
			PRINTF("eResetHfoServoPIDStatic\n");
#else
			HFOServo_ResetPIDStatic();
#endif
			break;
		case eResetHfoServo_20sTimer:
			//None Use
			break;
		case eSetHfoConstFlow:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoConstFlow constFlowFlag:%d\n",s_ipcMsgCommandData.data.constFlowFlag);
#else
			HFOController_SetConstFlow(s_ipcMsgCommandData.data.constFlowFlag);
#endif
			break;
		case eSetHfoControllerSIRequest:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoControllerSIRequest siRequestFlag:%d\n",s_ipcMsgCommandData.data.siRequestFlag);
#else
			HFOController_SetSIRequest(s_ipcMsgCommandData.data.siRequestFlag);
#endif
			break;
		case eSetHfoServoSIRequest:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoServoSIRequest siRequestFlag:%d\n",s_ipcMsgCommandData.data.siRequestFlag);
#else
			HFOServo_SetSIRequest(s_ipcMsgCommandData.data.siRequestFlag);
#endif
			break;
		case eSetHfoControllerSIType:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoControllerSIType siType:%d\n",s_ipcMsgCommandData.data.siType);
#else
			HFOController_SetSIType(s_ipcMsgCommandData.data.siType);
#endif

			break;
		case eSetAirStepPostion:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetAirStepPostion position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eAirMotor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eSetO2StepPostion:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetO2StepPostion position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eO2Motor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eSetExhStepPostion:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetExhStepPostion position:%d\n",s_ipcMsgCommandData.data.position);
#else
			MotorController_MoveToStepPositionOld(eExhMotor,s_ipcMsgCommandData.data.position);
#endif
			break;
		case eSetHfoStepPostion:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoStepPostion position:%d\n",s_ipcMsgCommandData.data.position);
#else
			HFOServo_HFOMotorMoveToStepPosition(s_ipcMsgCommandData.data.position);
#endif
			break;
		case eSetHfoValveFreq:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoValveFreq freq:%d\n",s_ipcMsgCommandData.data.freq);
#else
			MotorController_SetHFOFreq(s_ipcMsgCommandData.data.freq);
#endif
			break;
		case eSetHfoAlarmInactiveTimer:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoAlarmInactiveTimer inactiveTimer:%d\n",s_ipcMsgCommandData.data.inactiveTimer);
#else
			HFOController_SetHFOAlarmInactiveTimer(s_ipcMsgCommandData.data.inactiveTimer);
#endif
			break;
		case eSetCMVAlarmInactiveTimer:
			//None Use
			break;
		case eNCPAPResetIMVStaticVariables:
#ifdef TEST_COMMUNICATION
			PRINTF("eNCPAPResetIMVStaticVariables\n");
#else
			NCPAPController_ResetIMVStaticVariables();
#endif
			break;
		case eNCPAPControllerEnable:
#ifdef TEST_COMMUNICATION
			PRINTF("eNCPAPControllerEnable\n");
#else
			NCPAPController_Enable();
#endif
			break;
		case eNCPAPControllerDisable:
#ifdef TEST_COMMUNICATION
			PRINTF("eNCPAPControllerDisable\n");
#else
			NCPAPController_Disable();
#endif
			break;
		case eSetNCPAPTargetFlow:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetNCPAPTargetFlow targetFlow:%d\n",s_ipcMsgCommandData.data.targetFlow);
#else
			NCPAPController_SetTargetFlow(s_ipcMsgCommandData.data.targetFlow);
#endif
			break;
		case eAirFlowControllerReset:
#ifdef TEST_COMMUNICATION
			PRINTF("eAirFlowControllerReset\n");
#else
			FlowController_Reset(eAirFlowController);

#endif
			break;
		case eO2FlowControllerReset:
#ifdef TEST_COMMUNICATION
			PRINTF("eO2FlowControllerReset\n");
#else
			FlowController_Reset(eO2FlowController);

#endif
			break;
		case eExhControllerReset:
#ifdef TEST_COMMUNICATION
			PRINTF("eExhControllerReset\n");
#else
			ExhController_Reset();
#endif
			break;
		case eHfoFlowControllerReset:
			//None Use
			break;
		case eDisableHfoMotorErrorCheck:
			//None Use
			break;
		case eDisableO2MotorErrorCheck:
			//None Use
			break;
		case eDisableAirMotorErrorCheck:
			//None Use
			break;
		case eDisableExhMotorErrorCheck:
			//None Use
			break;
		case eEnableHfoMotorErrorCheck:
			//None Use
			break;
		case eEnableO2MotorErrorCheck:
			//None Use
			break;
		case eEnableAirMotorErrorCheck:
			//None Use
			break;
		case eEnableExhMotorErrorCheck:
			//None Use
			break;
		case eSetAirPressSwitchFault:
			//None Use
			break;
		case eSetO2PressSwitchFault:
			//None Use
			break;
		case eSetCauseOfAlarm:
			//None Use
			break;
		case eSendOilPumpRequestKey:
			//None Use
			break;
		case eRunHfoServo:
			//None Use
			break;
		case eRunHfoController:
			//None Use
			break;
		case eRunCMVServoController:
			//None Use
			break;
		case eRunVCVServoController:
			//None Use
			break;
		case eRunAirFlowController:
			//None Use
			break;
		case eRunO2FlowController:
			//None Use
			break;
		case eRunNCPAPController:
			//None Use
			break;
		case eRunExhController:
#ifdef TEST_COMMUNICATION
			PRINTF("eRunExhController\n");
#else
			devIf_MoveMotorToDesiredStep(ExhController_Run());
#endif
			break;
		case eRunHfoValveHFOMode:
#ifdef TEST_COMMUNICATION
			PRINTF("eRunHfoValveHFOMode\n");
#else
			MotorController_RunHFOMode();
#endif
			break;
		case eSetDesiredAirFlowController:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetDesiredAirFlowController desiredAirFlow:%d\n",s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow);
#else
			FlowController_SetDesiredFlow(eAirFlowController,s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow);

#endif
			break;
		case eSetDesiredO2FlowController:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetDesiredO2FlowController desiredAirFlow:%d\n",s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow);
#else
			FlowController_SetDesiredFlow(eO2FlowController,s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow);
#endif
			break;
		case eSetDesiredExhController:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetDesiredExhController desiredExhPressure:%d\n",s_ipcMsgCommandData.data.desiredExhPressure);
#else
			ExhController_SetDesired(s_ipcMsgCommandData.data.desiredExhPressure);
#endif
			break;
		case eSetCurrentAction:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCurrentAction action:%d\n",s_ipcMsgCommandData.data.action);
#else
			RS485ProxySensor_SetCurrentAction(s_ipcMsgCommandData.data.action);
#endif
			break;
		case eComputeProxySensorOffset:
#ifdef TEST_COMMUNICATION
			PRINTF("eComputeProxySensorOffset\n");
#else
			RS485ProxySensor_DoGetOffset();
#endif
			break;
		case eSetA11:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA11 %d\n",s_ipcMsgCommandData.data.aValue);
#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA11(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA11(s_ipcMsgCommandData.data.aValue);
#endif
			break;
		case eSetA12:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA12 %d\n",s_ipcMsgCommandData.data.aValue);

#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA12(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA12(s_ipcMsgCommandData.data.aValue);
#endif
			break;
		case eSetA13:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA13 %d\n",s_ipcMsgCommandData.data.aValue);

#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA13(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA13(s_ipcMsgCommandData.data.aValue);
#endif

			break;
		case eSetA14:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA14 %d\n",s_ipcMsgCommandData.data.aValue);

#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA14(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA14(s_ipcMsgCommandData.data.aValue);
#endif
			break;
		case eSetA21:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA21 %d\n",s_ipcMsgCommandData.data.aValue);
#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA21(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA21(s_ipcMsgCommandData.data.aValue);
#endif

			break;
		case eSetA22:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA22 %d\n",s_ipcMsgCommandData.data.aValue);
#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA22(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA22(s_ipcMsgCommandData.data.aValue);
#endif

			break;
		case eSetA23:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA23 %d\n",s_ipcMsgCommandData.data.aValue);
#else

			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA23(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA23(s_ipcMsgCommandData.data.aValue);
#endif
			break;
		case eSetA24:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetA24 %d\n",s_ipcMsgCommandData.data.aValue);
#else
			if(gs_settingA53ToM4.currentMode==ePcvMixedMode ||
					gs_settingA53ToM4.currentMode==ePcvACMode ||
					gs_settingA53ToM4.currentMode==ePcvSpontMode)
				CMVController_SetA24(s_ipcMsgCommandData.data.aValue);
			else
				if(gs_settingA53ToM4.currentMode==eVcvMixedMode ||
						gs_settingA53ToM4.currentMode==eVcvACMode ||
						gs_settingA53ToM4.currentMode==eVcvSpontMode)
					VCVController_SetA24(s_ipcMsgCommandData.data.aValue);
#endif
			break;
		case eSetFiO2ConversionFactor:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetFiO2ConversionFactor factor:%f offset:%f\n",s_ipcMsgCommandData.data.conversion.factor,s_ipcMsgCommandData.data.conversion.offset);
#else
			ADCSensorFiO2_SetConversionFactor(s_ipcMsgCommandData.data.conversion.factor,
					s_ipcMsgCommandData.data.conversion.offset);
#endif
			break;
		case eSetHfoStrokeVol:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoStrokeVol strokeVol:%d\n",s_ipcMsgCommandData.data.strokeVol);
#else
			MotorController_SetHFOStrokeVolume(s_ipcMsgCommandData.data.strokeVol);
#endif

			break;
		case eSetHfoValveOffsetPosition:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoValveOffsetPosition offsetPosition:%d\n",s_ipcMsgCommandData.data.offsetPosition);
#else
			MotorController_SetHFOOffSetPosition(s_ipcMsgCommandData.data.offsetPosition);
#endif
			break;
		case eSetExhValveLiftOff:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetExhValveLiftOff liftOff:%d \n",s_ipcMsgCommandData.data.liftOff);
#else
			MotorController_SetLiftOff( eExhMotor,s_ipcMsgCommandData.data.liftOff);
#endif
			break;
		case eSetAirValveLiftOff:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetAirValveLiftOff liftOff:%d \n",s_ipcMsgCommandData.data.liftOff);
#else
			MotorController_SetLiftOff( eAirMotor,s_ipcMsgCommandData.data.liftOff);
#endif
			break;
		case eSetO2ValveLiftOff:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetO2ValveLiftOff liftOff:%d \n",s_ipcMsgCommandData.data.liftOff);

#else
			MotorController_SetLiftOff( eO2Motor,s_ipcMsgCommandData.data.liftOff);
#endif
			break;
		case eSetHfoValveLiftOff:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetHfoValveLiftOff liftOff:%d \n",s_ipcMsgCommandData.data.liftOff);
#else
			MotorController_SetLiftOff( eHFOMotor,s_ipcMsgCommandData.data.liftOff);
#endif
			break;
		case eSetDesiredSolenoidState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetDesiredSolenoidState id:%d state:%d\n",s_ipcMsgCommandData.data.solenoid.id,s_ipcMsgCommandData.data.solenoid.state);
#else
			DigitalIO_SetState(s_ipcMsgCommandData.data.solenoid.id,
					s_ipcMsgCommandData.data.solenoid.state);
#endif
			break;
		case eSetExhPressZeroOffset:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetExhPressZeroOffset zeroOffset:%d\n",s_ipcMsgCommandData.data.zeroOffset);
#else
			ADCSensorExh_SetZeroOffSet(s_ipcMsgCommandData.data.zeroOffset);
#endif
			break;
		case eSetInhPressZeroOffset:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetInhPressZeroOffset zeroOffset:%d\n",s_ipcMsgCommandData.data.zeroOffset);
#else
			ADCSensorInh_SetZeroOffSet(s_ipcMsgCommandData.data.zeroOffset);
#endif
			break;
		case eSetRemoteAlarmLow:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetRemoteAlarmLow eOffSolenoidValve\n");
#else
			DigitalIO_SetState(eRemoteAlarm,eOffSolenoidValve);
#endif
			break;
		case eSetFiO2Offset:
			//None Use
			break;
		case eSetFiO2PassCalibration:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetFiO2PassCalibration fiO2PassCalibration:%d\n",s_ipcMsgCommandData.data.fiO2PassCalibration);
#else
			ADCSensorFiO2_SetIsCalibrated(s_ipcMsgCommandData.data.fiO2PassCalibration);
#endif
			break;
		case eSetElevationSetting:
			//None Use
			break;
		case eSetBtpsFactor:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetBtpsFactor air:%d ex:%d o2:%d\n",s_ipcMsgCommandData.data.btpsFactor.airFactor,s_ipcMsgCommandData.data.btpsFactor.exhFactor,s_ipcMsgCommandData.data.btpsFactor.o2Factor);
#else
			gs_settingA53ToM4.btpsFactor = s_ipcMsgCommandData.data.btpsFactor;
#endif
			break;
		case eSetRatio:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetRatio air:%d O2:%d\n",s_ipcMsgCommandData.data.ratio.DesiredAirRatio,s_ipcMsgCommandData.data.ratio.DesiredO2Ratio);
#else
			gs_settingA53ToM4.ratio=s_ipcMsgCommandData.data.ratio;
#endif
			break;
		case eSetCurrentMode:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCurrentMode currentMode:%d\n",s_ipcMsgCommandData.data.currentMode);
#else
			gs_settingA53ToM4.currentMode = s_ipcMsgCommandData.data.currentMode;
#endif
			break;
		case eSetCurrentAmp:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetCurrentAmp currentAmp:%d\n",s_ipcMsgCommandData.data.currentAmp);
#else
			gs_settingA53ToM4.currentAmp = s_ipcMsgCommandData.data.currentAmp;
#endif
			break;
		case eSetClock200hours:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetClock200hours %d\n",s_ipcMsgCommandData.data.clock200hours);
#else
			gs_settingA53ToM4.clock200hours = s_ipcMsgCommandData.data.clock200hours;
#endif
			break;
		case eSetAZTestState:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetAZTestState %d\n",s_ipcMsgCommandData.data.azTestState);
#else
			gs_settingA53ToM4.azTestState = s_ipcMsgCommandData.data.azTestState;
#endif
			break;
		case eSetDesiredFlowNCPAP:
#ifdef TEST_COMMUNICATION
			PRINTF("eSetDesiredFlowNCPAP %d %d\n",s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow,s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow);
#else
			gs_settingA53ToM4.desiredAirFlow = s_ipcMsgCommandData.data.desiredFlow.desiredAirFlow;
			gs_settingA53ToM4.desiredO2Flow = s_ipcMsgCommandData.data.desiredFlow.desiredO2Flow;
#endif
			break;
		case eDisableTrigger:
			//None Use
			break;
		case eEnableTrigger:
			//None Use
			break;
		case eProxyVersionGUIEvent:
			//None Use
			break;
		case eLogPostCustomEvent:
			//None Use
			break;
		case eUpdateSettingsHfoServo:
			//None Use
			break;
		case eA53AppReady:
			PRINTF("Receive eA53AppReady\n");
			gs_isA53AppReady = true;
			break;
		case eM4Ready:
			//None Use
			break;
		case A53OSReady:
			PRINTF("Receive eA53OSReady\n");
			gs_isA53OSReady = true;
//			devIf_InitDevice();
			break;
		default:
			break;
		}
	}
	return;
}
/**
 * Initialize all devices controlled by M4 core
 */
void devIf_InitDevice(void)
{
	PRINTF("Start Init Device\n");
	DigitalIO_Init();
//	ADCSensorFiO2_Init();   //Done
//	HFOServo_Init();		//Not do
//	I2CSensor_Init();		//Done
//	MotorController_Init();
	RS485ProxySensor_Init();
	PRINTF("Finish Init Device\n");
}
/**
 * Create device task queues
 */
void devIf_CreateQueue(void)
{
	s_devTskSettingQueue = xQueueCreate(DEV_RT_QUEUE_LENGTH, sizeof(Setting));
	s_devTskCommandQueue = xQueueCreate(DEV_CM_QUEUE_LENGTH, sizeof(Command));
}
/**
 * Set state of different digital output
 */
static void devIf_WriteDOutput()
{
	if(DigitalIO_GetState(eProxyFlowResetLine) != DigitalIO_GetDesiredState(eProxyFlowResetLine))
		DigitalIO_SetStateDirect(eProxyFlowResetLine,DigitalIO_GetDesiredState(eProxyFlowResetLine));

	if(DigitalIO_GetState(eExhPressureZero) != DigitalIO_GetDesiredState(eExhPressureZero))
		DigitalIO_SetStateDirect(eExhPressureZero,DigitalIO_GetDesiredState(eExhPressureZero));

	if(DigitalIO_GetState(eInhPressureZero) != DigitalIO_GetDesiredState(eInhPressureZero))
		DigitalIO_SetStateDirect(eInhPressureZero,DigitalIO_GetDesiredState(eInhPressureZero));

	if(DigitalIO_GetState(eSafetyValve) != DigitalIO_GetDesiredState(eSafetyValve))
		DigitalIO_SetStateDirect(eSafetyValve,DigitalIO_GetDesiredState(eSafetyValve));

	if(DigitalIO_GetState(eImpedanceLine) != DigitalIO_GetDesiredState(eImpedanceLine))
		DigitalIO_SetStateDirect(eImpedanceLine,DigitalIO_GetDesiredState(eImpedanceLine));

	if(DigitalIO_GetState(ePurgeFlow) != DigitalIO_GetDesiredState(ePurgeFlow))
		DigitalIO_SetStateDirect(ePurgeFlow,DigitalIO_GetDesiredState(ePurgeFlow));

	if(DigitalIO_GetState(eRemoteAlarm) != DigitalIO_GetDesiredState(eRemoteAlarm))
		DigitalIO_SetStateDirect(eRemoteAlarm,DigitalIO_GetDesiredState(eRemoteAlarm));

	if(DigitalIO_GetState(eEnableIO) != DigitalIO_GetDesiredState(eEnableIO))
		DigitalIO_SetStateDirect(eEnableIO,DigitalIO_GetDesiredState(eEnableIO));

	if(DigitalIO_GetState(eOilPump) != DigitalIO_GetDesiredState(eOilPump))
		DigitalIO_SetStateDirect(eOilPump,DigitalIO_GetDesiredState(eOilPump));
}
/**
 * This function runs every 2ms. Handle communicating process with all
 * sensors to read current values
 */
void devIf_UpdateCurrentSensorValue()
{
	static uint8_t s_timer10ms=0;
	I2CSensorAirFlow_GetCurrentReading();
	I2CSensorO2Flow_GetCurrentReading();
	ADCSensorExh_GetCurrentReading();
	ADCSensorInh_GetCurrentReading();
	ActivePressureSensor_Run();
	ADCSensorFiO2_GetCurrentVolt();
	if(s_timer10ms<(10/DEVICE_TASK_DELAY_TIME))
	{
		RS485ProxySensor_CommunicateProxySensor();
		devIf_WriteDOutput();
		if(ADCSensorFiO2_IsO2SensorConnect() == eTrue)
			ADCSensorFiO2_AddCounts();
		s_timer10ms = 0;
	}
	else
	{
		s_timer10ms++;
	}
	ProxyStatus proxyStatus = RS485ProxySensor_GetLastStatus();
	if((proxyStatus ==eCalibrated)||(proxyStatus==eResetAndRecovered))
	{
		RS485ProxySensor_GetCurrentStatus();
	}
	ADCSensorFiO2_ReadVoltageValue();
	MotorController_GetCurrentStepPosition(eHFOMotor);
	MotorController_GetCurrentStepPosition(eExhMotor);
	MotorController_GetCurrentStepPosition(eO2Motor);
	MotorController_GetCurrentStepPosition(eAirMotor);

}
/**
 * This function runs every 2ms to handle servo control.
 * @param monitor Struct contains current status of system
 */
void devIf_Run(RealTimeM4ToA53* monitor)
{
	ExhController_UpdateMonitorStruct(monitor);
	if(HFOServo_IsEnable()==True)
	{
		devIf_MoveMotorToDesiredStep(HFOServo_Run(monitor,MotorController_GetLastStepPosition(eHFOMotor)));
	}
	if(HFOController_IsEnable()==True)
	{
		DigitalIO_SetDesiredState(eImpedanceLine,eOnSolenoidValve);
		devIf_MoveMotorToDesiredStep(HFOController_Run(monitor));
	}
	if(CMVController_IsEnable()==True)
	{
		devIf_MoveMotorToDesiredStep(CMVController_Run(monitor));
	}
	if(VCVController_IsEnable()==True)
	{
		devIf_MoveMotorToDesiredStep(VCVController_Run(monitor));
	}
	if(FlowController_IsEnable(eAirFlowController)==True)
	{
		devIf_MoveMotorToDesiredStep(FlowController_Run(eAirFlowController,monitor));
	}
	if(FlowController_IsEnable(eO2FlowController)==True)
	{
		devIf_MoveMotorToDesiredStep(FlowController_Run(eO2FlowController,monitor));
	}
	if(NCPAPController_IsEnable()==True)
	{
		devIf_MoveMotorToDesiredStep(NCPAPController_Run(monitor));
	}

}

/**
 * Update value of monitor struct
 * @return RealTimeM4ToA53* Pointer to monitor struct
 */
RealTimeM4ToA53* devIf_UpdateMonitorStruct(void)
{
#ifdef TEST_COMMUNICATION
	gs_realTimeM4ToA53.activePress.currentReading = 1;
	gs_realTimeM4ToA53.activePress.currentVolt = 2;
	gs_realTimeM4ToA53.activePress.lastReading = 3;
	gs_realTimeM4ToA53.activePress.lastVolt = 4;
	gs_realTimeM4ToA53.airFlow.currentReading = 5;
	gs_realTimeM4ToA53.airFlow.currentVolt = 6;
	gs_realTimeM4ToA53.airFlow.lastReading = 7;
	gs_realTimeM4ToA53.airFlow.lastVolt = 8;
	gs_realTimeM4ToA53.airPressSwitch.state = eBlinking;
	gs_realTimeM4ToA53.airPressSwitch.stateDirect = eBlinking;
	gs_realTimeM4ToA53.airValve.liftOff=9;//MotorController_GetLiftOff(eAirMotor);
	gs_realTimeM4ToA53.airValve.maxStep=10;//MotorController_GetMaxStep(eAirMotor);
	gs_realTimeM4ToA53.airValve.stepPosition=11;//(MotorController_GetLastStepPosition(eAirMotor)/MAX_STEP_POSITION_CURRENT)*MAX_STEP_POSITION_OLD;
	gs_realTimeM4ToA53.exhPress.currentReading = 12;
	gs_realTimeM4ToA53.exhPress.currentVolt = 13;
	gs_realTimeM4ToA53.exhPress.lastReading = 14;
	gs_realTimeM4ToA53.exhPress.lastVolt = 15;
	gs_realTimeM4ToA53.exhPressureZero.state = 16;
	gs_realTimeM4ToA53.exhPressureZero.desiredState = 17;
	gs_realTimeM4ToA53.exhValve.liftOff = 18;//MotorController_GetLiftOff(eExhMotor);
	gs_realTimeM4ToA53.exhValve.maxStep = 19;//MotorController_GetMaxStep(eExhMotor);
	gs_realTimeM4ToA53.exhValve.stepPosition = 20;//(MotorController_GetLastStepPosition(eExhMotor)/MAX_STEP_POSITION_CURRENT)*MAX_STEP_POSITION_OLD;
	gs_realTimeM4ToA53.fio2.avgFlow = 21;
	gs_realTimeM4ToA53.fio2.avgPress = 22;
	gs_realTimeM4ToA53.fio2.currentReading = 23;
	gs_realTimeM4ToA53.fio2.currentVolt = 24;
	gs_realTimeM4ToA53.fio2.isO2SensorConnected = false;
	gs_realTimeM4ToA53.fio2.isPresent = true;
	gs_realTimeM4ToA53.fio2.lastVolt = 25;
	gs_realTimeM4ToA53.hardware.checkFlowSensor = false;
	gs_realTimeM4ToA53.hfoDoorSwitch.state =eBlinking;
	gs_realTimeM4ToA53.hfoDoorSwitch.stateDirect =eBlinking;
	gs_realTimeM4ToA53.hfoPistonSwitch.state = eBlinking;
	gs_realTimeM4ToA53.hfoPistonSwitch.stateDirect = eBlinking;
	gs_realTimeM4ToA53.hfoValve.liftOff = 26;
	gs_realTimeM4ToA53.hfoValve.maxStep = 27;
	gs_realTimeM4ToA53.hfoValve.stepPosition = 28;
	gs_realTimeM4ToA53.impedanceLine.state = eBlinking;
	gs_realTimeM4ToA53.impedanceLine.desiredState = eBlinking;
	gs_realTimeM4ToA53.inhPress.currentReading = 29;
	gs_realTimeM4ToA53.inhPress.currentVolt = 30;
	gs_realTimeM4ToA53.inhPress.lastReading = 31;
	gs_realTimeM4ToA53.inhPress.lastVolt = 32;
	gs_realTimeM4ToA53.inhPressureZero.state = eBlinking;
	gs_realTimeM4ToA53.inhPressureZero.desiredState = eBlinking;
	gs_realTimeM4ToA53.manualBreathConnection.state = eBlinking;
	gs_realTimeM4ToA53.manualBreathConnection.stateDirect = eBlinking;
	gs_realTimeM4ToA53.manualBreathInput.state = eBlinking;
	gs_realTimeM4ToA53.manualBreathInput.stateDirect = eBlinking;
	gs_realTimeM4ToA53.o2Flow.currentReading = 33;
	gs_realTimeM4ToA53.o2Flow.currentVolt = 34;
	gs_realTimeM4ToA53.o2Flow.lastReading = 35;
	gs_realTimeM4ToA53.o2Flow.lastVolt = 36;
	gs_realTimeM4ToA53.o2PressSwitch.state = eBlinking;
	gs_realTimeM4ToA53.o2PressSwitch.stateDirect = eBlinking;
	gs_realTimeM4ToA53.o2Valve.liftOff = 37;
	gs_realTimeM4ToA53.o2Valve.maxStep =38;
	gs_realTimeM4ToA53.o2Valve.stepPosition = 39;
	gs_realTimeM4ToA53.oilPump.desiredState = eBlinking;
	gs_realTimeM4ToA53.oilPump.state = eBlinking;
	gs_realTimeM4ToA53.proxy.currentReading = 40;
	gs_realTimeM4ToA53.proxy.isNeedVersion = false;
	gs_realTimeM4ToA53.proxy.isOKToShutDown =false;
	gs_realTimeM4ToA53.proxy.lastStatus = eCalibrated;
	gs_realTimeM4ToA53.proxyFlowResetLine.desiredState = eBlinking;
	gs_realTimeM4ToA53.proxyFlowResetLine.state =eBlinking;
	gs_realTimeM4ToA53.purgeFlow.desiredState = eBlinking;
	gs_realTimeM4ToA53.purgeFlow.state =eBlinking;
	gs_realTimeM4ToA53.safetyValve.desiredState = eOpen;
	gs_realTimeM4ToA53.safetyValve.state = eBlinking;
	gs_realTimeM4ToA53.vcvInhVolume = 41;
	gs_realTimeM4ToA53.IsEnableAirFlowController=false;
	gs_realTimeM4ToA53.IsEnableO2FlowController=false;
	gs_realTimeM4ToA53.IsEnableCMVServoController = false;
	gs_realTimeM4ToA53.IsEnableVCVServoController = false;
	gs_realTimeM4ToA53.IsEnableHfoController = false;
	gs_realTimeM4ToA53.IsEnableHfoServo = false;
	gs_realTimeM4ToA53.IsEnableNCPAPController = false;
	gs_realTimeM4ToA53.IsEnableExhController = false;
	return &gs_realTimeM4ToA53;
#else
	gs_realTimeM4ToA53.activePress.currentReading = ActivePressureSensor_GetLastReading();
	gs_realTimeM4ToA53.activePress.currentVolt = ActivePressureSensor_GetLastVoltage();
	gs_realTimeM4ToA53.activePress.lastReading = ActivePressureSensor_GetLastReading();
	gs_realTimeM4ToA53.activePress.lastVolt = ActivePressureSensor_GetLastVoltage();
	gs_realTimeM4ToA53.airFlow.currentReading = I2CSensorAirFlow_GetLastReading();
	gs_realTimeM4ToA53.airFlow.currentVolt = 0;
	gs_realTimeM4ToA53.airFlow.lastReading = I2CSensorAirFlow_GetLastReading();
	gs_realTimeM4ToA53.airFlow.lastVolt = 0;
	gs_realTimeM4ToA53.airPressSwitch.state = DigitalIO_GetSwitchState(eAirSw);
	gs_realTimeM4ToA53.airPressSwitch.stateDirect = DigitalIO_GetSwitchState(eAirSw);
	gs_realTimeM4ToA53.airValve.liftOff=MotorController_GetLiftOff(eAirMotor);
	gs_realTimeM4ToA53.airValve.maxStep=MotorController_GetMaxStep(eAirMotor);
	gs_realTimeM4ToA53.airValve.stepPosition=(MotorController_GetLastStepPosition(eAirMotor)/MAX_STEP_POSITION_CURRENT)*MAX_STEP_POSITION_OLD;
	gs_realTimeM4ToA53.exhPress.currentReading = ADCSensorExh_GetLastReading();
	gs_realTimeM4ToA53.exhPress.currentVolt = ADCSensorExh_GetLastVolt();
	gs_realTimeM4ToA53.exhPress.lastReading = ADCSensorExh_GetLastReading();
	gs_realTimeM4ToA53.exhPress.lastVolt = ADCSensorExh_GetLastVolt();
	gs_realTimeM4ToA53.exhPressureZero.state = DigitalIO_GetState(eExhPressureZero);
	gs_realTimeM4ToA53.exhPressureZero.desiredState = DigitalIO_GetDesiredState(eExhPressureZero);
	gs_realTimeM4ToA53.exhValve.liftOff = MotorController_GetLiftOff(eExhMotor);
	gs_realTimeM4ToA53.exhValve.maxStep = MotorController_GetMaxStep(eExhMotor);
	gs_realTimeM4ToA53.exhValve.stepPosition = (MotorController_GetLastStepPosition(eExhMotor)/MAX_STEP_POSITION_CURRENT)*MAX_STEP_POSITION_OLD;
	gs_realTimeM4ToA53.fio2.avgFlow = ADCSensorFiO2_AvgFlow();
	gs_realTimeM4ToA53.fio2.avgPress = ADCSensorFiO2_AvgPress();
	gs_realTimeM4ToA53.fio2.currentReading = ADCSensorFiO2_GetCurrentReading();
	gs_realTimeM4ToA53.fio2.currentVolt = ADCSensorFiO2_GetLastVolt();
	gs_realTimeM4ToA53.fio2.isO2SensorConnected = ADCSensorFiO2_IsO2SensorConnect();
	gs_realTimeM4ToA53.fio2.isPresent = ADCSensorFiO2_GetIsPresent();
	gs_realTimeM4ToA53.fio2.lastVolt = ADCSensorFiO2_GetLastVolt();
	gs_realTimeM4ToA53.hardware.checkFlowSensor = I2CSensorO2Flow_CheckSensor() || I2CSensorAirFlow_CheckSensor();
	gs_realTimeM4ToA53.hfoDoorSwitch.state =DigitalIO_GetSwitchState(ePistonSW);
	gs_realTimeM4ToA53.hfoDoorSwitch.stateDirect =DigitalIO_GetSwitchState(ePistonSW);
	gs_realTimeM4ToA53.hfoPistonSwitch.state = DigitalIO_GetSwitchState(ePistonSW);
	gs_realTimeM4ToA53.hfoPistonSwitch.stateDirect = DigitalIO_GetSwitchState(ePistonSW);
	gs_realTimeM4ToA53.hfoValve.liftOff = MotorController_GetLiftOff(eHFOMotor);
	gs_realTimeM4ToA53.hfoValve.maxStep = MotorController_GetMaxStep(eHFOMotor);
	gs_realTimeM4ToA53.hfoValve.stepPosition = HFOServo_HFOMotorGetCurrentStepPosition();
	gs_realTimeM4ToA53.impedanceLine.state = DigitalIO_GetState(eImpedanceLine);
	gs_realTimeM4ToA53.impedanceLine.desiredState = DigitalIO_GetDesiredState(eImpedanceLine);
	gs_realTimeM4ToA53.inhPress.currentReading = ADCSensorInh_GetLastReading();
	gs_realTimeM4ToA53.inhPress.currentVolt = ADCSensorInh_GetLastVolt();
	gs_realTimeM4ToA53.inhPress.lastReading = ADCSensorInh_GetLastReading();
	gs_realTimeM4ToA53.inhPress.lastVolt = ADCSensorInh_GetLastVolt();
	gs_realTimeM4ToA53.inhPressureZero.state = DigitalIO_GetState(eInhPressureZero);
	gs_realTimeM4ToA53.inhPressureZero.desiredState = DigitalIO_GetDesiredState(eInhPressureZero);
	gs_realTimeM4ToA53.manualBreathConnection.state = eOn;
	gs_realTimeM4ToA53.manualBreathConnection.stateDirect = eOn;
	gs_realTimeM4ToA53.manualBreathInput.state = DigitalIO_GetState(eManualBreathInput);
	gs_realTimeM4ToA53.manualBreathInput.stateDirect = DigitalIO_GetDesiredState(eManualBreathInput);
	gs_realTimeM4ToA53.o2Flow.currentReading = I2CSensorO2Flow_GetLastReading();
	gs_realTimeM4ToA53.o2Flow.currentVolt = 0;
	gs_realTimeM4ToA53.o2Flow.lastReading = I2CSensorO2Flow_GetLastReading();
	gs_realTimeM4ToA53.o2Flow.lastVolt = 0;
	gs_realTimeM4ToA53.o2PressSwitch.state = DigitalIO_GetSwitchState(eO2Sw);
	gs_realTimeM4ToA53.o2PressSwitch.stateDirect = DigitalIO_GetSwitchState(eO2Sw);
	gs_realTimeM4ToA53.o2Valve.liftOff = MotorController_GetLiftOff(eO2Motor);
	gs_realTimeM4ToA53.o2Valve.maxStep = MotorController_GetMaxStep(eO2Motor);
	gs_realTimeM4ToA53.o2Valve.stepPosition = (MotorController_GetLastStepPosition(eO2Motor)/MAX_STEP_POSITION_CURRENT)*MAX_STEP_POSITION_OLD;
	gs_realTimeM4ToA53.oilPump.desiredState = DigitalIO_GetDesiredState(eOilPump);
	gs_realTimeM4ToA53.oilPump.state = DigitalIO_GetState(eOilPump);
	gs_realTimeM4ToA53.proxy.currentReading = RS485ProxySensor_GetLastReading();
	gs_realTimeM4ToA53.proxy.isNeedVersion = RS485ProxySensor_IsNeedToGetVersionStr();
	gs_realTimeM4ToA53.proxy.isOKToShutDown = RS485ProxySensor_OkToShutdown();
	gs_realTimeM4ToA53.proxy.lastStatus = RS485ProxySensor_GetLastStatus();
	gs_realTimeM4ToA53.proxyFlowResetLine.desiredState = DigitalIO_GetDesiredState(eProxyFlowResetLine);
	gs_realTimeM4ToA53.proxyFlowResetLine.state = DigitalIO_GetState(eProxyFlowResetLine);
	gs_realTimeM4ToA53.purgeFlow.desiredState = DigitalIO_GetDesiredState(ePurgeFlow);
	gs_realTimeM4ToA53.purgeFlow.state = DigitalIO_GetState(ePurgeFlow);
	gs_realTimeM4ToA53.safetyValve.desiredState = DigitalIO_GetDesiredState(eSafetyValve);
	gs_realTimeM4ToA53.safetyValve.state = DigitalIO_GetState(eSafetyValve);
	gs_realTimeM4ToA53.vcvInhVolume = VCVController_GetInhVol();
	gs_realTimeM4ToA53.IsEnableAirFlowController=FlowController_IsEnable(eAirFlowController);
	gs_realTimeM4ToA53.IsEnableO2FlowController=FlowController_IsEnable(eO2FlowController);
	gs_realTimeM4ToA53.IsEnableCMVServoController = CMVController_IsEnable();
	gs_realTimeM4ToA53.IsEnableVCVServoController = VCVController_IsEnable();
	gs_realTimeM4ToA53.IsEnableHfoController = HFOController_IsEnable();
	gs_realTimeM4ToA53.IsEnableHfoServo = HFOServo_IsEnable();
	gs_realTimeM4ToA53.IsEnableNCPAPController = NCPAPController_IsEnable();
	gs_realTimeM4ToA53.IsEnableExhController = ExhController_IsEnable();
	return &gs_realTimeM4ToA53;
#endif
}
/**
 * Send M4 ready command to A53 via IPC
 */
void devIf_sendReadyMessage()
{
	PRINTF("A53 hasn't been ready yet!!!\n");
	Command sendMsg;
	sendMsg.id = eM4Ready;
	ipcIf_sendCommandMsg(&sendMsg);
}
