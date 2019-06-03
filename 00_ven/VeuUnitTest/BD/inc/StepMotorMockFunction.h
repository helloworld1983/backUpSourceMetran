#ifndef INC_STEPMOTORMOCKFUNCTION_H_
#define INC_STEPMOTORMOCKFUNCTION_H_

#include "gmock.h"
#include "Mode.h"

class StepMotorMockFunction
{
public:
    virtual ~StepMotorMockFunction(void){};

    MOCK_METHOD1(O2ValveSetStepPosition, void(int));
    MOCK_METHOD1(AirValveSetStepPosition, void(int));
    MOCK_METHOD1(ExhValveSetStepPosition, void(int));
    MOCK_METHOD1(HfoValveSetStepPosition, void(int));


    MOCK_METHOD0(MgrControllerCMVServoControllerDisable, void(void));
    MOCK_METHOD0(MgrControllerCMVServoControllerEnable, void(void));
    MOCK_METHOD0(MgrControllerVCVServoControllerEnable, void(void));
    MOCK_METHOD0(MgrControllerVCVServoControllerDisable, void(void));

    MOCK_METHOD0(O2ValveEnableMotorErrorCheck, void(void));
    MOCK_METHOD0(AirValveEnableMotorErrorCheck, void(void));
    MOCK_METHOD0(ExhValveEnableMotorErrorCheck, void(void));
    MOCK_METHOD0(HfoValveEnableMotorErrorCheck, void(void));

    MOCK_METHOD0(O2ValveDisableMotorErrorCheck, void(void));
    MOCK_METHOD0(AirValveDisableMotorErrorCheck, void(void));
    MOCK_METHOD0(ExhValveDisableMotorErrorCheck, void(void));
    MOCK_METHOD0(HfoValveDisableMotorErrorCheck, void(void));

    MOCK_METHOD1(PurgeFlowSetDesiredState, void(E_State));
    MOCK_METHOD1(SafetyValveSetDesiredState, void(E_State));
    MOCK_METHOD1(PurgeFlowSetStateDirect, void(E_State));
    MOCK_METHOD1(OilPumpSetDesiredState, void(E_State));

    MOCK_METHOD1(InpedanceLineSetState, void(E_State));
    MOCK_METHOD1(InpedanceLineSetDesiredState, void(E_State));

    MOCK_METHOD1(SafetyValveSetStateDirect, void(E_State));

    MOCK_METHOD0(HfoServoDisable, void(void));

    MOCK_METHOD0(NCPAPControllerResetIMVStaticVariables, void(void));

    MOCK_METHOD0(AirFlowControllerEnable, void(void));
    MOCK_METHOD0(O2FlowControllerEnable, void(void));
    MOCK_METHOD0(NCPAPControllerEnable, void(void));

    MOCK_METHOD0(AirFlowControllerDisable, void(void));
    MOCK_METHOD0(O2FlowControllerDisable, void(void));
    MOCK_METHOD0(NCPAPControllerDisable, void(void));
    MOCK_METHOD0(ExhControllerDisable, void(void));

    MOCK_METHOD0(MgrControllerCMVServoControllerResetIMVStaticVariables, void(void));
    MOCK_METHOD0(MgrControllerVCVServoControllerResetIMVStaticVariables, void(void));

    MOCK_METHOD1(BDRequestDevice, void(RequestId));

    MOCK_METHOD0(ProxyFlowSensorgetLastStatus, ProxyStatus(void));

    MOCK_METHOD1(MgrControllerCMVServoControllerSetExhalationTarget, void(LONG));
    MOCK_METHOD1(MgrControllerCMVServoControllerSetInhalationTarget, void(LONG));

    MOCK_METHOD1(MgrControllerVCVServoControllerSetExhalationTarget, void(LONG));
    MOCK_METHOD1(MgrControllerVCVServoControllerSetInhalationTarget, void(LONG));

    MOCK_METHOD0(PressureSensorActivePressureSensorSetPrimary, void(void));

    MOCK_METHOD1(NCPAPControllerSetTargetFlow, void(LONG));

    MOCK_METHOD0(O2FlowControllerReset, void(void));
    MOCK_METHOD0(AirFlowControllerReset, void(void));

    MOCK_METHOD1(AirFlowControllerSetDesired, void(int));
    MOCK_METHOD1(O2FlowControllerSetDesired, void(int));

    MOCK_METHOD1(MgrControllerCMVServoControllerSetBiasFlow, void(SHORT));
    MOCK_METHOD1(MgrControllerVCVServoControllerSetBiasFlow, void(SHORT));

    MOCK_METHOD0(MgrControllerVCVServoControllerSetSepecificPhase, void(void));

    MOCK_METHOD2(FlowPatternPtrCalcTargetInhTime, UNSIGNED(LONG, LONG));
    MOCK_METHOD1(FlowPatternPtrCalcCompensatedPeakFlow, void(LONG));

    MOCK_METHOD0(PressureSensorGetLastReading, int(void));

    MOCK_METHOD0(HfoValveGetStepPosition, int(void));
    MOCK_METHOD0(O2ValveGetStepPosition, int(void));
    MOCK_METHOD0(AirValveGetStepPosition, int(void));
    MOCK_METHOD0(ExhValveGetStepPosition, int(void));

    MOCK_METHOD1(HfoValveRampToPosition, void(USHORT));
    MOCK_METHOD1(O2ValveRampToPosition, void(USHORT));
    MOCK_METHOD1(AirValveRampToPosition, void(USHORT));
    MOCK_METHOD1(ExhValveRampToPosition, void(USHORT));

    MOCK_METHOD0(MgrControllerResetIMVStaticVariables, void(void));

    MOCK_METHOD0(ProxyFlowSensorGetLastReading, LONG(void));

};

#endif /* INC_STEPMOTORMOCKFUNCTION_H_ */
