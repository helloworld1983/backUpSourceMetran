#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_ExhPhase_InPLow.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

UNSIGNED APRV_ExhPhase_InPLow::S_ExhStartTime = 0;
UNSIGNED APRV_ExhPhase_InPLow::S_TimeElapsed = 0;
UNSIGNED APRV_ExhPhase_InPLow::S_Time = 0;
APRV_ExhPhase_InPLow* APRV_ExhPhase_InPLow::S_Instance = NULL;

APRV_ExhPhase_InPLow* APRV_ExhPhase_InPLow::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_ExhPhase_InPLow();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
UNSIGNED APRV_ExhPhase_InPLow::GetTimeDuration (void)
{
    return S_Time;
}
void APRV_ExhPhase_InPLow::ResetTimeDuration (void)
{
    S_Time = 0;
}
APRV_ExhPhase_InPLow::APRV_ExhPhase_InPLow(void) : Phase()
{
    TargetPressure = 0;
}   // end ExhPhase()
void APRV_ExhPhase_InPLow::Start(void)
{
//    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
//    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtrGetCurrentValue(eBaseFlow);
    SHORT TempBiasFlow = BiasFlow / 100;
    BDRequestDevice(eCMVServoControllerSetBiasFlow);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = SettingGetCurrentValue(eAPRVLowInhPress);

    MgrControllerCMVServoControllerSetExhalationTarget(TargetPressure/10);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
//    S_ExhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtrProcessNewPhase();
    // Update Patient Data
    MgrBreathDataAPRV_ExhPhaseStart_InPLow(eExhalation);


    TriggerMgrEnableExhaleTrigger();

    TriggerMgrEnableTrigger(ePtInhFlowTrigger);

    TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPLow);
}   // end Start()
void APRV_ExhPhase_InPLow::End(void)
{
    // Pointer to the Trigger Manager
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);

    triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPLow);

    PressureSensorActivePressureSensorSetPrimary();

    S_Time += APRV_ExhPhase_InPLow::S_CalcTimeSincePhaseStart();

}   // end End()
void APRV_ExhPhase_InPLow::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.
     MgrBdSettingPhaseInAllSettings();

//     S_TimeElapsed = APRV_PLowPhase::S_CalcTimeSincePhaseStart();

     TargetPressure = BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress);

     MgrControllerCMVServoControllerSetExhalationTarget(TargetPressure/10);

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
//             receiveTask1 = (AzTask::S_GetInstance())->S_SetEventFlag(INH_AZ_EVENT_FLAG);
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
//                 receiveTask2 = (AzTask::S_GetInstance())->S_SetEventFlag(EXH_AZ_EVENT_FLAG);
//                 if((receiveTask2 !=0))
//                     PExhAzTestPtr->SetAzTestState(eAzActive);
//             }
//         }
//     }
}   // end ProcessBdCycle()
E_Bool APRV_ExhPhase_InPLow::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
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
    if ((phaseMgrPtrGetCurrentPhaseId() == eAPRVExhPhase_InPLow) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()
UNSIGNED APRV_ExhPhase_InPLow::S_CalcTimeSincePhaseStart(void)
{

    // Return the time elapsed since the start of the exhalation phase.
//    return(CalcElapsedTime(S_ExhStartTime));

}   // end S_CalcTimeSincePhaseStart()

class APRV_ExhPhase_InPLowUnitMock : public BDTestFixture
{
public:
    APRV_ExhPhase_InPLowUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_ExhPhase_InPLowUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_ExhPhase_InPLow = new APRV_ExhPhase_InPLow();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_ExhPhase_InPLow;
    }
    APRV_ExhPhase_InPLow *MyAPRV_ExhPhase_InPLow;
};

//test for APRV_ExhPhase_InPLow* APRV_ExhPhase_InPLow::S_GetInstance(vo
TEST_F(APRV_ExhPhase_InPLowUnitMock, S_GetInstance)
{
    MyAPRV_ExhPhase_InPLow->S_GetInstance();

    EXPECT_NE(MyAPRV_ExhPhase_InPLow->S_Instance, (void*)0);
}

//test for UNSIGNED APRV_ExhPhase_InPLow::GetTimeDuration (void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, GetTimeDuration)
{
    MyAPRV_ExhPhase_InPLow->S_Time = 12;

    MyAPRV_ExhPhase_InPLow->GetTimeDuration();

    EXPECT_EQ(MyAPRV_ExhPhase_InPLow->S_Time, 12);
}

//test for void APRV_ExhPhase_InPLow::ResetTimeDuration (void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, ResetTimeDuration)
{
    MyAPRV_ExhPhase_InPLow->ResetTimeDuration();

    EXPECT_EQ(MyAPRV_ExhPhase_InPLow->S_Time, 0);
}

//test for APRV_ExhPhase_InPLow::APRV_ExhPhase_InPLow(void) : Phase()
TEST_F(APRV_ExhPhase_InPLowUnitMock, TargetPressure)
{
    EXPECT_EQ(MyAPRV_ExhPhase_InPLow->TargetPressure, 0);
}

//test for void APRV_ExhPhase_InPLow::Start(void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(100));

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerSetBiasFlow)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, SettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrProcessNewPhase()).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_ExhPhaseStart_InPLow(eExhalation)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableExhaleTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPLow)).Times(1);

    MyAPRV_ExhPhase_InPLow->Start();
}

//test for void APRV_ExhPhase_InPLow::End(void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPLow)).Times(1);

    EXPECT_CALL(*_StepMotor, PressureSensorActivePressureSensorSetPrimary()).Times(1);

    MyAPRV_ExhPhase_InPLow->End();
}

//test for void APRV_ExhPhase_InPLow::ProcessBdCycle(void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetExhalationTarget(10/10)).Times(1);

    MyAPRV_ExhPhase_InPLow->ProcessBdCycle();
}

//test for E_Bool APRV_ExhPhase_InPLow::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
TEST_F(APRV_ExhPhase_InPLowUnitMock, S_IsInUnrestrictedPhase_0)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVExhPhase_InPLow));
    MyAPRV_ExhPhase_InPLow->S_TimeElapsed = 2000;

    EXPECT_EQ(MyAPRV_ExhPhase_InPLow->S_IsInUnrestrictedPhase(1000), eTrue);
}
TEST_F(APRV_ExhPhase_InPLowUnitMock, S_IsInUnrestrictedPhase_1)
{
    MyAPRV_ExhPhase_InPLow->S_TimeElapsed = 2;

    EXPECT_EQ(MyAPRV_ExhPhase_InPLow->S_IsInUnrestrictedPhase(10), eFalse);
}

//test for UNSIGNED APRV_ExhPhase_InPLow::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_ExhPhase_InPLowUnitMock, S_CalcTimeSincePhaseStart)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_PHASE_UNIT_TEST
