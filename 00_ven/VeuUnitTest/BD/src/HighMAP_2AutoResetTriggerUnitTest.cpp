#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "HighMAP_2AutoResetTrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "StepMotorMockHeader.h"

// Define statics
HighMAP_2AutoResetTrigger* HighMAP_2AutoResetTrigger::S_Instance = NULL;
HighMAP_2AutoResetTrigger* HighMAP_2AutoResetTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HighMAP_2AutoResetTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end HighMAP_2AutoResetTrigger()
HighMAP_2AutoResetTrigger::HighMAP_2AutoResetTrigger(void): Trigger(eHighMAP_2AutoResetTrigger)
{
    TimeCounter = 0;
}   // end HighMAP_2AutoResetTrigger()
E_Bool HighMAP_2AutoResetTrigger::CheckCondition(void)
{
    const UNSIGNED ONE_SEC = 1000;
    TimeCounter++;

    if ( TimeCounter >= ONE_SEC ) // 1 second.
    {
        E_State ValveState = eClose;
        BDRequestDevice(eSafetyValveSetDesiredState);
    }

    LONG InhPressSensor = 4;//test
    BDRequestDevice(eInhPressSensorGetLastReading);


    if (InhPressSensor <= (4 * SCALE) )
    {
        if ( TimeCounter >= 1500)
        {
            TriggerDidFire = eTrue;
        }
    }

    return(TriggerDidFire);

}   // end CheckCondition()

void HighMAP_2AutoResetTrigger::EnableTrigger(void)
{

    TimeCounter = 0;
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
class HighMAP_2AutoResetTriggerUnitMock : public BDTestFixture
{
public:
    HighMAP_2AutoResetTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~HighMAP_2AutoResetTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyHighMAP_2AutoResetTrigger = new HighMAP_2AutoResetTrigger();
    }
    virtual void TearDown(void)
    {
        delete MyHighMAP_2AutoResetTrigger;
    }
    HighMAP_2AutoResetTrigger *MyHighMAP_2AutoResetTrigger;
};

//test for HighMAP_2AutoResetTrigger* HighMAP_2AutoResetTrigger::S_GetInstance(void)
TEST_F(HighMAP_2AutoResetTriggerUnitMock, S_GetInstance)
{
    MyHighMAP_2AutoResetTrigger->S_GetInstance();

    EXPECT_NE(MyHighMAP_2AutoResetTrigger->S_Instance, (void*)0);
}

//test for HighMAP_2AutoResetTrigger::HighMAP_2AutoResetTrigger(void): Trigger(eHighMAP_2AutoResetTrigger)
TEST_F(HighMAP_2AutoResetTriggerUnitMock, HighMAP_2AutoResetTrigger)
{
    EXPECT_EQ(MyHighMAP_2AutoResetTrigger->TimeCounter, 0);
}

//test for E_Bool HighMAP_2AutoResetTrigger::CheckCondition(void)
TEST_F(HighMAP_2AutoResetTriggerUnitMock, CheckCondition)
{
    MyHighMAP_2AutoResetTrigger->TimeCounter = 2000;

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eSafetyValveSetDesiredState)).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eInhPressSensorGetLastReading)).Times(1);

    EXPECT_EQ(MyHighMAP_2AutoResetTrigger->CheckCondition(), eTrue);
}

//test for void HighMAP_2AutoResetTrigger::EnableTrigger(void)
TEST_F(HighMAP_2AutoResetTriggerUnitMock, EnableTrigger)
{
    MyHighMAP_2AutoResetTrigger->EnableTrigger();

    EXPECT_EQ(MyHighMAP_2AutoResetTrigger->TimeCounter, 0);
}

#endif //end define BD_TRIGGER_UNIT_TEST
