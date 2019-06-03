#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ExhPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "StepMotorMockHeader.h"
#include "MgrBdSettingMockHeader.h"

// Define statics
UNSIGNED ExhPhase::S_ExhStartTime = 0;
UNSIGNED ExhPhase::S_TimeElapsed = 0;

// The FALL_TIME defines the amount of time it takes for the exhalation
// pressure to fall from the end inhalation pressure to the targeted baseline
// pressure.  This value is passed to Exponential::GetExpTerm() as it
// is used to calculate the exponential term of the desired pressure equation.
//
// This value is 150 ms in system clock ticks
static const UNSIGNED FALL_TIME = 150 / MS_PER_TICK;

// These adjustment factors are applied to the desired pressure sent
// to the exhalation controller to help prevent under or over-shoot depending
// upon the operator set baseline pressure and bias flow.
// The values are scaled by 2**7
static const LONG BASELINE_ADJUST_HIGH = 136;   // 1.06
static const LONG BASELINE_ADJUST_LOW = 125;    // 0.98

// Baseline pressure and bias flow cutoff values used to determine which
// adjustment factor to apply.
static const LONG BASELINE_CUTOFF = 20 * SCALE;     // 20 cmH2O
static const LONG BIAS_FLOW_CUTOFF = (20 * 1000 * SCALE) / SEC_PER_MIN; // 20 Lpm

ExhPhase::ExhPhase (void) :
        Phase ()
{
    TargetPressure = 0;

}   // end ExhPhase()

UNSIGNED ExhPhase::S_CalcTimeSincePhaseStart (void)
{

    // Return the time elapsed since the start of the exhalation phase.
//    return (CalcElapsedTime (S_ExhStartTime));

}   // end S_CalcTimeSincePhaseStart()
void ExhPhase::Start (void)
{
//    VCVServoController->SetSepecificPhase(eNoTrigExhPhase);

//    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
//    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = BDMgrBdSettingGetCurrentValue(eBaseFlow);
    MgrControllerCMVServoControllerSetBiasFlow(BiasFlow/100);
    MgrControllerVCVServoControllerSetBiasFlow(BiasFlow/100);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = BDMgrBdSettingGetCurrentValue(eBaselinePress);

//    CMVServoController->SetExhalationTarget(TargetPressure/10);
//    VCVServoController->SetExhalationTarget(TargetPressure/10);

    // During expiratory this bit should be clear.
    //    InpedanceLine->SetState(eOff);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_ExhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtrProcessNewPhase();
    // Update Patient Data
    MgrBreathDataExhalationStart();

    // Get the pointer to the TriggerMgr instance
   TriggerMgrEnableExhaleTrigger();

    // Enable the low baseline alarm trigger

}   // end Start()
void ExhPhase::End (void)
{
    // Pointer to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable the manual inhalation and the patient inhalation pressure
    // trigger.
    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);
    triggerMgrPtrDisableTrigger(ePtInhPressTrigger);
    triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger);

    // Disable the Patient Inhalation Flow Trigger; the Patient Inhalation
    // Pressure Trigger was disabled in ExhPhase::End()
    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);


    // Switch back to Pexh if the vent wasn't already using it.
    PressureSensorActivePressureSensorSetPrimary();
}   // end End()
void ExhPhase::ProcessBdCycle (void)
{
    // Calculate the time elapsed since the start of exhalation.

    MgrBdSettingPhaseInPIPSetting();
    MgrBdSettingPhaseInAllSettings();


//    S_TimeElapsed = ExhPhase::S_CalcTimeSincePhaseStart();

    TargetPressure = BDMgrBdSettingGetCurrentValue(eBaselinePress);
//    CMVServoController->SetExhalationTarget(TargetPressure/10);
//    VCVServoController->SetExhalationTarget(TargetPressure/10);

    // If it is time to auto-zero the exhalation pressure transducer,
    // kick start the auto-zero, update the state and switch the active
    // pressure transducer to the inhalation pressure transducer.  The
    // ventilator usually uses Pexh for monitoring and control.  During the
    // phase where Pexh is being auto-zeroed, Pinh is used for monitoring
    // and control.  It is always switched back at the end of exhalation.
//    if (eAzPending == PInhAzTestPtr->GetAzTestState())
//    {
//        if ( ActivePressureSensor->GetLastReading() <= (TargetPressure + 200)  )
//        {
//            RT_TASK* receiveTask1 = NULL;
//            receiveTask1 = (AzTask::S_GetInstance())->S_SetEventFlag(INH_AZ_EVENT_FLAG);
//            ActivePressureSensor->SetPrimary(ExhPressSensor);
//            if((receiveTask1 !=0))
//                PInhAzTestPtr->SetAzTestState(eAzActive);
//        }
//    }
//    else
//    {
//        if (eAzCompleted == PInhAzTestPtr->GetAzTestState())
//        {
//            ActivePressureSensor->SetPrimary(InhPressSensor);
//
//            // Otherwise if it is time to auto-zero Pinh, start the auto-zero and
//            // update the state.
//            if (eAzPending == PExhAzTestPtr->GetAzTestState())
//            {
//
//
//                RT_TASK* receiveTask2 = NULL;
//                receiveTask2 = (AzTask::S_GetInstance())->S_SetEventFlag(EXH_AZ_EVENT_FLAG);
//                if((receiveTask2 !=0))
//                    PExhAzTestPtr->SetAzTestState(eAzActive);
//            }
//        }
//    }

}   // end ProcessBdCycle()
E_Bool ExhPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
{
    UNSIGNED tmp = minExhTime;
    E_Bool rtnValue = eFalse;

    if(tmp >= 1000)
        tmp = 1000;
    else if(tmp < 200)
        tmp = 200;

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if ((phaseMgrPtrGetCurrentPhaseId() == eExhPhase) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()

class ExhPhaseUnitMock : public BDTestFixture
{
public:
    ExhPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ExhPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyExhPhase = new ExhPhase();
    }
    virtual void TearDown(void)
    {
        delete MyExhPhase;
    }
    ExhPhase *MyExhPhase;
};

//test for ExhPhase::ExhPhase (void) :
TEST_F(ExhPhaseUnitMock, ExhPhase)
{
    EXPECT_EQ(MyExhPhase->TargetPressure, 0);
}

//test for UNSIGNED ExhPhase::S_CalcTimeSincePhaseStart (void)
TEST_F(ExhPhaseUnitMock, S_CalcTimeSincePhaseStart)
{
    //no soruce, not need to test
}

//test for void ExhPhase::Start (void)
TEST_F(ExhPhaseUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(100));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetBiasFlow(1)).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerVCVServoControllerSetBiasFlow(1)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaselinePress));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrProcessNewPhase()).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataExhalationStart()).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableExhaleTrigger()).Times(1);

    MyExhPhase->Start();
}

//test for void ExhPhase::End (void)
TEST_F(ExhPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhPressTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);

    EXPECT_CALL(*_StepMotor, PressureSensorActivePressureSensorSetPrimary()).Times(1);


    MyExhPhase->End();
}

//test for void ExhPhase::ProcessBdCycle (void)
TEST_F(ExhPhaseUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInPIPSetting()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eBaselinePress)).Times(1);


    MyExhPhase->ProcessBdCycle();
}

//test for E_Bool ExhPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
TEST_F(ExhPhaseUnitMock, S_IsInUnrestrictedPhase_0)
{
    MyExhPhase->S_TimeElapsed = 1001;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));


    EXPECT_EQ(MyExhPhase->S_IsInUnrestrictedPhase(1000), eTrue);
}
TEST_F(ExhPhaseUnitMock, S_IsInUnrestrictedPhase_1)
{
    MyExhPhase->S_TimeElapsed = 201;

    EXPECT_EQ(MyExhPhase->S_IsInUnrestrictedPhase(0), eFalse);
}

#endif //end define BD_PHASE_UNIT_TEST
