#include "EnableTestCase.h"
#ifdef BD_PHASE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "APRV_ExhPhase_InPHigh.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "BreathMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "StepMotorMockHeader.h"

APRV_ExhPhase_InPHigh* APRV_ExhPhase_InPHigh::S_Instance = NULL;
UNSIGNED APRV_ExhPhase_InPHigh::S_InhStartTime = 0;
UNSIGNED APRV_ExhPhase_InPHigh::S_TimeElapsed = 0;
UNSIGNED APRV_ExhPhase_InPHigh::S_Time = 0;

APRV_ExhPhase_InPHigh* APRV_ExhPhase_InPHigh::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new APRV_ExhPhase_InPHigh();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()
UNSIGNED APRV_ExhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
{
    // Return the time elapsed since the start of the exhalation phase.
    //    return(CalcElapsedTime(S_InhStartTime));

}   // end S_CalcTimeSincePhaseStart()

APRV_ExhPhase_InPHigh::APRV_ExhPhase_InPHigh(void) : InhPhase()
{
    RiseTime = 0;
    TargetPressure = 0;
}   // end ExhPhase()

void APRV_ExhPhase_InPHigh::Start(void)
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

    TargetPressure = BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress);
    RiseTime = BDMgrBdSettingGetCurrentValue(eRiseTime);

    MgrControllerCMVServoControllerSetInhalationTarget(TargetPressure/10);

    // Snapshot the current system time as the start of exhalation and reset
    // the elapsed time
    //    S_InhStartTime = SystemClock::Retrieve();
    S_TimeElapsed = 0;
    //    InhPhase::Start();

    mgrBdSettingPtrProcessNewPhase();
    // Update Patient Data
    MgrBreathDataAPRV_ExhPhaseStart_InPHigh(breathType);


    TriggerMgrEnableExhaleTrigger();
    TriggerMgrEnableTrigger(ePtInhFlowTrigger);
    TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPHigh);
}   // end Start()
void APRV_ExhPhase_InPHigh::End(void)
{
    // Pointer to the Trigger Manager
    //    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger);

    triggerMgrPtrDisableTrigger(ePtInhFlowTrigger);

    triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPHigh);


    BDRequestDevice(eActivePressureSensorSetPrimaryInhPressSensor);
    PressureSensorActivePressureSensorSetPrimary();


    //    S_Time += APRV_ExhPhase_InPHigh::S_CalcTimeSincePhaseStart();

    MgrBreathDataAPRV_ExhPhaseEnd_InPHigh();
}   // end End()
void APRV_ExhPhase_InPHigh::ProcessBdCycle(void)
{
    // Calculate the time elapsed since the start of exhalation.

    MgrBdSettingPhaseInAllSettings();

    S_TimeElapsed = APRV_ExhPhase_InPHigh::S_CalcTimeSincePhaseStart();

    TargetPressure = BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress);

    RiseTime = BDMgrBdSettingGetCurrentValue(eRiseTime);

    MgrControllerCMVServoControllerSetInhalationTarget(TargetPressure/10);

    // If it is time to auto-zero the exhalation pressure transducer,
    // kick start the auto-zero, update the state and switch the active
    // pressure transducer to the inhalation pressure transducer.  The
    // ventilator usually uses Pexh for monitoring and control.  During the
    // phase where Pexh is being auto-zeroed, Pinh is used for monitoring
    // and control.  It is always switched back at the end of exhalation.
    //        if (eAzPending == PInhAzTestPtr->GetAzTestState())
    //        {
    //            if ( ActivePressureSensor->GetLastReading() <= (TargetPressure + 200)  )
    //            {
    //                RT_TASK* receiveTask1 = NULL;
    //                receiveTask1 = (AzTaskS_SetEventFlag(INH_AZ_EVENT_FLAG);
    //                ActivePressureSensor->SetPrimary(ExhPressSensor);
    //                if((receiveTask1 !=0))
    //                    PInhAzTestPtr->SetAzTestState(eAzActive);
    //            }
    //        }
    //        else
    //        {
    //            if (eAzCompleted == PInhAzTestPtr->GetAzTestState())
    //            {
    //                ActivePressureSensor->SetPrimary(InhPressSensor);
    //
    //                // Otherwise if it is time to auto-zero Pinh, start the auto-zero and
    //                // update the state.
    //                if (eAzPending == PExhAzTestPtr->GetAzTestState())
    //                {
    //
    //
    //                    RT_TASK* receiveTask2 = NULL;
    //                    receiveTask2 = (AzTaskS_SetEventFlag(EXH_AZ_EVENT_FLAG);
    //                    if((receiveTask2 !=0))
    //                        PExhAzTestPtr->SetAzTestState(eAzActive);
    //                }
    //            }
    //        }
}   // end ProcessBdCycle()
E_Bool APRV_ExhPhase_InPHigh::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
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
    if ((BDPhaseMgrGetCurrentPhaseId() == eAPRVExhPhase_InPHigh) &&
            (S_TimeElapsed >= tmp))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()

class APRV_ExhPhase_InPHighUnitMock : public BDTestFixture
{
    public:
        APRV_ExhPhase_InPHighUnitMock(void) : BDTestFixture(new BaseModuleMock)
    {
    }
        virtual ~APRV_ExhPhase_InPHighUnitMock(void){}
        virtual void SetUp(void)
        {
            MyAPRV_ExhPhase_InPHigh = new APRV_ExhPhase_InPHigh();
        }
        virtual void TearDown(void)
        {
            delete MyAPRV_ExhPhase_InPHigh;
        }
        APRV_ExhPhase_InPHigh *MyAPRV_ExhPhase_InPHigh;
};

//test for APRV_ExhPhase_InPHigh* APRV_ExhPhase_InPHigh::S_GetInstance(void)
TEST_F(APRV_ExhPhase_InPHighUnitMock, S_GetInstance)
{
    MyAPRV_ExhPhase_InPHigh->S_GetInstance();

    EXPECT_NE(MyAPRV_ExhPhase_InPHigh->S_Instance, (void*)0);
}

//test for UNSIGNED APRV_ExhPhase_InPHigh::S_CalcTimeSincePhaseStart(void)
TEST_F(APRV_ExhPhase_InPHighUnitMock, S_CalcTimeSincePhaseStart)
{
    //all calling function is tested, not need to test
}

//test for APRV_ExhPhase_InPHigh::APRV_ExhPhase_InPHigh(void) : InhPhase()
TEST_F(APRV_ExhPhase_InPHighUnitMock, APRV_ExhPhase_InPHigh)
{
    EXPECT_EQ(MyAPRV_ExhPhase_InPHigh->RiseTime, 0);
    EXPECT_EQ(MyAPRV_ExhPhase_InPHigh->TargetPressure, 0);
}

//test for void APRV_ExhPhase_InPHigh::Start(void)
TEST_F(APRV_ExhPhase_InPHighUnitMock, Start)
{
    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrGetCurrentValue(eBaseFlow)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerSetBiasFlow)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eRiseTime)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, mgrBdSettingPtrProcessNewPhase()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_ExhPhaseStart_InPHigh(eMandatory)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableExhaleTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, TriggerMgrEnableTrigger(eAPRV_PtInhPressTrigger_InPHigh)).Times(1);

    MyAPRV_ExhPhase_InPHigh->Start();
}

//test for void APRV_ExhPhase_InPHigh::End(void)
TEST_F(APRV_ExhPhase_InPHighUnitMock, End)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eManualInhOpReqTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(ePtInhFlowTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRV_PtInhPressTrigger_InPHigh)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eActivePressureSensorSetPrimaryInhPressSensor)).Times(1);
    EXPECT_CALL(*_StepMotor, PressureSensorActivePressureSensorSetPrimary()).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataAPRV_ExhPhaseEnd_InPHigh()).Times(1);

    MyAPRV_ExhPhase_InPHigh->End();
}

//test for void APRV_ExhPhase_InPHigh::ProcessBdCycle(void)
TEST_F(APRV_ExhPhase_InPHighUnitMock, ProcessBdCycle)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBdSettingPhaseInAllSettings()).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eAPRVHighInhPress)).Times(1).WillOnce(Return(10));

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eRiseTime)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerSetInhalationTarget(10/10)).Times(1);

    MyAPRV_ExhPhase_InPHigh->ProcessBdCycle();
}

//test for E_Bool APRV_ExhPhase_InPHigh::S_IsInUnrestrictedPhase(UNSIGNED minExhTime)
TEST_F(APRV_ExhPhase_InPHighUnitMock, S_IsInUnrestrictedPhase_0)
{
    MyAPRV_ExhPhase_InPHigh->S_TimeElapsed = 201;

    EXPECT_EQ(MyAPRV_ExhPhase_InPHigh->S_IsInUnrestrictedPhase(0), eFalse);
}
TEST_F(APRV_ExhPhase_InPHighUnitMock, S_IsInUnrestrictedPhase_1)
{
    EXPECT_CALL(*_PhaseMgrPtr, BDPhaseMgrGetCurrentPhaseId()).Times(1).WillOnce(Return(eAPRVExhPhase_InPHigh));

    MyAPRV_ExhPhase_InPHigh->S_TimeElapsed = 1001;

    EXPECT_EQ(MyAPRV_ExhPhase_InPHigh->S_IsInUnrestrictedPhase(2000), eTrue);
}

#endif //end define BD_PHASE_UNIT_TEST
