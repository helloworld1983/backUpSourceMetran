#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "CyclicTimeTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

CyclicTimeTrigger::CyclicTimeTrigger (E_TriggerId id, CHAR* timerName,
                                      UNSIGNED timePeriod) :
        TimeTrigger (id, timerName)
{

    SetTimePeriod (timePeriod);

}   // end CyclicTimeTrigger()
void CyclicTimeTrigger::EnableTrigger (void)
{
//    STATUS status;      // status of RTOS calls
//    UNSIGNED timePeriod = GetTimePeriod();

    // Reset the timer for the TimePeriod.  This is a cyclic timer.
    // Passing TRUE as the last argument indicates that the timer should be
    // enabled now.   Perform error handling if the reset is not successful.
//    status = Reset(timePeriod, timePeriod, TRUE);
//    ASSERTION(status == NU_SUCCESS);
    Reset();

    // Invoke the base class method to perform common processing.
    Trigger::EnableTrigger ();

}   // end EnableTrigger()
void CyclicTimeTrigger::EnableTrigger (UNSIGNED time)
{
//    STATUS status;      // status of RTOS calls

    // Reset the timer using the time value just calculated for the
    // initial time period.  The second argument indicates the time value for
    // subsequent timeout periods.  Passing TRUE as the last argument
    // indicates that the timer should be enabled now.
    // Perform error handling if the reset is not successful.
//    status = Reset(time, GetTimePeriod(), TRUE);
//    ASSERTION(status == NU_SUCCESS);
    Reset();

    // Invoke the base class method to perform common processing.
    Trigger::EnableTrigger ();

}  // end EnableTrigger()
E_Bool CyclicTimeTrigger::CheckCondition (void)
{
    E_Bool rtnValue = TriggerDidFire;

    // If the trigger fired, reset it.
    if(eTrue == rtnValue)
    {
        TriggerDidFire = eFalse;
    }

    return (rtnValue);

}   // end CheckCondition()
void CyclicTimeTrigger::PhaseInSettingChange (LONG)
{
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()
void CyclicTimeTrigger::SetTimePeriod (UNSIGNED time)
{

    // Update both the TimePeriod and the BackupTimePeriod.  BackupTimePeriod
    // is a one's complement of the TimePeriod and is used to protect against
    // data corruption.
    TimePeriod = time;
    BackupTimePeriod = ~time;

}   // end SetTimePeriod()

class CyclicTimeTriggerUnitMock : public BDTestFixture
{
public:
    CyclicTimeTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~CyclicTimeTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyCyclicTimeTrigger = new CyclicTimeTrigger(eAlarmResetTrigger, (CHAR*)"test", 1);
    }
    virtual void TearDown(void)
    {
        delete MyCyclicTimeTrigger;
    }
    CyclicTimeTrigger *MyCyclicTimeTrigger;
};

//test for CyclicTimeTrigger::CyclicTimeTrigger
TEST_F(CyclicTimeTriggerUnitMock, CyclicTimeTrigger)
{
    //all calling function is tested, not need to test
}

//test for void CyclicTimeTrigger::EnableTrigger (void)
TEST_F(CyclicTimeTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for void CyclicTimeTrigger::EnableTrigger (UNSIGNED time)
TEST_F(CyclicTimeTriggerUnitMock, EnableTrigger_time)
{
    //all calling function is tested, not need to test
}

//test for E_Bool CyclicTimeTrigger::CheckCondition (void)
TEST_F(CyclicTimeTriggerUnitMock, CheckCondition_0)
{
    MyCyclicTimeTrigger->TriggerDidFire = eTrue;

    MyCyclicTimeTrigger->CheckCondition();

    EXPECT_EQ(MyCyclicTimeTrigger->TriggerDidFire, eFalse);
}
TEST_F(CyclicTimeTriggerUnitMock, CheckCondition_1)
{
    MyCyclicTimeTrigger->TriggerDidFire = eFalse;

    EXPECT_EQ(MyCyclicTimeTrigger->CheckCondition(), eFalse);

}

//test for void CyclicTimeTrigger::PhaseInSettingChange (LONG)
TEST_F(CyclicTimeTriggerUnitMock, PhaseInSettingChange)
{
    //no source, not need to test
}

//test for void CyclicTimeTrigger::SetTimePeriod (UNSIGNED time)
TEST_F(CyclicTimeTriggerUnitMock, SetTimePeriod)
{
    MyCyclicTimeTrigger->SetTimePeriod(10);

    EXPECT_EQ(MyCyclicTimeTrigger->TimePeriod, 10);
}

#endif //end define BD_TRIGGER_UNIT_TEST
