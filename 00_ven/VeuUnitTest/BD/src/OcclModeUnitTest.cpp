#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"

#include "DebugTrace.h"
#include "OcclMode.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "AlarmMockHeader.h"

const int INIT_UPDATE_STAT_TIME = 0;
// Initialize statics
OcclMode* OcclMode::S_Instance = NULL;
OcclMode* OcclMode::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new OcclMode ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()
OcclMode::OcclMode (void) :
        NonVenMode (eFalse)
{

}   // end OcclMode()
void OcclMode::Enter (E_VentilationMode fromModeId)
{
    // Invoke the base class Enter() operation.  This must be done before
    // enabling any triggers.
    NonVenMode::Enter (fromModeId);

    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Enable the reset occlusion trigger.
    triggerMgrPtrEnableTrigger (eOcclAutoResetTrigger);

    // Update the Occlusion alarm status.
    BDAlarmTaskSetIsActive (eOcclusion, eActive);

}   // end Enter()

void OcclMode::Exit (void)
{
    // Initialize pointer to the TriggerMgr
//    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    //Disable eOcclAutoResetTrigger
    triggerMgrPtrDisableTrigger (eOcclAutoResetTrigger);

    // Update the Occlusion alarm status
    BDAlarmTaskSetIsActive (eOcclusion, eNotActive);

    // The base class method takes care of closing the safety valve.
    NonVenMode::Exit ();

}   // end Exit()
void OcclMode::ProcessOcclAutoResetTrigger (void)
{
    // The Occlusion Reset Trigger should only fire during non-ventilation
    // phase since the ventilator was forced into this phase when occlusion was
    // detected.
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eAlarmPhase);

    ModeMgrReturnToLastMode ();

}   // end ProcessOcclAutoResetTrigger()
void OcclMode::UpdateAlarmStatus (void)
{
    E_ReturnStatus status = eEmpty;
//    TriggerMgr* triggerMgrPtr;

    BDAlarmTaskSetIsActive (eOcclusion, eActive);

    // If the status update was unsuccessful, reschedule the next update
    // for the short interval
    if(status != eSuccess)
    {
//        triggerMgrPtr = TriggerMgr::S_GetInstance ();
        triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger);
        triggerMgrPtrEnableTriggerWith (eUpdateStatusTimeTrigger,
                                      INIT_UPDATE_STAT_TIME);
    }

}   // end UpdateAlarmStatus()
void OcclMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eOcclAutoResetTrigger:
            ProcessOcclAutoResetTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

class OcclModeUnitMock : public BDTestFixture
{
public:
    OcclModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~OcclModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyOcclMode = new OcclMode();
    }
    virtual void TearDown(void)
    {
        delete MyOcclMode;
    }
    OcclMode *MyOcclMode;
};

//test for OcclMode* OcclMode::S_GetInstance (void)
TEST_F(OcclModeUnitMock, S_GetInstance)
{
    MyOcclMode->S_GetInstance();

    EXPECT_NE((void*)0, MyOcclMode->S_Instance);
}

//test for OcclMode::OcclMode (void) :
TEST_F(OcclModeUnitMock, OcclMode)
{
    //no source, not need to test
}

//test for void OcclMode::Enter (E_VentilationMode fromModeId)
TEST_F(OcclModeUnitMock, Enter)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTrigger (eOcclAutoResetTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eOcclusion, eActive)).Times(1);

    MyOcclMode->Enter(eHardwareErrorMode);
}

//test for void OcclMode::Exit (void)
TEST_F(OcclModeUnitMock, Exit)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eOcclAutoResetTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger (eUpdateStatusTimeTrigger)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eOcclusion, eNotActive)).Times(1);

    MyOcclMode->Exit();
}

//test for void OcclMode::ProcessOcclAutoResetTrigger (void)
TEST_F(OcclModeUnitMock, ProcessOcclAutoResetTrigger)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrReturnToLastMode ()).Times(1);

    MyOcclMode->ProcessOcclAutoResetTrigger();
}

//test for void OcclMode::UpdateAlarmStatus (void)
TEST_F(OcclModeUnitMock, UpdateAlarmStatus)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive (eOcclusion, eActive));

    MyOcclMode->UpdateAlarmStatus();
}

//test for void OcclMode::ProcessTriggerID (E_TriggerId id)
TEST_F(OcclModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}
#endif //end define BD_MODE_UNIT_TEST
