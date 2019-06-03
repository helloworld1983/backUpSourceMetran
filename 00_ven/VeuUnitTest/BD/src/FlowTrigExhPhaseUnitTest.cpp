#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "FlowTrigExhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "TriggerMgrMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "StepMotorMockHeader.h"

// Define statics
FlowTrigExhPhase* FlowTrigExhPhase::S_Instance = NULL;

// Rise time to the targeted bias flow.  At the start of exhalation the
// flow is 0.  Flow then exponentially rises to the targeted Bias flow
// using the BIAS_FLOW_RISE_TIME which is 200ms.
static const UNSIGNED BIAS_FLOW_RISE_TIME = 200 / MS_PER_TICK;

FlowTrigExhPhase::FlowTrigExhPhase (void) :
        ExhPhase ()
{

}   // end FlowTrigExhPhase()
FlowTrigExhPhase* FlowTrigExhPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new FlowTrigExhPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

void FlowTrigExhPhase::Start (void)
{
    // Get a pointer to the TriggerMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

//    ExhPhase::Start();

    E_VentilationMode currentMode = BDModeMgrGetCurrentModeId();
    if(currentMode == eNivMode)
    {
        triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);
        return;
    }

    ProxyStatus Status = ProxyFlowSensorgetLastStatus();

    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
        triggerMgrPtrDisableTrigger(ePtInhPressTrigger);
        triggerMgrPtrEnableTrigger(ePtInhFlowTrigger);
    }
    else
        triggerMgrPtrEnableTrigger(ePtInhPressTrigger);

}   // end Start()

void FlowTrigExhPhase::End (void)
{
    // Invoke the base class method to perform processing common to all
    // exhalation phases.
//    ExhPhase::End();
    // Get a pointer to the TriggerMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    // Disable the Patient Inhalation Flow Trigger; the Patient Inhalation
    // Pressure Trigger was disabled in ExhPhase::End()
    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);
    triggerMgrPtrDisableTrigger(ePtInhPressTrigger);

}   // end End()
void FlowTrigExhPhase::ProcessBdCycle (void)
{

    LONG desiredAir = 0;    // desired air flow
    LONG desiredO2 = 0;     // desired O2 flow
//    LONG biasFlow ;      // desired bias flow
//    UNSIGNED timeElapsed;   // time elapsed since start of exhalation in ticks
//    LONG expTerm;       // exponential term

    // invoke the base class method
//    ExhPhase::ProcessBdCycle ();

    // Determine the time elapsed since the start of the exhalation phase;
    // offset this value by 1 flow control cycle.   Air and O2 use the same
    // control cycle so it does not matter which one is used.
//    timeElapsed = ExhPhase::S_TimeElapsed + AirO2Task->GetSamplePeriod();

    // Calculate the exponential term of the equation based upont the rise
    // time and the time elapsed since the start of exhalation.
//    expTerm = Exponential::GetExpTerm (BIAS_FLOW_RISE_TIME, timeElapsed);

    // biasFlow starts at 0 at the start of the exhalation phase and
    // exponentially rises to the targeted bias flow.
//    biasFlow = (BiasFlow * (EXP_DATA_SF - expTerm)) >> EXP_DATA_SHIFT;

    // Determine the air/oxygen split of the bias flow for this control cycle
//    (O2Setting::S_GetInstance ())->CalculateAirO2Flows (biasFlow, &desiredAir,
//                                                        &desiredO2);

    // Update the air and O2 flow controllers
    AirFlowControllerSetDesired(desiredAir);
    O2FlowControllerSetDesired(desiredO2);

}   // end ProcessBdCycle()

class FlowTrigExhPhaseUnitMock : public BDTestFixture
{
public:
    FlowTrigExhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~FlowTrigExhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyFlowTrigExhPhase = new FlowTrigExhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyFlowTrigExhPhase;
    }
    FlowTrigExhPhase *MyFlowTrigExhPhase;
};

//test for FlowTrigExhPhase::FlowTrigExhPhase (void) :
TEST_F(FlowTrigExhPhaseUnitMock, FlowTrigExhPhase)
{
    //no source, not need to test
}

//test for FlowTrigExhPhase* FlowTrigExhPhase::S_GetInstance (void)
TEST_F(FlowTrigExhPhaseUnitMock, S_GetInstance)
{
    MyFlowTrigExhPhase->S_GetInstance();

    EXPECT_NE(MyFlowTrigExhPhase->S_Instance, (void*)0);
}

//test for void FlowTrigExhPhase::Start (void)
TEST_F(FlowTrigExhPhaseUnitMock, Start_0)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eNivMode));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);

    MyFlowTrigExhPhase->Start();
}

TEST_F(FlowTrigExhPhaseUnitMock, Start_1)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eCalibrated));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtInhFlowTrigger)).Times(1);

    MyFlowTrigExhPhase->Start();
}
TEST_F(FlowTrigExhPhaseUnitMock, Start_2)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eResetAndRecovered));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtInhFlowTrigger)).Times(1);

    MyFlowTrigExhPhase->Start();
}
TEST_F(FlowTrigExhPhaseUnitMock, Start_3)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eApneaBackupMode));

    EXPECT_CALL(*_StepMotor, ProxyFlowSensorgetLastStatus()).Times(1).WillOnce(Return(eSensorError));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(ePtInhPressTrigger)).Times(1);

    MyFlowTrigExhPhase->Start();
}

//test for void FlowTrigExhPhase::End (void)
TEST_F(FlowTrigExhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);

    MyFlowTrigExhPhase->End();
}

//test for void FlowTrigExhPhase::ProcessBdCycle (void)
TEST_F(FlowTrigExhPhaseUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_StepMotor, AirFlowControllerSetDesired(0)).Times(1);
    EXPECT_CALL(*_StepMotor, O2FlowControllerSetDesired(0)).Times(1);

    MyFlowTrigExhPhase->ProcessBdCycle();
}

#endif //end define BD_PHASE_UNIT_TEST
