#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ImmediateTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

ImmediateTrigger::ImmediateTrigger (void) :
        Trigger (eImmediateTrigger)
{

}   // end ImmediateTrigger()
E_Bool ImmediateTrigger::CheckCondition (void)
{
    // The immediate trigger should fire immediately if it is enabled; so,
    // just return the value of the IsEnabled attribute.
    return (IsEnabled);

}   // end CheckCondition()

class ImmediateTriggerUnitMock : public BDTestFixture
{
public:
    ImmediateTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ImmediateTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyImmediateTrigger = new ImmediateTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyImmediateTrigger;
    }
    ImmediateTrigger *MyImmediateTrigger;
};

//test for ImmediateTrigger::ImmediateTrigger (void) :
TEST_F(ImmediateTriggerUnitMock, ImmediateTrigger)
{
    //no source, not need to test
}

//test for E_Bool ImmediateTrigger::CheckCondition (void)
TEST_F(ImmediateTriggerUnitMock, CheckCondition)
{
    MyImmediateTrigger->IsEnabled = eTrue;

    EXPECT_EQ(MyImmediateTrigger->CheckCondition(), eTrue);
}

#endif //end define BD_TRIGGER_UNIT_TEST
