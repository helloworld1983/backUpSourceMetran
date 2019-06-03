#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"

#include "DebugTrace.h"
#include "APRVMode.h"
using namespace ::testing;

#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "TimerMockHeader.h"
#include "PdTaskMockHeader.h"
#include "StepMotorMockHeader.h"
#include "MgrBdSettingMockHeader.h"


#include "PdInterface.h"

#include "EPhaseId.h"

APRVMode* APRVMode::S_Instance = NULL;

APRVMode* APRVMode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new APRVMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
APRVMode::APRVMode() : SpontMode(),Timer("TimerOfAPRVMode", 1000, pdTRUE)
{
    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
    IsPtInhTrigger = false;
    mPressureWindow = ePLowWindow;
}
void APRVMode::Enter(E_VentilationMode fromModeId)
{
    const int APRV_UPDATE_BRTRIGGER = 60000;
    mTimeWindow = eNot_ReleaseWindow;
    mPressureWindow = ePLowWindow;

    IsInReleaseWindow = false;
    IsPtInhTrigger = false;

    PhaseMgrProcessNewVentType(ePressureVentilation);
    MgrControllerCMVServoControllerResetIMVStaticVariables();
    MgrControllerCMVServoControllerEnable();

//    SpontMode::Enter(fromModeId);

    //mStart PLow phase
//    ProcessAPRVPHighTimeTrigger();

    triggerMgrPtrEnableTriggerWith(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER);
    triggerMgrPtrEnableTrigger(eExtremeHigh_PHigh_Trigger);
}
void APRVMode::ProcessPtInhTrigger()
{
    if(mPressureWindow == ePLowWindow)
    {
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
        phaseMgrPtrProcessNewPhase(eSpontInhPhase_InAPRV_PLow);
        IsPtInhTrigger = false;
    }
    else
    {
        triggerMgrPtrDisableTrigger(eHPHighAlarmTrigger);
        IsPtInhTrigger = true;
        switch(mTimeWindow)
        {
            case eNot_ReleaseWindow:
                IsInReleaseWindow = false;
                break;
            case eReleaseWindow:
                IsInReleaseWindow = true;
                triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger);
                mPressureWindow = ePLowWindow;
                break;
        }

        //Initialize pointer to the PhaseMgr object
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        phaseMgrPtrProcessNewPhase(eSpontInhPhase_InAPRV_PHigh);
    }
}
void APRVMode::ProcessHipAlarmTrigger(void)
{
    ProcessAPRVPHighTimeTrigger();
}
void APRVMode::ProcessItLongAlarmTimeTrigger()
{
    //End Inhalation
    if(IsInReleaseWindow)
    {
        EndSpontInhalation_InReleaseWindow();
    }
    else
    {
        EndSpontInhalation();
    }
    //Active ITimeTooLong
    BDAlarmTaskSetIsActive(eITimeTooLong, eActive);
}
void APRVMode::ProcessPtExhTrigger()
{
    //Clear ITimeTooLong
    BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive);

    if(IsInReleaseWindow)
    {
        EndSpontInhalation_InReleaseWindow();
    }
    else
    {
        EndSpontInhalation();
    }
}
void APRVMode::Exit()
{
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    triggerMgrPtrDisableTrigger(eAPRVLowPressTimeTrigger);
    triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger);
    triggerMgrPtrDisableTrigger(eExtremeHigh_PHigh_Trigger);
    triggerMgrPtrDisableTrigger(eHPHighAlarmTrigger);

    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
    //    CMVServoController->Disable();
}
void APRVMode::ProcessAPRVPLowTimeTrigger(void)
{
    mPressureWindow = ePHighWindow;
    PhaseMgrProcessNewPhase(eAPRVHighPhase);
    triggerMgrPtrDisableTrigger(eAPRVLowPressTimeTrigger);
    triggerMgrPtrEnableTrigger(eAPRVHighPressTimeTrigger);

    //    LONG time = CalculateReleaseWindowTime()*10;
    //    Disable();
    //    Reset(time,time,true);
    //    Enable();
}
void APRVMode::ProcessAPRVPHighTimeTrigger(void)
{
    if(phaseMgrPtrGetCurrentPhaseId() == eSpontInhPhase_InAPRV_PHigh)
    {
        BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive);
    }
    mPressureWindow = ePLowWindow;
    PhaseMgrProcessNewPhase(eAPRVLowPhase);
    triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger);
    triggerMgrPtrEnableTrigger(eAPRVLowPressTimeTrigger);

    //Disable timer
    mStop();
    mTimeWindow = eNot_ReleaseWindow;
    IsInReleaseWindow = false;
}
void APRVMode::ExpirationRoutine()
{
    //Disable timer
    mStop();
    mTimeWindow = eReleaseWindow;
}
void APRVMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case ePtExhTrigger:
            ProcessPtExhTrigger();
            break;

        case eAPRVLowPressTimeTrigger:
            ProcessAPRVPLowTimeTrigger();
            break;

        case eAPRVHighPressTimeTrigger:
            ProcessAPRVPHighTimeTrigger();
            break;

        case eItLongAlarmTimeTrigger:
            ProcessItLongAlarmTimeTrigger();
            break;

        case eHipAlarmTrigger:
            ProcessHipAlarmTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        case eAPRVUpdateBRTrigger:
            ProcessAPRVUpdateBRTrigger();
            break;

        case eHPHighAlarmTrigger:
            ProcessHPHighAlarmTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
LONG APRVMode::CalculateReleaseWindowTime()
{
    LONG T_HighSetting = SettingGetCurrentValue(eAPRVTimeHigh);
    LONG percent25_THigh = T_HighSetting*25/100;
    LONG Ti_Limit = SettingGetCurrentValue(eTiLimit);

    LONG ReleaseWindowTime;

    if(Ti_Limit > percent25_THigh)
    {
        ReleaseWindowTime = percent25_THigh;
    }
    else
    {
        ReleaseWindowTime = Ti_Limit;
    }

    LONG time = (T_HighSetting - ReleaseWindowTime);

    return (time);
}
void APRVMode::EndSpontInhalation()
{
    // Initialize pointer to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    if(mPressureWindow == ePLowWindow)
    {
        //mStart APRV Low Pressure Phase
        phaseMgrPtrProcessNewPhase(eAPRVExhPhase_InPLow);
    }
    else
    {
        phaseMgrPtrProcessNewPhase(eAPRVExhPhase_InPHigh);
    }
}
void APRVMode::EndSpontInhalation_InReleaseWindow()
{
    ProcessAPRVPHighTimeTrigger();
}
void APRVMode::ProcessApneaAlarmTimeTrigger()
{
    BDAlarmTaskSetIsActive(eApnea, eActive);

    triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger);
}
void APRVMode::ProcessAPRVUpdateBRTrigger()
{
    const int APRV_UPDATE_BRTRIGGER = 60000;

    triggerMgrPtrDisableTrigger(eAPRVUpdateBRTrigger);
    PdTaskS_SetEventFlag(UPDATE_APRV_BR_FLAG);
    triggerMgrPtrEnableTriggerWith(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER);
}
void APRVMode::ProcessHPHighAlarmTrigger()
{
    ProcessAPRVPHighTimeTrigger();
}

class APRVModeUnitMock : public BDTestFixture
{
public:
    APRVModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~APRVModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAPRVMode = new APRVMode();
    }
    virtual void TearDown(void)
    {
        delete MyAPRVMode;
    }
    APRVMode *MyAPRVMode;
};

//test for APRVMode* APRVMode::S_GetInstance(void)
TEST_F(APRVModeUnitMock, S_GetInstance)
{
    MyAPRVMode->S_GetInstance();

    EXPECT_NE(MyAPRVMode->S_Instance, (void*)0);
}

//test for APRVMode::APRVMode() : SpontMode(),Timer("TimerOfAPRVMode", 1000, pdTRUE)
TEST_F(APRVModeUnitMock, SpontMode)
{
    EXPECT_EQ(MyAPRVMode->mTimeWindow, eNot_ReleaseWindow);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, false);
    EXPECT_EQ(MyAPRVMode->IsPtInhTrigger, false);
    EXPECT_EQ(MyAPRVMode->mPressureWindow, ePLowWindow);
}

//test for void APRVMode::Enter(E_VentilationMode fromModeId)
TEST_F(APRVModeUnitMock, Enter)
{
    const int APRV_UPDATE_BRTRIGGER = 60000;

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);
    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eExtremeHigh_PHigh_Trigger)).Times(1);

    MyAPRVMode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyAPRVMode->mTimeWindow, eNot_ReleaseWindow);
    EXPECT_EQ(MyAPRVMode->mPressureWindow, ePLowWindow);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, false);
    EXPECT_EQ(MyAPRVMode->IsPtInhTrigger, false);
}

//test for void APRVMode::ProcessPtInhTrigger()
TEST_F(APRVModeUnitMock, ProcessPtInhTrigger_1)
{
    MyAPRVMode->mPressureWindow = ePLowWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eSpontInhPhase_InAPRV_PLow)).Times(1);

    MyAPRVMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyAPRVMode->IsPtInhTrigger, false);

}
TEST_F(APRVModeUnitMock, ProcessPtInhTrigger_2)
{
    MyAPRVMode->mPressureWindow = ePHighWindow;
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHPHighAlarmTrigger)).Times(1);

    MyAPRVMode->mTimeWindow = eNot_ReleaseWindow;


    MyAPRVMode->ProcessPtInhTrigger();
    EXPECT_EQ(MyAPRVMode->IsPtInhTrigger, true);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, false);

}
TEST_F(APRVModeUnitMock, ProcessPtInhTrigger_3)
{
    MyAPRVMode->mPressureWindow = ePHighWindow;
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHPHighAlarmTrigger)).Times(1);

    MyAPRVMode->mTimeWindow = eReleaseWindow;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger)).Times(1);

    MyAPRVMode->ProcessPtInhTrigger();
    EXPECT_EQ(MyAPRVMode->IsPtInhTrigger, true);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, true);
    EXPECT_EQ(MyAPRVMode->mPressureWindow, ePLowWindow);

}

//test for void APRVMode::ProcessHipAlarmTrigger(void)
TEST_F(APRVModeUnitMock, ProcessHipAlarmTrigger)
{
    //calling function is tested, not need to test
}

//test for void APRVMode::ProcessItLongAlarmTimeTrigger()
TEST_F(APRVModeUnitMock, ProcessItLongAlarmTimeTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eActive)).Times(1);

    MyAPRVMode->ProcessItLongAlarmTimeTrigger();
}

//test for void APRVMode::ProcessPtExhTrigger()
TEST_F(APRVModeUnitMock, ProcessPtExhTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive)).Times(1);

    MyAPRVMode->ProcessPtExhTrigger();
}

//test for void APRVMode::Exit()
TEST_F(APRVModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVLowPressTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eExtremeHigh_PHigh_Trigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHPHighAlarmTrigger)).Times(1);

    MyAPRVMode->Exit();

    EXPECT_EQ(MyAPRVMode->mTimeWindow, eNot_ReleaseWindow);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, false);
}

//test for void APRVMode::ProcessAPRVPLowTimeTrigger(void)
TEST_F(APRVModeUnitMock, ProcessAPRVPLowTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eAPRVHighPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVLowPressTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAPRVHighPressTimeTrigger)).Times(1);
    MyAPRVMode->ProcessAPRVPLowTimeTrigger();

    EXPECT_EQ(MyAPRVMode->mPressureWindow, ePHighWindow);
}

//test for void APRVMode::ProcessAPRVPHighTimeTrigger(void)
TEST_F(APRVModeUnitMock, ProcessAPRVPHighTimeTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eSpontInhPhase_InAPRV_PHigh));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eAPRVLowPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVHighPressTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eAPRVLowPressTimeTrigger)).Times(1);

    EXPECT_CALL(*_Timer, mStop()).Times(1);

    MyAPRVMode->ProcessAPRVPHighTimeTrigger();

    EXPECT_EQ(MyAPRVMode->mPressureWindow, ePLowWindow);
    EXPECT_EQ(MyAPRVMode->mTimeWindow, eNot_ReleaseWindow);
    EXPECT_EQ(MyAPRVMode->IsInReleaseWindow, false);
}

//test for void APRVMode::ExpirationRoutine()
TEST_F(APRVModeUnitMock, ExpirationRoutine)
{
    EXPECT_CALL(*_Timer, mStop()).Times(1);

    MyAPRVMode->ExpirationRoutine();

    EXPECT_EQ(MyAPRVMode->mTimeWindow, eReleaseWindow);
}

//test for void APRVMode::ProcessTriggerID (E_TriggerId id)
TEST_F(APRVModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for LONG APRVMode::CalculateReleaseWindowTime()
TEST_F(APRVModeUnitMock, CalculateReleaseWindowTime_1)
{
    EXPECT_CALL(*_MgrBdSetting, SettingGetCurrentValue(eAPRVTimeHigh)).Times(1).WillOnce(Return(100/25));
    EXPECT_CALL(*_MgrBdSetting, SettingGetCurrentValue(eTiLimit)).Times(1).WillOnce(Return(50));


    EXPECT_EQ(MyAPRVMode->CalculateReleaseWindowTime(), 1);

}
TEST_F(APRVModeUnitMock, CalculateReleaseWindowTime_2)
{
    EXPECT_CALL(*_MgrBdSetting, SettingGetCurrentValue(eAPRVTimeHigh)).Times(1).WillOnce(Return(100/25));
    EXPECT_CALL(*_MgrBdSetting, SettingGetCurrentValue(eTiLimit)).Times(1).WillOnce(Return(0));


    EXPECT_EQ(MyAPRVMode->CalculateReleaseWindowTime(), 1);

}

//test for void APRVMode::EndSpontInhalation()
TEST_F(APRVModeUnitMock, EndSpontInhalation_0)
{
    MyAPRVMode->mPressureWindow = ePLowWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eAPRVExhPhase_InPLow)).Times(1);

    MyAPRVMode->EndSpontInhalation();
}
TEST_F(APRVModeUnitMock, EndSpontInhalation_1)
{
    MyAPRVMode->mPressureWindow = ePHighWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eAPRVExhPhase_InPHigh)).Times(1);

    MyAPRVMode->EndSpontInhalation();
}

//test for void APRVMode::EndSpontInhalation_InReleaseWindow()
TEST_F(APRVModeUnitMock, EndSpontInhalation_InReleaseWindow)
{
    //calling function is tested, not need to test
}

//test for void APRVMode::ProcessApneaAlarmTimeTrigger()
TEST_F(APRVModeUnitMock, ProcessApneaAlarmTimeTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eActive)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    MyAPRVMode->ProcessApneaAlarmTimeTrigger();
}

//test for void APRVMode::ProcessAPRVUpdateBRTrigger()
TEST_F(APRVModeUnitMock, ProcessAPRVUpdateBRTrigger)
{
    const int APRV_UPDATE_BRTRIGGER = 60000;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eAPRVUpdateBRTrigger)).Times(1);

    EXPECT_CALL(*_PdTask, PdTaskS_SetEventFlag(UPDATE_APRV_BR_FLAG)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eAPRVUpdateBRTrigger,APRV_UPDATE_BRTRIGGER)).Times(1);

    MyAPRVMode->ProcessAPRVUpdateBRTrigger();
}

//test for void APRVMode::ProcessHPHighAlarmTrigger()
TEST_F(APRVModeUnitMock, ProcessHPHighAlarmTrigger)
{
    //all calling function is tested, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
