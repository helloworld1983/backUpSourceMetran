#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "NormBreathingMode.h"
#include "DebugTrace.h"

#include "PhaseMgrMockHeader.h"
#include "ModeMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "BreathMgrMockHeader.h"

using namespace ::testing;

const UNSIGNED INIT_UPDATE_STAT_TIME = 350 / MS_PER_TICK; // 350ms in ticks

NormBreathingMode::NormBreathingMode (void) :
BreathingMode ()
{

}   // end NormBreathingMode()

void NormBreathingMode::Exit (void)
{
    // Get a handle to the TriggerMgr object
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable the normal mode specific triggers
    triggerMgrPtrDisableTrigger(eBpTimeTrigger);
    triggerMgrPtrDisableTrigger(eModeChangeTrigger);
    triggerMgrPtrDisableTrigger(eHip2AlarmTrigger);
    triggerMgrPtrDisableTrigger(eTotalGasLossTrigger);
    triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtrDisableTrigger(eDisconnectionAlarmTrigger);

}   // end Exit()

void NormBreathingMode::UpdateCommonTriggers (void)
{

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Enable triggers

    triggerMgrPtrEnableTrigger(eModeChangeTrigger);
    triggerMgrPtrEnableTrigger(eHip2AlarmTrigger);
    triggerMgrPtrEnableTrigger(eHipAlarmTrigger);
    triggerMgrPtrEnableTrigger(eTotalGasLossTrigger);
    triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    triggerMgrPtrEnableTrigger(eApneaAlarmTimeTrigger);
    triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger);
    triggerMgrPtrEnableTrigger(eOcclusionTrigger);

}   // end UpdateCommonTriggers()

void NormBreathingMode::UpdateAlarmStatus (void)
{
    SHORT status = 0;
//    TriggerMgr* triggerMgrPtr;

    // Notify AlarmStat objects that Occlusion, SVO and Apnea are not active
    status += (SHORT) BDAlarmTaskSetIsActive(eApnea, eNotActive);
    status += (SHORT) BDAlarmTaskSetIsActive(eOcclusion, eNotActive);
    status += (SHORT) BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive);

    // If any of the status updates was unsuccessful, reschedule the next update
    // for the short interval
    if(status != (SHORT) eSuccess)
    {
//        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,
                                      INIT_UPDATE_STAT_TIME);
    }

}   // UpdateAlarmStatus()

void NormBreathingMode::ProcessModeChangeTrigger (void)
{
    // Transition to the new mode
    ModeMgrGoToOperatorSetMode();

}   // end ProcessModeChangeTrigger()

void NormBreathingMode::ProcessItLongAlarmTimeTrigger (void)
{
    if(eNotActive == BDAlarmTaskGetIsActive(eITimeTooLong))
    {
        BDAlarmTaskSetIsActive(eITimeTooLong, eActive);
    }
    EndSpontInhalation();

}   // end ProcessItLongAlarmTimeTrigger()

void NormBreathingMode::ProcessPtExhTrigger (void)
{
    BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive);
    EndSpontInhalation ();

}   // ProcessPtExhTrigger()

void NormBreathingMode::EndSpontInhalation (void)
{
    // Initialize pointer to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance ();

    // Verify that the active phase is a spontaneous inhalation phase
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eSpontInhPhase);

    // Transition to exhalation.
    phaseMgrPtrProcessNewPhase (eExhPhase);

}   // end EndSpontInhalation()

void NormBreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        case eModeChangeTrigger:
            ProcessModeChangeTrigger();
            break;

        case eItLongAlarmTimeTrigger:
            ProcessItLongAlarmTimeTrigger();

        case ePtExhTrigger:
            ProcessPtExhTrigger();

        default:
            ASSERTION(eFalse);
            break;
    }
}

void NormBreathingMode::ProcessHipAlarmTrigger(void)
{
    // Initialize pointer to the PhaseMgr
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Verify that the active phase is a inhalation phase
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() != eExhPhase);

    // Transition to exhalation.
    phaseMgrPtrProcessNewPhase(eExhPhase);
}

void NormBreathingMode::ProcessExtremePressAlarmTrigger(void)
{
    ModeMgrGoToAlarmMode(eHighPressMode);
}   // end ProcessHip2AlarmTrigger()

void NormBreathingMode::ProcessOcclusionAlarmTrigger(void)
{
    ModeMgrGoToAlarmMode(eHighPressMode);
}

void NormBreathingMode::ProcessApneaAlarmTimeTrigger(void)
{
    ModeMgrGoToAlarmMode(eApneaBackupMode);
}   // end ProcessApneaAlarmTimeTrigger()

void NormBreathingMode::ProcessAlarmResetTrigger()
{
    ASSERTION(eFalse);
}

void NormBreathingMode::ProcessManualInhOpReqTrigger()
{
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    TriggerMgrReEnableTrigger(eBpTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtrProcessNewPhase(eMandInhPhase);

}

void NormBreathingMode::ProcessProxyDisconnectTrigger()
{
    triggerMgrPtrEnableTrigger(eProxyConnectTrigger);
    triggerMgrPtrDisableTrigger(eProxyDisconnectTrigger);
}

void NormBreathingMode::ProcessProxyConnectTrigger()
{
    triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger);
    triggerMgrPtrDisableTrigger(eProxyConnectTrigger);
    MgrBreathDataClearCompensateBuffer();
}

class NormBreathingModeUnitMock : public BDTestFixture
{
public:
    NormBreathingModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~NormBreathingModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyNormBreathingMode = new NormBreathingMode();
    }
    virtual void TearDown(void)
    {
        delete MyNormBreathingMode;
    }
    NormBreathingMode *MyNormBreathingMode;
};

//test for NormBreathingMode::NormBreathingMode (void)
TEST_F(NormBreathingModeUnitMock, NormBreathingMode)
{
    //no source code, not need to test
}

//test for void NormBreathingMode::Exit (void)
TEST_F(NormBreathingModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eBpTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eModeChangeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eHip2AlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eDisconnectionAlarmTrigger)).Times(1);

    MyNormBreathingMode->Exit();
}

//test for void NormBreathingMode::UpdateCommonTriggers (void)
TEST_F(NormBreathingModeUnitMock, UpdateCommonTriggers_)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eModeChangeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHip2AlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHipAlarmTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eTotalGasLossTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eApneaAlarmTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eHardwareErrorAlarmTriger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eOcclusionTrigger)).Times(1);

    MyNormBreathingMode->UpdateCommonTriggers();
}

//test for void NormBreathingMode::UpdateAlarmStatus (void)
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_0)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_1)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_2)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_3)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_4)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_5)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(0));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}
TEST_F(NormBreathingModeUnitMock, UpdateAlarmStatus_6)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eOcclusion, eNotActive)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eGasSuppliesLost, eNotActive)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,INIT_UPDATE_STAT_TIME)).Times(1);

    MyNormBreathingMode->UpdateAlarmStatus();
}

//test for void NormBreathingMode::ProcessModeChangeTrigger (void)
TEST_F(NormBreathingModeUnitMock, ProcessModeChangeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode ()).Times(1);

    MyNormBreathingMode->ProcessModeChangeTrigger();
}

//test for void NormBreathingMode::ProcessItLongAlarmTimeTrigger (void)
TEST_F(NormBreathingModeUnitMock, ProcessItLongAlarmTimeTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskGetIsActive(eITimeTooLong)).Times(1).WillOnce(Return(eNotActive));

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eActive)).Times(1);

    MyNormBreathingMode->ProcessItLongAlarmTimeTrigger();

}

//test for void NormBreathingMode::ProcessPtExhTrigger (void)
TEST_F(NormBreathingModeUnitMock, ProcessPtExhTrigger)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eITimeTooLong, eNotActive)).Times(1);

    MyNormBreathingMode->ProcessPtExhTrigger();

}

//test for void NormBreathingMode::EndSpontInhalation (void)
TEST_F(NormBreathingModeUnitMock, EndSpontInhalation)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase (eExhPhase)).Times(1);

    MyNormBreathingMode->EndSpontInhalation();
}

//test for void NormBreathingMode::ProcessTriggerID (E_TriggerId id)
TEST_F(NormBreathingModeUnitMock, ProcessTriggerID)
{
    //call all function that is tested, not need to test
}

//test for void NormBreathingMode::ProcessHipAlarmTrigger(void)
TEST_F(NormBreathingModeUnitMock, ProcessHipAlarmTrigger)
{
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eExhPhase)).Times(1);

    MyNormBreathingMode->ProcessHipAlarmTrigger();
}

//test for void NormBreathingMode::ProcessExtremePressAlarmTrigger(void)
TEST_F(NormBreathingModeUnitMock, ProcessExtremePressAlarmTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode(eHighPressMode)).Times(1);

    MyNormBreathingMode->ProcessExtremePressAlarmTrigger();
}

//test forvoid NormBreathingMode::ProcessOcclusionAlarmTrigger(void)
TEST_F(NormBreathingModeUnitMock, ProcessOcclusionAlarmTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode(eHighPressMode)).Times(1);

    MyNormBreathingMode->ProcessOcclusionAlarmTrigger();
}

//test for void NormBreathingMode::ProcessApneaAlarmTimeTrigger(void)
TEST_F(NormBreathingModeUnitMock, ProcessApneaAlarmTimeTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToAlarmMode(eApneaBackupMode)).Times(1);

    MyNormBreathingMode->ProcessApneaAlarmTimeTrigger();
}

//test for void NormBreathingMode::ProcessAlarmResetTrigger()
TEST_F(NormBreathingModeUnitMock, ProcessAlarmResetTrigger)
{
    //no source, not need to test
}

//test fot void NormBreathingMode::ProcessManualInhOpReqTrigger()
TEST_F(NormBreathingModeUnitMock, ProcessManualInhOpReqTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger(eBpTimeTrigger)).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyNormBreathingMode->ProcessManualInhOpReqTrigger();
}

//test for void NormBreathingMode::ProcessProxyDisconnectTrigger()
TEST_F(NormBreathingModeUnitMock, ProcessProxyDisconnectTrigger)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyConnectTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eProxyDisconnectTrigger)).Times(1);

    MyNormBreathingMode->ProcessProxyDisconnectTrigger();
}

//test for void NormBreathingMode::ProcessProxyConnectTrigger()
TEST_F(NormBreathingModeUnitMock, ProcessProxyConnectTrigger)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger(eProxyDisconnectTrigger)).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eProxyConnectTrigger)).Times(1);
    EXPECT_CALL(*_BreathMgr, MgrBreathDataClearCompensateBuffer()).Times(1);

    MyNormBreathingMode->ProcessProxyConnectTrigger();
}
#endif //end define BD_MODE_UNIT_TEST
