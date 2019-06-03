#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ReconnectionTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "BreathMgrMockHeader.h"

ReconnectionTrigger* ReconnectionTrigger::S_Instance = NULL;
static const LONG PRESSURE_LIMIT  = 3 * SCALE; // 3 cmH2O
ReconnectionTrigger* ReconnectionTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ReconnectionTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end ReconnectionTrigger()

ReconnectionTrigger::ReconnectionTrigger(void): Trigger(eReconnectionTrigger)
{

}   // end ReconnectionTrigger()

void ReconnectionTrigger::EnableTrigger(void)
{
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();
}   // end EnableTrigger()
E_Bool ReconnectionTrigger::CheckCondition(void)
{
    static USHORT filtertimes = 0;

    if(MgrBreathDataGetMonitoredPressure()> PRESSURE_LIMIT)
    {
        filtertimes++;
        if(filtertimes > 250)
        {
            filtertimes = 0;
            TriggerDidFire = eTrue;
        }
    }
    else
    {
        filtertimes = 0;
    }

    return(TriggerDidFire);

}   // end CheckCondition()

class ReconnectionTriggerUnitMock : public BDTestFixture
{
public:
    ReconnectionTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ReconnectionTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyReconnectionTrigger = new ReconnectionTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyReconnectionTrigger;
    }
    ReconnectionTrigger *MyReconnectionTrigger;
};

//test for ReconnectionTrigger* ReconnectionTrigger::S_GetInstance(void)
TEST_F(ReconnectionTriggerUnitMock, S_GetInstance)
{
    MyReconnectionTrigger->S_GetInstance();

    EXPECT_NE(MyReconnectionTrigger->S_Instance, (void*)0);
}

//test for ReconnectionTrigger::ReconnectionTrigger(void): Trigger(eReconnectionTrigger)
TEST_F(ReconnectionTriggerUnitMock, Trigger)
{
    //no source, not need to test
}

//test for void ReconnectionTrigger::EnableTrigger(void)
TEST_F(ReconnectionTriggerUnitMock, EnableTrigger)
{
    //all calling function is tested, not need to test
}

//test for E_Bool ReconnectionTrigger::CheckCondition(void)
TEST_F(ReconnectionTriggerUnitMock, CheckCondition_0)
{
    MyReconnectionTrigger->TriggerDidFire = eFalse;
    for(int i = 0; i < 251; ++i)
    {
        EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(PRESSURE_LIMIT + 1));
        MyReconnectionTrigger->CheckCondition();
    }

    EXPECT_EQ(MyReconnectionTrigger->TriggerDidFire, eTrue);
}
TEST_F(ReconnectionTriggerUnitMock, CheckCondition_1)
{
    MyReconnectionTrigger->TriggerDidFire = eFalse;

    EXPECT_CALL(*_BreathMgr, MgrBreathDataGetMonitoredPressure()).Times(1).WillOnce(Return(1));
    MyReconnectionTrigger->CheckCondition();

    EXPECT_EQ(MyReconnectionTrigger->TriggerDidFire, eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
