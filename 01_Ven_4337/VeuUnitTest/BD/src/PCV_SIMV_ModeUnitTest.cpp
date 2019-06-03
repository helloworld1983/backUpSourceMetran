#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "PCV_CPAP_Mode.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "StepMotorMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "MgrBdSettingMockHeader.h"

#include "PCV_SIMV_Mode.h"

using namespace ::testing;

PCV_SIMV_Mode* PCV_SIMV_Mode::S_Instance = NULL;

PCV_SIMV_Mode* PCV_SIMV_Mode::S_GetInstance(void)
{
    // If the pointer to the instance is NULL, create the object.
    if (NULL == S_Instance)
    {
        S_Instance = new PCV_SIMV_Mode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

PCV_SIMV_Mode::PCV_SIMV_Mode(void): MixedMode()
{
    IsSynOn = false;
}
void PCV_SIMV_Mode::Enter(E_VentilationMode fromModeId)
{
    IsSynOn = false;
//    MixedMode::Enter(fromModeId);
    PhaseMgrProcessNewVentType(ePressureVentilation);
    if(fromModeId != eApneaBackupMode)
    {
        MgrControllerCMVServoControllerResetIMVStaticVariables();
    }
    MgrControllerCMVServoControllerEnable();
    PhaseMgrProcessNewPhase(eExhPhase);

}
void PCV_SIMV_Mode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
//    MgrBdSetting* mgrBdSetting = MgrBdSetting::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    //    TriggerMgr::S_GetInstance()->DisableTrigger(eBpTimeTrigger);
    if (eMandWindow == Window)
    {
        // Start a mandatory inhalation
        if(eSwitchTypeOn == BDMgrBdSettingGetCurrentValue(eSyncPlus))
        {
            phaseMgrPtrProcessNewPhase(eSpontInhPhase);
            IsSynOn = true;
            TriggerMgrDisableTrigger(eBpTimeTrigger);
        }
        else
        {
            phaseMgrPtrProcessNewPhase(eMandInhPhase);
        }

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtrProcessNewPhase(eSpontInhPhase);
        if(eSwitchTypeOn == BDMgrBdSettingGetCurrentValue(eSyncPlus))
        {
            IsSynOn = true;
            TriggerMgrDisableTrigger(eBpTimeTrigger);
        }
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }

}   // end ProcessPtInhTrigger()

void PCV_SIMV_Mode::ProcessPtExhTrigger()
{
    BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive);
    //    EndSpontInhalation();

    if(IsSynOn)
    {
        UNSIGNED breathPeriod = triggerMgrPtrGetTimePeriod(eBpTimeTrigger);
        breathPeriod = breathPeriod - BDMgrBdSettingGetCurrentValue(eInhTime)*10;

        triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,breathPeriod);
    }
    IsSynOn = false;
}

void PCV_SIMV_Mode::ProcessBpTimeTrigger(void)
{
//    MgrBdSetting* mgrBdSetting = MgrBdSetting::S_GetInstance();
    E_PhaseId phaseId;          // Current Phase ID
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
//    phaseMgrPtr = PhaseMgr::S_GetInstance();
    switch (Window)
    {
        case eMandWindow:
            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtrGetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtrProcessNewPhase(eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open,
            // the ventilator just re-opens the mandatory window.
            Window = eMandWindow;

            if(eSwitchTypeOn == BDMgrBdSettingGetCurrentValue(eSyncPlus))
            {
                phaseMgrPtrProcessNewPhase(eMandInhPhase);
            }
            break;

        default:
            // Trigger fired in wrong window
            ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
            break;
    }

}   // end ProcessBpTimeTrigger()

void PCV_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case ePtExhTrigger:
            ProcessPtExhTrigger();
            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class PCV_SIMV_ModeUnitMock : public BDTestFixture
{
public:
    PCV_SIMV_ModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~PCV_SIMV_ModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPCV_SIMV_Mode = new PCV_SIMV_Mode();
    }
    virtual void TearDown(void)
    {
        delete MyPCV_SIMV_Mode;
    }
    PCV_SIMV_Mode *MyPCV_SIMV_Mode;
};

//test forPCV_SIMV_Mode* PCV_SIMV_Mode::S_GetInstance(void)
TEST_F(PCV_SIMV_ModeUnitMock, S_GetInstance)
{
    MyPCV_SIMV_Mode->S_GetInstance();

    EXPECT_NE(MyPCV_SIMV_Mode->S_Instance, (void*)0);
}

//test for PCV_SIMV_Mode::PCV_SIMV_Mode(void): MixedMode()
TEST_F(PCV_SIMV_ModeUnitMock, PCV_SIMV_Mode)
{
    EXPECT_EQ(MyPCV_SIMV_Mode->IsSynOn, false);
}

//test for void PCV_SIMV_Mode::Enter(E_VentilationMode fromModeId)
TEST_F(PCV_SIMV_ModeUnitMock, Enter)
{
    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewVentType(ePressureVentilation)).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerResetIMVStaticVariables()).Times(1);

    EXPECT_CALL(*_StepMotor, MgrControllerCMVServoControllerEnable()).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, PhaseMgrProcessNewPhase(eExhPhase)).Times(1);

    MyPCV_SIMV_Mode->Enter(eHardwareErrorMode);

    EXPECT_EQ(MyPCV_SIMV_Mode->IsSynOn, false);
}

//test for void PCV_SIMV_Mode::ProcessPtInhTrigger(void)
TEST_F(PCV_SIMV_ModeUnitMock, PCV_SIMV_Mode_0)
{
    MyPCV_SIMV_Mode->Window = eMandWindow;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eSyncPlus)).Times(1).WillOnce(Return(eSwitchTypeOn));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eSpontInhPhase)).Times(1);

    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger(eBpTimeTrigger)).Times(1);

    MyPCV_SIMV_Mode->ProcessPtInhTrigger();

    EXPECT_EQ(MyPCV_SIMV_Mode->IsSynOn, true);
    EXPECT_EQ(MyPCV_SIMV_Mode->Window, eSpontWindow);
}
TEST_F(PCV_SIMV_ModeUnitMock, PCV_SIMV_Mode_1)
{
    MyPCV_SIMV_Mode->Window = eMandWindow;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eSyncPlus)).Times(1).WillOnce(Return(eSwitchTypeOff));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyPCV_SIMV_Mode->ProcessPtInhTrigger();
    EXPECT_EQ(MyPCV_SIMV_Mode->Window, eSpontWindow);
}
TEST_F(PCV_SIMV_ModeUnitMock, PCV_SIMV_Mode_2)
{
    MyPCV_SIMV_Mode->Window = eSpontWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eSpontInhPhase)).Times(1);

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eSyncPlus)).Times(1).WillOnce(Return(eSwitchTypeOn));

    EXPECT_CALL(*_TriggerMgr, TriggerMgrDisableTrigger(eBpTimeTrigger)).Times(1);

    MyPCV_SIMV_Mode->ProcessPtInhTrigger();
    EXPECT_EQ(MyPCV_SIMV_Mode->IsSynOn, true);
}

//test for void PCV_SIMV_Mode::ProcessPtExhTrigger()
TEST_F(PCV_SIMV_ModeUnitMock, ProcessPtExhTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive)).Times(1);

    MyPCV_SIMV_Mode->IsSynOn = true;

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpTimeTrigger)).Times(1).WillOnce(Return(20));
    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eInhTime)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpTimeTrigger,10)).Times(1);

    MyPCV_SIMV_Mode->ProcessPtExhTrigger();
    EXPECT_EQ(MyPCV_SIMV_Mode->IsSynOn, false);
}

//test for void PCV_SIMV_Mode::ProcessBpTimeTrigger(void)
TEST_F(PCV_SIMV_ModeUnitMock, ProcessBpTimeTrigger_0)
{
    MyPCV_SIMV_Mode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyPCV_SIMV_Mode->ProcessBpTimeTrigger();

}
TEST_F(PCV_SIMV_ModeUnitMock, ProcessBpTimeTrigger_1)
{
    MyPCV_SIMV_Mode->Window = eSpontWindow;

    EXPECT_CALL(*_MgrBdSetting, BDMgrBdSettingGetCurrentValue(eSyncPlus)).Times(1).WillOnce(Return(eSwitchTypeOn));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyPCV_SIMV_Mode->ProcessBpTimeTrigger();
    EXPECT_EQ(MyPCV_SIMV_Mode->Window, eMandWindow);

}

//test for void PCV_SIMV_Mode::ProcessTriggerID (E_TriggerId id)
TEST_F(PCV_SIMV_ModeUnitMock, ProcessTriggerID)
{
    //no source, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
