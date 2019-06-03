#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_PHighPhase.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

using namespace ::testing;

APRV_PHighPhase* APRV_PHighPhase::S_Instance = NULL;
UNSIGNED APRV_PHighPhase::S_InhStartTime = 0;
UNSIGNED APRV_PHighPhase::S_TimeElapsed = 0;
UNSIGNED APRV_PHighPhase::S_Time = 0;

APRV_PHighPhase* APRV_PHighPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_PHighPhase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

APRV_PHighPhase::APRV_PHighPhase(void) : InhPhase()
{
    RiseTime = 0;
    TargetPressure = 0;
}   // end ExhPhase()

void APRV_PHighPhase::Start(void)
{
    E_BreathPhaseIndicator breathType = eMandatory;

//    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
//    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtrGetCurrentValue(eBaseFlow);

    SHORT TempBiasFlow = BiasFlow / 100;
    BDRequestDevice(eCMVServoControllerSetBiasFlow);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);
    RiseTime = mgrBdSettingPtrGetCurrentValue(eRiseTime);

    MgrControllerCMVServoControllerSetInhalationTarget(TargetPressure/10);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtrProcessNewPhase();
    // Update Patient Data
    MgrBreathDataAPRV_PHighPhaseStart(breathType);


    TriggerMgrEnableExhaleTrigger();
    TriggerMgrEnableTrigger(ePtInhFlowTrigger);
    TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPHigh);

    TriggerMgrReEnableTrigger(eApneaAlarmTimeTrigger);

    TriggerMgrEnableTrigger(eHPHighAlarmTrigger);
}   // end Start()

void APRV_PHighPhase::End(void)
{
    // Pointer to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger);

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);

    TriggerMgrDisableTrigger(eAPRV_PtInhPressTrigger_InPHigh);

    PressureSensorActivePressureSensorSetPrimary();

//    S_Time += APRV_PHighPhase::S_CalcTimeSincePhaseStart();
}   // end End()

void APRV_PHighPhase::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.

     MgrBdSettingPhaseInAllSettings();

//     S_TimeElapsed = APRV_PHighPhase::S_CalcTimeSincePhaseStart();

     TargetPressure = mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress);
     RiseTime = mgrBdSettingPtrGetCurrentValue(eRiseTime);

     MgrControllerCMVServoControllerSetInhalationTarget(TargetPressure/10);


     // If it is time to auto-zero the exhalation pressure transducer,
     // kick start the auto-zero, update the state and switch the active
     // pressure transducer to the inhalation pressure transducer.  The
     // ventilator usually uses Pexh for monitoring and control.  During the
     // phase where Pexh is being auto-zeroed, Pinh is used for monitoring
     // and control.  It is always switched back at the end of exhalation.
//     if (eAzPending == PInhAzTestPtr->GetAzTestState())
//     {
//         if ( ActivePressureSensor->GetLastReading() <= (TargetPressure + 200)  )
//         {
//             RT_TASK* receiveTask1 = NULL;
//             receiveTask1 = (AzTaskS_SetEventFlag(INH_AZ_EVENT_FLAG);
//             ActivePressureSensor->SetPrimary(ExhPressSensor);
//             if((receiveTask1 !=0))
//                 PInhAzTestPtr->SetAzTestState(eAzActive);
//         }
//     }
//     else
//     {
//         if (eAzCompleted == PInhAzTestPtr->GetAzTestState())
//         {
//             ActivePressureSensor->SetPrimary(InhPressSensor);
//
//             // Otherwise if it is time to auto-zero Pinh, start the auto-zero and
//             // update the state.
//             if (eAzPending == PExhAzTestPtr->GetAzTestState())
//             {
//
//
//                 RT_TASK* receiveTask2 = NULL;
//                 receiveTask2 = (AzTaskS_SetEventFlag(EXH_AZ_EVENT_FLAG);
//                 if((receiveTask2 !=0))
//                     PExhAzTestPtr->SetAzTestState(eAzActive);
//             }
//         }
//     }
}   // end ProcessBdCycle()
E_Bool APRV_PHighPhase::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
{
    UNSIGNED tmp = minExhTime;
    E_Bool rtnValue = eFalse;
    if(tmp >= 1000)
    {
        tmp = 1000;
    }
    else if(tmp < 200)
    {
        tmp = 200;
    }

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if ((phaseMgrPtrGetCurrentPhaseId() == eAPRVHighPhase) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()
UNSIGNED APRV_PHighPhase::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.
//    return(CalcElapsedTime(S_InhStartTime));


}   // end S_CalcTimeSincePhaseStart()
UNSIGNED APRV_PHighPhase::GetTimeDuration(void)
{
    return S_Time;
}

UNSIGNED APRV_PHighPhase::GetTimeElapsed(void)
{
    return S_TimeElapsed;
}
void APRV_PHighPhase::ResetTimeDuration(void)
{
    S_Time = 0;
}

class APRV_PHighPhaseUnitMock : public BDTestFixture
{
public:
        APRV_PHighPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_PHighPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_PHighPhase = new APRV_PHighPhase();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_PHighPhase;
    }
    APRV_PHighPhase *MyAPRV_PHighPhase;
};

//test for APRV_PHighPhase* APRV_PHighPhase::S_GetInstance(void)
TEST_F(APRV_PHighPhaseUnitMock, S_GetInstance)
{
    MyAPRV_PHighPhase->S_GetInstance();

    EXPECT_NE(MyAPRV_PHighPhase->S_Instance, (void*)0);
}

//test for APRV_PHighPhase::APRV_PHighPhase(void) : InhPhase()
TEST_F(APRV_PHighPhaseUnitMock, APRV_PHighPhase)
{
    EXPECT_EQ(MyAPRV_PHighPhase->RiseTime, 0);
    EXPECT_EQ(MyAPRV_PHighPhase->TargetPressure, 0);
}

//test for void APRV_PHighPhase::Start(void)
TEST_F(APRV_PHighPhaseUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eBaseFlow)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerSetBiasFlow)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eRiseTime)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrProcessNewPhase()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_PHighPhaseStart(eMandatory)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableExhaleTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPHigh)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger(eApneaAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eHPHighAlarmTrigger)).Times(1);

    MyAPRV_PHighPhase->Start();
}

//test for void APRV_PHighPhase::End(void)
TEST_F(APRV_PHighPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger(eAPRV_PtInhPressTrigger_InPHigh)).Times(1);
    EXPECT_CALL(*_StepMotor, PressureSensorActivePressureSensorSetPrimary()).Times(1);

    MyAPRV_PHighPhase->End();
}

//test for void APRV_PHighPhase::ProcessBdCycle(void)
TEST_F(APRV_PHighPhaseUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eRiseTime)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    MyAPRV_PHighPhase->ProcessBdCycle();
}

//test for E_Bool APRV_PHighPhase::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
TEST_F(APRV_PHighPhaseUnitMock, S_IsInUnrestrictedPhase_0)
{
    MyAPRV_PHighPhase->S_TimeElapsed = 200;

    EXPECT_EQ(MyAPRV_PHighPhase->S_IsInUnrestrictedPhase(0), eFalse);
}

TEST_F(APRV_PHighPhaseUnitMock, S_IsInUnrestrictedPhase_1)
{
    MyAPRV_PHighPhase->S_TimeElapsed = 1001;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVHighPhase));

    EXPECT_EQ(MyAPRV_PHighPhase->S_IsInUnrestrictedPhase(0), eTrue);
}

//test for UNSIGNED APRV_PHighPhase::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_PHighPhaseUnitMock, S_CalcTimeSincePhaseStart)
{
    //all calling function is tested, not need to test
}

//test for UNSIGNED APRV_PHighPhase::GetTimeDuration(void)
TEST_F(APRV_PHighPhaseUnitMock, GetTimeDuration)
{
    MyAPRV_PHighPhase->S_Time = 100;

    EXPECT_EQ(MyAPRV_PHighPhase->GetTimeDuration(), 100);
}

//test for UNSIGNED APRV_PHighPhase::GetTimeElapsed(void)
TEST_F(APRV_PHighPhaseUnitMock, GetTimeElapsed)
{
    MyAPRV_PHighPhase->S_TimeElapsed = 19;

    EXPECT_EQ(MyAPRV_PHighPhase->GetTimeElapsed(), 19);
}

//test for void APRV_PHighPhase::ResetTimeDuration(void)
TEST_F(APRV_PHighPhaseUnitMock, ResetTimeDuration)
{
    MyAPRV_PHighPhase->S_Time = 100;

    MyAPRV_PHighPhase->ResetTimeDuration();

    EXPECT_EQ(MyAPRV_PHighPhase->S_Time, 0);
}

#endif //end define BD_PHASE_UNIT_TEST
