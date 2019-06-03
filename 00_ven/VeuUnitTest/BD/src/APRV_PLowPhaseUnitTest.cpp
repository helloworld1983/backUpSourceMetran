#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_PLowPhase.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"
// Define statics
UNSIGNED APRV_PLowPhase::S_ExhStartTime = 0;
UNSIGNED APRV_PLowPhase::S_TimeElapsed = 0;
UNSIGNED APRV_PLowPhase::S_Time = 0;
APRV_PLowPhase* APRV_PLowPhase::S_Instance = NULL;
APRV_PLowPhase* APRV_PLowPhase::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_PLowPhase();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
UNSIGNED APRV_PLowPhase::GetTimeDuration (void)
{
    return S_Time;
}
void APRV_PLowPhase::ResetTimeDuration (void)
{
    S_Time = 0;
}
APRV_PLowPhase::APRV_PLowPhase(void) : Phase()
{
    TargetPressure = 0;
}   // end ExhPhase()
void APRV_PLowPhase::Start(void)
{

    //    MgrBdSetting* mgrBdSettingPtr;  // pointer to MgrBdSetting

    // Get a handle to the BD Setting Manager
    //    mgrBdSettingPtr = MgrBdSetting::S_GetInstance();


    SHORT BiasFlow = mgrBdSettingPtrGetCurrentValue(eBaseFlow);
    SHORT TempBiasFlow = BiasFlow / 100;
    BDRequestDevice(eCMVServoControllerSetBiasFlow);

    // Save the target pressure so that it does not have to be retrieved
    // every BD cycle.

    TargetPressure = BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress);
    MgrControllerCMVServoControllerSetExhalationTarget(TargetPressure/10);

    // During expiratory this bit should be clear.
    //    InpedanceLine->SetState(eOff);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
    //    S_ExhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;

    mgrBdSettingPtrProcessNewPhase();
    // Update Patient Data
    MgrBreathDataAPRV_PLowPhaseStart();


    TriggerMgrEnableExhaleTrigger();

    TriggerMgrEnableTrigger(ePtInhFlowTrigger);

    TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPLow);

    TriggerMgrDisableTrigger(eHPHighAlarmTrigger);
}   // end Start()
void APRV_PLowPhase::End(void)
{
    // Pointer to the Trigger Manager
    //    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger);

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);

    triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPLow);

    PressureSensorActivePressureSensorSetPrimary();

    //    S_Time += APRV_PLowPhase::S_CalcTimeSincePhaseStart();
}   // end End()
void APRV_PLowPhase::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.
    MgrBdSettingPhaseInAllSettings();

    S_TimeElapsed = APRV_PLowPhase::S_CalcTimeSincePhaseStart();

    TargetPressure = BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress);

    MgrControllerCMVServoControllerSetExhalationTarget(TargetPressure/10);

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
    //            receiveTask1 = (AzTaskS_SetEventFlag(INH_AZ_EVENT_FLAG);
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
    //                receiveTask2 = (AzTaskS_SetEventFlag(EXH_AZ_EVENT_FLAG);
    //                if((receiveTask2 !=0))
    //                    PExhAzTestPtr->SetAzTestState(eAzActive);
    //            }
    //        }
    //    }
}   // end ProcessBdCycle()

E_Bool APRV_PLowPhase::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
{
    UNSIGNED tmp = minExhTime;
    E_Bool rtnValue = eFalse;
    if(tmp >= 1000)
    {
        tmp = 1000;
    }
    else if(tmp < 200)
    {
        tmp=200;
    }

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if ((phaseMgrPtrGetCurrentPhaseId() == eAPRVLowPhase) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()
UNSIGNED APRV_PLowPhase::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.
    //    return(CalcElapsedTime(S_ExhStartTime));

}   // end S_CalcTimeSincePhaseStart()

class APRV_PLowPhaseUnitMock : public BDTestFixture
{
public:
    APRV_PLowPhaseUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRV_PLowPhaseUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRV_PLowPhase = new APRV_PLowPhase();
    }
    virtual void TearDown(void)
    {
        delete MyAPRV_PLowPhase;
    }
    APRV_PLowPhase *MyAPRV_PLowPhase;
};

//test for APRV_PLowPhase* APRV_PLowPhase::S_GetInstance(void)
TEST_F(APRV_PLowPhaseUnitMock, S_GetInstance)
{
    MyAPRV_PLowPhase->S_GetInstance();

    EXPECT_NE(MyAPRV_PLowPhase->S_Instance, (void*)0);
}

//test for UNSIGNED APRV_PLowPhase::GetTimeDuration (void)
TEST_F(APRV_PLowPhaseUnitMock, GetTimeDuration)
{
    MyAPRV_PLowPhase->S_Time = 100;

    EXPECT_EQ(100, MyAPRV_PLowPhase->GetTimeDuration());
}

//test for void APRV_PLowPhase::ResetTimeDuration (void)
TEST_F(APRV_PLowPhaseUnitMock, ResetTimeDuration)
{
    MyAPRV_PLowPhase->S_Time = 100;
    MyAPRV_PLowPhase->ResetTimeDuration();

    EXPECT_EQ(0, MyAPRV_PLowPhase->S_Time);
}

//test for APRV_PLowPhase::APRV_PLowPhase(void) : Phase()
TEST_F(APRV_PLowPhaseUnitMock, APRV_PLowPhase)
{
    EXPECT_EQ(MyAPRV_PLowPhase->TargetPressure, 0);
}

//test for void APRV_PLowPhase::Start(void)
TEST_F(APRV_PLowPhaseUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eBaseFlow)).Times(1).WillOnce(Return(100));

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerSetBiasFlow)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrProcessNewPhase()).Times(1);
    EXPECT_CALL(*_MgrBdSetting, MgrBreathDataAPRV_PLowPhaseStart()).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableExhaleTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPLow)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger(eHPHighAlarmTrigger)).Times(1);

    MyAPRV_PLowPhase->Start();
}

//test for void APRV_PLowPhase::End(void)
TEST_F(APRV_PLowPhaseUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eExtremeHighPEEPTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPLow)).Times(1);

    EXPECT_CALL(*_StepMotor, PressureSensorActivePressureSensorSetPrimary()).Times(1);

    MyAPRV_PLowPhase->End();

}

//test for void APRV_PLowPhase::ProcessBdCycle(void)
TEST_F(APRV_PLowPhaseUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVLowInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetExhalationTarget(1)).Times(1);

    MyAPRV_PLowPhase->ProcessBdCycle();
}

//test for E_Bool APRV_PLowPhase::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
TEST_F(APRV_PLowPhaseUnitMock, S_IsInUnrestrictedPhase_0)
{
    MyAPRV_PLowPhase->S_TimeElapsed = 1001;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVLowPhase));


    EXPECT_EQ(MyAPRV_PLowPhase->S_IsInUnrestrictedPhase(1000), eTrue);
}
TEST_F(APRV_PLowPhaseUnitMock, S_IsInUnrestrictedPhase_1)
{
    MyAPRV_PLowPhase->S_TimeElapsed = 201;

    EXPECT_EQ(MyAPRV_PLowPhase->S_IsInUnrestrictedPhase(0), eFalse);
}

//test for UNSIGNED APRV_PLowPhase::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_PLowPhaseUnitMock, S_CalcTimeSincePhaseStart)
{
    //calling function is tested, not need to test
}

#endif //end define BD_PHASE_UNIT_TEST
