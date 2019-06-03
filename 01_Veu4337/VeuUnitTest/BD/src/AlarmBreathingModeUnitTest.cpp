#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "AlarmBreathingMode.h"
#include "ModeMgrMockHeader.h"
#include "DebugTrace.h"

using namespace ::testing;

AlarmBreathingMode::AlarmBreathingMode (E_Bool doesManualReset) :
            BreathingMode (doesManualReset)
{

}   // end AlarmBreathingMode()
void AlarmBreathingMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eAlarmResetTrigger:
            ProcessAlarmResetTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
void AlarmBreathingMode::ProcessAlarmResetTrigger (void)
{
    // Pointer to the Mode Manager
//    ModeMgr* modeMgrPtr;

    // The Alarm Reset Trigger should only fire during exhalation.  The
    // operator can press the Alarm Reset key at any time, but the trigger
    // will only fire once the unrestricted phase of exhalation is active.
    ASSERTION((PhaseMgr::S_GetInstance())->GetCurrentPhaseId() == eExhPhase);

    // Go to previous mode or the operator set mode if no non-resettable
    // breathing alarms are active.
//    modeMgrPtr = ModeMgr::S_GetInstance ();
    modeMgrPtrReturnToLastMode ();

}   // ProcessAlarmResetTrigger()
void AlarmBreathingMode::Enter (E_VentilationMode fromModeId)
{
}
void AlarmBreathingMode::Exit (void)
{
}

class AlarmBreathingModeUnitMock : public BDTestFixture
{
public:
    AlarmBreathingModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~AlarmBreathingModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyAlarmBreathingMode = new AlarmBreathingMode(eTrue);
    }
    virtual void TearDown(void)
    {
        delete MyAlarmBreathingMode;
    }
    AlarmBreathingMode *MyAlarmBreathingMode;
};

//test for AlarmBreathingMode::AlarmBreathingMode (E_Bool doesManualReset)
TEST_F(AlarmBreathingModeUnitMock, AlarmBreathingMode)
{
    //no source code, not need to test
}

//test for void AlarmBreathingMode::ProcessTriggerID (E_TriggerId id)
TEST_F(AlarmBreathingModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for void AlarmBreathingMode::ProcessAlarmResetTrigger (void)
TEST_F(AlarmBreathingModeUnitMock, ProcessAlarmResetTrigger)
{
    EXPECT_CALL(*_ModeMgr, modeMgrPtrReturnToLastMode ()).Times(1);

    MyAlarmBreathingMode->ProcessAlarmResetTrigger();
}

//test for void AlarmBreathingMode::Enter (E_VentilationMode fromModeId)
TEST_F(AlarmBreathingModeUnitMock, Enter)
{
    //no source code, not need to test
}

//test for void AlarmBreathingMode::Exit (void)
TEST_F(AlarmBreathingModeUnitMock, Exit)
{
    //no source code, not need to test
}
#endif //end define BD_MODE_UNIT_TEST
