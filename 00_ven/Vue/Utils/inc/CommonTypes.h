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

    eFirstSIMVMode, eHighPressMode = eFirstSIMVMode,
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
    eAdult,
    ePediatric
};

// Trigger Type setting
enum E_TriggerType
{
    eFlowTrigger,
    ePressureTrigger,
    eNoTriggerType
};

// Flow pattern setting
enum E_FlowPattern
{
    eSquare,
    eRamp
};

// The Breath Phase Indicator is used to indicate breath phase and type.
enum E_BreathPhaseIndicator
{
    eSpontaneous,
    eAssisted,
    eSupported,
    eMandatory,
    ePlateau,
    eNCPAP,
    eExhalation,
    eExhHold,
    eNoVentilation,
    eNoPhase
};

// Alarm level defines the exact level at which the alarm is being annunciated.
enum E_AlarmLevel
{
    eHigh,
    eMedium,
    eLow,
    eAutoReset,
    eInactive,
    eLastAlarmLevel = eInactive
};

enum E_GUIEventID
{
    eNoEventID,
    eCheckGuiTask,
    eIsTesting,
    eAirFlowSensorRes,
    eO2FlowSensorRes,
    eExhValveRes,
    eGetHWDialogAirValveSteps,
    eGetHWDialogAirFlow,
    eSetHWDialogAirValveSteps,
    eSetHWDialogAirFlow,
    eGetHWDialogO2ValveSteps,
    eGetHWDialogO2Flow,
    eSetHWDialogO2ValveSteps,
    eSetHWDialogO2Flow,
    eSetHWDialogVoltageWrap,
    eSetHWDialogExhValveSetting,

    eMonitoredRealTimeData,

    // patient data ID's
    eBreathIndicator,
    eFirstPatientDataEventID = eBreathIndicator,
    eMonitoredPressure,
    eFirstNumPatientDataEventID = eMonitoredPressure,
    eMonitoredVolume,
    eMonitoredFlow,
    eHFOMonitoredPressure,
    eHFOMonitoredMAP,
    eInhProxyPressure,
    eExhProxyPressure,
    eAirFlow,
    eO2Flow,

    ePeakInspPressPtDataID,
    eMeanAirwayPressurePtDataID,
    ePEEPPtDataID,
    eEndInhalationPressure,
    ePlateauPressPtDataID,
    eTotalBreathRatePtDataID,
    eSpontBreathRatePtDataID,
    eIERatioPtDataID,
    eExpTidalVolumePtDataID,
    eProximalLeakPtDataID,

    eSpontaneousMinuteVolume,
    eExpMinuteVolumePtDataID,

    eHFOMapPtDataID,
    eHFOSIPtDataID,
    eHFOTidalVolumePtDataID,
    eHFOBaseFlowPtDataID,
    eHFOExhFlow,
    eHFOMinPress,
    eHFOMaxPress,
    eHFOAmplitudePtDataID,
    eHFOFreshGas,

    eNCPAPPtDataID,
    eAPRVPHighPtDataID,
    eAPRVPLowPtDataID,
    eCompliancePtDataID,
    eInspTidalVolumePtDataID,
    eInspMinuteVolumePtDataID,

    eManInspHoldTimePtDataID,
    eMonitoredO2PtDataID,
    eMonitoredCO2,
    eLastNumPatientDataEventID = eMonitoredCO2,
    eLastPatientDataEventID = eMonitoredCO2,

    //GUI internal settings
    eExpiratoryTriggerSettingID,
    eFirstSettingID = eExpiratoryTriggerSettingID,
    eFirstNumSettingID = eFirstSettingID,
    ePressureInspTriggerSettingID,

    eFlowInspTriggerSettingID,
    eBaseFlowSettingID,
    ePSVSettingID,
    eTriggerSettingID,
    eFreshGasSettingID,
    eLowAmplitudeAlarmSettingID,
    eHighAmplitudeAlarmSettingID,
    eApneaIntervalSettingID,
    eTiLimitAlarmSettingID,

    eAPRVHighPHighAlarmSettingID,
    eAPRVLowPHighAlarmSettingID,
    eAPRVHighPLowAlarmSettingID,
    eAPRVLowPLowAlarmSettingID,

    eHighTidalVolAlarmAutoSetID,
    eLowTidalVolAlarmAutoSetID,

    eProximalLeakAlarmSettingID,

    ePCVa11Setting,
    ePCVa12Setting,
    ePCVa13Setting,
    ePCVa14Setting,

    ePCVa21Setting,
    ePCVa22Setting,
    ePCVa23Setting,
    ePCVa24Setting,

    ePCVPressureGainSetting,
    ePCVFlowGainSetting,
    ePIPServoTuningSetting,
    ePEEPServoTuningSetting,
    eBaseFlowServoTuningSetting,

#ifdef HFO_SYSTEM

    eHfoExHighMapAlarmAutoSetID,
    eHfoHighAmpAlarmAutoSetID,
    eHfoLowAmpAlarmAutoSetID,

    eHFOAmplitudeSettingID,
    eHFOSITimeSettingID,
    eHFOSIRateSettingID,

#endif

    eNasalCPAPFlowSettingID,
    eBackupBRSettingID,
    eTubeDiameterSettingID,
    eTubeLengthSettingID,
    eCompensationRateSettingID,

    eVCVTidalVolumeSettingID,
    eVCVInspFlowSettingID,
    eVCVPlateauTimeSettingID,

    eAPRVPSVSettingID,
    eAPRVTimeHighSettingID,
    eAPRVTimeLowSettingID,

    eAPRVExtremeHighPHighAutoSetID,

    eExHighPEEPAlarmAutoSetID,
    eExHighInspAlarmAutoSetID,

    eLastNumSettingID = eExHighInspAlarmAutoSetID,

    eHighInspPressureAlarmSettingID,
    eFirstUpperLimitSettingID = eHighInspPressureAlarmSettingID,
    ePCVPressureSettingID,
    eITimeSettingID,
    eHighMinuteVolAlarmSettingID,
    eHighPSVAlarmSettingID,

    eAPRVPHighSettingID,
    eHighNasalCPAPAlarmSettingID,
    eHighTidalVolAlarmSettingID,
    eFlushOxygenSettingID,
    eVcvHighInspPressAlarmAutoSetID,

    eHFOFrequencySettingID,
    eLastUpperLimitSettingID = eHFOFrequencySettingID,

    eFirstTrackingUpperLimitSettingID,
    eSISettingID = eFirstTrackingUpperLimitSettingID,
    eHFOStrokeVolumeSettingID,
    eHighPEEPAlarmSettingID,
    eHighRespRateAlarmSettingID,
    eHiMAP_1AlarmSettingID,
    eHiMAP_2AlarmSettingID,
    eLastTrackingUpperLimitSettingID = eHiMAP_2AlarmSettingID,

    eFirstTrackingLowerLimitSettingID,
    eLowMAPAlarmSettingID = eFirstTrackingLowerLimitSettingID,
    eLastTrackingLowerLimitSettingID = eLowMAPAlarmSettingID,
    eExtremeLowMapAlarmSettingID,
    eHighFiO2ID,
    eLowFiO2ID,
    eFirstLowerLimitSettingID,
    eLowInspPressureAlarmSettingID = eFirstLowerLimitSettingID,
    eLowBasePressureAlarmSettingID,
    eLowMandTidalVolAlarmSettingID,
    eLowSpontTidalVolAlarmSettingID,
    eLowMinuteVolAlarmSettingID,

    eAPRVPLowSettingID,
    eLowNasalCPAPAlarmSettingID,
    eLowTidalVolAlarmSettingID,
    eOxygenSettingID,
    eNIVBreathRateSettingID,
    eVcvLowInspPressAlarmAutoSetID,

    eLowPSVAlarmSettingID,
    ePSVRiseTimeSettingID,
    eBaselinePressureSettingID,
    eHFOMapSettingID,
    eBreathRateSettingID,
    eLastLowerLimitSettingID = eBreathRateSettingID,
    eITriggerTypeSettingID,

#ifdef HFO_SYSTEM
    eHFOAmpControlSettingID,
    eHFOAutoSISettingID,
#endif

    eSIMVSyncPlusSettingID,

    eVCVWaveFormSettingID,//Flow Pattern

    eActiveVentilationType,
    eModeSettingID,
    eHummingVSettingID,
    eLastSettingID = eHummingVSettingID,
    eNumberOfModels,      //must be at end of all settings
    //   and patient data

    // front panel events
    eFirstFrontPanelEventID,
    eScreenUnLock = eFirstFrontPanelEventID,
    eScreenLock,        // start of front panel key events
    eManInsp,
#ifdef HFO_SYSTEM
    eManHoldInspStop,
    eManHoldInspStart,
#endif
    eAlarmInfomation,
    eAllO2,
    eAlarmReset,
    eAlarmSilence,
    eCancelAlarmSilence, // This is a virtual key, no hardware attach to this key.
    eRotaryKnobCancel,
    eRotaryKnobAccept,  // end of front panel key events
    eModeSelection,
    eTouchPanelIn,
    eTouchPanelExit,
    ePanelErrorMsg,
    eRotaryKnob,
    eLastFrontPanelEventID = eRotaryKnob,

    // miscellaneous events
    eDialogTimeoutEvent,
    eAlarmWindowFlashing,
    eToggleReverseVideoEvent,
    eExpHoldButtonDown,
    eExpHoldButtonUp,
    eHFOSamplingFinished,
    eHFODelayTimeout,
    eWidgetBlinking,
    eO2CalibrationStart,
    eO2SensorMissing,
    eO2SensorBad,
    eO2CalibrationComplete,
    eConnectO2,
    eCancelO2Calibration,
    eWarningLimit,

    //Alarms and notifications section: arranged in order of precedence
    eFirstNotificationType,
    eFirstAlarmType = eFirstNotificationType,
    eFirstIMVAlarmType = eFirstAlarmType,
    eOcclusion = eFirstIMVAlarmType,
    eCorruptAltitude,
    eCorruptCompliance,
    eHip,
    eExtremeHip,
    eHighInspPressure,
    eLowInspPressure,
    eHighNCPAP,
    eLowNCPAP,
    eApnea,
    eITimeTooLong,
    eHighRespiratoryRate,
    eLowMinuteVolume,
    eHighMinuteVolume,
    eLowVtMandatory,
    eLowVtSpontaneous,
    eExtremeHighPeep,

    //APRV Alarm
    eExtremeHigh_PHigh,
    eAPRVHigh_PHigh,
    eAPRVLow_PHigh,
    eAPRVHigh_PLow,
    eAPRVLow_PLow,
    eOverLimitPressure,
    eLowerLimitPressure,
    eHighVte,
    eLowVte,
    eProximalLeakage,
    eHighPeep,
    eLowPeep,
    eLastIMVAlarmType = eLowPeep,
#ifdef HFO_SYSTEM
    eFirstHFOAlarmType,
    eHighMAP = eFirstHFOAlarmType,
    eETTubeDisconnect,
    eExtremeHighMAP,
    eAbnormalMAP,
    eLowMAP,
    eExtremeLowMAP,
    eHighAmplitude,
    eLowAmplitude,
    eHighSIPress,
    eLowSIPress,
    eExtremeHighSIPress,

    eCircuitLeakInHFO,
    eLastHFOAlarmType = eCircuitLeakInHFO,
#endif

    eDaemonLostSignal,

    eDisconnectedCircuit,

    eGasSuppliesLost,
    eAirSourceFault,

    eAirValveStuckClosed,
    eAirValveStuckOpen,
    eO2ValveStuckClosed,
    eO2ValveStuckOpen,
    eExhValveStuckOpen,
    eExhValveStuckClosed,

    eO2MotorError,
    eAirMotorError,
    eExhMotorError,
    eHFOMotorError,

    eO2MotorLostCommunication,
    eAirMotorLostCommunication,
    eExhMotorLostCommunication,
    eHFOMotorLostCommunication,

    eAirFlowSensorError,
    eOxygenFlowsensorError,

    eLowO2Supply,
    eUseO2Only,
    eLowAirSupply,
    eUseAirOnly,
    eLowO2,
    eHighO2,
    eHighEnclosureTemp,
    eHighInternalO2,
    eSpeakerError,
    eFanError,
    eSettingChange,
    e100percentO2Enable,
    eHFODoorOpen,
    eHFOPistonNotConnect,
    eProxyDisconnect,
    eDACLoopBackFailure,

    eInhPAutoZero,
    eExhPAutoZero,

    eOxygenCalibration,
    eFrontPanelDisconnect,
    eUsingDefaultSettings,
    ePrimaryAlarmFailure,

    eBadBatteryCurrentSensor,
    eBadBatteryVoltageSensor,
    eBadInternalTempSensor,
    eBadInternalO2Sensor,
    eBadADCWrapAISensor,
    eBadFileSystem,

    ePreviousPowerFailed,
    eWatchDogKickoutAlarm,

    eLowInternalBattery,
    eLowExternalBattery,
    eLastAlarmType = eLowExternalBattery,


    eAbnormalPressureSensor,
    eAbnormalAirFlowSensor,
    eAbnormalO2FlowSensor,
    eAbnormalExhFlowSensor,
    eAbnormalInhPressSensor,
    eAbnormalExhPressSensor,
    eAbnormalSensor,

    eInternalBatteryOn,
    eIntBatteryCharging,
    eServiceDue,
    //O2 sensor not operational or  unavailable
    eLastNotificationType = eServiceDue,
    //message type identifiers
    eInitialSettingsEvent,
    eEndInhalationData,
#ifdef HFO_SYSTEM
    eHFOData,
    eModeChangeToHFO,
#endif
    eEndExhalationData,
    eEndAPRVPLowData,
    eExhHoldMnvr,

    eComplianceDefaultSetting,
    eAltitudeDefaultSetting,

    eLogEvent,
    ePutVentInOpEvent,
    eSensorDataEvent,

    eAmpAlarmAutoSetStartEvent,
    eAmpAlarmAutoSetStopEvent,
    eSetAlarmAmpBlankEvent,
    eProxyVersionEvent,
    ePICVersionEvent,
    eHummingVEvent,
    eMaxID
};

struct AlarmStatusGUIEvent
{
        E_GUIEventID ID;
        E_AlarmLevel Status;
};

// Alarm status indicates if an alarm is active or not.
enum E_AlarmStatus
{
    eActive = 0xBEEF,
    eNotActive = 0xDEAD
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

//Switch Type Setting
enum E_SwitchType
{
    eSwitchTypeOn,
    eSwitchTypeOff
};

// EventCode Errors.
enum E_ConfigIndex
{
    eBlockInitialized = 0,
    eTimeStampFormat,
    eUnitsOfMeasure,
    eAlarmSilenceDuration,
    eAltitudeCompensation,
    eCircuitComplianceOnOff,
    eCircuitCompliance,
    eLanguageOption,
    eLeakCompensation,
    eFiO2ConversionFactor,
    eFiO2Offset,
    eSuctionDuration,
    eHFOOilPumpTime,//in milisecond
    ePowerFailed, //change eLPS to ePowerFailed
    //Service date config
    eElevationSetting,
    eAirValveLiftOff,
    eO2ValveLiftOff,
    eWatchDogEnabled,//eFalse:Disalbed; eTrue:Enabled
    ePressureUnit,//0: hPa; #0: cmH2O
    eHFOOperTime,//in Minute
    eConvOperTime,//in Minute
    eSpontOperTime,//in Minute
    eFirstHfoMonitoredItem,
    eSecondHfoMonitoredItem,
    eThirdHfoMonitoredItem,
    eForthHfoMonitoredItem,

    eFirstImvMonitoredItem,
    eSecondImvMonitoredItem,
    eThirdImvMonitoredItem,
    eForthImvMonitoredItem,

    eFirstNivMonitoredItem,
    eSecondNivMonitoredItem,
    eThirdNivMonitoredItem,
    eForthNivMonitoredItem,

    eFirstAprvMonitoredItem,
    eSecondAprvMonitoredItem,
    eThirdAprvMonitoredItem,
    eForthAprvMonitoredItem,

    eScreenBrightness,
    eScreenMonitor,//Monitor patient data on an external screen

    //    eSoundLevel,

    eMEModePasswordConfig,
    eWatchDogKickout,

    eFeatureLicenseKey,
    ePtCircuitCompliance,
    eSoundLevel,
    eFiO2PassCalibration,

    eHvPlusMacAddress,
    eHvAdvancedMacAddress,

    NUMOFSYSCONFIG//when adding a new enum, we must add before NUMOFSYSCONFIG and after the previous of NUMOFSYSCONFIG
};

#endif // COMMONTYPES_H
