/*
 * IpcInterface.h
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */

#ifndef IPC_IPCINTERFACE_H_
#define IPC_IPCINTERFACE_H_
#include <stdbool.h>
#include "standard.h"

#define STX 0xaa
#define ETX 0xff

#ifndef CORE_A53
#define LOWBYTE(v)   ((unsigned char) (v))
#define HIGHBYTE(v)  ((unsigned char) (((unsigned int) (v)) >> 8))
#endif


#ifndef CORE_A53
typedef enum
{
    e9600,
    e19200,
    e38400,
    e57600
} E_BaudRate;

typedef enum
{
    eAirValveId,
    eO2ValveId,
    eExhValveId,
    eHfoValveId
} E_MotorId;

typedef enum
{
    eGetProxyStatus = 0xA5,
    eGetProxyVersion,
    eDoProxyCalibration,
    eClearProxyCalibration,
    eComfirmProxyReset
} ProxyActions;

typedef enum
{
    eMetranMonitor = 0x0F,
    ePhillipMonitor,
    eGEMonitor,
    eDisableMinotor
} E_Monitor;

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

typedef enum
{
    eProxyFlowResetLineId,
    eInpedanceLineId,
    eSafetyValveId,
    eExhPressureZeroId,
    eInhPressureZeroId,
    ePurgeFlowId,
    eOilPumpId,
    eEnableIOId,
    eRemoteAlarmId
} E_DiscreteOutputId;

typedef enum
{
  eManualSIType,
  eAutoSIType
} E_SIType;

typedef enum {
    eNotCalibrated = 0x55,
    eCalibrated,
    eResetAndRecovered,
    eSensorError
} ProxyStatus;

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

typedef enum
{
    eOff,
    eOpen = eOff,   //eOpen and eClose just use for safety valve
    eOn,
    eClose = eOn,
    eBlinking,
    eInUse,             // For InternalBatteryLED only
    eCharging,          // For InternalBatteryLED only
    eLowChg,            // For InternalBatteryLED only
    eVeryLowChg,        // For InternalBatteryLED only
    eUnknown
} E_State;

#endif

typedef enum
{
    // There is only 1 Plateau and Exhalation Pause Phase.
    eThePlateauPhase,
    // Specific HFO Phases
    eTheHFOPhase,

    // Specific Mandatory Phases
    eVcvInhPhase,
    ePcvInhPhase,

    eTheAPRVLowPhase,
    eTheAPRVHighPhase,
    eTheAPRVExhPhase_InPHigh,
    eTheAPRVExhPhase_InPLow,


    eTheMandInhTestBreathPhase,
    eTheMandInhVABreathPhase,

    // Specific Spontaneous Phases
    ePsvInhPhase,
    ePsvInhPhase_InAPRV_PLow,
    ePsvInhPhase_InAPRV_PHigh,

    // Specific Exhalation Phases
    eFlowTrigExhPhase,
    ePressTrigExhPhase,
    eNoTrigExhPhase,

    // Specific None Vent Phases
    eTheNonePhase,

    eTheDiagPhase,

    eTheSelfTestPhase,

    eTheNCAPPhase,

    eThePsvVAInhPhase, //for new VA

    // Total number of specific breath phases.  This entry must remain as the
    // last one in the list.
    eNumSpecificPhases
} E_SpecificPhaseId;

//#endif

typedef enum
{
    eNone,
    eSingleSetting,
    eHfoSetting,
    ePcvSetting,
    eVcvSetting,
    eRealTime,
    //Command id
    eSetInpedanceLineState,
    eSetInpedanceLineStateDirect,
    eSetInpedanceLineDesiredState,
    eSetSafetyValveDesiredState,
    eSetSafetyValveStateDirect,
    eCMVServoEnable,
    eCMVServoDisable,
    eSetCMVBiasFlow,
    eSetCMVInhalationTarget,
    eSetCMVExhlationTarget,
    eCMVResetIMVStaticVariables,
    eCMVCompCompensatePress,
    eVCVServoEnable,
    eVCVServoDisable,
    eVCVSetSpecificPhase,
    eVCVSetBiasFlow,
    eVCVSetExhalationTarget,
    eVCVSetInhalationTarget,
    eVCVResetIMVStaticVariables,
    eVCVSetDesiredFlow,
    eCommunicateProxySensor,
    eSetPrimaryActivePress,
    eFio2ReadVoltageValue,
    eFio2AddCounts,
    eSetProxyFlowResetLineStateDirect,
    eSetProxyFlowResetLineDesiredState,
    eSetExhPressZeroDesiredState,
    eSetExhPressZeroStateDirect,
    eSetInhPressZeroDesiredState,
    eSetInhPressZeroStateDirect,
    eSetPurgeFlowStateDirect,
    eSetPurgeFlowDesiredState,
    eSetOilPumpStateDirect,
    eSetOilPumpDesiredState,
    eAirFlowControllerEnable,
    eAirFlowControllerDisable,
    eO2FlowControllerEnable,
    eO2FlowControllerDisable,
    eExhControllerEnable,
    eExhControllerDisable,
    eHfoRequestDoSVCtr,
    eHfoRequestDoAmpCtr,
    eStopHfoDoAmpCtr,
    eStopHfoDoSVCtr,
    eHfoSetExhValveLimit,
    eHfoValveRampToPosition,
    eHfoServoEnable,
    eHfoServoDisable,
    eHfoControllerEnable,
    eHfoControllerDisable,
    eExhValveRampToPosition,
    eO2ValveRampToPosition,
    eAirValveRampToPosition,
    eResetHFOStaticVariables,
    eClearHfoServoStaticVariables,
    eResetHfoServoPIDStatic,
    eResetHfoServo_20sTimer,
    eSetHfoConstFlow,
    eSetHfoControllerSIRequest,
    eSetHfoServoSIRequest,
    eSetHfoControllerSIType,
    eSetAirStepPostion,
    eSetO2StepPostion,
    eSetExhStepPostion,
    eSetHfoStepPostion,
    eSetHfoValveFreq,
    eSetHfoAlarmInactiveTimer,
    eSetCMVAlarmInactiveTimer,
    eNCPAPResetIMVStaticVariables,
    eNCPAPControllerEnable,
    eNCPAPControllerDisable,
    eSetNCPAPTargetFlow,
    eAirFlowControllerReset,
    eO2FlowControllerReset,
    eExhControllerReset,
    eHfoFlowControllerReset,
    eDisableHfoMotorErrorCheck,
    eDisableO2MotorErrorCheck,
    eDisableAirMotorErrorCheck,
    eDisableExhMotorErrorCheck,
    eEnableHfoMotorErrorCheck,
    eEnableO2MotorErrorCheck,
    eEnableAirMotorErrorCheck,
    eEnableExhMotorErrorCheck,
    eSetAirPressSwitchFault,
    eSetO2PressSwitchFault,
    eSetCauseOfAlarm,
    eSendOilPumpRequestKey,
    eDoLedAlarm,
    eRunHfoServo,
    eRunHfoController,
    eRunCMVServoController,
    eRunVCVServoController,
    eRunAirFlowController,
    eRunO2FlowController,
    eRunNCPAPController,
    eRunExhController,
    eRunHfoValveHFOMode,
    eSetDesiredAirFlowController,
    eSetDesiredO2FlowController,
    eSetDesiredExhFlowController,
    eSetSoundDevice,
    eSetAlarm,
    eTSCalibrationExec,
    eEnableMetranProtocol,
    eEnablePhilipsProtocol,
    eDisableMetranProtocol,
    eDisablePhilipsProtocol,
    eSetActiveMonitor,
    eSetAlarmSilenceTime,
    eSetCurrentAction,
    eSetBaudRate,
    eSetMotorConfig,
    eComputeProxySensorOffset,
    eSetA11,
    eSetA12,
    eSetA13,
    eSetA14,
    eSetA21,
    eSetA22,
    eSetA23,
    eSetA24,
    eSetFiO2ConversionFactor,
    eSetHfoStrokeVol,
    eSetHfoValveOffsetPosition,
    eSetExhValveLiftOff,
    eSetExhValveMaxStep,
    eSetAirValveLiftOff,
    eSetAirValveMaxStep,
    eSetO2ValveLiftOff,
    eSetO2ValveMaxStep,
    eSetHfoValveLiftOff,
    eSetHfoValveMaxStep,
    eSetDesiredSolenoidState,
    eSetExhPressZeroOffset,
    eSetInhPressZeroOffset
} E_MsgId;
typedef enum
{
    eCompRateSettingId,
    eBaseFlowSettingId,
    eTubeLengthSettingId,
    eTubeDiameterSettingId,
    eHFOMapSettingId,
    eFreshGasSettingId,
    eSISettingId,
    eHFOFrequencySettingId,
    eHFOStrokeVolumeSettingId,
    eAmplitudeSettingId
} SingleSettingId;
typedef enum
{
    eInhPressSensorType,
    eExhPressSensorType
} E_ActivePressType;

typedef struct
{
    SingleSettingId settingId;
    int value;
} SingleSetting;

typedef struct
{
    int map;
    int freshGas;
    int si;
    int frequency;
    int strokeVolume;
    int amplitude;
} HFOSetting;
typedef struct
{
    int compRate;
    int baseFlow;
    int tubeLength;
    int tubeDiameter;
} PCVSetting;
typedef struct
{
    int compRate;
    int baseFlow;
    int tubeLength;
    int tubeDiameter;
} VCVSetting;

typedef struct
{
    bool isOKToShutDown;
    ProxyStatus lastStatus;
    int currentReading;
} ProxyData;

typedef struct
{
    bool isO2SensorConnected;
    bool isPresent;
    int currentVolt;
    int lastVolt;
    int currentReading;
    int avgPress;
    int avgFlow;
} FiO2Data;

typedef struct
{
   int currentReading;
   int lastReading;
   int lastVolt;
   int currentVolt;
} SensorData;

typedef struct
{
    E_State state;
    E_State stateDirect;
} SwitchData;

typedef struct
{
    E_State state;
    E_State desiredState;
} LineData;

typedef struct
{
    int filterPressIntegrator;
    int exhMap;
    bool siRequest;
    bool himp;
} HfoControllerData;

typedef struct
{
    int stepPosition;
    USHORT diffStep;
    USHORT liftOff;
    USHORT maxStep;
} ValveData;

typedef struct
{
    USHORT targetPressure;
    USHORT riseTime;
} InhalationTargetData;

typedef struct
{
    double compensateRate;
    bool type;
} CompensateData;

typedef struct
{
    bool isSpeakerEnabled;
    bool isBuzzerEnabled;
} SoundDeviceData;
typedef struct
{
    float factor;
    float offset;
} ConversionFactor;

typedef struct
{
    E_DiscreteOutputId id;
    E_State state;
} SolenoidStateData;

typedef union
{
    E_State state;
    unsigned short biasFlow;
    InhalationTargetData inhTarget;
    unsigned short targetPress;
    CompensateData compensate;
    E_ActivePressType primary;
    int exhValveLimit;
    int position;
    bool constFlowFlag;
    bool siRequestFlag;
    E_SIType siType;
    unsigned short inactiveTimer;
    int targetFlow;
    bool pressSwitchFault;
    int causeOfAlarm;
    E_SpecificPhaseId specificPhaseId;
    int desiredAirFlow;
    int desiredO2Flow;
    int desiredExhFlow;
    E_AlarmLevel alarmLevel;
    int freq;
    SoundDeviceData sound;
    E_SoundType alarm;
    E_Monitor activeMonitor;
    int alarmSilenceTime;
    ProxyActions action;
    E_BaudRate baudRate;
    E_MotorId motorId;
    int aValue;
    ConversionFactor conversion;
    USHORT strokeVol;
    USHORT offsetPosition;
    USHORT liftOff;
    USHORT maxStep;
    SolenoidStateData solenoid;
    USHORT zeroOffset;
} IpcCommandData;

typedef struct
{
    bool checkHFODoor;
    bool checkHFOPosition;
    bool checkFanError;
    bool checkO2MotorError;
    bool checkAirMotorError;
    bool checkExhMotorError;
    bool checkHfoMotorError;
    bool checkO2MotorLostCommunitcation;
    bool checkAirMotorLostCommunitcation;
    bool checkExhMotorLostCommunitcation;
    bool checkHFOMotorLostCommunitcation;
    bool checkFlowSensor;
    bool checkDACLoopback;
    bool checkO2ValveStuckOpen;
    bool checkO2ValveStuckClosed;
    bool checkAirValveStuckOpen;
    bool checkAirValveStuckClosed;
} HardwareCheckingData;

typedef struct
{
    int DesiredO2Raito;
    int DesiredAirRaito;
} DesiredRaitoData;

typedef struct
{
    int exhFactor;
    int airFactor;
    int o2Factor;
} BtpsFactor;

typedef struct
{
    //Needed by BD
    bool isNeedVersion;
    int vcvInhVolume;

    SwitchData airPressSwitch;
    SwitchData o2PressSwitch;
    SwitchData hfoDoorSwitch;
    SwitchData hfoPistonSwitch;
    SwitchData manualBreathInput;
    SwitchData manualBreathConnection;
    SwitchData o2MotorStatus;
    SwitchData airMotorStatus;
    SwitchData exhMotorStatus;
    SwitchData hfoMotorStatus;

    LineData inpedanceLine;
    LineData safetyValve;
    LineData proxyFlowResetLine;
    LineData exhPressureZero;
    LineData inhPressureZero;
    LineData purgeFlow;
    LineData oilPump;

    ValveData airValve;
    ValveData o2Valve;
    ValveData hfoValve;
    ValveData exhValve;
    HfoControllerData hfoController;

    ProxyData proxy;
    FiO2Data fio2;

    SensorData airFlow;
    SensorData o2Flow;
    SensorData inhPress;
    SensorData exhPress;
    SensorData activePress;

    HardwareCheckingData hardware;
    //Needed by Device
    E_VentilationMode currentMode;
    int currentAmp;
    int desiredAirFlow;
    int desiredO2Flow;
    DesiredRaitoData ratio;
    BtpsFactor btpsFactor;
} IpcRealTimeData;

typedef union
{
    SingleSetting singleSetting;
    HFOSetting hfo;
    PCVSetting pcv;
    VCVSetting vcv;
    IpcRealTimeData realTime;
    IpcCommandData command;
} IpcData;

typedef struct
{
    int index;
    E_MsgId id;
    IpcData data;
} IpcMessage;

#endif /* IPC_IPCINTERFACE_H_ */
