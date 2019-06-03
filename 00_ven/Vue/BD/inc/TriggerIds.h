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
#ifdef HFO_SYSTEM
    eFirstHFOTrigger,

    eHighMAP_2AlarmTrigger = eFirstHFOTrigger,
    eExtremeLowMapAlarmTrigger,
    eExtremeLowMapAutoResetTrigger,
#endif
    eHighMAP_2AutoResetTrigger,

    eAutoSIOnTimeTrigger,
    eAutoSIOffTimeTrigger,
    eSI10sTimeTrigger,
    eResetSITimeTrigger,


    eFirstIMVTrigger,
    eTotalGasLossTrigger = eFirstIMVTrigger,
    eOcclusionTrigger,
    eResetGasLossTrigger,

    eHardwareErrorAlarmTriger,
    eHardwareErrorAutoresetTrigger,
    eAlarmResetOpReqTrigger,
    eModeChangeTrigger,
    eUpdateStatusTimeTrigger,
    eLastHFOTrigger = eUpdateStatusTimeTrigger,

    eAPRVUpdateBRTrigger,
    eHip2AlarmTrigger,          // HIP-2 must appear before HIP-1
    eHipAlarmTrigger,
    eExtremeHighPEEPTrigger,

    eExtremeHigh_PHigh_Trigger,

    eApneaAlarmTimeTrigger,
    eItLongAlarmTimeTrigger,
    eDisconnectionAlarmTrigger,
    eReconnectionTrigger,
    eApneaAutoResetTrigger,
    eMandPressInhTimeTrigger,
    eManualInhOpReqTrigger,
    ePtInhPressTrigger,
    ePtInhFlowTrigger,

    eBpTimeTrigger,
    eBpBackupTimeTrigger,
    eNIVBpTimeTrigger,

    eAvBpTimeTrigger,

    eImmediateTrigger,
    ePtExhFlowSpontTrigger,

    eVcvInhTimeTrigger,

    ePlateauTimeTrigger,

    eAPRVLowPressTimeTrigger,
    eAPRVHighPressTimeTrigger,

    eAPRV_PtInhPressTrigger_InPLow,
    eAPRV_PtInhPressTrigger_InPHigh,

    eHPHighAlarmTrigger,

//    eAPRVInhPressTrigger,
//    eAPRVInhFlowTrigger,
    eProxyDisconnectTrigger,
    eProxyConnectTrigger,

    ePtExhFlowSuppTrigger,


    eLastIMVTrigger = ePtExhFlowSuppTrigger,

    eNumTriggers, // these 2 must remain at the end of list
    eNoTrigger = eNumTriggers
};

#endif // TRIGGERIDS_H
