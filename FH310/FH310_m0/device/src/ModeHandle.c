/********************************************************************/
/*                                                                  */
/* File Name     : ModeHandle.c		                               	*/
/*                                                                  */
/* General       : handle mode operation				            */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/05      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove assert												*/
/* #0002	   2017/09/04	   Quyen Ngo	modify					*/
/*		-add flowCtrl_Reset() and compressor_ResetWhenAlarm() when  */
/*		gotoAlarmNoOxyCylinder										*/
/* #0003	   2017/09/07	   Quyen Ngo	modify					*/
/*		-change function name of opsw								*/
/*		-change call to new latchvalve request function				*/
/*		-remove goto stop mode										*/
/* #0004	   2017/09/15	   Quyen Ngo	modify					*/
/*		-change for fix with new mode manager design				*/
/* #0005	   2017/09/20	   Quyen Ngo	modify					*/
/*		-correct the mode transition case when recover, close or 	*/
/*		open latch valve based on mode transition					*/
/* #0006	   2017/10/06	   Quyen Ngo	modify					*/
/*		-Clear error status of flow controller when go to standby 	*/
/*		mode for fix Bug 1959 for avoid the case that flow controller*/
/*		and compressor become normal in alarm mode					 */
/*			   2017/10/12	   Quyen Ngo	modify					*/
/* #0007	   Call flowCtrl_SetAllowRecordLogNoAlarmFlag(true)		*/
/*			  (Bug 1958) for allow record log when mode change      */
/********************************************************************/


#include "ModeHandle.h"
#include "PsaControl.h"
#include "OperationswHandle.h"
#include "DualCoreCommon.h"
#include "PowerMgr.h"
#include "AlarmLed.h"
#include "LatchValve.h"
#include "BatteryHandling.h"
#include "FlowController.h"
#include "Compressor.h"

#ifdef DEBUG
#define DEBUG_MODE_EN
#endif


#ifdef DEBUG_MODE_EN
#define DEBUG_MODE(...) printf(__VA_ARGS__)
#else
#define DEBUG_MODE(...)
#endif

static E_ModeSetting s_currentMode = eStandyMode;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: goto_standby_mode
//
//    Processing: Processing device operation when go to stand by mode fromMode?
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_GotoStandby(E_ModeSetting fromMode)
{
	flowCtrl_SetAllowRecordLogNoAlarmFlag(true);
	flowCtrl_ClearAlarmStatus();
	compressor_ClearAlarmStatus();
	latchValve_RequestClose();
	operationsw_DisablePush2S();
	aled_StopHigh();
	aled_StopMedium();
	switch (fromMode) {
	case ePatientMode:
	case eServiceMode:
		psa_EnterEndSequence();
		break;
	default:
		break;
	}
	s_currentMode = eStandyMode;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: goto_patient_mode
//
//    Processing: Processing device operation when go to patient mode fromMode?
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_GotoPatient(E_ModeSetting fromMode)
{
	DEBUGOUT("\nmode_GotoPatient *** = %d", fromMode);
	switch (fromMode) {
	case eStandyMode:
	case ePatientMode://recover case
	case ePowerErrorMode:
		DEBUGOUT("\nmode_GotoPatient");
		flowCtrl_SetAllowRecordLogNoAlarmFlag(true);
		operationsw_disable();
		psa_RequestEnterStartSequence();
		break;
	default:
		break;
	}
	s_currentMode = ePatientMode;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: goto_service_mode
//
//    Processing: Processing device operation when go to service mode fromMode?
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_GotoService(E_ModeSetting fromMode)
{
	s_currentMode = eServiceMode;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: goto_alarm_mode
//
//    Processing: Processing device operation when go to alarm mode fromMode?
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_gotoAlarm(E_ModeSetting fromMode)
{
	DEBUG_MODE("fromMode: %d \n",fromMode);
	s_currentMode = eAlarmMode;
	switch (fromMode) {
	case ePatientMode:
		psa_EnterEndSequence();
		flowCtrl_Reset();
		compressor_ResetWhenAlarm();
		break;
	case eServiceMode:
		psa_EnterEndSequence();
		flowCtrl_Reset();
		compressor_ResetWhenAlarm();
		break;
	case eAlarmMode:
	case eAlarmNoOxyCylinderMode:
		//recover case
		latchValve_RequestClose();
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: mode_gotoAlarmExtFlow
//
//    Processing: Alarm mode change flow path to external
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_gotoAlarmExtFlow(E_ModeSetting fromMode)
{
	DEBUG_MODE("fromMode: %d \n",fromMode);
	s_currentMode = eAlarmExternalFlowPathMode;
	switch (fromMode) {
	case eAlarmMode:
		latchValve_RequestOpen();
		break;
	case eAlarmExternalFlowPathMode:
		//recover case
		latchValve_RequestOpen();
		break;
	default:
		DEBUGOUT("Invalid mode \n");
		break;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: mode_gotoAlarmNoOxyCylinder
//
//    Processing: Goto alarm mode without oxygen cylinder attach
//
//    Input Parameters: E_ModeSetting fromMode
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void mode_gotoAlarmNoOxyCylinder(E_ModeSetting fromMode)
{
	DEBUG_MODE("fromMode: %d \n",fromMode);
	s_currentMode = eAlarmNoOxyCylinderMode;
	switch (fromMode) {
	case ePatientMode:
		psa_EnterEndSequence();
		flowCtrl_Reset();
		compressor_ResetWhenAlarm();
		break;
	case eServiceMode:
		psa_EnterEndSequence();
		flowCtrl_Reset();
		compressor_ResetWhenAlarm();
		break;
	case eAlarmExternalFlowPathMode:
		//recover case
		latchValve_RequestClose();
		break;
	case eAlarmMode:
		latchValve_RequestClose();
		break;
		//End add
	case eAlarmNoOxyCylinderMode:
		//recover case
		latchValve_RequestClose();
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: mode_GetCurrentMode()
//
//    Processing: Get current operation mode
//
//    Input Parameters:
//
//    Output Parameters: None
//
//    Return Values:  E_ModeSetting
//
//    Using golobal data: (I) s_currentMode
//						  (O)
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
E_ModeSetting  mode_GetCurrentMode(void)
{
	return s_currentMode;
}
/* end of file */



