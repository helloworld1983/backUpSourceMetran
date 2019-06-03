/******************************************************************************/
//$COMMON.H$
//   File Name:  BdInterface.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains type definitions used by objects that interface
//      with Breath Delivery.
/******************************************************************************/
#ifndef BDINTERFACE_H
#define BDINTERFACE_H
#include "CommonTypes.h"
#include "CommonConstants.h"

// The following enumeration specifies the events that can be placed on the 
// BD Queue.
typedef enum {
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

//    //lam
//    eHighFiO2LimitChangeId,
//    eLowFiO2LimitChangeId,
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

// This enumeration specifies opeator request commands.  An operator request
// is either a request to start or stop.
enum E_OpRequestCommand
{
    eStart,
    eStop,
    eAutoStart,
    eAutoStop,
    eUnblock,

    eStrokVolumeChange,
    eFreqChange,
    eMapChange,
    eAmpChange,
    eSuspend,
};

// The BdBasicEvent structure is used by the BD Task when pulling events off
// its queue before it knows what the event is.
struct BdBasicEvent
{
        E_BdEventId Id;

};

// The operator request event contains the ID and the start/stop command.
struct BdOpRequestEvent
{
        E_BdEventId Id;
        E_OpRequestCommand Command;

};

// A single setting change event contains the ID and the new setting value.
struct BdSingleSettingChangeEvent
{
        E_BdEventId Id;
        LONG Value;
};

// This structure defines settings that are specific to the Volume 
// Ventilation Type.
struct VolumeSpecificSettings
{
        // VCV specific parameters
        E_FlowPattern FlowPattern;
        LONG PeakFlow;
        LONG TidalVol;
        SHORT Plateau;

        // Alarm Specific parameters
        LONG LemtvLimit;
        LONG LestvLimit;
        SHORT HipLimit;

        // PSV specific parameters
        SHORT SupportPressure;

        SHORT PressureSens;
};

// This structure defines settings that are specific to the Pressure 
// Ventilation Type.
typedef struct
{

//    E_VentilationMode Mode;
    SHORT BackupBR;
    SHORT TubeDiameter;
    SHORT TubeLength;
    SHORT CompRate;//Compensation Rate

    // PCV Specific parameters
    E_TriggerType TriggerType;
    E_SwitchType SyncPlusType;
    SHORT InhPress;
    SHORT InhTime;
    SHORT RiseTime;
    SHORT BaselinePress;
    SHORT RespRate;
    SHORT NIVRespRate;
    SHORT ExhFlow;//Base Flow

    //VCV
    SHORT TidalVolume;
    SHORT InhFlow;//Inspiratory Flow
    E_FlowPattern WaveForm;
    SHORT PlateauTime;

    // PSV specific parameters
    SHORT PressureSens;
    LONG FlowSens;
    SHORT SupportPressure;
    SHORT ExhSens;

    // Alarm Specific parameters
    SHORT HipLimit;
    SHORT LipLimit;
    SHORT HighBaselineLimit;
    SHORT LowBaselineLimit;
    LONG HemvLimit;
    LONG LemvLimit;
    LONG LemtvLimit;
    LONG LestvLimit;
    SHORT HrrLimit;
    SHORT LpsvLimit;
    LONG HpsvLimit;
    LONG eApneaLimit;
    LONG ExtremeHighPEEPAutoset;
    LONG ExtremeHighInspAutoset;
    SHORT TiLimit;//Spont time too long
    LONG HighTidalVolume;
    LONG LowTidalVolume;
    LONG HighTidalVolAutoSet;
    LONG LowTidalVolAutoSet;
    LONG ProximalLeak;

    // HFO specific parameters
    SHORT Map;
    SHORT StrokeVolume;
    SHORT Frequency;
    SHORT FreshGas;
    SHORT SI;
    SHORT HiMAP_1Alarm;
    SHORT HiMAP_2Alarm;
    SHORT LowMAPAlarm;
    SHORT LowAmplitudeLimit;
    SHORT HighAmplitudeLimit;
    SHORT HighAmplitudeAutoset;
    SHORT LowAmplitudeAutoset;
    SHORT ExtremeHighMapAutoset;
    SHORT Amplitude;
    E_SwitchType AMPControlType;
    E_SwitchType AutoSIType;
    SHORT SITime;
    SHORT SIRate;

    //APRV
    SHORT APRVLowInhPress;
    SHORT APRVHighInhPress;
    SHORT APRVTimeLow;
    SHORT APRVTimeHigh;
    LONG APRVHighPHigh;
    LONG APRVLowPHigh;
    LONG APRVHighPLow;
    LONG APRVLowPLow;
    SHORT APRVExHighPHigh;

    //N-CPAP
    SHORT NCPAPFlow;
    SHORT HighNCPAP;
    SHORT LowNCPAP;

}PressureSpecificSettings;

// This structure defines settings that are specific to the Non-Invasive 
// Ventilation Type.
struct NippvSpecificSettings
{
        // NIPPV specific parameters
        SHORT Ipap;
        SHORT InhTime;

        // Alarm Specific parameters
        LONG LetvLimit;

};
typedef struct
{

    SHORT PctO2;
    SHORT FlushPctO2;
    E_Bool ComplianceOnOff;

}BatchCommonSettingsChangeEvent;

// A batch setting event is made up of the ID, all settings that are common
// to Pressure, Volume and NIPPPV and the settings specific to the requested
// ventilation type.
typedef struct
{
    E_BdEventId Id;

    // A Batch setting request contains one of the specific set of settings.
    union
    {
        BatchCommonSettingsChangeEvent CommonSettings;
        PressureSpecificSettings PressureSettings;

    }   VentTypeSettings;


}BdBatchSettingsChangeEvent;

// A BdEvent is either an operator request, a single or batch setting change.
union BdEvent
{
        BdBasicEvent BasicEvent;
        BdOpRequestEvent OpRequestEvent;
        BdSingleSettingChangeEvent SingleSettingChangeEvent;
        BdBatchSettingsChangeEvent BatchSettingsChangeEvent;
};

// This value defines the maximum message size for a message on the BD Queue.
// The size is specified in the number of UNSIGNED data elements.  
// If the message is not an exact multiple of an UNSIGNED, it is rounded up
// to the next size.  
const UNSIGNED BD_Q_MSG_SIZE = (sizeof(BdEvent) + sizeof(UNSIGNED) - 1)
        / sizeof(UNSIGNED);

// The destination for any event pulled off of the BD queue and the sourse
// of anything placed onto the queue must be able to hold something the size 
// of BD_Q_MSG_SIZE.  
//
// If the BdEvent structure is not evenly divisible by UNSIGNED, it can be 1, 2,
// or 3 bytes smaller than BD_Q_MSG_SIZE.  To prevent data corruption
// when an event is received from the BD Queue, the union is defined
// with an array that is the maximum message size.
//
// Whenever any Task is placing events ON the BD Queue or receiving from the
// BD Queue, it should use something of type BdQEntry.
union BdQEntry
{
        BdEvent Event;
        UNSIGNED EventWithPadding[BD_Q_MSG_SIZE];
};

enum E_DeviceId
{
    eNoDeviceId = 0,

    // HMV func panel, ...
    ePowerButton,
    eO2_100percentButton,
    eManualSighButton,
    ePanelLockButton,
    eAlarmMuteButton,
    eMainEncoderButton,
    eMainEncoder,

    //edited by phuoc
    ePICVersion,
    ePICReleaseTime,

    // Flow Sensors
    eAirFlowSensor,
    eO2FlowSensor,
    eExhFlowSensor,
    eExhProxyFlowSensor,
    eInhProxyFlowSensor,

    // Pressure Sensors
    eInhPressSensor,
    eExhPressSensor,

    // Misc. Sensors
    eFiO2Sensor,
    eBatteryVoltage,
    eBatteryCurrent,
    eADCWrapAI,
    eEnclosureTemp,
    eInternalO2,

    // Discrete Inputs
    ePowerSource0,
    ePowerSource1,
    eFanAlarm,
    eBatChargingStatus,
    eA2DOutOfRange,
    eOnOffSwitch,
    ePowerFailInd,
    eAlarmWrapDI,
    eAlarmFlick,
    eKnobCarry,
    eKnobBorrow,
    eSoftwareJumper,
    eExhHoldKey,
    eExhHoldKeyOff,
    eO2Pressure,
    eO2PressureOff,
    e100PercentO2,
    e100PercentO2Off,
    eAirPressure,
    eAirPressureOff,
    eBackupAirPressure,
    eBackupAirPressureOff,
    eHFODoorSwitch,
    eHFOPistonSwitch,
    eManualBreathInput,
    eManualBreathConnection,
    eO2MotorStatus,
    eAirMotorStatus,
    eExhMotorStatus,
    eHFOMotorStatus,

    // Control Panel Keys
    eButtonSelectKey,
    eCancelKey,
    eManInspKey,
#ifdef HFO_SYSTEM
    eManHoldInspKeyUp,    // virtual key
    eManHoldInspKeyDown,    // virtual key
    eAutoSIOn,
    eAutoSIOff,
    eDelayRequest,
    eAmpChangeRequest,
    eStopAutoSetRequest,
    eDoAutoSetRequest,
    eSVChangeRequest,
    eSuspendAutoSetRequest,
    eFreqChangeRequest,
    eMapChangeRequest,
#endif

    eAlarmResetKey,     // virtual key
    eAlarmSilenceKey,
    eAcceptKey,
    eUpKey,
    eUpDownKey,
    eDownKey,
    eVCVModeKey,
    ePCVModeKey,
    eOilPumpRequestKey,
    eShutdownRequestKey,
    // Discrete Outputs
    eProxyFlowResetLine,
    ePurgeFlow,
    eOilPump,
    eAirPump,
    eBigLED,
    eHeatedFilter,
    eInpedanceLine,
    eProxiPressure,
    eDebugPort,
    eSafetyValve,
    eExhPressZero,
    eInhPressZero,
    eRemoteAlarm,
    eIOEnable,
    eNebulizer,
    eAirNebulizer,
    eO2Nebulizer,
    eExternalNebulizer,
    eSVCrossover,
    eGate,         // enables Timer1 and Timer2 of 71054
    ePOSTTimer,
    eRunACPower,
    eRunExternalBattery,
    eCrossoverPilot,

    // LEDs
    eNormalLED,
    eHighAlarmLED,
    eMedAlarmLED,
    eVentInopLED,
    eAlarmSilenceLED,
    eAllO2LED,
    eScreenLockLED,
    eExternBatteryLED,
    eInternalBatteryLED,

    // Analog Outputs
    eFlowRateAO,
    eVolumeAO,
    ePressureAO,
    eO2PressZeroRefAO,
    eVoltageWrapAO,
    eSpare1AO
};

#endif // BDINTERFACE_H
