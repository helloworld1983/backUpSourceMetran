#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "Trigger.h"
#include "DebugTrace.h"

using namespace ::testing;

Trigger::Trigger (E_TriggerId id) :
        IsEnabled (eFalse), TriggerDidFire (eFalse), TriggerId (id)
{

}   // end Trigger()
E_Bool Trigger::CheckCondition (void)
{
    return (TriggerDidFire);

}   // end CheckCondition()
void Trigger::EnableTrigger (void)
{

    IsEnabled = eTrue;
    TriggerDidFire = eFalse;

}   // end EnableTrigger()
void Trigger::DisableTrigger (void)
{

    IsEnabled = eFalse;

}   // end DisableTrigger()
void Trigger::EnableTrigger (UNSIGNED)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end EnableTrigger()
void Trigger::ReEnableTrigger (void)
{
    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end ReEnableTrigger()
void Trigger::UpdateState (E_TriggerId)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end UpdateState()
E_Bool Trigger::PreCheckCondition (void)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

    return (eFalse);

}   // end PreCheckCondition()
void Trigger::PhaseInSettingChange (LONG)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()
UNSIGNED Trigger::GetTimePeriod (void)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);
    return (0);

}   // end GetTimePeriod()

class TriggerUnitMock : public BDTestFixture
{
public:
    TriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~TriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyTrigger = new Trigger();
    }
    virtual void TearDown(void)
    {
        delete MyTrigger;
    }
    Trigger *MyTrigger;
};

//test for Trigger::Trigger (E_TriggerId id)
TEST_F(TriggerUnitMock, Trigger)
{
    //no source, not need to test
}

//test for E_Bool Trigger::CheckCondition (void)
TEST_F(TriggerUnitMock, CheckCondition)
{
    MyTrigger->TriggerDidFire = eTrue;

    EXPECT_EQ(MyTrigger->CheckCondition(), eTrue);
}

//test for void Trigger::EnableTrigger (void)
TEST_F(TriggerUnitMock, EnableTrigger_void)
{
    MyTrigger->EnableTrigger();

    EXPECT_EQ(MyTrigger->IsEnabled, eTrue);
    EXPECT_EQ(MyTrigger->TriggerDidFire, eFalse);

}

//test for void Trigger::DisableTrigger (void)
TEST_F(TriggerUnitMock, DisableTrigger)
{
    MyTrigger->DisableTrigger();

    EXPECT_EQ(MyTrigger->IsEnabled, eFalse);
}

//test for void Trigger::EnableTrigger (UNSIGNED)
TEST_F(TriggerUnitMock, EnableTrigger_UNSIGNED)
{
    //virtual function, not need to test
}

//test for void Trigger::ReEnableTrigger (void)
TEST_F(TriggerUnitMock, ReEnableTrigger)
{
    //virtual function, not need to test
}

//test for void Trigger::UpdateState (E_TriggerId)
TEST_F(TriggerUnitMock, UpdateState)
{
    //virtual function, not need to test
}

//test for E_Bool Trigger::PreCheckCondition (void)
TEST_F(TriggerUnitMock, PreCheckCondition)
{
    //virtual function, not need to test
}

//test for void Trigger::PhaseInSettingChange (LONG)
TEST_F(TriggerUnitMock, PhaseInSettingChange)
{
    //virtual function, not need to test
}

//test for UNSIGNED Trigger::GetTimePeriod (void)
TEST_F(TriggerUnitMock, GetTimePeriod)
{
    //virtual function, not need to test
}

#endif //end define BD_TRIGGER_UNIT_TEST
