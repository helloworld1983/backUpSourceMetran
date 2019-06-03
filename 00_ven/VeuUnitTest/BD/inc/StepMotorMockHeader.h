#ifndef INC_STEPMOTORMOCKHEADER_H_
#define INC_STEPMOTORMOCKHEADER_H_

#include "DeviceInterface.h"

//define function to mock
void O2ValveSetStepPosition(int);
void AirValveSetStepPosition(int);
void ExhValveSetStepPosition(int);
void HfoValveSetStepPosition(int);

void O2ValveEnableMotorErrorCheck(void);
void AirValveEnableMotorErrorCheck(void);
void ExhValveEnableMotorErrorCheck(void);
void HfoValveEnableMotorErrorCheck(void);

void O2ValveDisableMotorErrorCheck(void);
void AirValveDisableMotorErrorCheck(void);
void ExhValveDisableMotorErrorCheck(void);
void HfoValveDisableMotorErrorCheck(void);

void MgrControllerCMVServoControllerDisable(void);
void MgrControllerCMVServoControllerEnable(void);

void MgrControllerCMVServoControllerResetIMVStaticVariables(void);
void MgrControllerVCVServoControllerResetIMVStaticVariables(void);

void MgrControllerVCVServoControllerDisable(void);
void MgrControllerVCVServoControllerEnable(void);

void PurgeFlowSetDesiredState(E_State);
void SafetyValveSetDesiredState(E_State);
void PurgeFlowSetStateDirect(E_State);
void OilPumpSetDesiredState(E_State);

void InpedanceLineSetState(E_State);
void InpedanceLineSetDesiredState(E_State);

void SafetyValveSetStateDirect(E_State);

void HfoServoDisable(void);

void NCPAPControllerResetIMVStaticVariables(void);

void AirFlowControllerEnable(void);
void O2FlowControllerEnable(void);
void NCPAPControllerEnable(void);

void O2FlowControllerReset(void);
void AirFlowControllerReset(void);

void AirFlowControllerSetDesired(int);
void O2FlowControllerSetDesired(int);

void AirFlowControllerDisable(void);
void O2FlowControllerDisable(void);
void ExhControllerDisable(void);
void NCPAPControllerDisable(void);

void BDRequestDevice(RequestId);

ProxyStatus ProxyFlowSensorgetLastStatus(void);

void MgrControllerCMVServoControllerSetExhalationTarget(LONG );
void MgrControllerCMVServoControllerSetInhalationTarget(LONG);

void MgrControllerVCVServoControllerSetInhalationTarget(LONG);
void MgrControllerVCVServoControllerSetExhalationTarget(LONG);

void PressureSensorActivePressureSensorSetPrimary(void);

void NCPAPControllerSetTargetFlow(LONG);

void MgrControllerCMVServoControllerSetBiasFlow(SHORT);
void MgrControllerVCVServoControllerSetBiasFlow(SHORT);

void MgrControllerVCVServoControllerSetSepecificPhase(void);

UNSIGNED FlowPatternPtrCalcTargetInhTime(LONG, LONG);
void FlowPatternPtrCalcCompensatedPeakFlow(LONG);

int PressureSensorGetLastReading(void);

int HfoValveGetStepPosition(void);
int O2ValveGetStepPosition(void);
int AirValveGetStepPosition(void);
int ExhValveGetStepPosition(void);

void HfoValveRampToPosition(USHORT);
void O2ValveRampToPosition(USHORT);
void AirValveRampToPosition(USHORT);
void ExhValveRampToPosition(USHORT);

void MgrControllerResetIMVStaticVariables(void);

LONG ProxyFlowSensorGetLastReading(void);

#endif /* INC_STEPMOTORMOCKHEADER_H_ */
