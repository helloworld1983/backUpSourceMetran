/******************************************************************************/
//$COMMON.H$
//   File Name:  TriggerIds.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file does not contain a class declaration.  It contains an
//      enumerated list of all of the Trigger Identifiers.
/******************************************************************************/
#ifndef TRIGGERIDS_H
#define TRIGGERIDS_H

/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/

// The E_TriggerId enumeration is a PRIORITIZED list to triggers.
// If multiple triggers fire during the same BD Cycle, the highest priority 
// trigger is processed first, which may cause other triggers that have fired 
// to be disabled and not processed.

enum E_TriggerId
{
    eFirstHFOTrigger,
    eHighMAP_2AlarmTrigger = eFirstHFOTrigger,
    eExtremeLowMapAlarmTrigger,
    eExtremeLowMapAutoResetTrigger,
    eHighMAP_2AutoResetTrigger,

    eAutoSIOnTimeTrigger,
    eAutoSIOffTimeTrigger,
    eSI10sTimeTrigger,
    eResetSITimeTrigger,
    eAlarmResetTrigger,


    eFirstIMVTrigger,
    eTotalGasLossTrigger = eFirstIMVTrigger,
    eOcclusionTrigger,
    eOcclusionAlarmTrigger,
    eResetGasLossTrigger,//10

    eHardwareErrorAlarmTriger,
    eHardwareErrorAutoresetTrigger,
    eAlarmResetOpReqTrigger,
    eModeChangeTrigger,
    eUpdateStatusTimeTrigger,//15
    eLastHFOTrigger = eUpdateStatusTimeTrigger,

    eAPRVUpdateBRTrigger,
    eHip2AlarmTrigger,          // HIP-2 must appear before HIP-1
    eHipAlarmTrigger,
    eExtremeHighPEEPTrigger,

    eExtremeHigh_PHigh_Trigger,//20

    eApneaAlarmTimeTrigger,
    eItLongAlarmTimeTrigger,
    eDisconnectionAlarmTrigger,
    eReconnectionTrigger,
    eApneaAutoResetTrigger,//25
    eMandPressInhTimeTrigger,
    eManualInhOpReqTrigger,
    ePtInhPressTrigger,
    ePtInhFlowTrigger,

    eBpTimeTrigger,//30
    eBpBackupTimeTrigger,
    eNIVBpTimeTrigger,

    eAvBpTimeTrigger,

    eImmediateTrigger,
    ePtExhFlowSpontTrigger,

    eVcvInhTimeTrigger,

    ePlateauTimeTrigger,

    eAPRVLowPressTimeTrigger,
    eAPRVHighPressTimeTrigger,
    eAPRV_PtInhPressTrigger,
    eAPRV_PtInhPressTrigger_InPLow,
    eAPRV_PtInhPressTrigger_InPHigh,

    eHPHighAlarmTrigger,

    eProxyDisconnectTrigger,
    eProxyConnectTrigger,

    ePtExhFlowSuppTrigger,

    eLastIMVTrigger = ePtExhFlowSuppTrigger,

    eBlowerSpinDownTimeTrigger,
    eOcclInhTrigger,
    eOcclExhTrigger,
    eOcclAutoResetTrigger,
    eHip1AlarmTrigger,
    ePtExhPressTrigger,
    ePtExhFlowTrigger,
    eExhHoldOpReqTrigger,
    eExhHoldTimeTrigger,
    eLowBaselineAlarmTrigger,
    ePtInhTrigger,
    ePtExhTrigger,

    eExhHoldTrigger,

    eNumTriggers, // these 2 must remain at the end of list
    eNoTrigger = eNumTriggers
};

#endif // TRIGGERIDS_H
