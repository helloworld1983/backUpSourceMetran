/******************************************************************************/
//$COMMON.H$
//   File Name:  CommonTypes.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      common ventilator specific types used throughout the system. 
//
/******************************************************************************/
#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include "Standard.h"

// Operator set Ventilation Type can be Volume, Pressure or Non-Invasive.
enum E_VentilationType
{
    eVolumeVentilation,
    ePressureVentilation,
    eNonInvasiveVentilation,
    eNoVentType
};

// Ventilation Mode includes a list of both operator set modes and alarm
// modes and a start-up mode.  
enum E_VentilationMode
{
    eFirstHFOMode = 0, eHFOMode = eFirstHFOMode, eStartUpMode = eFirstHFOMode,
    eFirstAlarmMode, eApneaMode = eFirstAlarmMode, eHFOHighMAP_2Mode = eFirstAlarmMode,
    eHFOExtremeLowMAPMode, eOcclusionMode = eHFOExtremeLowMAPMode, eLastHFOMode = eHFOExtremeLowMAPMode,

    eFirstCommonMode,

    eHardwareErrorMode = eFirstCommonMode,
    eNoGasMode,

    eLastCommonMode = eNoGasMode,

    eFirstSIMVMode, eAssistControlMode = eFirstSIMVMode, eHighPressMode = eFirstSIMVMode,
    eDisconnectionMode, eSpontaneousMode = eDisconnectionMode,
    eLastAlarmMode = eDisconnectionMode,

    eMixedMode ,//7//Should be removed
    ePcvMixedMode,
    eVcvMixedMode,
    eVaMixedMode,
    eAPRVMode,//PCV

    ePcvACMode,
    eVcvACMode,
    eVaACMode,

    ePcvSpontMode,
    eVcvSpontMode,
    eNivMode,//18
    eNasalCPAPMode,
    eApneaVAMode,
    eApneaBackupMode,//21

    eLastSIMVMode = eApneaBackupMode,

    eStandbyMode,//22
    eMEMode,
    eSelfTestMode,
    eNumberOfModes  // must be last entry
};

// Patient Type setting
enum E_PatientType
{
    eAdult, ePediatric
};

// Trigger Type setting
enum E_TriggerType
{
    eFlowTrigger, ePressureTrigger, eNoTriggerType
};

// Flow pattern setting
enum E_FlowPattern
{
    eSquare, eRamp
};

// The Breath Phase Indicator is used to indicate breath phase and type.
enum E_BreathPhaseIndicator
{
    eSpontaneous,
    eAssisted,
    eSupported,
    eMandatory,
    ePlateau,
    eExhalation,
    eExhHold,
    eNoVentilation,
    eNoPhase
};

// Alarm level defines the exact level at which the alarm is being annunciated.
enum E_AlarmLevel
{
    eHigh, eMedium, eLow, eAutoReset, eInactive, eLastAlarmLevel = eInactive
};

enum E_GUIEventID
{
    eNoEventID,

    // patient data ID's
    eBreathIndicator,
    eFirstPatientDataEventID = eBreathIndicator,
    ePlateauIndicator,
    eMonitoredPressure,
    eFirstNumPatientDataEventID = eMonitoredPressure,
    eMonitoredVolume,
    ePeakInspiratoryPressure,
    eEndInhalationPressure,
    eMeanAirwayPressure,
    eExhaledMinuteVolume,
    eSpontaneousMinuteVolume,
    eTotalRespiratoryRate,
    eSpontaneousRespiratoryRate,
    eIERatio,
    eRapidShallowBreathingIndex,
    eTidalVolume,
    eMonitoredO2,
    ePEEP,
    eLastNumPatientDataEventID = ePEEP,
    eLastPatientDataEventID = ePEEP,

    //GUI internal settings
    eNPPVExpiratoryTriggerSettingID,
    eFirstSettingID = eNPPVExpiratoryTriggerSettingID,
    eFirstNumSettingID = eFirstSettingID,
    eNPPVFlowInspTriggerSettingID,
    eNPPVOxygenSettingID,
    eVCVPeakFlowSettingID,
    eVCVPlateauSettingID,
    eVCVPSVSettingID,
    eVCVTidalVolumeSettingID,
    eVCVBaselinePressureSettingID,
    eVCVExpiratoryTriggerSettingID,
    eVCVFlowInspTriggerSettingID,
    eVCVPressureInspTriggerSettingID,
    eVCVOxygenSettingID,
    eVCVPSVRisetimeSettingID,

    ePCVOxygenSettingID,
    ePCVPressureSettingID,
    ePCVBaseLinePressureSettingID,
    ePCVFlowInspTriggerSettingID,
    ePCVPressureInspTriggerSettingID,
    ePCVPSVSettingID,
    ePCVExpiratoryTriggerSettingID,

    eNPPVLowTidalVolumeAlarmSettingID,

    //ApneaSettings
    eVCVApneaIntervalSettingID,
    ePCVApneaIntervalSettingID,
    eNPPVApneaIntervalSettingID,
    eLastNumSettingID = eNPPVApneaIntervalSettingID,

    eVCVHighInspPressureAlarmSettingID,
    eFirstUpperLimitSettingID = eVCVHighInspPressureAlarmSettingID,
    eNPPVIPAPSettingID,
    eNPPVITimeSettingID,
    ePCVITimeSettingID,
    eVCVApneaRateSettingID,
    ePCVApneaRateSettingID,
    eNPPVApneaRateSettingID,
    ePCVHighMinuteVolAlarmSettingID,
    eVCVHighRespRateAlarmSettingID,
    ePCVHighInspPressureAlarmSettingID,
    ePCVHighRespRateAlarmSettingID,
    eNPPVHighRespRateAlarmSettingID,
    eLastUpperLimitSettingID = eNPPVHighRespRateAlarmSettingID,

    eFirstLowerLimitSettingID,
    eVCVLowInspPressureAlarmSettingID = eFirstLowerLimitSettingID,
    eVCVLowBasePressureAlarmSettingID,
    eVCVLowMandTidalVolAlarmSettingID,
    eVCVLowSpontTidalVolAlarmSettingID,
    eVCVLowMinuteVolAlarmSettingID,
    eVCVRespiratoryRateSettingID,
    ePCVRespiratoryRateSettingID,
    eNPPVRespiratoryRateSettingID,

    ePCVLowInspPressureAlarmSettingID,
    ePCVLowBasePressureAlarmSettingID,
    ePCVLowMandTidalVolAlarmSettingID,
    ePCVLowSpontTidalVolAlarmSettingID,
    ePCVLowMinuteVolAlarmSettingID,
    ePCVRiseTimeSettingID,

    eNPPVEPAPSettingID,
    eNPPVRiseTimeSettingID,
    eNPPVLowInspPressureAlarmSettingID,
    eNPPVLowEPAPPressureAlarmSettingID,
    eNPPVLowMinuteVolAlarmSettingID,

    eLastLowerLimitSettingID = eNPPVLowMinuteVolAlarmSettingID,

    eVCVFlowPatternSettingID,
    eVCVModeSettingID,
    ePCVModeSettingID,
    eVCVITriggerTypeSettingID,
    ePCVITriggerTypeSettingID,
    eNPPVModeSettingID,
    eActiveVentilationType,
    eVCVPatientTypeSettingID,
    ePCVPatientTypeSettingID,
    eNPPVPatientTypeSettingID,
    eLastSettingID = eNPPVPatientTypeSettingID,

    eTimeStamp,
    eNumberOfModels,      //must be at end of all settings
    //   and patient data

    // front panel events
    eFirstFrontPanelEventID,
    eScreenUnLock = eFirstFrontPanelEventID,
    eScreenLock,        // start of front panel key events
    eManInsp,
    eAllO2,
    eAlarmReset,
    eAlarmSilence,
    eOptionLeft,
    eOptionRight,
    eExpHold,
    eRotaryKnobAccept,  // end of front panel key events
    eTouchPanelIn,
    eTouchPanelExit,
    eRotaryKnob,
    eLastFrontPanelEventID = eRotaryKnob,

    // miscellaneous events
    eDialogTimeoutEvent,
    eToggleReverseVideoEvent,
    eExpHoldButtonDown,
    eExpHoldButtonUp,

    //Alarms and notifications section: arranged in order of precedence
    eFirstNotificationType,
    eFirstAlarmType = eFirstNotificationType,
    eOcclusion = eFirstNotificationType, eDisconnectedCircuit = eFirstNotificationType,
    eGasSuppliesLost,
    eLowInternalBattery,
    eCorruptAltitude,
    eCorruptCompliance,
    eUsingDefaultSettings,
    ePrimaryAlarmFailure,
    eHip1,
    eHip2,
    eHighInspPressure,
    eApnea,
    eLowInspPressure,
    eAirSourceFault,
    eO2ValveStuckClosed,
    eExhValveStuckOpen,
    eLowO2Supply,
    eLowO2,
    eLowMinuteVolume,
    eLowVtMandatory,
    eHighMinuteVolume,
    eLowVt,
    eLowVtSpontaneous,
    eITimeTooLong,
    eHighRespiratoryRate,
    eHighO2,
    eHighEnclosureTemp,
    eHighInternalO2,
    eLowPeep,
    eLowEpap,
    eBadBatteryCurrentSensor,
    eBadBatteryVoltageSensor,
    eBadInternalTempSensor,
    eBadInternalO2Sensor,
    eBadADCWrapAISensor,
    eLowExternalBattery,
    eLastAlarmType = eLowExternalBattery,

    eInternalBatteryOn,
    eIntBatteryCharging,
    eServiceDue,
    //O2 sensor not operational or  unavailable
    eLastNotificationType = eServiceDue,

    //message type identifiers

    eInitialSettingsEvent,
    eEndInhalationData,
    eEndExhalationData,
    eExhHoldMnvr,

    eComplianceDefaultSetting,
    eAltitudeDefaultSetting,

    eRS232AllSettingsRequest,
    eRS232AllAlarmsRequest,
    eRS232EndInhalationDataRequest,
    eRS232EndExhalationDataRequest,
    eRS232ActiveScreenRequest,

    // diagnostic events
    eGetResponseEvent,
    eGetRetestResponseEvent,
    eTestStartedEvent,
    eTestCompletedEvent,
    eComplianceEvent,
    eResetTestResultsEvent,
    eDisplayFailureLineEvent,
    eClearFailureLineEvent,
    eClearLastTestLineEvent,
    eSensorDataEvent,
    eFailedBeamReportEvent,
    eExtremeHigh_PHigh
};

struct AlarmStatusGUIEvent
{
        E_GUIEventID ID;
        E_AlarmLevel Status;
};

// Alarm status indicates if an alarm is active or not.
enum E_AlarmStatus
{
    eActive = 0xBEEF, eNotActive = 0xDEAD
};

enum E_State
{
    eOff,
    eClose = eOff,
    eOn,
    eOpen = eOn,
    eBlinking,
    eInUse,             // For InternalBatteryLED only
    eCharging,          // For InternalBatteryLED only
    eLowChg,            // For InternalBatteryLED only
    eVeryLowChg,        // For InternalBatteryLED only
    eUnknown
};

enum E_ResponseType
{
    eAcceptResponse,
    eRejectResponse,
    eRetestResponse,
    eContinueResponse,
    eManInspKeyResponse,
    eAllO2KeyResponse,
    eAlarmResetKeyResponse,
    eAlarmSilenceKeyResponse,
    eScreenLockKeyResponse,
    eRotaryKnobAcceptKeyResponse,
    eOptionLeftKeyResponse,
    eOptionRightKeyResponse,
    eExpHoldKeyResponse,
    eRotaryKnobTurnedResponse,
    eFailedBeamReportPassResponse,
    eFailedBeamReportFailResponse
};

#endif // COMMONTYPES_H
