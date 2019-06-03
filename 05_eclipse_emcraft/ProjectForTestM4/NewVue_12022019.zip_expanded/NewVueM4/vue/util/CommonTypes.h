/*
 * CommonTypes.h
 *
 *  Created on: 28 thg 12, 2018
 *      Author: haxua
 */

#ifndef UTIL_COMMONTYPES_H_
#define UTIL_COMMONTYPES_H_
/******************************************************************************/
//$COMMON.H$
//   File Name:  CommonTypes.h
//   Copyright 2006 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains no class declaration.  It is a repository for
//      common ventilator specific types used throughout the system.
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      01    01/28/06  K.A. McGuigan       HUMX
//      Description: Initial Revision
//
/******************************************************************************/
#include "standard.h"
//#define COMMON_TYPES_A53
#define HFO_SYSTEM
#define DEVICE_TASK_DELAY_TIME 2 //2ms
#define IPC_TASK_DELAY_TIME 2	//2ms
#define   MIN_STEP_POSITION_OLD  0
#define   MAX_STEP_POSITION_OLD  2200
#define MAX_STEP_POSITION_CURRENT 12800
#define MIN_STEP_POSITION_HFO 0
#define MAX_STEP_POSITION_HFO 10421
#define HARD_STEP_POSITION_HFO 9214
#define MIDDLE_STEP_POSITION_HFO 9214/2 //((10515-1450)/2)
typedef enum
{
	eSmallCompliance,
	eLargeCompliance,
	eLastCompliance
}E_ParameterType;
typedef enum {
    eFalse = 0,
    eTrue
} E_Bool;
// Ventilation Mode includes a list of both operator set modes and alarm
// modes and a start-up mode.
typedef enum
{
    eManualSIType,
    eAutoSIType
} E_SIType;
typedef enum
{
    eAzPending,
    eAzActive,
    eAzCompleted
} E_AzTestState;
typedef enum  {
    eNotCalibrated = 0x55,
    eCalibrated,
    eResetAndRecovered,
    eSensorError
} ProxyStatus;

typedef enum
{
    eVolumeVentilation = 0x0010,
    ePressureVentilation = 0x0001,
    eNoVentType
} E_VentilationType;

typedef enum
{
    eFirstHFOMode = 0,
    eHFOMode = eFirstHFOMode,
    eFirstAlarmMode,
    eHFOHighMAP_2Mode = eFirstAlarmMode,
    eHFOExtremeLowMAPMode,
    eLastHFOMode = eHFOExtremeLowMAPMode,

    eFirstCommonMode,

    eHardwareErrorMode = eFirstCommonMode,
    eNoGasMode,

    eLastCommonMode = eNoGasMode,

    eFirstSIMVMode,
    eHighPressMode = eFirstSIMVMode,
    eDisconnectionMode,
    eLastAlarmMode = eDisconnectionMode,

    eMixedMode ,//7//Should be removed
    ePcvMixedMode,
    eVcvMixedMode,
    eVaMixedMode,
    eAPRVMode,//PCV

    ePcvACMode,
    eVcvACMode,
    eVaACMode,

    eSpontaneousMode,//Should be removed
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

} E_VentilationMode;

//Ventilation mode tab view
typedef enum {
    eHFOTabView,
    eConvTabView,
    eSpontTabView,
    eStandbyTabView
} E_VentTabView;

//TabView Order
typedef enum {
    eFirstTabViewOrder,
    eSecondTabViewOrder,
    eThirdTabViewOrder
} E_TabViewOrder;

//Monitored Loop Graph Data Pair
typedef enum
{
    ePressureVolumeGraph,//Default First Graph
    ePressureFlowGraph,
    eVolumeFlowGraph//Default Second Graph
} MonitoredLoopDataPair;

//End User Feature
typedef enum
{
    eLowEndFeature,
    eMiddleEndFeature,
    eHighEndFeature
} EndUserFeature;

//Language Options
typedef enum
{
    eEnglish,
    eJapanese,
    eSpanish,
    eFrench,
    eRussian
} LanguageOptions;
typedef enum
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


    eFirstIMVTrigger,
    eTotalGasLossTrigger = eFirstIMVTrigger,
    eOcclusionTrigger,
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

    eAPRV_PtInhPressTrigger_InPLow,
    eAPRV_PtInhPressTrigger_InPHigh,

    eHPHighAlarmTrigger,

    eProxyDisconnectTrigger,
    eProxyConnectTrigger,

    ePtExhFlowSuppTrigger,

    eLastIMVTrigger = ePtExhFlowSuppTrigger,

    eNumTriggers, // these 2 must remain at the end of list
    eNoTrigger = eNumTriggers
} E_TriggerId;
// Trigger Type setting
typedef enum
{
    eFlowTrigger,
    ePressureTrigger,
    eNoTriggerType
} E_TriggerType;

//Switch Type Setting
typedef enum
{
    eSwitchTypeOn,
    eSwitchTypeOff
} E_SwitchType;

// Valve Units setting
typedef enum
{
    eSecondaryUnits,
    ePrimaryUnits,
    eNoDualUnits
} E_DualUnitsType;


// The Breath Phase Indicator is used to indicate breath phase and type.
typedef enum
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
} E_BreathPhaseIndicator;


// Alarm level defines the exact level at which the alarm is being annunciated.
typedef enum
{
    eHigh = 0,
    eMedium,
    eLow,
    eAutoReset,
    eInactive,
    eLastAlarmLevel = eInactive
} E_AlarmLevel;


// Alarm status indicates if an alarm is active or not.
typedef enum
{
    eActive,
    eNotActive
} E_AlarmStatus;

typedef enum
{
    eNoDevice = 0,
    eSpeaker,
    eBuzzer
} E_SoundDevice;

typedef enum
{
    eNoSound,
    eHighSound,                // highest prioriity
    eMediumSound,
    eLowSound,
    eGoodBeep,
    eBadBeep,
    eClick,
    eInformationalSound,    // lowest priority
    ePannelLockSound,
    eTerminateNow            // used to terminate alarms immediately
    // alarms are normally allowed to finish
    // their normal sound sequence
} E_SoundType;
typedef enum
{
    eMEModeLogin,
    eFactoryModeLogin
} E_ModeLogin;
typedef enum
{
    eSquare,//Square Wave Type
    eRamp//Decrease Wave Type
} E_FlowPattern;

//When insert new string IDs, please insert at the end of E_StringID typedef enum
typedef enum
{
    eNoString,

    // warning messages
    eUpperLimitExceeded,
    eLowerLimitExceeded,
    eUsingDefaults,
    eAltitudeMessage,
    eComplianceMessage,
    eGSLMessage,
    eOcclusionMessage,
    eDisconnectETMessage,
    eO2TestStartMessage,
#ifdef	HFO_SYSTEM
    eHighMapMessage,
    eExtremeLowMAPMessage,
    eAbnormalMapMessage,
#endif
    eAirValveOpenMessage,
    eO2ValveOpenMessage,
    eMEModeMessage,

    // constraint violations
    eBelowRiseTime,
    eITimeExceeded,
    eVCVIERatioViolatedApnea,
    eVCVIERatioViolatedFlow,
    eVCVIERatioViolatedVolume,
    eVCVIERatioViolatedRate,
    eVCVIERatioViolatedPlateau,
    eVCVIERatioViolatedWaveForm,
    ePCVIERatioViolatedApnea,
    ePCVIERatioViolatedITimeByBR,
    ePCVIERatioViolatedITimeByBackupBR,
    eIERatioViolated,
    eVCVMaxITimeViolated,
    eAboveApnea,
    eBelowRespRate,
    eAboveBreathRate,
    ePipBelowPeep,
    ePeepAbovePip,
#ifdef	HFO_SYSTEM
    eMAP_1BelowMAPSetting,
    eMAP_2BelowMAP_1,
    eLowMAPAboveMAPSetting,
    eSIBelowMAPSetting,
    eMapAboveSISetting,
    eBelowLowAmplitude,
    eAboveHighAmplitude,
    eHighMap_2TolBelowHighMap_1Tol,
#endif
    eHighPipAlarmBelowPip,
    eLowPipAlarmAbovePip,

    eHighPipAlarmBelowLowPipAlarm,
    eLowPipAlarmAboveHighPipAlarm,

    eHighPEEPAlarmBelow,
    eLowPEEPAlarmAbove,

    eHighMinuteVolAlarmBelow,
    eLowMinuteVolAlarmAbove,

    eAPRVHighPHighAlarmBelow,
    eAPRVLowPHighAlarmAbove,
    eAPRVHighPLowAlarmBelow,
    eAPRVLowPLowAlarmAbove,
    eAPRVBelowPLow,
    eAPRVAbovePHigh,
    eBelowLowNCPAP,
    eAboveHighNCPAP,
    eBelowLowVt,
    eAboveHighVt,
    eBelowFiO2,
    eAboveFlushFiO2,
    eBelowBreathRate,
    eAboveBackupBR,
    eAprvTHighBelowRiseTime,
    eAprvRiseTimeAboveTHigh,

    //confirmation messages
    eMixedModeChange,
    eAssistModeChange,
    eRampChange,
    eSquareChange,
    eModeChangeInCPAP,
    eAdultChange,
    ePediatricChange,
    eITrigFlowChange,
    eITrigPressureChange,
    ePEEPChange,

    eUnknownModel,

    //values returned by Model::GetValueString
    eRampPattern,
    eSquarePattern,
    eFlowTriggerText,
    ePressureTriggerText,
    eHFOModeText,
    eMixedModeText,
    eSpontaneousModeText,
    eStandbyModeText,
    eIMVPatientDataList,
    eAdultPatientTypeText,
    ePediatricPatientTypeText,
    eSquareWaveFormText,
    eDecreaseWaveFormText,
    eSwitchOnText,
    eSwitchOffText,

    // Setting Widget Labels
    eSWETrigger,
    eSWPlateau,
    eSWRisetime,
    eSWHighInspPressure,
    eSWLowInspPressure,
    eSWLowPEEP,
    eSWLowVteMand,
    eSWLowVteSpont,
    eSWHighRespRate,
    eSWLowMinuteVolume,
    eSWHighMinuteVolume,
    eSWApneaRate,
    eSWApneaInterval,
    eSWPCVHIPTolerance,
    eSWVCVHIPTolerance,
    eSWO2Calibration,

#ifdef	HFO_SYSTEM
    eSWHFOHighMAP_1Tolerance,
    eSWHFOHighMAP_2Tolerance,
    eSWHFOLowMAPTolerance,
    eSWHFOHighAmpTolerance,
    eSWHFOLowExhFlowAlarm,
    eSWHFOLowAmpTolerance,
    eSWFreshGas,
    eSWHiMAP_1Alarm,
    eSWHiMAP_2Alarm,
    eSWLowMAPAlarm,
    eSWHiAmplitudeAlarm,
    eSWLowAmplitudeAlarm,
    eSWSInflation,
#endif

    //Units for Setting Widgets
    eBPM,
    emL,
    eL,
    eLPM,
    emLSec,
    ecmH2O,
    eStep,
    emmHg,
    ePerCent,
    eSec,
    eFeet,
    eMeters,
    eSTR_kpa,
    eSTR_mb,
    eHz,
    eCPH,
    eMM,
    emLOverCmH2O,

    //Strings for UnitsSelector button labels
    eSTR_Feet,
    eSTR_Meters,
    eSTR_KiloPascals,
    eSTR_Millibars,
    eSTR_cmH2O,

    //Breath indicator labels
    eSpontBI,
    eMandBI,
    eAssistBI,
    ePlateauBI,
    eSupportBI,
    eExhalationBI,

    //Alarm mode labels
    eNormalOP,
    eDisconnectET_VIO,
    eHighMap_VIO,
    eAbnormalMap_VIO,
    eOcclusion_VIO,
    eGSL_VIO,
    eExtremeLowMAP_VIO,
    //Ventilation mode labels
    eVCVSIMV,
    eVCVAssist,
    eVCVCPAP,
    ePCVSIMV,
    ePCVAssist,
    ePCVCPAP,
    eN_CPAP,
#ifdef HFO_SYSTEM
    eHFO,
    // Menu bar labels
    eMBHfo,
#endif
    eMBVent,
    eMBAlarms,
    eMBPatientData,
    eMBOptions,
    eMBOption,
    eMBAudibleOn,
    eMBAudibleOff,
    eMBOperationalHour,

    // Graphic labels
    eButtonShowGraphicsDialog,
    ePressureGraphSelectCaption,
    eFlowGraphSelectCaption,
    eVolumeGraphSelectCaption,
    eFreezeGraphCaption,
    eContinueGraphCaption,
    eAutoScaleCaption,
    ePressureVolumeLabel,
    eFlowVolumeLabel,
    ePressureLabel,
    eFlowLabel,
    eVolumeLabel,
    eCaptureCaption,
    eShowLoopLabel,


    //Diagnostic Menu bar labels
    eMBSST,
    eMBEST,
    eMBHardwareDiag,
    eMBSoftwareDiag,
    eMBUserConfig,
    eMBErrorCodes,
    eMBInformation,

    // Diagnostic mode message labels
    eWarning,
    eWarningMessage1,
    eWarningMessage2,
    eCompliance,
    eBlockPatientWyeMsg,
    eUnblockPatientWyeMsg,
    eVerifyReliefValvePosition,
    eConnectO2Msg,
    eDisconnectO2Msg,
    eO2SensorBadMsg,
    eO2SensorMissingMsg,
    eO2SupplyMissingMsg,
    eO2CalibratingMsg,
    eO2CalibrationCompleteMsg,
    eO2CalibrationFailedMsg,
    eOxygenCalibrationCancelMsg,

    eDisconnectCircuitFromFilterMsg,
    ePatientIsConnectedMsg,
    ePrimaryAudioMsg,
    eBackupAudioMsg,
    ePressManInspKeyMsg,
    ePressAllO2KeyMsg,
    ePressAlarmResetKeyMsg,
    ePressAlarmSilenceKeyMsg,
    ePressScreenLockKeyMsg,
    ePressRotaryKnobAcceptKeyMsg,
    ePressOptionLeftKeyMsg,
    ePressOptionRightKeyMsg,
    ePressExpHoldKeyMsg,
    eTurnRotaryKnobMsg,
    eNormalLEDMsg,
    eAlarmHighLEDMsg,
    eAlarmMedLEDMsg,
    eVentInopLEDMsg,
    eSafetyValveLEDMsg,
    eAlarmSilenceLEDMsg,
    eAllO2LEDMsg,
    eScreenLockLEDMsg,
    eIntBattInUseLEDMsg,
    eIntBattChargingLEDMsg,
    eIntBattLowLEDMsg,
    eSSTPassedMsg,
    eESTPassedMsg,
    eSSTFailedMsg,
    eESTFailedMsg,
    eStepModeMsg,
    eInvalidDateMsg,
    eStartupMsg,
    eDiagCodesConfirmMsg,

    // Diagnostic mode test results labels
    ePassed,
    eFailed,
    eCanceled,
    eBlockPatientWyeTest,
    eLiftOffValve,
    ePatientCircuitTest,
    eOxygenSupplyTest,
    eOxygenDeliveryTest,
    eOxygenSensorTest,
    eAirDeliveryTest,
    eCrossoverCircuitTest,
    eBlowerTest,
    eHeatedFilterTest,
    eSafetyValveTest,
    eReliefValveTestAtHFO,
    eExhalationValveTest,
    eAudioTest,
    eKeyboardTest,
    eLEDTest,
    eFailureCode,
    eMeasured,
    eTolerance,
    eTestResults,
    eFailureData,

    // diagnostic button labels
    eStartSSTLabel,
    eStartESTLabel,
    eAirLabel,
    eOxygenLabel,
    eExhalationLabel,
    eInhalationLabel,
    eMonitorsLabel,
    eVoltageWrapLabel,
    eBlowerLabel,
    eFilterLabel,
    e24V_Label,
    eSafetyLabel,
    eCrossoverLabel,
    eMonthLabel,
    eDayLabel,
    eYearLabel,
    eHourLabel,
    eMinuteLabel,
    eSecondLabel,
    eResetOperHourLabel,
    eMonthOfServiceLabel,
    eDayOfServiceLabel,
    eYearOfServiceLabel,
    eApplyNewDateLabel,
    eApplyNewTimeLabel,
    eComplianceCompEnbLabel,
    eNextLabel,
    ePrevLabel,
    eClearCodesLabel,
    eAltitudeLabel,
    e24hrClockLabel,

    // diagnostic sensors labels
    eAirFlowSensorLabel,
    eOxygenFlowSensorLabel,
    eExhalationFlowSensorLabel,
    eAirPositionLabel,
    eOxygenPositionLabel,
    eExhalationPositionLabel,
    eInhalationPressureLabel,
    eExhalationPressureLabel,
    eOxygenSupplyLabel,
    eOxygenSensorLabel,
    eBatteryVoltageLabel,
    eBatteryCurrentLabel,
    ePCMCIATypeLabel,
    eEnclosureTempLabel,
    eInternalOxygenLabel,
    eADC_WrapLabel,

    // diagnostic hardware screen labels
    eEventNumberLabel,
    eEventCodeLabel,
    eDateLabel,
    eTimeLabel,
    eRepeatLabel,
    eCorruptedLabel,
    eEmptyLabel,

    // diagnostic software screen labels
    eSerialNumberLabel,
    eFlashPartNumberLabel,
    eFlashVersionLabel,
    eOTPPartNumberLabel,
    eOTPVersionLabel,
    eAirStepperVersionLabel,
    eO2StepperVersionLabel,
    eExhStepperVersionLabel,
    eAirFlowSensPartNumLabel,
    eO2FlowSensPartNumLabel,
    eExhFlowSensPartNumLabel,
    eDisplayVersionLabel,

    // diagnostic units labels
    eSteps,
    eStepsSml,
    eV,
    eV_Sml,
    emV,
    euV,
    eA,
    eA_Sml,
    eA2DCnts,
    eDACCnts,
    emlcmH2O,
    eDegrees,

    // diagnostic test failure labels
    eTheLbl,
    eFailedLbl,

    // clock labels
    eAM,
    ePM,

    //ModeSettingWidget Labels
    eModeAssist,
    eModeSIMV,

    //ValueSettingDialog
    eVSDIncrease,
    eVSDDecrease,

    //SpecialDataValues
    eBlankedValue,

    //ITrigger Selector buttons,
    eITrigPressure,
    eITrigFlow,

    // Valve Units Selector buttons,
    eValveUnitsLpm,
    eValveUnitsStep,

    //Ventilation type indicators
    ePressureControl,
    eVolumeControl,
    eNonInvasiveControl,

    // Labels for DataView groups
    eDVPressure,
    eDVVolume,
    eDVRate,
    eDVPatientData,
#ifdef HFO_SYSTEM
    eDVHFOData,
#endif
    // Miscellaneous   buttons
    eCloseBtn,
    eCancelBtn,
    eAcceptBtn,
    eOKBtn,
    eYesBtn,
    eNoBtn,
    eContinueBtn,
    eRetestBtn,

    eSTR_ModeSettingAbbrevName,
    eSTR_ModeSettingFullName,
    //Alarm messages
    eSTR_Alarm_Occlusion,
    eSTR_Alarm_CorruptAltitude,
    eSTR_Alarm_CorruptCompliance,
    eSTR_Alarm_Hip,
    eSTR_Alarm_ExtremeHip,
    eSTR_Alarm_HighInspPressure,
    eSTR_Alarm_Apnea,
    eSTR_Alarm_LowInspPressure,

    eSTR_Alarm_HighNCPAP,
    eSTR_Alarm_LowNCPAP,

    eSTR_Alarm_LowMinuteVolume,
    eSTR_Alarm_LowVtMandatory,
    eSTR_Alarm_HighMinuteVolume,
    eSTR_Alarm_LowVt,
    eSTR_Alarm_LowVtSpontaneous,
    eSTR_Alarm_ITimeTooLong,
    eSTR_Alarm_HighRespiratoryRate,
    eSTR_Alarm_LowPeep,
    eSTR_Alarm_HighPeep,
    eSTR_Alarm_ExtremeHighPeep,
    eSTR_Alarm_LowPip,
    eSTR_Alarm_HighPip,

#ifdef HFO_SYSTEM
    eSTR_Alarm_HighMAP,
    eSTR_Alarm_OverLimitPressure,
    eSTR_Alarm_LowerLimitPressure,
    eSTR_Alarm_ExtremeHighMAP,
    eSTR_Alarm_LowMAP,
    eSTR_Alarm_AbnormalMAP,
    eSTR_Alarm_ExtremeLowMAP,
    eSTR_Alarm_HighAmplitude,
    eSTR_Alarm_LowAmplitude,
    eSTR_Alarm_ProximalLeakage,
    eSTR_Alarm_ETTubeDisconnect,
    eSTR_Alarm_HighSIPressure,
    eSTR_Alarm_LowSIPressure,
    eSTR_Alarm_ExtremeHighSIPressure,
    eSTR_Alarm_CircuitLeakInHFO,

#endif

    eSTR_Alarm_DisconnectedCircuit,
    eSTR_Alarm_GasSuppliesLost,
    eSTR_Alarm_AirSourceFault,
    eSTR_Alarm_DaemonLostSignal,

    eSTR_Alarm_AirValveStuckClosed,
    eSTR_Alarm_AirValveStuckOpen,
    eSTR_Alarm_O2ValveStuckClosed,
    eSTR_Alarm_O2ValveStuckOpen,
    eSTR_Alarm_ExhValveStuckOpen,
    eSTR_Alarm_ExhValveStuckClosed,

    eSTR_Alarm_O2MotorError,
    eSTR_Alarm_AirMotorError,
    eSTR_Alarm_ExhMotorError,
    eSTR_Alarm_HFOMotorError,

    eSTR_Alarm_SpeakerError,

    eSTR_Alarm_O2MotorLostCommunication,
    eSTR_Alarm_AirMotorLostCommunication,
    eSTR_Alarm_ExhMotorLostCommunication,
    eSTR_Alarm_HFOMotorLostCommunication,

    eSTR_Alarm_AirFowSensorError,
    eSTR_Alarm_OxygenFlowSensorError,

    eSTR_Alarm_LowO2Supply,
    eSTR_Alarm_UseO2Only,
    eSTR_Alarm_LowAirSupply,
    eSTR_Alarm_UseAirOnly,
    eSTR_Alarm_LowO2,
    eSTR_Alarm_HighO2,
    eSTR_Alarm_HighEnclosureTemp,
    eSTR_Alarm_HighInternalO2,
    eSTR_Alarm_FanError,
    eSTR_Alarm_SettingChange,
    eSTR_Alarm_100percentO2Enable,
    eSTR_Alarm_HFODoorOpen,
    eSTR_Alarm_HFOPistonNotConnect,
    eSTR_Alarm_ProxyDisconnect,
    eSTR_Alarm_DACLoopBackFailure,
    eSTR_Alarm_OxygenCalibration,
    eSTR_Alarm_FrontPanelDisconnect,
    eSTR_Alarm_UsingDefaultSettings,
    eSTR_Alarm_PrimaryAlarmFailure,

    eSTR_Alarm_BadBatteryCurrentSensor,
    eSTR_Alarm_BadBatteryVoltageSensor,
    eSTR_Alarm_BadInternalTempSensor,
    eSTR_Alarm_BadInternalO2Sensor,
    eSTR_Alarm_BadADCWrapAISensor,
    eSTR_Alarm_BadFileSystem,

    eSTR_Alarm_PreviousPowerFailed,
    eSTR_Alarm_eWatchDogKickoutAlarm,

    eSTR_Alarm_LowInternalBattery,
    eSTR_Alarm_LowExternalBattery,
    eSTR_Alarm_IntBatteryCharging,
    eSTR_Alarm_InternalBatteryO,
    eSTR_Alarm_ServiceDue,

    eSTR_Alarm_ExtremeHigh_PHigh,

    eSTR_Alarm_APRVHigh_PHigh,
    eSTR_Alarm_APRVLow_PHigh,
    eSTR_Alarm_APRVHigh_PLow,
    eSTR_Alarm_APRVLow_PLow,

    eSTR_Alarm_HighVte,
    eSTR_Alarm_LowVte,

    eSTR_Alarm_NoEventID,

    // names for models
    eSTR_MName_ApInt,
    eSTR_MName_ApneaInterval,
    eSTR_MName_ApneaRate,
    eSTR_MName_ApneaRR,
    eSTR_MName_EndExp,
    eSTR_MName_ETrigger,
    eSTR_MName_FVt,
    eSTR_MName_HighPressure,
    eSTR_MName_HighRate,
    eSTR_MName_HighPSV,
    eSTR_MName_HIP,
    eSTR_MName_HRR,
    eSTR_MName_PP,
    eSTR_MName_EP,
    eSTR_MName_IERatio,
    eSTR_MName_ITime,
    eSTR_MName_ITrigger,
    eSTR_MName_LIP,
    eSTR_MName_O2Flow,
    eSTR_MName_LMSTV,
    eSTR_MName_LMTV,
    eSTR_MName_LowVE,
    eSTR_MName_LVE,
    eSTR_MName_LOP,
    eSTR_MName_HighVE,
    eSTR_MName_HVE,
    eSTR_MName_LowPEEP,
    eSTR_MName_HighPEEP,
    eSTR_MName_LowPressure,
    eSTR_MName_LowPSV,
    eSTR_MName_LowVtMand,
    eSTR_MName_LowVtSpont,
    eSTR_MName_AirFlow,
    eSTR_MName_MeanAirwayPress,
    eSTR_MName_MAP,
    eSTR_MName_O2,
    eSTR_MName_FlushO2,
    eSTR_MName_MonitorO2,
    eSTR_MName_CO2,
    eSTR_MName_PeakFlow,
    eSTR_MName_PEEP,
    eSTR_MName_PEEP_CPAP,
    eSTR_MName_PIEnd,
    eSTR_MName_PeakInspPressure,
    eSTR_MName_PIP,
    eSTR_MName_Plateau,
    eSTR_MName_Pressure,
    eSTR_MName_InspPressure,
    eSTR_MName_InspPres,
    eSTR_MName_NCPAP,
    eSTR_MName_VcvAutoHighPIP,
    eSTR_MName_VcvAutoLowPIP,

#ifdef HFO_SYSTEM
    eSTR_MName_HFOFreq,
    eSTR_MName_HFOMPress,
    eSTR_MName_HFOMSI,
    eSTR_MName_SIPress,
    eSTR_MName_MinPress,
    eSTR_MName_MaxPress,
    eSTR_MName_Amplitude,
    eSTR_MName_SVolume,
    eSTR_MName_FreshGas,
    eSTR_MName_ExhFlow,
    eSTR_MName_HiMAP_1Alarm,
    eSTR_MName_HiMAP_2Alarm,
    eSTR_MName_LowMAPAlarm,
    eSTR_MName_ExtremeLowMAPAlarm,
    eSTR_MName_HiAmplitudeAlarm,
    eSTR_MName_LowAmplitudeAlarm,
    eSTR_MName_HFOPressure,
    eSTR_MName_HFOAmplitudeControl,
    eSTR_MName_HFOAutoSI,
    eSTR_MName_HFOSIRate,

#endif

    eSTR_MName_NasalCPAPFlow,
    eSTR_MName_BackupBreathRate,
    eSTR_MName_SyncPlus,
    eSTR_MName_TubeDiameter,
    eSTR_MName_TubeLength,
    eSTR_MName_CompensationRate,
    eSTR_MName_InspFlow,
    eSTR_MName_PlateauTime,
    eSTR_MName_APRVPHigh,
    eSTR_MName_APRVPLow,
    eSTR_MName_APRVTHigh,
    eSTR_MName_APRVTLow,
    eSTR_MName_WaveForm,
    eSTR_MName_TiLimit,
    eSTR_MName_HighNasalCPAP,
    eSTR_MName_LowNasalCPAP,
    eSTR_MName_APRVHighPHigh,
    eSTR_MName_APRVLowPHigh,
    eSTR_MName_APRVHighPLow,
    eSTR_MName_APRVLowPLow,
    eSTR_MName_HighExhTidalVolume,
    eSTR_MName_LowExhTidalVolume,
    eSTR_MName_Compliance,

    eSTR_MName_Vti,
    eSTR_MName_MVi,

    eSTR_MName_BaseFlow,
    eSTR_MName_PressureTrigger,
    eSTR_MName_FlowTrigger,
    eSTR_MName_PSV,
    eSTR_MName_Rate,
    eSTR_MName_BR_Total,
    eSTR_MName_Rate_Abbv,
    eSTR_MName_RiseTime,
    eSTR_MName_RR,
    eSTR_MName_SpontRate,
    eSTR_MName_SpontRR,
    eSTR_MName_SpontVE,
    eSTR_MName_Vte,
    eSTR_MName_ProximalLeak,
    eSTR_MName_HFOTidalVol,
    eSTR_MName_TidalVolume,
    eSTR_MName_MVe,
    eSTR_MName_VE,
    eSTR_MName_Vol,
    eSTR_MName_Volume,
    eSTR_MName_Flow,
    eSTR_MName_Vt,
    eSTR_MName_TriggerType,

    eSTR_MName_a11,
    eSTR_MName_a12,
    eSTR_MName_a13,
    eSTR_MName_a14,

    eSTR_MName_a21,
    eSTR_MName_a22,
    eSTR_MName_a23,
    eSTR_MName_a24,

    eSTR_MName_PressureGain,
    eSTR_MName_FlowGain,

    eSTR_MName_PIPServoTuningSetting,
    eSTR_MName_PEEPServoTuningSetting,
    eSTR_MName_BaseFlowServoTuningSetting,

#ifdef HFO_SYSTEM
    eSTR_MName_ManInspHoldTime,
    eSTR_MName_HighMapAlarmOffset,
    eSTR_MName_LowMapAlarmOffset,
    eSTR_MName_ExtremeLowMapAlarmOffset,
    eSTR_MName_ExtremeHighMapAlarmOffset,
    eSTR_MName_LowAmplitudeAlarmOffset,
    eSTR_MName_HighAmplitudeAlarmOffset,
#endif
    eSTR_MName_HighInspAlarmOffset,
    eSTR_MName_LowInspAlarmOffset,
    eSTR_MName_HighPEEPAlarmOffset,
    eSTR_MName_LowPEEPAlarmOffset,

    eSTR_MName_ExtremeHighPEEPAlarmOffset,
    eSTR_MName_ExtremeHighInspAlarmOffset,

    eSTR_MName_ExtremeHighPHighAlarmOffSet,

    eSTR_MName_HardwareDiagHFOFreq,
    eSTR_MName_HardwareDiagHFOStrokeVol,
    eSTR_MName_HardwareDiagHFOOffset,

    eSTR_MName_HardwareDiagAirValve,
    eSTR_MName_HardwareDiagO2Valve,
    eSTR_MName_HardwareDiagExhValve,
    eSTR_MName_HardwareDiagHFOValve,

    eSTR_ConfirmMessage_ActivateVCV,
    eSTR_ConfirmMessage_ActivatePCV,

    eSTR_VentInopDlg_Title,
    eSTR_VentInopDlg_Error,

    eAlarmsListTitle,

    eSTR_OperationalHour,
    eSTR_BisOperationalHour,
    eSTR_SoftwareVersion,
    eStaticText ,

    eTestFont,
    eTestFont1,
    eTestFont2,
    eVentInfo,
    ePrintTestStatus,
    ePrintTestCode,
    ePrintTestName,
    ePrintSSTSummary,
    ePrintESTSummary,
    eBlankString,

    eInspBelowPSV_PEEP,
    ePEEPAboveInsp_PSV,
    ePSVAboveInsp_PEEP,
    eLogic,
    eSetReliefValveToSIMVLowPressureMode,
    eSetReliefValveToSIMVHighPressureMode,
    eSetReliefValveToHFOMode,
    eReliefValveTestAtLowSIMVPressure,
    eReliefValveTestAtHighSIMVPressure,

    //Log String; only information string
    eSTR_LOG_GuiTaskError,
    eSTR_LOG_AirValveStuckOpen,
    eSTR_LOG_AirValveStuckClose,
    eSTR_LOG_O2ValveStuckOpen,
    eSTR_LOG_O2ValveStuckClose,
    eSTR_LOG_PowerSupplyLost,
    eSTR_LOG_AnalogLoopbackError,
    eSTR_LOG_FileSystemError,
    eSTR_LOG_ProxyBoardReset,
    eSTR_LOG_MotorError,
    eSTR_LOG_SpeakerError,
    eSTR_LOG_O2MotorLostCommunication,
    eSTR_LOG_AirMotorLostCommunication,
    eSTR_LOG_ExhMotorLostCommunication,
    eSTR_LOG_HFOMotorLostCommunication,
    eSTR_LOG_FlowSensorError,
    eSTR_LOG_InhPressSensorError,
    eSTR_LOG_ExhPressSensorError,
    eSTR_LOG_TestCancelled,
    eSTR_LOG_TestFailed,
    eSTR_LOG_TestPassed,
    eSTR_LOG_FrontPanelDisconnect,
    eSTR_LOG_ClearProxyCalibrationFailed,
    eSTR_LOG_AirFlowSensorCRCError,
    eSTR_LOG_AirFlowSensorModelError,
    eSTR_LOG_O2FlowSensorCRCError,
    eSTR_LOG_O2FlowSensorModelError,
    eSTR_LOG_ConfigSoundError2Play,
    eSTR_LOG_ConfigSoundError2Record,
    eSTR_LOG_ChangePrivilegeLevelError,

    eSTR_LOG_AlarmMuteButtonPressed,
    eSTR_LOG_AlarmResetButtonPressed,
    eSTR_LOG_IncreaseBrightnessButtonPressed,
    eSTR_LOG_DecreaseBrightnessButtonPressed,
    eSTR_LOG_ControlLockButtonPressed,
    eSTR_LOG_HFOSIButtonPressed,
    eSTR_LOG_ManualBreathButtonPressed,
    eSTR_LOG_AcceptShutDownButtonPressed,
    eSTR_LOG_ModeChanged,
    eSTR_LOG_100PercentO2ButtonPressed,
    eSTR_LOG_TriggerTypeSettingChanged,

    eSTR_LOG_TaskTimeout,
    eSTR_LOG_ReadFromFileError,
    eSTR_LOG_WriteToFileError,

    eSTR_LOG_CopyFileError,
    eSTR_LOG_ReadTrendingFileError,

    ePcMixedModeText,
    ePcMixedModeWithSyncPlusText,
    eVcMixedModeText,
    eVaMixedModeText,
    eAprvModeText,
    ePcAcModeText,
    eVcAcModeText,
    eVaAcModeText,
    ePcSpontModeText,
    eVcSpontModeText,
    eNivModeText,
    eNcpapModeText,
    eApneaBackupModeText,
    eVentInOpModeText,

    eAprvHighPipBelowPhighAndPsv,
    ePcvSimvHighPipBelowPsvPeep,
    eVcvBaseFlowAboveInspFlow,
    eVcvInspFlowBelowBaseFlow,

    eSTR_Alarm_InhPAutoZero,
    eSTR_Alarm_eExhPAutoZero,
    eSTR_LOG_BackupModeStart,
    eSTR_LOG_BackupModeStop,
    eSTR_LOG_ResumeVentPushed,

    // Insert all new entries above this point

    eEmptyString,
    eLastString = eEmptyString,// eLastString is used to size the TextLibrary
    eNoError = eNoString
} E_StringID;

// EventCode Errors.
//      *** When adding new log entries, be sure to add to the end of a section.
//          DO NOT RE-ASSIGN ANY LOG ENTRY ID NUMBERS.
typedef enum
{
    eEmptyRecord = 0,
    eGUINotRespone = 70,        // Misc. anomaly, system reset event
    ePostBase = 1000,       // Vent Inop Conditions during Power On Self Test

    ePostInhAzTest,                 // 1001
    ePostExhAzTest,                 // 1002
    eConfigSoundError,              // 1003
    eChangePrivilegeError,          // 1004
    eAirFlowSensorCRCError,         // 1005
    eOxygenFlowSensorCRCError,      // 1006
    eAirFlowSensorModelError,       // 1007
    eOxygenFlowSensorModelError,    // 1008

    eSST_Base  = 2000,
    eSST_PassedCode = eSST_Base,    // 2000
    eEST_Base  = 3000,
    eEST_PassedCode = eEST_Base,    // 3000

    eCBITBase = 4000,
    eCbitAdcDacTestCode,            // 4001
    eCbitInhPAutoZeroCode,          // 4002
    eCbitExhPAutoZeroCode,          // 4003
    eFileSystemError,               // 4004

    eCbitAirValveOpenTestCode = 4008,      // 4008
    eCbitO2ValveOpenTestCode,       // 4009
    eCbitAirValveClosedTestCode,    // 4010
    eCbitO2ValveClosedTestCode,     // 4011

    eCbitAirMotorError = 4019,             // 4019
    eCbitO2MotorError,              // 4020
    eCbitExhMotorError,             // 4021
    eCbitHfoMotorError,             // 4022
    eCbitAirMotorLostCommunication,             // 4023
    eCbitO2MotorLostCommunication,              // 4024
    eCbitExhMotorLostCommunication,             // 4025
    eCbitHfoMotorLostCommunication,             // 4026

    eVentInop = 5000,
    eProxyBoardReset,               //9010 Proxy board reset
    eReadFileError,
    eWriteFileError,

    ecopyFileError,
    eReadTrendingFileError,//5005
    eReadTrendingFileErrorByTime,//5006

    eFrontPanelCommFailure = 6001,           // 6001

    ePowerSupplyPriviousFailed = 7001,
    eFlowSensorFailure = 9003,             // 9003
    eInhPressureSensorFailure,         // 9004
    eExhPressureSensorFailure,         // 9005
    ePlaySoundFailed,                   // 9006
    eClearProxyCalibrationFailed,
    eTaskTimeout,

	eNumOfEventCode
}E_EventCode;


typedef enum
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

    NUMOFSYSCONFIG//when adding a new typedef enum, we must add before NUMOFSYSCONFIG and after the previous of NUMOFSYSCONFIG
} E_ConfigIndex;

typedef enum
{
    eDiagnostics,
    eDataIO,
    eBd,
    ePanel,
    eErrHand,
    eAlarms,
    ePd,
    //    eBatteryTask,
    //    eCbit,
    eAzTask,
    eLastTask
} domainIDs;

typedef enum
{
    eLogHandlerVentSettingFileType,
    eLogHandlerResetVentSettingFileType,
    eLogHandlerSystemConfigFileType,
    eLogHandlerErrorLogFileType,
    eLogHandlerAlarmSettingLogFileType,
    eLogHandlerResetErrorLogFileType,
    eLogHandlerResetAlarmSettingLogFileType
} E_LogFileType;
typedef enum
{

    eNoEvent,

    eAirValveEvent,
    eO2ValveEvent,
    eExhValveEvent,
    eHfoValveEvent,
    eAllValveEvent, //for command ram to position 0
    eCommandEvent
} E_DiagnosticID;
// ********************* motor define typedef struct ***********************
typedef enum
{
    eIdleCmd,
    ePositionCmd,
    eRampCmd,
    eFlowControlCmd,
    eHFOStrokeVolumeCmd,
    eHFOFrequencyCmd,
    eHFOOffsetCmd
} E_MotorCommandType;

typedef struct
{
    E_DiagnosticID M_ID;
    E_MotorCommandType M_Type;
    USHORT M_Data;
} MotorEventData;

typedef  enum
{
    eNotACmd = 0,
    eDoExhAlignmentCmd,
    eProxyResetLineCmd,
    eInpedanceLineCmd,
    eSafetyValveCmd,
    eExhPressureZeroCmd,
    eInhPressureZeroCmd,
    eDebugPortCmd,
    ePurgeFlowCmd,
    eOilPumpCmd,
    eGateCmd,
} E_DiagnosticCommand;

typedef enum
{
    eStartStt,
    eOnStt = eStartStt,
    eStopStt,
    eOffStt = eStopStt
} E_CommandStatus;
typedef struct
{
    E_DiagnosticID ID;
    E_DiagnosticCommand Cmd;
    E_CommandStatus Stt;
} CommandData;

typedef struct
{
    E_DiagnosticID ID;
} BasicEvent;

typedef union
{
    //Used to HardwareDiagnostic
    BasicEvent basicDiagnosticEvent;
    CommandData Command;
    MotorEventData Motor;
} DiagnosticEventUnion;
#define TRUE true
#define FALSE false

typedef enum  {
    eNoBdEventId,               // place holder; not a valid event.

    eFirstEventId,
    eFirstSettingChangeId = eFirstEventId,

    // Batch Setting Change Events
    ePressureBatchChangeId = eFirstSettingChangeId,
    eCommonBatchChangeId,

    // Insert new batch setting change values here

    // Single Setting Change Events
    eFirstSingleSettingChangeId,
    eModeChangeId = eFirstSingleSettingChangeId,
    eTriggerTypeChangeId,
    ePressSensChangeId,
    eFlowSensChangeId,
    eBaseFlowChangeId,
    ePctO2ChangeId,
    eFlushPctO2ChangeID,
    eRrChangeId,//Breath Rate
    eNIVRrChangeId,
    eSupportPressChangeId,
    eRiseTimeChangeId,
    eExhSensChangeId,
    eBaselinePressChangeId,
    eInhTimeChangeId,
    ePcvInhPressChangeId,

    eAPRVLowInhPressChangeId,
    eAPRVHighInhPressChangeId,

    eAPRVTimeHighChangeId,
    eAPRVTimeLowChangeId,

    eHipLimitChangeId,
    eLipLimitChangeId,
    eLowBaselineLimitChangeId,
    eHrrLimitChangeId,
    eApneaIntChangeId,
    eExtremeHighPEEPAlarm_AutosetChangedId,
    eExtremeHighInspAlarm_AutosetChangedId,

    eHFOFrequencyChangeId,
    eHFOStrokeVolumeChangeId,
    eHFOMapChangeId,
    eFreshGasChangeId,
    eLowMAPLimitChangeId,
    eHiMAP_1LimitChangeId,
    eHiMAP_2LimitChangeId,
    eExtremeHighMapAlarm_AutosetChangedId,
    eLowAmplitudeLimitChangeId,
    eHighAmplitudeLimitChangeId,
    eLowAmplitude_AutosetChangedID,
    eHighAmplitude_AutosetChangedID,
    eSIChangeId,

    eHFOAmplitudeChangeID,
    eHFOSITimeChangeID,
    eHFOSIRateChangeID,
    eHFOAmpControlChangeID,
    eHFOAutoSIChangeID,

    eTidalVolumeChangeID,
    eInspFlowChangeID,
    eWaveFormChangeID,
    ePlateauTimeChangeID,

    eNasalCPAPFlowChangeID,

    eBackupBRChangeID,
    eSyncPlusChangeID,
    eTubeDiameterChangeID,
    eTubeLengthChangeID,
    eCompRateChangeID,

    eTiLimitChangeId,
    eHighNCPAPLimitChangeId,
    eLowNCPAPLimitChangeId,

    eAPRVHighPHighLimitChangeId,
    eAPRVLowPHighLimitChangeId,
    eAPRVHighPLowLimitChangeId,
    eAPRVLowPLowLimitChangeId,

    eHighTidalVolLimitChangeId,
    eLowTidalVolLimitChangeId,

    eHighTidalVolAutoSetChangeId,
    eLowTidalVolAutoSetChangeId,
    eAPRVExtremeHighPHighAutoSetChangeId,

    eProximalLeakLimitChangeId,

    eHighBaselineLimitChangeId,
    eHpsvLimitChangeId,
    eLpsvLimitChangeId,


    eLemtvLimitChangeId,
    eLestvLimitChangeId,
    eHemvLimitChangeId,
    eLemvLimitChangeId,

    // Insert new single setting change values here
    eLastSingleSettingChangeId = eLemvLimitChangeId,
    eLastSettingChangeId = eLastSingleSettingChangeId,
    // First Operator Request event
    eFirstOpRequestId,
    eAlarmResetId = eFirstOpRequestId,
    eManualHoldId,
    eManualInhId,
    e100PercentO2Id,
    // Insert new operator request events here
    eStandbyModeId,
    eMEModeId,
    eSTModeId,

    eDoAutoSetId,

    eShutdownRequestId,

    eStartOilPumpId,
    eLastOpRequestId = eStartOilPumpId,

    eLastBdEventId = eLastOpRequestId

}E_BdEventId;

// The E_BdSettingId typedef enumeration is a list of BD settings.  These values
// are used by MgrBdSetting and any other object that wants to access a
// setting.
typedef enum  {

    eMode = (eModeChangeId - eFirstSingleSettingChangeId),
    eTriggerType = (eTriggerTypeChangeId - eFirstSingleSettingChangeId),
    ePressSens = (ePressSensChangeId - eFirstSingleSettingChangeId),
    eFlowSens = (eFlowSensChangeId - eFirstSingleSettingChangeId),
    eBaseFlow = (eBaseFlowChangeId - eFirstSingleSettingChangeId),
    ePctO2 = (ePctO2ChangeId - eFirstSingleSettingChangeId),
    eFlushPctO2 = (eFlushPctO2ChangeID - eFirstSingleSettingChangeId),
    eRr = (eRrChangeId - eFirstSingleSettingChangeId),
    eNIVRr = (eNIVRrChangeId - eFirstSingleSettingChangeId),

    eSupportPress = (eSupportPressChangeId - eFirstSingleSettingChangeId),
    eRiseTime = (eRiseTimeChangeId - eFirstSingleSettingChangeId),
    eExhSens = (eExhSensChangeId - eFirstSingleSettingChangeId),
    eBaselinePress = (eBaselinePressChangeId - eFirstSingleSettingChangeId),
    eInhTime = (eInhTimeChangeId - eFirstSingleSettingChangeId),
    ePcvInhPress = (ePcvInhPressChangeId - eFirstSingleSettingChangeId),

    eAPRVLowInhPress = (eAPRVLowInhPressChangeId - eFirstSingleSettingChangeId),
    eAPRVHighInhPress = (eAPRVHighInhPressChangeId - eFirstSingleSettingChangeId),

    eAPRVTimeHigh = (eAPRVTimeHighChangeId - eFirstSingleSettingChangeId),
    eAPRVTimeLow = (eAPRVTimeLowChangeId - eFirstSingleSettingChangeId),

    eHipLimit = (eHipLimitChangeId - eFirstSingleSettingChangeId),
    eLipLimit = (eLipLimitChangeId - eFirstSingleSettingChangeId),
    eLowBaselineLimit = (eLowBaselineLimitChangeId - eFirstSingleSettingChangeId),

    eHrrLimit = (eHrrLimitChangeId - eFirstSingleSettingChangeId),
    eApneaLimit = (eApneaIntChangeId - eFirstSingleSettingChangeId),

    eExtremeHighPEEP_Autoset = (eExtremeHighPEEPAlarm_AutosetChangedId - eFirstSingleSettingChangeId),
    eExtremeHighInsp_Autoset = (eExtremeHighInspAlarm_AutosetChangedId - eFirstSingleSettingChangeId),

    //#ifdef HFO_SYSTEM
    eHFOFrequency = (eHFOFrequencyChangeId - eFirstSingleSettingChangeId),
    eHFOStrokeVolume = (eHFOStrokeVolumeChangeId - eFirstSingleSettingChangeId),
    eHFOMap          = (eHFOMapChangeId - eFirstSingleSettingChangeId),
    eFreshGas = (eFreshGasChangeId - eFirstSingleSettingChangeId),
    eLowMAPLimit = (eLowMAPLimitChangeId - eFirstSingleSettingChangeId),
    eHiMAP_1Limit = (eHiMAP_1LimitChangeId - eFirstSingleSettingChangeId),
    eHiMAP_2Limit = (eHiMAP_2LimitChangeId - eFirstSingleSettingChangeId),
    eExtremeHighMap_Autoset = (eExtremeHighMapAlarm_AutosetChangedId - eFirstSingleSettingChangeId),

    eLowAmplitudeLimit = (eLowAmplitudeLimitChangeId - eFirstSingleSettingChangeId),
    eHighAmplitudeLimit = (eHighAmplitudeLimitChangeId - eFirstSingleSettingChangeId),
    eLowAmplitude_Autoset = (eLowAmplitude_AutosetChangedID - eFirstSingleSettingChangeId),
    eHighAmplitude_Autoset = (eHighAmplitude_AutosetChangedID - eFirstSingleSettingChangeId),

    eSI = (eSIChangeId - eFirstSingleSettingChangeId),

    eAmplitude = (eHFOAmplitudeChangeID - eFirstSingleSettingChangeId),
    eSITime = (eHFOSITimeChangeID - eFirstSingleSettingChangeId),
    eSIRate = (eHFOSIRateChangeID - eFirstSingleSettingChangeId),
    eAmplitudeControl = (eHFOAmpControlChangeID - eFirstSingleSettingChangeId),
    eAutoSI = (eHFOAutoSIChangeID - eFirstSingleSettingChangeId),

    eTidalVolume = (eTidalVolumeChangeID - eFirstSingleSettingChangeId),
    eInspFlow = (eInspFlowChangeID - eFirstSingleSettingChangeId),
    eWaveForm = (eWaveFormChangeID - eFirstSingleSettingChangeId),
    ePlateauTime = (ePlateauTimeChangeID - eFirstSingleSettingChangeId),
    eBackupBR = (eBackupBRChangeID - eFirstSingleSettingChangeId),
    eSyncPlus = (eSyncPlusChangeID - eFirstSingleSettingChangeId),
    eTubeDiameter = (eTubeDiameterChangeID - eFirstSingleSettingChangeId),
    eTubeLength = (eTubeLengthChangeID - eFirstSingleSettingChangeId),
    eCompRate = (eCompRateChangeID - eFirstSingleSettingChangeId),

    eNasalCPAPFlow = (eNasalCPAPFlowChangeID -eFirstSingleSettingChangeId),
    eTiLimit = (eTiLimitChangeId - eFirstSingleSettingChangeId),
    eHighNCPAPLimit = (eHighNCPAPLimitChangeId - eFirstSingleSettingChangeId),
    eLowNCPAPLimit = (eLowNCPAPLimitChangeId - eFirstSingleSettingChangeId),

    eAPRVHighPHighLimit = (eAPRVHighPHighLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVLowPHighLimit = (eAPRVLowPHighLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVHighPLowLimit = (eAPRVHighPLowLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVLowPLowLimit = (eAPRVLowPLowLimitChangeId - eFirstSingleSettingChangeId),
    eHighTidalVolLimit = (eHighTidalVolLimitChangeId - eFirstSingleSettingChangeId),
    eLowTidalVolLimit = (eLowTidalVolLimitChangeId - eFirstSingleSettingChangeId),

    eProximalLeakLimit = (eProximalLeakLimitChangeId - eFirstSingleSettingChangeId),

    eHighTidalVolAutoSet = (eHighTidalVolAutoSetChangeId - eFirstSingleSettingChangeId),
    eLowTidalVolAutoSet = (eLowTidalVolAutoSetChangeId - eFirstSingleSettingChangeId),

    eAPRVExtremeHighPHighAutoSet = (eAPRVExtremeHighPHighAutoSetChangeId - eFirstSingleSettingChangeId),

    //#endif
    // ADD OPERATOR SETTABLE VALUES HERE
    eHighBaselineLimit = (eHighBaselineLimitChangeId - eFirstSingleSettingChangeId),
    eHpsvLimit = (eHpsvLimitChangeId - eFirstSingleSettingChangeId),
    eLpsvLimit = (eLpsvLimitChangeId - eFirstSingleSettingChangeId),

    eLemtvLimit = (eLemtvLimitChangeId - eFirstSingleSettingChangeId),	//
    eLestvLimit = (eLestvLimitChangeId - eFirstSingleSettingChangeId),	//
    eHemvLimit = (eHemvLimitChangeId - eFirstSingleSettingChangeId),
    eLemvLimit = (eLemvLimitChangeId - eFirstSingleSettingChangeId),

    // Values retrieved from NVRAM
    eComplFactor = (eLastSingleSettingChangeId - eFirstSingleSettingChangeId + 1),
    eComplOnOff,

    // This dummy setting is never changed by the operator and is used during
    // NIPPV since NIPPV inhalations are not offset from true baseline, they
    // are offset from 0; the dummy Baseline is always 0.
    eDummyBaselinePress,

    // This value must follow all of the settings
    eNumBdSettings

}E_BdSettingId;

// E_PhaseId is used to define the general identifiers associated with breath
// phases.  For most of the BD Application subsystem, it is only important to
// know what the general phase ID is.
typedef enum
{
    eInitPhase,
    eNonVentPhase,
    eDiagnosticPhase,
    eSelfTestPhase,
    eMandInhPhase,

    eSpontInhPhase,
    eSpontInhPhase_InAPRV_PLow,
    eSpontInhPhase_InAPRV_PHigh,
    ePlateauPhase,

    eMandInhTestBreathPhase,
    eMandInhVABreathPhase,

#ifdef HFO_SYSTEM
    eHFOPhase,
#endif
    eExhPhase,
    eAPRVLowPhase,
    eAPRVHighPhase,
    eAPRVExhPhase_InPHigh,
    eAPRVExhPhase_InPLow,

#ifdef  VA_WITH_PS
    eNCPAPPhase,
    ePsvInhVABreathPhase //for new VA
#else
    eNCPAPPhase
#endif
} E_PhaseId;

typedef enum
{
    eGeneral,
    eAlarm
} E_NotificationCategory;

typedef enum
{
    eSettingType,
    eAlarmHistoryType
} SettingAlarmHistoryType;
typedef enum
{
    eGraphicsTrendDataType,
    eGraphicsTrendResetType
} E_GraphicsTrendType;

typedef enum
{
    // diagnostic setting IDs
    eAirValveSettingID,
    eAirStepUnitsSettingID,
    eAirLpmUnitsSettingID,

    eOxygenValveSettingID,
    eOxygenStepUnitsSettingID,
    eOxygenLpmUnitsSettingID,

    eExhalationValveSettingID,

    eHfoValveSettingID,
    eHfoFreqSettingID,
    eHfoStrokeVolSettingID,
    eHfoOffsetPositionSettingID,

    eMonitorsSettingID,
    eVoltageWrapSettingID,
    eBlowerSettingID,
    eMonthSettingID,
    eDaySettingID,
    eYearSettingID,
    eHourSettingID,
    eMinuteSettingID,
    eSecondSettingID,
    eAltitudeSettingID,
    eNoSettingChangedID             // for Views.h
} E_SettingChanged;

typedef enum
{
    eButtonReleased,
    eButtonPressed
} E_ButtonState;



typedef enum
{
    eNoDirection,
    eEncoderDirectionCw, // left
    eEncoderDirectionCcw //right
}E_EncoderDir;



typedef enum
{
    eGreaterThan,
    eGreaterThanOrEqual,
    eLessThan,
    eLessThanOrEqual,
    eRange,
    eEqual
} E_ConstraintType;

typedef enum  {
    eMetranMonitor,
    ePhillipMonitor,
    eGEMonitor,
    eDisableMinotor
} E_Monitor;

typedef enum
{
    eAcceptResponse,
    eRejectResponse,
    eRetestResponse,
    eContinueResponse,

    eWaitingResponse,

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
} E_ResponseType;

// Diagnostic event typedef enumeration
typedef enum
{
    // events from client (used to command self tests)
    eStartOxygenCalibration,
    eStartSSTEvent,
    eStartESTEvent,
    eCancelTestEvent,
    eSSTChildCompleteEvent,
    eESTChildCompleteEvent,
    eFailureEvent,
    eTestResponseEvent,
    eContextSwitchEvent,
    eHardwareTimerExpiredEvent,
    eOutputChangeEvent,
    eDiagCodesResponseEvent
}E_DiagnosticEvent;


typedef enum
{
    eGetProxyStatus,
    eGetProxyVersion,
    eDoProxyCalibration,
    eClearProxyCalibration,
    eComfirmProxyReset
} ProxyActions;

typedef enum
{
    ePipTrendingFileType,
    ePeepTrendingFileType,
    eIeRatioTrendingFileType,
    eTotalBrTrendingFileType,
    eInspVtTrendingFileType,
    eMapTrendingFileType,
    ePlateauPressTrendingFileType,
    eSpontBrTrendingFileType,
    eExpVtTrendingFileType,

    eInspMvTrendingFileType,
    eProxLeakTrendingFileType,
    eAprvPhighTrendingFileType,
    eNcpapTrendingFileType,
    eExpMvTrendingFileType,
    eCompTrendingFileType,
    eAprvPlowTrendingFileType,

    eHfoAmpTrendingFileType,
    eHfoVtTrendingFileType,
    eMonitoredO2TrendingFileType,
    eHfoMapTrendingFileType,
    eHfoBaseFlowTrendingFileType,

    eResetAllTrendingFileType,
    eCheckOneWeekLimitType,
    eInsertMissingTrendDataType
} E_TrendingFileType;

// IMPORTANT: Existing failure code numberings should not change. These codes
// are displayed to the user if a failure occurs and may be referenced in
// the maintenance manual. Keeping a failure code for the duration of the
// product prevents manual changes and customer confusion regarding the
// failure code meanings.
//
// When making a new diagnostic failure code follow these guidelines:
// 1) All failure codes start with eFC_.
// 2) The first part of the name should be the device.
// 3) The second half should be the problem.
// 4) Any new codes need to create a unique number. Do not let the compiler
//    auto assign a value, explicitly create each new number.
// 5) If a code is no longer needed, comment it out. Commenting keeps it around
//    as a a reminder not to reassign that number in the future.
//
// Naming example:
//  eFC_InhValveStuck - 1) eFC_ is there, 2) the device in question is the
//      inhalation valve, and 3) the problem is it is stuck.

typedef enum
{
    eFC_None =                                      0,
    eFC_TestCanceledByUser =                        100,
    eFC_AirStepperOutsideRange =                    101,
    eFC_O2StepperOutsideRange =                     102,
    eFC_AirFlowOutsideRange =                       103,
    eFC_O2FlowOutsideRange =                        104,
    eFC_PatientCircuitLeak =                        105,
    eFC_InhPressureTooLow =                         106,
    eFC_ExhPressureOutsideRange =                   107,
    eFC_OxygenSourceDisconnected =                  108,
    eFC_O2NotDisconnected =                         109,
    eFC_O2Sensor100PercentSampleRange =             110,
    eFC_O2Sensor100PercentAvgRange =                111,
    eFC_ReliefValveCrackFlowPressureTooHigh =       112,
    eFC_ReliefValveCrackFlowPressureNotStable =     113,
    eFC_ReliefValveCrackFlowPressureOutsideRange =  114,
    eFC_O2FlowCrackFlowOutsideRange =               115,
    eFC_DifferenceInhVsExh =                        116,
    eFC_CircuitComplianceRange =                    117,
    eFC_PressureLeakRange =                         118,
    eFC_SafeyValveCannotOpen =                      119,
    eFC_PatientWyeNotBlocked =                      120,
    eFC_InhAutoZeroSolenoidCannotOpen =             121,
    eFC_ExhAutoZeroSolenoidCannotOpen =             122,
    eFC_AirStepperPositionOpenOutsideRange =        123,
    eFC_AirStepperPositionMidOutsideRange =         124,
    eFC_AirStepperPositionCloseOutsideRange =       125,
    eFC_O2StepperPositionOpenOutsideRange =         126,
    eFC_O2StepperPositionMidOutsideRange =          127,
    eFC_O2StepperPositionCloseOutsideRange =        128,
    eFC_ExhStepperPositionOpenOutsideRange =        129,
    eFC_ExhStepperPositionMidOutsideRange =         130,
    eFC_ExhStepperPositionCloseOutsideRange =       131,
    eFC_PatientWyeNotUnblocked =                    132,
    eFC_InhAutoZeroFailure =                        133,
    eFC_ExhAutoZeroFailure =                        134,
    eFC_AirValveLiftOffTestFailed =                 135,
    eFC_O2ValveLiftOffTestFailed =                  136,
    eFC_OxygenSensorDisconnected =                  137,
    eFC_PatientCircuitKinked =                      138

} E_FailureCodes;


typedef enum
{
    eOxygenCalibrationEvent,
    eTestStartedEvent,
    eTestRespondEvent,
    eRetestEvent,
    eShowResultEvent,
    eClearLastTestLineEvent,
    eTestCompletedEvent
} E_SelfTestID;

typedef enum
{
    eAirValve,
    eO2Valve,
    eExhValve,
    eHfoValve
} E_ValveID;

typedef enum
{
    eAirFlowController,
    eO2FlowController
} E_FlowControllerId;

// The E_TestStatus defines a test status.  It contains values for check
// status as well as overall test status.
//
// These must match values defined in ramcell.s
typedef enum
{
    eCheckFailed = 0,
    eCheckPassed = 1,
    eTestPassed = eCheckPassed,
    eTestFailed = 2
} E_TestStatus;

typedef enum
{
    eExhPressureZeroId,
    eInhPressureZeroId,
    eSafetyValveId,
    eImpedanceLineId,
    ePurgeFlowId,
    eOilPumpId,
    eRemoteAlarmId,
    eEnableIOId,
    eProxyFlowResetLineId
} E_DiscreteOutputId;

typedef struct
{
    UNSIGNED mtype;
    char mtext[1];
} msg_buf;
#ifdef COMMON_TYPES_A53
typedef struct FileTaskEvent
{
    int infoID;
    QString info;

    FileTaskEvent& operator = (FileTaskEvent tmpEvent)
    {
        infoID = tmpEvent.infoID;
        info = tmpEvent.info;
        return *this;
    }
};

 EventCode Errors.
class AlarmSettingLogEntry
{
public:
    AlarmSettingLogEntry()
    {
        type = eGeneral;//General or Alarm
        status = eInactive;
        extraMsg = QString::null;
        dateTimeStr = QString::null;
        strID = eNoString;
    }

    E_NotificationCategory type;//General or Alarm
    E_AlarmLevel status;
    QString extraMsg;
    QString dateTimeStr;
    E_StringID strID;//used for translation

    AlarmSettingLogEntry & operator=(const AlarmSettingLogEntry &entry)
    {
        type = entry.type;
        status = entry.status;
        extraMsg = entry.extraMsg;
        dateTimeStr = entry.dateTimeStr;
        strID = entry.strID;
        return *this;
    }
};
typedef struct PanelInData
{
    char value; //data of button or encoder: in encoder value can be positive or negative

    PanelInData& operator = (PanelInData tmpEvent)
    {
        value = tmpEvent.value;
        return *this;
    }
};
typedef struct DiagnosticEventData
{
    LONG data1;  //respond data, failured code and test status(passed/failed)
    LONG data2;  //test name and retest
    DiagnosticEventData& operator = (DiagnosticEventData tmpEvent)
    {
        data1 = tmpEvent.data1;
        data2 = tmpEvent.data2;
        return *this;
    }
};
typedef struct SelfTestEventData
{
    E_StringID StringID;
    USHORT Code;
    E_ConstraintType Constraint;
    USHORT Scale;
    LONG TestResult;
    LONG Lower;
    LONG Upper;

    SelfTestEventData& operator = (SelfTestEventData tmpEvent)
    {
        StringID = tmpEvent.StringID;
        Code = tmpEvent.Code;
        Constraint = tmpEvent.Constraint;
        Scale = tmpEvent.Scale;
        TestResult = tmpEvent.TestResult;
        Lower = tmpEvent.Lower;
        Upper = tmpEvent.Upper;

        return *this;
    }
};
#endif

#endif /* UTIL_COMMONTYPES_H_ */
