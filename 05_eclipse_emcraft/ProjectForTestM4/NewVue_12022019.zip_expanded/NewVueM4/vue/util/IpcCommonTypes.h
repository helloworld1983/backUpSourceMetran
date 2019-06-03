/*
 * IpcInterface.h
 *
 *  Created on: Dec 25, 2018
 *      Author: qsbk0
 */

#ifndef UTIL_IPCCOMMONTYPES_H_
#define UTIL_IPCCOMMONTYPES_H_
#include <CommonTypes.h>
#include <stdbool.h>
#include "stdint.h"
#define MAX_CMD_A53_TO_M4 18
#define MAX_CMD_M4_TO_A53  10

#define STX 0xaa
#define ETX 0xff

#define LOWBYTE(w16) ((uint8_t)w16)
#define HIGHBYTE(w16) ((uint8_t)(((((uint16_t)(w16))>>8)) & 0xFF))
typedef struct
{
	int32_t desiredHFOPosition;
	int32_t desiredExhPosition;
	int32_t desiredO2Position;
	int32_t desiredAirPosition;
} desiredMotorPositionStruct;

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
}E_MotorId;

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

typedef enum
{
	//Command id
	eSetImpedanceLineState,
	eSetImpedanceLineStateDirect,
	eSetImpedanceLineDesiredState,
	eSetSafetyValveDesiredState,
	eSetSafetyValveStateDirect,
	eCMVServoEnable,
	eCMVServoDisable,
	eSetCMVBiasFlow,
	eSetCMVInhalationTarget,
	eSetCMVExhlationTarget,
	eCMVResetIMVStaticVariables,
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
	eSetDesiredExhController,
	eSetCurrentAction,
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
	eSetAirValveLiftOff,
	eSetO2ValveLiftOff,
	eSetHfoValveLiftOff,
	eSetDesiredSolenoidState,
	eSetExhPressZeroOffset,
	eSetInhPressZeroOffset,
	eSetRemoteAlarmLow,
	eSetFiO2Offset,
	eSetFiO2PassCalibration,
	eSetElevationSetting,
    eSetBtpsFactor,
    eSetRatio,
    eSetCurrentMode,
	eSetCurrentAmp,
	eSetClock200hours,
	eSetAZTestState,
	eSetDesiredFlowNCPAP,
	//Device require Bd
	eDisableTrigger,
	eEnableTrigger,
	eProxyVersionGUIEvent,
	eLogPostCustomEvent,
	eUpdateSettingsHfoServo,
	eA53AppReady,
	eM4Ready,
	A53OSReady
} E_CmdID;
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
	bool isNeedVersion;
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
typedef struct
{
	char version[8];
} VersionGUIEventData;

typedef struct
{
	int infoId;
	int infoValue;
	E_LogFileType fileType;
} FileTaskEventData;


typedef struct
{
	bool checkFlowSensor;
} HardwareCheckingData;

typedef struct
{
	int DesiredO2Ratio;
	int DesiredAirRatio;
} DesiredRatioData;

typedef struct
{
	int exhFactor;
	int airFactor;
	int o2Factor;
} BtpsFactor;
typedef struct
{
	int desiredAirFlow;
	int desiredO2Flow;
} DesiredFlow;

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
	int desiredExhPressure;
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
	LONG fiO2Offset;
	LONG fiO2PassCalibration;
	LONG elevation;
	VersionGUIEventData proxyVersion;
	FileTaskEventData logEvent;
	char triggerId;
    BtpsFactor btpsFactor;
    DesiredRatioData ratio;
    E_VentilationMode currentMode;
    int currentAmp;
	int32_t clock200hours;
	E_AzTestState azTestState;
	DesiredFlow desiredFlow;
} CmdData;


typedef struct
{
	E_CmdID id;
	CmdData data;
} Command;

typedef struct
{
	BtpsFactor btpsFactor;
	DesiredRatioData ratio;
	int currentAmp;
	int desiredAirFlow;
	int desiredO2Flow;
	int32_t clock200hours;
	E_VentilationMode currentMode;
	E_AzTestState azTestState;
	HFOSetting hfo;
	PCVSetting pcv;
	VCVSetting vcv;
} SettingA53ToM4;
typedef struct
{
    HFOSetting hfo;
    PCVSetting pcv;
    VCVSetting vcv;
} Setting;
typedef union
{
    Command command[MAX_CMD_A53_TO_M4];
    Setting setting;
}DataA53ToM4;
typedef struct
{
    DataA53ToM4 data;
    uint8_t nCmd;
} MessageA53ToM4;

typedef struct
{
    HardwareCheckingData hardware;

    bool IsEnableCMVServoController;
    bool IsEnableVCVServoController;
    bool IsEnableAirFlowController;
    bool IsEnableO2FlowController;
    bool IsEnableExhController;
    bool IsEnableHfoServo;
    bool IsEnableHfoController;
    bool IsEnableNCPAPController;

    SwitchData airPressSwitch;
    SwitchData o2PressSwitch;
    SwitchData hfoDoorSwitch;
    SwitchData hfoPistonSwitch;
    SwitchData manualBreathInput;
    SwitchData manualBreathConnection;

    LineData impedanceLine;
    LineData safetyValve;
    LineData proxyFlowResetLine;
    LineData exhPressureZero;
    LineData inhPressureZero;
    LineData purgeFlow;
    LineData oilPump;


    int vcvInhVolume;
    ValveData airValve;
    ValveData o2Valve;
    ValveData hfoValve;
    ValveData exhValve;
    HfoControllerData hfoController;
    SensorData airFlow;
    SensorData o2Flow;
    SensorData inhPress;
    SensorData exhPress;
    SensorData activePress;

    ProxyData proxy;
    FiO2Data fio2;
} RealTimeM4ToA53;

typedef struct
{
    uint8_t nOfCmd;
    bool isUpdateRealTime;
	RealTimeM4ToA53 realTime;
    Command command[MAX_CMD_M4_TO_A53];
} MessageM4ToA53;

typedef union
{
    MessageM4ToA53 M4ToA53;
    MessageA53ToM4 A53ToM4;
} IPCMessage;
#endif /* UTIL_IPCCOMMONTYPES_H_ */
